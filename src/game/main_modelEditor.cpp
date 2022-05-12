#include "framework/mgf.h"
#include "framework/SystemWindow.h"
#include "framework/BackendGDI.h"
#include "framework/GSD3D11.h"
#include "framework/Window.h"
#include "framework/Rectangle.h"

// there is 3 types of menu 
// (1)native menu, (2)migui with system window, and (3)migui without system window
// comment all for 3
//#define DEMO_NATIVE_WIN32MENU
#define DEMO_SYSTEM_POPUP_MENU

#ifdef DEMO_NATIVE_WIN32MENU
#include <Windows.h>
HWND d3dwindow = 0;
//HWND menuwindow = 0;
LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#define IDM_FILE_NEW 1
#define IDM_FILE_OPEN 2
#define IDM_FILE_QUIT 3
#endif

// I think this is unnecessary. 
//#if defined(DEMO_NATIVE_WIN32MENU) && defined(DEMO_SYSTEM_POPUP_MENU)
//#undef DEMO_SYSTEM_POPUP_MENU
//#endif

class ModelEditor;

class WindowMain : public mgf::SystemWindow
{
	bool m_isMaximized = false;
	uint32_t m_cursorInSystemButtons = 0;
	uint32_t m_pushedSystemButton = 0;

	ModelEditor* m_app = 0;
public:
	WindowMain(ModelEditor*, int windowFlags, const mgPoint& windowPosition, const mgPoint& windowSize);
	virtual ~WindowMain();
	
	virtual void OnSize() override;
	virtual void OnMove() override;
};

WindowMain::WindowMain(
	ModelEditor* app,
	int windowFlags,
	const mgPoint& windowPosition,
	const mgPoint& windowSize)
	:
	mgf::SystemWindow(windowFlags, windowPosition, windowSize)
{
	m_app = app;

}

WindowMain::~WindowMain()
{

}

class WindowMainMenu : public mgf::Window
{
	ModelEditor* m_app;
public:
	WindowMainMenu(ModelEditor*);
	virtual ~WindowMainMenu();

	virtual void OnMenuCommand(int id) override;
	//virtual bool OnIsMenuItemEnabled(int id, bool prev) override;
	//virtual bool OnIsMenuEnabled(int id, bool prev) override;
	//virtual bool OnIsMenuChecked(int id, bool prev) override;
	//virtual bool OnIsMenuAsRadio(int id) override;
	//virtual bool OnIcon(int id, mgf::Icons** icons, int* iconID, mgColor* color) override;

	enum
	{
		MenuItemID_File_Exit = 1,
	};
};

class ModelEditor
{
public:
	ModelEditor();
	~ModelEditor();

	mgf::Framework* m_framework = 0;
	WindowMain* m_windowMain = 0;
	WindowMainMenu* m_windowMenu = 0;

	mgf::GS* m_gs = 0;
	mgf::Backend* m_backend = 0;
	mgf::Context* m_GUIContext = 0;
	mgf::Font* m_menuFont = 0;
	mgf::Rectangle* m_renderRect = 0;

	mgf::Image* m_GDIRenderTextureImage = 0;
	mgTexture* m_GDIRenderTexture = 0;
	mgf::GSTexture* m_renderTexture = 0;

	mgPoint m_renderRectSize;

	bool Init();
	void Run();
	void Quit();
};

ModelEditor::ModelEditor()
{

}

ModelEditor::~ModelEditor()
{
	if (m_renderTexture) m_renderTexture->Release();
	if (m_gs) m_gs->Release();
	if (m_windowMenu) m_windowMenu->Release();

	if (m_GDIRenderTextureImage) m_GDIRenderTextureImage->Release();
	if (m_GDIRenderTexture && m_backend) m_backend->DestroyTexture(m_GDIRenderTexture);

	if (m_backend)
	{
	}
	if (m_GUIContext) m_GUIContext->Release();
	if (m_windowMain) m_windowMain->Release();
	if (m_backend) m_backend->Release();
	if (m_framework) m_framework->Release();
}

void ModelEditor::Quit()
{
	m_framework->m_run = false;
}

