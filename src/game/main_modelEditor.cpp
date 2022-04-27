#include "framework/mgf.h"
#include "framework/SystemWindow.h"
#include "framework/BackendGDI.h"
#include "framework/GSD3D11.h"
#include "framework/Window.h"
#include "framework/Rectangle.h"

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
	
	virtual void OnSize();
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
	UseMenu(true, app->m_menuFont);
	BeginMenu(L"File");
	{
		AddMenuItem(0, 0);
		AddMenuItem(L"Exit", WindowMainMenu::MenuItemID_File_Exit);
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

	m_windowMenu = new WindowMainMenu(this);

	m_windowMain->OnSize();

	mgPoint renderRectSize(m_windowMain->GetSize().x, 400);
	m_renderRect = new mgf::Rectangle(m_windowMenu);
	m_renderRect->SetPositionAndSize(0, 0,
		renderRectSize.x, renderRectSize.y);
	
	mgf::GSD3D11* gsd3d11 = new mgf::GSD3D11();
	m_gs = gsd3d11;
	if (!gsd3d11->Init(m_windowMain, 0))
	{
		return false;
	}

	mgf::GSTextureInfo fboinfo;
	fboinfo.m_filter = mgf::GSTextureFilter::PPP;
	m_renderTexture = gsd3d11->CreateRenderTargetTexture(renderRectSize.x, renderRectSize.y, &fboinfo);
	gsd3d11->SetRenderTarget(m_renderTexture);
	gsd3d11->SetViewport(0, 0, renderRectSize.x, renderRectSize.y, 0, 0);

	float cclr[4] = { 1.f, 0.4f, 0.4f, 1.f };
	m_gs->SetClearColor(cclr);
	m_gs->ClearAll();
	
	m_GDIRenderTextureImage = new mgf::Image;
	mgColor c;
	c.r = c.g = c.b = c.a = 1.f;
	m_GDIRenderTextureImage->Create(renderRectSize.x, renderRectSize.y, c);

	gsd3d11->GetTextureCopyForImage(m_renderTexture, m_GDIRenderTextureImage);
	
	m_GDIRenderTexture = m_backend->CreateTexture(m_GDIRenderTextureImage->GetMGImage());
	m_renderRect->SetTexture(m_GDIRenderTexture);


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

		m_gs->ClearAll();

		m_gs->GetTextureCopyForImage(m_renderTexture, m_GDIRenderTextureImage);
		m_backend->UpdateTexture(m_GDIRenderTexture, m_GDIRenderTextureImage->GetMGImage());
		
		m_framework->DrawAll();
	}
}

void WindowMain::OnSize()
{
	SystemWindow::OnSize();

	if(m_app->m_windowMenu)
		m_app->m_windowMenu->SetSize(GetSize().x, GetSize().y);
}