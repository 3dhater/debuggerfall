#include "framework/mgf.h"
#include "framework/SystemWindow.h"
#include "framework/BackendGDI.h"
#include "framework/GSD3D11.h"
#include "framework/Window.h"
#include "framework/Rectangle.h"
#include "framework/Camera.h"
#include "framework/Log.h"
#include <filesystem>
#include <stdio.h>

// there is 3 types of menu 
// (1)native menu, (2)migui with system window, and (3)migui without system window
// comment all for 3
//#define DEMO_NATIVE_WIN32MENU
#define DEMO_SYSTEM_POPUP_MENU

// And also 2 types of how to draw Graphics API things:
//  1 - native Windows window
//  2 - using render target texture. This type is more complex because
//       this app use GDI. We need to get image from GPU texture, then
//       put this data in BackendGDI texture.

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

class MyShader : public mgf::GSD3D11Shader
{
public:
	MyShader() {}
	virtual ~MyShader()
	{
		if (m_cbVertex) m_cbVertex->Release();
		if (m_cbPixel) m_cbPixel->Release();
	}

	ID3D11Buffer* m_cbVertex = 0;
	ID3D11Buffer* m_cbPixel = 0;

	struct cbVertex
	{
		mgf::Mat4 WVP;
		mgf::Mat4 W;
	}m_cbVertexData;

	struct cbPixel
	{
		mgf::Color BaseColor;
		mgf::v4f SunPosition;
	}m_cbPixelData;

	virtual bool OnCreate(void* _data) override
	{
		mgf::GSData_D3D11* data = (mgf::GSData_D3D11*)_data;

		if (!mgf::GSD3D11_createConstantBuffer(data->m_d3d11Device, sizeof(cbVertex), &m_cbVertex))
		{
			mgf::LogWriteError("%s: cant create constant buffer\n", MGF_FUNCTION);
			return false;
		}

		if (!mgf::GSD3D11_createConstantBuffer(data->m_d3d11Device, sizeof(cbPixel), &m_cbPixel))
		{
			mgf::LogWriteError("%s: cant create constant buffer\n", MGF_FUNCTION);
			return false;
		}
		return true;
	}

	virtual void SetConstants(void* _data) override
	{
		mgf::GSData_D3D11* data = (mgf::GSData_D3D11*)_data;

		m_cbVertexData.WVP = data->m_gs->m_matrices[mgf::GS::MatrixType_WorldViewProjection];
		m_cbVertexData.W = data->m_gs->m_matrices[mgf::GS::MatrixType_World];
		m_cbPixelData.BaseColor.set(1.f, 1.f, 1.f);
		m_cbPixelData.SunPosition.set(3.f, 10.f, 0.1f, 1.f);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		D3D11_BUFFER_DESC d;
		data->m_d3d11DevCon->Map(m_cbVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		m_cbVertex->GetDesc(&d);
		memcpy(mappedResource.pData, &m_cbVertexData, d.ByteWidth);
		data->m_d3d11DevCon->Unmap(m_cbVertex, 0);
		data->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbVertex);

		data->m_d3d11DevCon->Map(m_cbPixel, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		m_cbPixel->GetDesc(&d);
		memcpy(mappedResource.pData, &m_cbPixelData, d.ByteWidth);
		data->m_d3d11DevCon->Unmap(m_cbPixel, 0);
		data->m_d3d11DevCon->PSSetConstantBuffers(0, 1, &m_cbPixel);

		if (data->m_gs->m_currentTextures[0])
		{
			mgf::GSD3D11Texture* currTex = (mgf::GSD3D11Texture*)data->m_gs->m_currentTextures[0];
			data->m_d3d11DevCon->PSSetShaderResources(0, 1, &currTex->m_textureResView);
			data->m_d3d11DevCon->PSSetSamplers(0, 1, &currTex->m_samplerState);
		}
	}
};

class EditorModel;
class MyMeshLoader : public mgf::MeshLoader
{
public:
	MyMeshLoader(EditorModel* editorModel) : m_editorModel(editorModel) {}
	virtual ~MyMeshLoader() {}

	virtual void OnLoad(mgf::MeshBuilder* meshBuilder, mgf::Material* mat) override;

	EditorModel* m_editorModel = 0;
};

class EditorModelPart
{
public:
	EditorModelPart() {}
	~EditorModelPart() 
	{
		if (m_meshCPU)
			delete m_meshCPU;
		if (m_meshGPU)
			m_meshGPU->Release();
	}

	mgf::Mesh* m_meshCPU = 0;
	mgf::GSMesh* m_meshGPU = 0;
	mgf::GSMeshInfo m_meshInfo;
};

class EditorModel
{
	mgf::GS* m_gs = 0;
	MyMeshLoader* m_loader = 0;
public:
	EditorModel(mgf::GS* gs)
		:
		m_gs(gs)
	{
		m_loader = new MyMeshLoader(this);
	}
	~EditorModel() 
	{
		for (size_t i = 0; i < m_parts.size(); ++i)
		{
			delete m_parts[i];
		}
		if (m_loader) delete m_loader;
	}