WindowMainMenu::WindowMainMenu(ModelEditor* app)
	:
	mgf::Window::Window(app->m_GUIContext),
	m_app(app)
{
	this->SetCanDock(false);
	this->SetCanMove(false);
	this->SetCanResize(false);
	this->SetCanToTop(false);
	//this->SetDrawBG(false);
	this->SetWithCloseButton(false);
	this->SetWithCollapseButton(false);
	this->SetWithTitlebar(false);

	this->SetNoMenuBG(false);

	bool useSystemWindowMenu = false;
#ifdef DEMO_SYSTEM_POPUP_MENU
	useSystemWindowMenu = true;
#endif

	UseMenu(true, useSystemWindowMenu, app->m_menuFont);
	BeginMenu(L"File");
	{
#ifndef DEMO_NATIVE_WIN32MENU
		AddMenuItem(0, 0);
		AddMenuItem(L"Exit", WindowMainMenu::MenuItemID_File_Exit);
#endif
		EndMenu();
	}
	BeginMenu(L"View");
	{
#ifndef DEMO_NATIVE_WIN32MENU
		AddMenuItem(0, 0);
		AddMenuItem(L"Reset camera", WindowMainMenu::MenuItemID_File_Exit);
#endif
		EndMenu();
	}
	RebuildMenu();
}

WindowMainMenu::~WindowMainMenu()
{

}

void WindowMainMenu::OnMenuCommand(int id)
{
	switch (id)
	{
	case WindowMainMenu::MenuItemID_File_Exit:
		m_app->Quit();
		break;
	}
}

int main_modelEditor()
{
	ModelEditor* app = new ModelEditor;
	if (app->Init())
		app->Run();

	if (app)
		delete app;

	return 1;
}

bool ModelEditor::Init()
{
	m_framework = mgf::InitFramework();
	m_windowMain = new WindowMain(this,
		MGWS_SYSMENU | MGWS_CAPTION,
		mgPoint(MGCW_USEDEFAULT, 0),
		mgPoint(500, 700));

	m_windowMain->SetVisible(true);
	m_windowMain->Rebuild();
	
	m_backend = new mgf::BackendGDI();
	m_GUIContext = m_framework->CreateContext(m_windowMain, m_backend);
	m_menuFont = m_backend->CreateFontPrivate(L"..\\data\\fonts\\lt_internet\\LTInternet-Regular.ttf", 11, false, false, L"LT Internet");

	m_windowMain->OnSize();

	m_renderRectSize.x = m_windowMain->GetSize().x;
	m_renderRectSize.y = 400;
	

	mgf::GSD3D11* gsd3d11 = new mgf::GSD3D11();
	m_gs = gsd3d11;

#ifdef DEMO_NATIVE_WIN32MENU
	m_backend->SetEndDrawIndent(0, 20);

	WNDCLASSEXW wcex;
	memset(&wcex, 0, sizeof(WNDCLASSEXW));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
	wcex.lpfnWndProc = PopupWndProc;
	wcex.hInstance = GetModuleHandle(0);
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = L"popup_window";
	RegisterClassExW(&wcex);
	d3dwindow = CreateWindowExW(WS_EX_NOACTIVATE, wcex.lpszClassName, L"",
		WS_VISIBLE | WS_POPUP,
		0, 0, m_renderRectSize.x, m_renderRectSize.y, (HWND)m_windowMain->GetOSData()->hWnd, NULL, GetModuleHandle(0), NULL);
	
	HMENU hMenubar = CreateMenu();
	HMENU hMenu = CreateMenu();
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&New");
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_OPEN, L"&Open");
	AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_QUIT, L"&Quit");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&File");

	hMenu = CreateMenu();
	AppendMenuW(hMenu, MF_STRING, IDM_FILE_NEW, L"&Reset camera");
	AppendMenuW(hMenubar, MF_POPUP, (UINT_PTR)hMenu, L"&View");

	SetMenu((HWND)m_windowMain->GetOSData()->hWnd, hMenubar);

	m_windowMain->OnMove();

	static mgf::SystemWindow tmpwnd;
	mgSystemWindowOSData* wndOSData = new mgSystemWindowOSData; // delete it later
	wndOSData->hWnd = d3dwindow;
	tmpwnd.SetOSData(wndOSData);
	tmpwnd.SetSize(m_renderRectSize.x, m_renderRectSize.y);
	if (!gsd3d11->Init(&tmpwnd, 0))
	{
		return false;
	}
