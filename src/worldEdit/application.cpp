#include "mixer.lib.h"
#include "mixer.lib.inputContext.h"
#include "mixer.lib.window.h"
#include "mixer.lib.videoDriver.h"
#include "mixer.lib.event.h"
#include "mixer.lib.popup.h"
#include "mixer.lib.material.h"
#include "mixer.gui.h"

#include "application.h"
#include "MapCell.h"

#include <filesystem>

#define CommandID_TerrainEditor 1

Application* g_app = nullptr;

void log_onError(const char* message) {
	fprintf(stderr, message);
	g_app->WriteLog(message);
}

void log_onInfo(const char* message) {
	fprintf(stdout, message);
	g_app->WriteLog(message);
}

void log_onWarning(const char* message) {
	fprintf(stdout, message);
	g_app->WriteLog(message);
}

void window_onCLose(miWindow* window) {
	miQuit();
}

void window_onActivate(miWindow* window)
{
	g_app->m_inputContext->m_isMMBHold = false;
	g_app->m_inputContext->m_isLMBHold = false;
	g_app->m_inputContext->m_isRMBHold = false;
	g_app->m_inputContext->ResetHold();
}

void window_callbackOnCommand(s32 commandID) {
	g_app->m_dt = 0.f;
	switch (commandID)
	{
	default:
		break;
	case CommandID_TerrainEditor: g_app->ShowGUITab(CommandID_TerrainEditor); break;
	}
}

ApplicationGUI::ApplicationGUI()
{
	m_fontDefault = miGUILoadFont(L"../res/fonts/Noto/notosans.txt");
	m_context = miGUICreateContext(g_app->m_mainWindow);
}

ApplicationGUI::~ApplicationGUI()
{
	if (m_panel_terrain) m_context->DeleteElement(m_panel_terrain);
	if (m_context) miGUIDestroyContext(m_context);
}

void ApplicationGUI::Init()
{
	m_panel_terrain = m_context->CreatePanel(v2f(0.f, 0.f), v2f(200.f, 800.f));
	m_panel_terrain->m_color = ColorWhite;
	m_panel_terrain->m_color.setAlpha(0.3f);
	//m_panel_terrain->m_onUpdateTransform = GUICallback_pnlLeft_onUpdateTransform;
	m_panel_terrain->m_draw = true;
	m_panel_terrain->m_useScroll = false;
	m_panel_terrain->SetVisible(false);
}

Application::Application()
{
	g_app = this;
}

Application::~Application()
{
	for (u32 i = 0; i < m_mapCells.m_size; ++i)
	{
		delete m_mapCells.m_data[i];
	}
	if (m_testMapCell)
		delete m_testMapCell;

	if (m_GUI)
		delete m_GUI;
	if (m_cameraFly)
		delete m_cameraFly;
	if (m_libContext)
		miDestroy(m_libContext);
	if (m_inputContext)
		miDestroy(m_inputContext);
}

void Application::OnCreate(const char* videoDriver)
{
	{
		FILE* logFile = fopen("log.txt", "w");
		if (logFile)
			fclose(logFile);
	}

	miLogSetErrorOutput(log_onError);
	miLogSetWarningOutput(log_onWarning);
	miLogSetInfoOutput(log_onInfo);

	m_inputContext = miCreate<miInputContext>();
	m_libContext = miCreate<miLibContext>();
	m_libContext->Start(m_inputContext);

	u32 windowFlags = 0;
	m_mainWindow = miCreateWindow(800, 600, windowFlags, 0);
	m_mainWindow->m_onClose = window_onCLose;
	m_mainWindow->m_onActivate = window_onActivate;
	m_mainWindow->m_onCommand = window_callbackOnCommand;
	m_mainWindow->Show();

	if (!miInitVideoDriver(videoDriver, m_mainWindow))
	{
		miLogWriteWarning("Can't load video driver : %s\n", videoDriver);
		for (auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
		{
			auto path = entry.path();
			if (path.has_extension())
			{
				auto ex = path.extension();
				if (ex == ".dll")
				{
					miLogWriteWarning("Trying to load video driver : %s\n", path.generic_string().c_str());

					if (miInitVideoDriver(path.generic_string().c_str(), m_mainWindow))
					{
						goto vidOk;
					}
					else
					{
						miLogWriteWarning("Can't load video driver : %s\n", path.generic_string().c_str());
					}
				}
			}
		}
		MI_PRINT_FAILED;
		throw std::exception("Failed to init video driver");
	}

vidOk:

	m_gpu = miGetVideoDriver();
	m_gpu->SetClearColor(0.41f, 0.41f, 0.41f, 1.f);
	m_gpu->GetDepthRange(&m_gpuDepthRange);

	m_cameraFly = new FlyCamera;
	m_cameraFly->m_localPosition.set(0.f, 0.0002f, 0.f, 0.f);
	m_cameraFly->m_near = 0.00001000f;
	m_cameraFly->m_far = 100.f;
	m_cameraFly->m_moveSpeedDefault = 0.0001f;
	{
		Mat4 lookAt;
		math::makeLookAtRHMatrix(lookAt, m_cameraFly->m_localPosition, v4f(0.f, 0.f, 1.f, 0.f), v4f(0.f, 1.f, 0.f, 0.f));
		m_cameraFly->m_rotationMatrix.setBasis(lookAt);
	}
	m_activeCamera = m_cameraFly;

	m_GUI = new ApplicationGUI;
	m_GUI->Init();

	/*u32 num = 2;
	for (u32 i = 0; i < 100; ++i)
	{
		num = i;
		if ((num & 0x1) == 0)
			printf("%u EVEN\n", num);
	}*/

	/*{
		std::string src = "hello hello hello hello hello hello hello hello";
		auto compressBound = miGetCompressBound(src.size());
		u8* compBuf = (u8*)malloc(compressBound);
		
		u32 srcSize = src.size();
		u32 compSize = compressBound;
		miCompress(src.data(), srcSize, compBuf, &compSize);

		{
			u8* decompBuf = (u8*)malloc(srcSize+1);
			u32 dstLen = srcSize;

			miUncompress(compBuf, compSize, decompBuf, &dstLen);
			decompBuf[srcSize] = 0;

			free(decompBuf);
		}

		free(compBuf);
	}*/

	OpenMap();
}

void Application::MainLoop()
{
	miEvent currentEvent;
	while (miRun(&m_dt))
	{
		bool isSpace = m_inputContext->IsKeyHold(miKey::K_SPACE);

		if (m_inputContext->m_isRMBDown && !isSpace)
		{
			auto p = ShowPopup();
			if (p)
			{
				p->Show(m_mainWindow, (s32)m_inputContext->m_cursorCoords.x, (s32)m_inputContext->m_cursorCoords.y);
				miDestroy(p);
			}
		}

		/*if (m_inputContext->m_kbm == miKeyboardModifier::Alt)
		{
			Mat4 lookAt;
			math::makeLookAtRHMatrix(lookAt, v4f(), -m_activeCamera->m_localPosition, v4f(0.f, 1.f, 0.f, 0.f));
			m_activeCamera->m_rotationMatrix.identity();
			m_activeCamera->m_rotationMatrix.setBasis(lookAt);
		}*/
		m_activeCamera->OnUpdate();
		if (isSpace)
		{
			m_activeCamera->Rotate(m_inputContext->m_mouseDelta, m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_LSHIFT) || m_inputContext->IsKeyHold(miKey::K_RSHIFT))
				m_activeCamera->m_moveSpeed = m_activeCamera->m_moveSpeedDefault * 5.f;
			else
				m_activeCamera->m_moveSpeed = m_activeCamera->m_moveSpeedDefault;

			if (m_inputContext->IsKeyHold(miKey::K_W))
				m_activeCamera->MoveForward(m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_S))
				m_activeCamera->MoveBackward(m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_A))
				m_activeCamera->MoveLeft(m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_D))
				m_activeCamera->MoveRight(m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_E))
				m_activeCamera->MoveUp(m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_Q))
				m_activeCamera->MoveDown(m_dt);

			

			auto cursorX = std::floor((f32)m_mainWindow->m_currentSize.x / 2.f);
			auto cursorY = std::floor((f32)m_mainWindow->m_currentSize.y / 2.f);
			//m_inputContext->m_cursorCoordsOld.set(cursorX, cursorY);

			// move cursor to center of the window
			// this example not about it, so implement it by yourself
			miSetCursorPosition(cursorX, cursorY, m_mainWindow);
		}

		FrustumCullMap();

		m_GUI->m_context->Update(m_dt);
		//m_isCursorInGUI = miIsCursorInGUI();
		//m_isGUIInputFocus = miIsInputInGUI();

		while (miPollEvent(currentEvent))
		{
			switch (currentEvent.m_type)
			{
			default:
			case miEventType::Engine:
				break;
			case miEventType::System:
				break;
			case miEventType::Window: {
				if (currentEvent.m_event_window.m_event == miEvent_Window::size_changed) {
					m_gpu->UpdateMainRenderTarget(v2f((f32)m_mainWindow->m_currentSize.x, (f32)m_mainWindow->m_currentSize.y));
					m_GUI->m_context->NeedRebuild();
					//_callViewportOnWindowSize();
				}
			}break;
			}
		}

		switch (*m_libContext->m_state)
		{
		default:
			break;
		case miSystemState::Run:
		{
			m_gpu->BeginDraw();
			m_gpu->ClearAll();

			miSetMatrix(miMatrixType::View, &m_activeCamera->m_view);
			miSetMatrix(miMatrixType::Projection, &m_activeCamera->m_projection);
			miSetMatrix(miMatrixType::ViewProjection, &m_activeCamera->m_viewProjection);
			

			/*m_gpu->UseDepth(false);*/
			miMaterial default_polygon_material;
			default_polygon_material.m_colorDiffuse.set(1.f, 0.5f, 0.5f, 0.8f);
			default_polygon_material.m_type = miMaterialType::Standart;
			default_polygon_material.m_sunPos = v4f(0.f, 10.f, 0.f, 0.f);
			default_polygon_material.m_wireframe = true;
			miSetMaterial(&default_polygon_material);
			if (m_testMapCell)
			{
				DrawMapCell(m_testMapCell);
			}

			for (u32 i = 0; i < m_visibleMapCells.m_size; ++i)
			{
				DrawMapCell(m_visibleMapCells.m_data[i]);
			}

			m_gpu->DrawLine3D(v4f(1.f, 0.f, 0.f, 0.f), v4f(-1.f, 0.f, 0.f, 0.f), ColorRed);
			m_gpu->DrawLine3D(v4f(0.f, 0.f, 1.f, 0.f), v4f(0.f, 0.f, -1.f, 0.f), ColorLime);

			m_gpu->BeginDrawGUI();
			m_GUI->m_context->DrawAll();
			m_gpu->EndDrawGUI();

			m_gpu->EndDraw();
			m_gpu->SwapBuffers();
		}
		break;
		}

	}
}