	std::vector<EditorModelPart*> m_parts;
	void AddMesh(mgf::Mesh* m)
	{
		if (m)
		{
			mgf::GSMeshInfo mi;
			mi.m_meshPtr = m;
			mgf::GSMesh* meshGPU = m_gs->CreateMesh(&mi);
			if (meshGPU)
			{
				EditorModelPart* newPart = new EditorModelPart;
				newPart->m_meshCPU = m;
				newPart->m_meshGPU = meshGPU;
				newPart->m_meshInfo = mi;
				m_parts.push_back(newPart);
			}
		}
	}

	void Load(const wchar_t* fn)
	{
		m_loader->LoadMesh(fn, 0);
	}
};



class ModelEditor
{
public:
	ModelEditor();
	~ModelEditor();

	mgf::Framework* m_framework = 0;
	WindowMain* m_windowMain = 0;
	WindowMainMenu* m_windowMenu = 0;

	EditorModel* m_model = 0;
	mgf::GSTexture* m_generatedTexture = 0;
	MyShader* m_myShader = 0;

	mgf::GS* m_gs = 0;
	mgf::Backend* m_backend = 0;
	mgf::Context* m_GUIContext = 0;
	mgf::Font* m_menuFont = 0;
	mgf::Rectangle* m_renderRect = 0;

	mgf::Image* m_GDIRenderTextureImage = 0;
	mgTexture* m_GDIRenderTexture = 0;
	mgf::GSTexture* m_renderTexture = 0;

	mgColor m_colorRed;
	mgColor m_colorGreen;
	mgColor m_colorBlue;

	mgPoint m_renderRectSize;

	mgf::Camera m_camera;