#else
	m_windowMenu = new WindowMainMenu(this);
	m_renderRect = new mgf::Rectangle(m_windowMenu);
	m_renderRect->SetPositionAndSize(0, 0,
		m_renderRectSize.x, m_renderRectSize.y);

	if (!gsd3d11->Init(m_windowMain, 0))
	{
		return false;
	}

	mgf::GSTextureInfo fboinfo;
	fboinfo.m_filter = mgf::GSTextureInfo::Filter_PPP;
	m_renderTexture = gsd3d11->CreateRenderTargetTexture(m_renderRectSize.x, m_renderRectSize.y, &fboinfo);
	gsd3d11->SetRenderTarget(m_renderTexture);
		
	m_GDIRenderTextureImage = new mgf::Image;
	mgColor c;
	c.r = c.g = c.b = c.a = 1.f;
	m_GDIRenderTextureImage->Create(m_renderRectSize.x, m_renderRectSize.y, c);

	gsd3d11->GetTextureCopyForImage(m_renderTexture, m_GDIRenderTextureImage);
	
	m_GDIRenderTexture = m_backend->CreateTexture(m_GDIRenderTextureImage->GetMGImage());
	m_renderRect->SetTexture(m_GDIRenderTexture);
#endif

	gsd3d11->SetViewport(0, 0, m_renderRectSize.x, m_renderRectSize.y, 0, 0);

	float cclr[4] = { 1.f, 0.40f, 0.4f, 1.f };
	m_gs->SetClearColor(cclr);
	m_gs->ClearAll();

	m_windowMain->OnSize();
	return true;
}

void ModelEditor::Run()
{
	//mgColor cc;
	//uint32_t cci = 0x000000;

	while (m_framework->Run())
	{
		//cc.setAsIntegerRGB(cci++);
		//m_gs->SetClearColor(&cc.r);
		m_framework->DrawAll();

#ifdef DEMO_NATIVE_WIN32MENU
		m_gs->BeginDraw();
		m_gs->ClearAll();
		m_gs->EndDraw();
		m_gs->SwapBuffers();
#else
		m_gs->ClearAll();

		m_gs->GetTextureCopyForImage(m_renderTexture, m_GDIRenderTextureImage);
		m_backend->UpdateTexture(m_GDIRenderTexture, m_GDIRenderTextureImage->GetMGImage());
#endif

	}
}

void WindowMain::OnSize()
{
	SystemWindow::OnSize();
	if (m_app->m_windowMenu)
		m_app->m_windowMenu->SetSize(GetSize().x, GetSize().y);
}

void WindowMain::OnMove()
{
	SystemWindow::OnMove();

#ifdef DEMO_NATIVE_WIN32MENU
	if (d3dwindow)
	{
		RECT clrct;
		GetClientRect((HWND)GetOSData()->hWnd, &clrct);
		POINT pt;
		pt.x = clrct.left;
		pt.y = clrct.top;
		ClientToScreen((HWND)GetOSData()->hWnd, &pt);
		clrct.left = pt.x;
		clrct.top = pt.y;
		pt.x = clrct.right;
		pt.y = clrct.bottom;
		ClientToScreen((HWND)GetOSData()->hWnd, &pt);
		clrct.right = pt.x;
		clrct.bottom = pt.y;

		MoveWindow(d3dwindow, clrct.left, clrct.top,
			m_app->m_renderRectSize.x,
			m_app->m_renderRectSize.y,
			FALSE);

		/*MoveWindow(menuwindow, clrct.left, clrct.top,
			m_app->m_renderRectSize.x,
			20,
			FALSE);*/
		//menuwindow
	}
#endif
}

#ifdef DEMO_NATIVE_WIN32MENU
LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif

