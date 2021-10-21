#include "mixer.lib.h"
#include "mixer.lib.inputContext.h"
#include "mixer.lib.window.h"
#include "mixer.lib.videoDriver.h"
#include "mixer.lib.event.h"
#include "application.h"

#include <filesystem>

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

Application::Application()
{
	g_app = this;
}

Application::~Application()
{
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

	m_inputContext = miCreate<miInputContext>();
	m_libContext = miCreate<miLibContext>();
	m_libContext->Start(m_inputContext);

	u32 windowFlags = 0;
	m_mainWindow = miCreateWindow(800, 600, windowFlags, 0);
	m_mainWindow->m_onClose = window_onCLose;
	m_mainWindow->m_onActivate = window_onActivate;
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
	m_cameraFly->m_localPosition.set(10.f, 5.f, 10.f, 0.f);
	{
		Mat4 lookAt;
		math::makeLookAtRHMatrix(lookAt, m_cameraFly->m_localPosition, v4f(), v4f(0.f, 1.f, 0.f, 0.f));
		m_cameraFly->m_rotationMatrix.setBasis(lookAt);
	}
	m_activeCamera = m_cameraFly;
}

void Application::MainLoop()
{
	miEvent currentEvent;
	while (miRun(&m_dt))
	{
		m_cameraFly->OnUpdate();
		if (m_inputContext->m_isRMBHold)
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

		//m_GUI->m_context->Update(m_dt);
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
					//m_GUI->m_context->NeedRebuild();
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
			m_gpu->DrawLine3D(v4f(1.f, 0.f, 0.f, 0.f), v4f(-1.f, 0.f, 0.f, 0.f), ColorRed);
			m_gpu->DrawLine3D(v4f(0.f, 0.f, 1.f, 0.f), v4f(0.f, 0.f, -1.f, 0.f), ColorLime);

			//m_gpu->BeginDrawGUI();
			//m_GUI->m_context->DrawAll();
			//m_gpu->EndDrawGUI();

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