/*
  Copyright (C) 2021 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
  3. The names of the authors may not be used to endorse or promote
	 products derived from this software without specific prior
	 written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "mi/MainSystem/MainSystem.h"
#include "miGUI.h"

#include "mi/Window/window.h"
#include "mi/GraphicsSystem/GS.h"

#include "application.h"
#include "MapCell.h"
#include "Player.h"

#include <filesystem>

#include <Windows.h>

#define CommandID_TerrainEditor 1

f32 g_terrainLODDistance_0 = 0.005f;
f32 g_terrainLODDistance_1 = 0.02f;
f32 g_terrainLODDistance_2 = 0.03f;
f32 g_terrainLODDistance_3 = 0.04f;
//f32 g_terrainLODDistance_4 = 0.025f;

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
	miGetMainSystem(0, 0)->Quit();
}

void window_onActivate(miWindow* window)
{
	/*g_app->m_inputContext->m_isMMBHold = false;
	g_app->m_inputContext->m_isLMBHold = false;
	g_app->m_inputContext->m_isRMBHold = false;
	g_app->m_inputContext->ResetHold();*/
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
	//m_fontDefault = miGUILoadFont(L"../res/fonts/Noto/notosans.txt");
	//m_context = miGUICreateContext(g_app->m_mainWindow);
}

ApplicationGUI::~ApplicationGUI()
{
	//if (m_panel_debug)m_context->DeleteElement(m_panel_debug);
	//if (m_panel_terrain) m_context->DeleteElement(m_panel_terrain);
	//if (m_context) miGUIDestroyContext(m_context);
}

void ApplicationGUI::Init()
{
	//m_panel_terrain = m_context->CreatePanel(v2f(0.f, 0.f), v2f(200.f, 800.f));
	//m_panel_terrain->m_color = ColorWhite;
	//m_panel_terrain->m_color.setAlpha(0.3f);
	////m_panel_terrain->m_onUpdateTransform = GUICallback_pnlLeft_onUpdateTransform;
	//m_panel_terrain->m_draw = true;
	//m_panel_terrain->m_useScroll = false;
	//m_panel_terrain->SetVisible(false);

	//m_panel_debug = m_context->CreatePanel(v2f(0.f, 0.f), v2f(800.f, 200.f));
	//m_panel_debug->m_color = ColorWhite;
	//m_panel_debug->m_color.setAlpha(0.3f);
	////m_panel_terrain->m_onUpdateTransform = GUICallback_pnlLeft_onUpdateTransform;
	//m_panel_debug->m_draw = true;
	//m_panel_debug->m_useScroll = false;
	//{
	//	m_debug_text_FPS = m_context->CreateText(v2f(), m_fontDefault, L"FPS: ");
	//	m_debug_text_FPS->SetParent(m_panel_debug);

	//	m_debug_text_position = m_context->CreateText(v2f(100.f, 0.f), m_fontDefault, L":");
	//	m_debug_text_position->SetParent(m_panel_debug);

	//	m_debug_text_cameraCellID = m_context->CreateText(v2f(0.f, 20.f), m_fontDefault, L":");
	//	m_debug_text_cameraCellID->SetParent(m_panel_debug);
	//}

	//m_panel_debug->SetVisible(false);
}

Application::Application()
{
	g_app = this;
}

Application::~Application()
{
	if (m_file_gen) fclose(m_file_gen);
	if (m_file_ids) fclose(m_file_ids);
	
	if (m_player) 
		delete m_player;
	/*for (u32 i = 0; i < m_mapCells.m_size; ++i)
	{
		delete m_mapCells.m_data[i];
	}*/
	//if (m_testMapCell)
	//	delete m_testMapCell;

	if (m_GUI)
		delete m_GUI;

	if (m_libContext)
		miDestroy(m_libContext);

	if (m_inputContext)
		miDestroy(m_inputContext);
}

bool Application::OnCreate(const char* videoDriver)
{
	{
		FILE* logFile = fopen("log.txt", "w");
		if (logFile)
			fclose(logFile);
	}


	miLogSetErrorOutput(log_onError);
	miLogSetWarningOutput(log_onWarning);
	miLogSetInfoOutput(log_onInfo);

	
	m_file_gen = fopen("../data/world/gen.dpk", "rb");
	if (!m_file_gen)
	{
		miLogWriteError("cant open gen.dpk\n");
		return false;
	}

	m_file_ids = fopen("../data/world/ids.bin", "rb");
	if (!m_file_ids)
	{
		miLogWriteError("cant open ids.bin\n");
		return false;
	}
	
	for (int i = 0; i < 9; ++i)
	{
		m_mapCells.push_back(new MapCell);
	}

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
		return false;
	}

vidOk:

	m_gpu = miGetVideoDriver();
	m_gpu->SetClearColor(0.41f, 0.41f, 0.41f, 1.f);
	m_gpu->GetDepthRange(&m_gpuDepthRange);
	//m_gpu->UseVSync(false);

	

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

	m_player = new Player;

	m_activeCamera = m_player->m_cameraFly;

	if (!OpenMap())
		return false;

	return true;
}

void Application::MainLoop()
{
	s32 fps = 0;
	f32 fpsTime = 0.f;

	miEvent currentEvent;
	while (miRun(&m_dt))
	{

		if (m_needUpdateMapCell)
			_updateMapCell();

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

		if (m_inputContext->IsKeyHit(miKey::K_F3))
		{
			m_GUI->m_panel_debug->SetVisible(m_GUI->m_panel_debug->m_visible ? false : true);
		}

		if (m_GUI->m_panel_debug->m_visible)
		{
			++fps;
			fpsTime += m_dt;
			if (fpsTime > 1.f)
			{
				m_GUI->m_debug_text_FPS->SetText(L"FPS:%i", fps);
				fps = 0;
				fpsTime = 0.f;
			}

			m_GUI->m_debug_text_position->SetText(L"%f %f %f (%Lf %Lf %Lf)", 
				m_activeCamera->m_localPosition.x,
				m_activeCamera->m_localPosition.y, 
				m_activeCamera->m_localPosition.z,
				
				(f64)m_activeCamera->m_localPosition.x * 10000., 
				(f64)m_activeCamera->m_localPosition.y * 10000.,
				(f64)m_activeCamera->m_localPosition.z * 10000.
			);

			m_GUI->m_debug_text_cameraCellID->SetText(L"Cell: %i", m_player->m_cellID);
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
			m_cameraWasMoved = true;
			m_player->m_cameraFly->Rotate(m_inputContext->m_mouseDelta, m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_LSHIFT) || m_inputContext->IsKeyHold(miKey::K_RSHIFT))
				m_player->m_cameraFly->m_moveSpeed = 100.f;
			else
				m_player->m_cameraFly->m_moveSpeed = m_player->m_cameraFly->m_moveSpeedDefault;

			if (m_inputContext->IsKeyHold(miKey::K_W))
				m_player->MoveForward(m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_S))
				m_player->MoveBackward(m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_A))
				m_player->MoveLeft(m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_D))
				m_player->MoveRight(m_dt);
			if (m_inputContext->IsKeyHold(miKey::K_E))
			{
				m_player->MoveUp(m_dt);
			}
			if (m_inputContext->IsKeyHold(miKey::K_Q))
			{
				m_player->MoveDown(m_dt);
			}

			

			auto cursorX = std::floor((f32)m_mainWindow->m_currentSize.x / 2.f);
			auto cursorY = std::floor((f32)m_mainWindow->m_currentSize.y / 2.f);
			//m_inputContext->m_cursorCoordsOld.set(cursorX, cursorY);

			// move cursor to center of the window
			// this example not about it, so implement it by yourself
			miSetCursorPosition(cursorX, cursorY, m_mainWindow);
		}

	//	FindCurrentCellID();

		if (m_cameraWasMoved)
		{
			m_cameraWasMoved = false;
			FrustumCullMap();
			FindLODs();
		}

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
		//	if (m_testMapCell)
		//	{
		//		DrawMapCell(m_testMapCell);
		//	}

		/*	for (u32 i = 0; i < m_visibleMapCells.m_size; ++i)
			{
				DrawMapCell(m_visibleMapCells.m_data[i]);
			}*/

			m_gpu->DrawLine3D(v4f(1.f, 0.f, 0.f, 0.f), v4f(0.f, 0.f, 0.f, 0.f), ColorRed);
			m_gpu->DrawLine3D(v4f(0.f, 0.f, 1.f, 0.f), v4f(0.f, 0.f, 0.f, 0.f), ColorLime);

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