void Application::WriteLog(const char* message)
{
	FILE* logFile = fopen("log.txt", "a");
	if (logFile)
	{
		fprintf(logFile, "%s", message);
		fclose(logFile);
	}
}

miPopup* Application::ShowPopup()
{
	miPopup* p = miCreatePopup(); // new miPopup;
	p->AddItem(L"Terrain editor", CommandID_TerrainEditor, 0);
	return p;
}

void Application::ShowGUITab(u32 id)
{
	switch (id)
	{
	case CommandID_TerrainEditor:
		m_GUI->m_panel_terrain->SetVisible(true);
		break;
	}
}

void Application::OpenMap()
{
	if (std::filesystem::exists("../data/world.dat"))
	{
		ReadWorld();
	}
	else
	{
		GenerateWorld();
	}
}

void Application::GenerateWorld()
{
	/*m_testMapCell = new MapCell;
	m_testMapCell->Generate();*/
	
	// Создать ячейки. Записать их в файл.
	// m_mapCells будет хранить указатели на все ячейки.

	u32 cellsNumX = 3;
	u32 cellsNumY = 3;

	f32 cellSize = 0.01f;// 0.01f = 100m

	f32 mapSizeX = (f32)cellsNumX * cellSize; 
	f32 mapSizeY = (f32)cellsNumY * cellSize;

	f32 mapSizeHalfX = mapSizeX * 0.5f;
	f32 mapSizeHalfY = mapSizeY * 0.5f;


	v3f position;
	position.x = -mapSizeHalfX;
	position.z = -mapSizeHalfY;

	for (u32 iy = 0; iy < cellsNumY; ++iy)
	{
		for (u32 ix = 0; ix < cellsNumX; ++ix)
		{
			MapCell* newCell = new MapCell;
			newCell->m_id = iy + ix;
			newCell->Generate();

			newCell->m_position = position;

			m_mapCells.push_back(newCell);

			position.x += cellSize;
		}
	
		position.x = -mapSizeHalfX;
		position.z += cellSize;
	}
}

void Application::ReadWorld()
{

}

void Application::DrawMapCell(MapCell* cell)
{
	Mat4 W;
	W.setTranslation(cell->m_position);

	miSetMatrix(miMatrixType::World, &W);

	Mat4 WVP = m_activeCamera->m_projection * m_activeCamera->m_view * W;
	miSetMatrix(miMatrixType::WorldViewProjection, &WVP);
	m_gpu->SetTexture(0, miGetBlackTexture());
	m_gpu->SetMesh(cell->m_meshGPU0[0]);
	m_gpu->Draw();
	m_gpu->SetMesh(cell->m_meshGPU1[0]);
	m_gpu->Draw();
	m_gpu->SetMesh(cell->m_meshGPU2[0]);
	m_gpu->Draw();
	m_gpu->SetMesh(cell->m_meshGPU3[0]);
	m_gpu->Draw();
}

void Application::FrustumCullMap()
{
	m_visibleMapCells.clear();
	
	for (u32 i = 0; i < m_mapCells.m_size; ++i)
	{
		if (m_activeCamera->m_frust.AABBInFrustum(m_mapCells.m_data[i]->m_aabb, m_mapCells.m_data[i]->m_position))
		{
			m_visibleMapCells.push_back(m_mapCells.m_data[i]);
		}
	}
}


