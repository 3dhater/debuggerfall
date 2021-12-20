#include "mi/MainSystem/MainSystem.h"
#include "mi/Mesh/mesh.h"
#include "mi/GraphicsSystem/GPUMesh.h"

#include "miGUI.h"

#include "application.h"
#include "Player.h"
#include "GUI.h"

extern Application* g_app;

void gui_beginDraw();

ApplicationGUI::ApplicationGUI()
{
	//m_fontDefault = miGUILoadFont(L"../res/fonts/Noto/notosans.txt");
	//m_context = miGUICreateContext(g_app->m_mainWindow);
}

ApplicationGUI::~ApplicationGUI()
{
	if (m_guiContext)
		mgDestroyContext(m_guiContext);

	if (m_guiGPU)
		delete m_guiGPU;

	//if (m_panel_debug)m_context->DeleteElement(m_panel_debug);
	//if (m_panel_terrain) m_context->DeleteElement(m_panel_terrain);
	//if (m_context) miGUIDestroyContext(m_context);
}

void ApplicationGUI::Init(mgInputContext_s* ic)
{
	m_guiGPU = new mgVideoDriverAPI;
	
	m_guiGPU->beginDraw = gui_beginDraw;

	m_guiContext = mgCreateContext(m_guiGPU, ic);

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


void gui_beginDraw()
{
	
}