bool Application::OpenMap()
{
	if (std::filesystem::exists("../data/world.dat"))
	{
		ReadWorld();
	}
	else
	{
		GenerateWorld();
	}

	return true;
}

void Application::GenerateWorld()
{
	//miImage* heightMap = miLoadImage(L"../data/world/heightmap.png");
	//if (!heightMap)
	//	return;

	///*m_testMapCell = new MapCell;
	//m_testMapCell->Generate();*/
	//
	//// Создать ячейки. Записать их в файл.
	//// m_mapCells будет хранить указатели на все ячейки.

	//// 10 = 1km
	//// 100 = 10km
	//u32 cellsNumX = 100;
	//u32 cellsNumY = 100;

	//f32 cellSize = 0.01f;// 0.01f = 100m

	//f32 mapSizeX = (f32)cellsNumX * cellSize; 
	//f32 mapSizeY = (f32)cellsNumY * cellSize;

	//f32 mapSizeHalfX = mapSizeX * 0.5f;
	//f32 mapSizeHalfY = mapSizeY * 0.5f;

	//v3f position;
	//position.x = -mapSizeHalfX;
	//position.z = -mapSizeHalfY;

	//std::map<std::string, std::pair<v3f, u32>> vMap;

	//for (u32 iy = 0; iy < cellsNumY; ++iy)
	//{
	//	for (u32 ix = 0; ix < cellsNumX; ++ix)
	//	{
	//		printf("%u %u\n", iy, ix);

	//		MapCell* newCell = new MapCell;
	//		newCell->m_position = position;
	//		newCell->Generate(ix, iy, vMap, heightMap, mapSizeX, mapSizeY);

	//		m_mapCells.push_back(newCell);
	//		newCell->m_id = m_mapCells.size();

	//		//////
	//		//newCell->WriteToFile(ix, iy);
	//		newCell->DeleteCPUMesh();

	//		position.x += cellSize;
	//	}
	//
	//	position.x = -mapSizeHalfX;
	//	position.z += cellSize;
	//}
	//printf("MAP SIZE: %u\n", vMap.size());

	//std::vector<v3f> vArray;
	//MapToVec(vMap, vArray);
	//
	//{
	//	miStringA str;
	//	str += "../data/world/vertices.bin";

	//	FILE* f = fopen(str.data(), "wb");

	//	if (vArray.size())
	//	{
	//		u32 uncompSize = vArray.size() * sizeof(v3f);
	//	//	fwrite(vArray.data(), vArray.size() * sizeof(v3f), 1, f);
	//		auto compressBound = miGetCompressBound(uncompSize);
	//		u8* compBuf = (u8*)malloc(compressBound);
	//		u32 srcSize = uncompSize;
	//		u32 compSize = compressBound;
	//		miCompress(vArray.data(), srcSize, compBuf, &compSize, miCompressAlgorithm::Deflate, miCompressStrategy::Default);
	//		fwrite(compBuf, compSize, 1, f);
	//		free(compBuf);
	//	}

	//	fclose(f);
	//}

	//miDestroy(heightMap);
}

void Application::ReadWorld()
{

}

void Application::DrawMapCell(MapCell* cell)
{
	//Mat4 W;
	//W.setTranslation(cell->m_position);

	//miSetMatrix(miMatrixType::World, &W);

	//Mat4 WVP = m_activeCamera->m_projection * m_activeCamera->m_view * W;
	//miSetMatrix(miMatrixType::WorldViewProjection, &WVP);
	//m_gpu->SetTexture(0, miGetBlackTexture());
	//m_gpu->SetMesh(cell->m_meshGPU0[cell->m_activeLOD[0]]);
	//m_gpu->Draw();
	//m_gpu->SetMesh(cell->m_meshGPU1[cell->m_activeLOD[1]]);
	//m_gpu->Draw();
	//m_gpu->SetMesh(cell->m_meshGPU2[cell->m_activeLOD[2]]);
	//m_gpu->Draw();
	//m_gpu->SetMesh(cell->m_meshGPU3[cell->m_activeLOD[3]]);
	//m_gpu->Draw();
}

void Application::FindLODs()
{
	//for (u32 i = 0; i < m_visibleMapCells.m_size; ++i)
	//{
	//	auto cell = m_visibleMapCells.m_data[i];
	//	
	//	for (u32 k = 0; k < 4; ++k)
	//	{
	//		f32 d = cell->m_data->m_positionInWorld[k].distance(m_activeCamera->m_localPosition);

	//		if (d < g_terrainLODDistance_0)        cell->m_data->m_activeLOD[k] = 0;
	//		else if (d < g_terrainLODDistance_1)   cell->m_data->m_activeLOD[k] = 1;
	//		//else if (d < g_terrainLODDistance_2)   cell->m_activeLOD[k] = 2;
	//		//else if (d < g_terrainLODDistance_3)   cell->m_activeLOD[k] = 3;
	//		//else if (d < g_terrainLODDistance_4)   cell->m_activeLOD[k] = 4;
	//		else cell->m_data->m_activeLOD[k] = 2;
	//	}
	//}
}

void Application::FrustumCullMap()
{
	//m_visibleMapCells.clear();
	//
	//for (u32 i = 0; i < m_mapCells.m_size; ++i)
	//{
	//	////m_mapCells.m_data[i]->m_activeLOD = 0;
	//	//if (m_inputContext->IsKeyHold(miKey::K_1))
	//	//	m_mapCells.m_data[i]->m_activeLOD_0 = 1;
	//	//if (m_inputContext->IsKeyHold(miKey::K_2))
	//	//	m_mapCells.m_data[i]->m_activeLOD_0 = 2;
	//	//if (m_inputContext->IsKeyHold(miKey::K_3))
	//	//	m_mapCells.m_data[i]->m_activeLOD_0 = 3;
	//	//if (m_inputContext->IsKeyHold(miKey::K_4))
	//	//	m_mapCells.m_data[i]->m_activeLOD_0 = 4;
	//	//if (m_inputContext->IsKeyHold(miKey::K_5))
	//	//	m_mapCells.m_data[i]->m_activeLOD_0 = 5;
	//	//if (m_inputContext->IsKeyHold(miKey::K_0))
	//	//	m_mapCells.m_data[i]->m_activeLOD_0 = 0;
	//	//
	//	//m_mapCells.m_data[i]->m_activeLOD_1 = m_mapCells.m_data[i]->m_activeLOD_0;
	//	//m_mapCells.m_data[i]->m_activeLOD_2 = m_mapCells.m_data[i]->m_activeLOD_0;
	//	//m_mapCells.m_data[i]->m_activeLOD_3 = m_mapCells.m_data[i]->m_activeLOD_0;

	//	if (m_activeCamera->m_frust.AABBInFrustum(m_mapCells.m_data[i]->m_data->m_aabb, m_mapCells.m_data[i]->m_data->m_position))
	//	{
	//		m_mapCells.m_data[i]->m_data->m_inView = true;
	//		m_visibleMapCells.push_back(m_mapCells.m_data[i]);
	//	}
	//	else
	//	{
	//		m_mapCells.m_data[i]->m_data->m_inView = false;
	//	}
	//}
}