	bool Init();
	void Run();
	void Quit();
};

ModelEditor::ModelEditor()
{

}

ModelEditor::~ModelEditor()
{
	if (m_model) delete m_model;
	if (m_generatedTexture) m_generatedTexture->Release();
	if (m_myShader) m_myShader->Release();
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
	if (std::filesystem::exists("..\\data\\fonts\\lt_internet\\LTInternet-Regular.ttf"))
		m_menuFont = m_backend->CreateFontPrivate(L"..\\data\\fonts\\lt_internet\\LTInternet-Regular.ttf", 11, false, false, L"LT Internet");
	else
		m_menuFont = m_backend->CreateFont(L"Arial", 11, false, false);

	m_windowMain->OnSize();

	m_renderRectSize.x = m_windowMain->GetSize().x;
	m_renderRectSize.y = 400;

	if (m_renderRectSize.x < 100) // ??. who knows maybe m_windowMain->GetSize().x is bad  
		m_renderRectSize.x = 100;
	

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

	m_camera.Reset(); // make default rotations for EditorCamera
	m_camera.m_aspect = m_renderRectSize.x / m_renderRectSize.y; // set by hands
	for(int i = 0; i < 20; ++i)
		m_camera.EditorZoom(1); // zoom for mouse wheel, so simulate it
	m_camera.EditorUpdate(); // and first update. next update only when move\rotate camera

	{
		mgf::Image img;
		img.Create(32, 32, m_framework->GetColor(mgf::ColorName::Red));
		img.Fill(img.fillType_checkers, m_framework->GetColor(mgf::ColorName::Red), m_framework->GetColor(mgf::ColorName::Blue));
		mgf::GSTextureInfo ti;
		ti.SetImage(&img);
		m_generatedTexture = m_gs->CreateTexture(&ti);
	}

	{
		m_myShader = new MyShader;
		mgf::GSShaderInfo si;
		si.vsEntry = "VSMain";
		si.psEntry = "PSMain";
		si.vsText = "Texture2D tex2d_1;\n"
			"SamplerState tex2D_sampler_1;\n"
			"struct VSIn{\n"
			"   float3 position : POSITION;\n"
			"	float2 uv : TEXCOORD;\n"
			"   float3 normal : NORMAL;\n"
			"   float3 binormal : BINORMAL;\n"
			"   float3 tangent : TANGENT;\n"
			"   float4 color : COLOR;\n"
			"};\n"
			"cbuffer cbVertex{\n"
			"	float4x4 WVP;\n"
			"	float4x4 W;\n"
			"};\n"
			"cbuffer cbPixel{\n"
			"	float4 BaseColor;\n"
			"	float4 SunPosition;\n"
			"};\n"
			"struct VSOut{\n"
			"   float4 pos : SV_POSITION;\n"
			"	float2 uv : TEXCOORD0;\n"
			"	float4 vColor : COLOR0;\n"
			"	float3 normal : NORMAL0;\n"
			"	float4 fragPos : NORMAL1;\n"
			"};\n"
			"struct PSOut{\n"
			"    float4 color : SV_Target;\n"
			"};\n"
			"VSOut VSMain(VSIn input){\n"
			"   VSOut output;\n"
			"	output.pos   = mul(WVP, float4(input.position.x, input.position.y, input.position.z, 1.f));\n"
			"	output.uv.x    = input.uv.x;\n"
			"	output.uv.y    = input.uv.y;\n"
			"	output.vColor    = input.color;\n"
			"	output.normal    = normalize(mul((float3x3)W, input.normal));\n"
			"	output.fragPos    = mul(W, float4(input.position.x, input.position.y, input.position.z, 1.f));\n"
			"	return output;\n"
			"}\n"
			"PSOut PSMain(VSOut input){\n"
			"	float3 lightDir = normalize(SunPosition.xyz - input.fragPos.xyz);\n"
			"	float diff = max(dot(input.normal, lightDir), 0.0);\n"

			"   PSOut output;\n"
			"   output.color = tex2d_1.Sample(tex2D_sampler_1, input.uv) * BaseColor;\n"
			"	if(diff>1.f) diff = 1.f;\n"
			"	output.color.xyz *= diff;\n"
			"    return output;\n"
			"}\n";
		si.psText = si.vsText;
		m_gs->CreateShader(m_myShader, &si);
	}

	m_model = new EditorModel(m_gs);

	return true;
}

void ModelEditor::Run()
{
	m_colorRed.setAsIntegerRGB(0xFF0000);
	m_colorGreen.setAsIntegerRGB(0xFF00);
	m_colorBlue.setAsIntegerRGB(0xFF);

	mgPoint cameraRotation(1, 0);
	float* dt = m_GUIContext->GetDeltaTime();

	while (m_framework->Run())
	{
		m_framework->DrawAll();

#ifdef DEMO_NATIVE_WIN32MENU
		m_gs->BeginDraw();
		m_gs->ClearAll();
		m_gs->DrawLine3D(mgf::v4f(-1.f, 0.f, 0.f, 0.f), mgf::v4f(1.f, 0.f, 0.f, 0.f), m_colorRed);
		m_gs->DrawLine3D(mgf::v4f(0.f, -1.f, 0.f, 0.f), mgf::v4f(0.f, 1.f, 0.f, 0.f), m_colorGreen);
		m_gs->DrawLine3D(mgf::v4f(0.f, 0.f, -1.f, 0.f), mgf::v4f(0.f, 0.f, 1.f, 0.f), m_colorBlue);
		m_gs->EndDraw();
		m_gs->SwapBuffers();
#else
		m_gs->ClearAll();
		m_gs->DrawLine3D(mgf::v4f(-1.f, 0.f, 0.f, 0.f), mgf::v4f(1.f, 0.f, 0.f, 0.f), m_colorRed);
		m_gs->DrawLine3D(mgf::v4f(0.f, -1.f, 0.f, 0.f), mgf::v4f(0.f, 1.f, 0.f, 0.f), m_colorGreen);
		m_gs->DrawLine3D(mgf::v4f(0.f, 0.f, -1.f, 0.f), mgf::v4f(0.f, 0.f, 1.f, 0.f), m_colorBlue);
		
		if (m_model)
		{
			m_gs->SetTexture(0, m_generatedTexture);
			for (size_t i = 0, sz = m_model->m_parts.size(); i < sz; ++i)
			{
				if (m_model->m_parts[i]->m_meshGPU)
				{
					m_gs->SetMesh(m_model->m_parts[i]->m_meshGPU);
					
					m_gs->SetShader(m_myShader);
				}
			}
			mgf::Mat4 World;
			m_gs->m_matrices[mgf::GS::MatrixType_World] = World;
			m_gs->m_matrices[mgf::GS::MatrixType_WorldViewProjection] = m_camera.m_projectionMatrix * m_camera.m_viewMatrix * World;
			m_gs->Draw(0);
		}

		m_gs->GetTextureCopyForImage(m_renderTexture, m_GDIRenderTextureImage);
		m_backend->UpdateTexture(m_GDIRenderTexture, m_GDIRenderTextureImage->GetMGImage());
#endif

		m_camera.EditorRotate(&cameraRotation, *dt);
		m_camera.EditorUpdate();
		m_gs->m_matrices[mgf::GS::MatrixType_ViewProjection] = m_camera.m_viewProjectionMatrix;
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

void MyMeshLoader::OnLoad(mgf::MeshBuilder* meshBuilder, mgf::Material* mat)
{
	m_editorModel->AddMesh(meshBuilder->CreateMesh(0));
}