//void Application::FindCurrentCellID()
//{
//	m_player->m_cellID = -1;
//	//for (u32 i = 0; i < m_mapCells.m_size; ++i)
//	//{
//	//	if (m_activeCamera->m_localPosition.x < m_mapCells.m_data[i]->m_data->m_aabbTransformed.m_min.x)
//	//		continue;
//	//	if (m_activeCamera->m_localPosition.x > m_mapCells.m_data[i]->m_data->m_aabbTransformed.m_max.x)
//	//		continue;
//	//	if (m_activeCamera->m_localPosition.z < m_mapCells.m_data[i]->m_data->m_aabbTransformed.m_min.z)
//	//		continue;
//	//	if (m_activeCamera->m_localPosition.z > m_mapCells.m_data[i]->m_data->m_aabbTransformed.m_max.z)
//	//		continue;
//
//	//	/*if (m_activeCamera->m_localPosition.x >= m_mapCells.m_data[i]->m_aabbTransformed.m_min.x
//	//		&& m_activeCamera->m_localPosition.x <= m_mapCells.m_data[i]->m_aabbTransformed.m_max.x
//	//		&& m_activeCamera->m_localPosition.z >= m_mapCells.m_data[i]->m_aabbTransformed.m_min.z
//	//		&& m_activeCamera->m_localPosition.z <= m_mapCells.m_data[i]->m_aabbTransformed.m_max.z)
//	//	{
//	//		m_player->m_cellID = (s32)m_mapCells.m_data[i]->m_id;
//	//		return;
//	//	}*/
//	//	m_player->m_cellID = (s32)m_mapCells.m_data[i]->m_data->m_id;
//	//	return;
//	//}
//}

void Application::_updateMapCell()
{
	m_needUpdateMapCell = false;

	// 1.f = 1km
	f64 map_size_x = 1000.0;
	f64 map_size_y = 1000.0;

	f64 map_half_size_x = map_size_x * 0.5;
	f64 map_half_size_y = map_size_y * 0.5;

	f64 pos_x = (f64)m_player->m_position.x + map_half_size_x;
	f64 pos_y = (f64)m_player->m_position.z + map_half_size_y;

	s32 x = 0;
	s32 y = 0;

	if (pos_x > 0.0)
		x = (s32)std::ceil(pos_x);

	if (pos_y > 0.0)
		y = (s32)std::ceil(pos_y);

	s32 real_x = x;
	s32 real_y = y;

	if (x > 999) x = 999;
	if (y > 999) y = 999;
	if (x < 0) x = 0;
	if (y < 0) y = 0;

	m_player->m_cellID = (y * 1000) + x;

	static s32 prev_ID = -1;

	if (m_player->m_cellID != prev_ID)
	{
		/*
		* [3][4][5]
		* [1][0][2]
		* [6][7][8]
		*/
		s32 newIDs[9];

		fseek(m_file_ids, m_player->m_cellID * (9*sizeof(s32)), SEEK_SET);
		fread(&newIDs[0], sizeof(s32) * 9, 1, m_file_ids);
		/*printf("%i %i %i\n%i %i %i\n%i %i %i\n\n",
			newIDs[5], newIDs[4], newIDs[3],
			newIDs[2], newIDs[0], newIDs[1],
			newIDs[8], newIDs[7], newIDs[6]);*/

		/*
		* Сначала прохожусь по массиву с ячейками.
		*  Беру m_id и если он не равен -1 проверяю с массивом newIDs
		*  Если не найдено значит нужно очистить ячейку и сделать m_id = -1
		* Далее прохожусь по newIDs.
		*  Беру значение, если оно не -1 то прохожусь по массиву с ячейками.
		*   Беру ячейку, если m_id равен -1 то инициализирую эту ячейку новыми
		*    данными и присваиваю новый m_id
		*/
		for (s32 i = 0; i < 9; ++i)
		{
			if (m_mapCells.m_data[i]->m_id != -1)
			{
				bool found = false;
				for (s32 i2 = 0; i2 < 9; ++i2)
				{
					if (newIDs[i2] == m_mapCells.m_data[i]->m_id)
					{
						found = true;
						break;
					}
				}
				if(!found)
					m_mapCells.m_data[i]->Clear();
			}
		}
		
		for (s32 i = 0; i < 9; ++i)
		{

		}

		prev_ID = m_player->m_cellID;
	}
}