#include "mi/MainSystem/MainSystem.h"
#include "mi/Mesh/mesh.h"
#include "mi/GraphicsSystem/util.h"

#include "miGUI.h"

#include "application.h"
#include "Player.h"
#include "GUI.h"

#include <cwchar>

extern Application* g_app;

void gui_beginDraw();
mgTexture gui_createTexture(mgImage* img);
void gui_destroyTexture(mgTexture t);
void gui_endDraw();
void gui_drawRectangle(mgElement* element, mgPoint* position, mgPoint* size, mgColor* color1,
	mgColor* color2, mgTexture texture, mgVec4* UVRegion);
void gui_drawText(
	mgPoint* position,
	const wchar_t* text,
	int textLen,
	mgColor* color,
	mgFont* font);
void gui_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz);
mgRect gui_setClipRect(mgRect* r);

//class GUIShader : public miGPUShaderCallback
//{
//public:
//	GUIShader() {}
//	virtual ~GUIShader() {
//		if (m_shader)
//			m_shader->Release();
//	}
//
//	struct cbVertex
//	{
//		Mat4 m_ProjMtx;
//		mgVec4 m_Corners;
//		mgColor m_Color1;
//		mgColor m_Color2;
//		mgVec4 m_UVs;
//	}
//	m_cbVertexData;
//
//	miGPUShader* m_shader = 0;
//
//	void* m_cbVertex = 0;
//
//	virtual void OnSetConstants(miGraphicsSystem* gs, miGSDrawCommand* cmd) override
//	{
//		/*cmd->m_shader->SetTexture(cmd->m_material->m_maps[0].m_GPUTexture, 0);
//
//		m_cbVertexData.W = *cmd->m_matWorld;
//		m_cbVertexData.WVP = *cmd->m_matWVP;
//
//		cmd->m_shader->MapConstantBuffer(m_cbVertex, &m_cbVertexData);
//		cmd->m_shader->SetConstantBuffer(m_cbVertex, 0, 0);*/
//	};
//};

ApplicationGUI::ApplicationGUI(mgInputContext_s* ic)
{
	m_guiGPU = new mgVideoDriverAPI;
	m_guiGPU->createTexture = gui_createTexture;
	m_guiGPU->destroyTexture = gui_destroyTexture;
	m_guiGPU->beginDraw = gui_beginDraw;
	m_guiGPU->endDraw = gui_endDraw;
	m_guiGPU->drawRectangle = gui_drawRectangle;
	m_guiGPU->drawText = gui_drawText;
	m_guiGPU->setClipRect = gui_setClipRect;

	m_guiContext = mgCreateContext(m_guiGPU, ic);
	m_guiContext->getTextSize = gui_getTextSize;

	//m_fontDefault = miGUILoadFont(L"../res/fonts/Noto/notosans.txt");
	//m_context = miGUICreateContext(g_app->m_mainWindow);
}

ApplicationGUI::~ApplicationGUI()
{
	if (m_font1)
		mgDestroyFont(m_guiContext, m_font1);

	if (m_guiContext)
		mgDestroyContext(m_guiContext);

	if (m_guiGPU)
		delete m_guiGPU;

	/*if (m_shader)
		delete m_shader;*/
	//if (m_panel_debug)m_context->DeleteElement(m_panel_debug);
	//if (m_panel_terrain) m_context->DeleteElement(m_panel_terrain);
	//if (m_context) miGUIDestroyContext(m_context);
}

void ApplicationGUI::UpdateMatrix(s32 x, s32 y)
{
	m_proj.m_data[0] = v4f(2.0f / x, 0.0f, 0.0f, 0.0f);
	m_proj.m_data[1] = v4f(0.0f, 2.0f / -y, 0.0f, 0.0f);
	m_proj.m_data[2] = v4f(0.0f, 0.0f, 0.5f, 0.0f);
	m_proj.m_data[3] = v4f(-1.f, 1.f, 0.5f, 1.0f);
}

void ApplicationGUI::Init()
{
	/*m_shader = new GUIShader;
	m_shader->m_shader = util::ShaderCreateFromTextFile(
		g_app->m_gs,
		"../data/shaders/gui.hlsl",
		"../data/shaders/gui.hlsl",
		"../data/shaders/gui.hlsl",
		"vs_5_0",
		"gs_5_0",
		"ps_5_0",
		"VMain",
		"GMain",
		"PMain",
		miMeshVertexType::Point,
		m_shader);
	m_drawCommand.m_shader = m_shader->m_shader;
	m_drawCommand.m_matProjection = &m_proj;*/
	//m_drawCommand.

	mgPoint position;

	m_font1 = mgCreateFont(m_guiContext, "Noto Sans", 0, 10, "Noto Sans");
	if (!m_font1)
		miLogWriteError("Can't create font!\n");
	
	mgPointSet(&position, 0, 0);
	m_textFPS = mgCreateText(m_guiContext, &position, L"FPS: ", m_font1);
	((mgElementText*)m_textFPS)->color.a = 1.f;

	mgPointSet(&position, 0, 16);
	m_textCELL_ID = mgCreateText(m_guiContext, &position, L"Cell ID: ", m_font1);
	((mgElementText*)m_textCELL_ID)->color.a = 1.f;

	mgPointSet(&position, 0, 32);
	m_textPOSITION = mgCreateText(m_guiContext, &position, L" ", m_font1);
	((mgElementText*)m_textCELL_ID)->color.a = 1.f;

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

void ApplicationGUI::SetTextCellID(s32 id)
{
	static wchar_t fps_buf[30];
	fps_buf[29] = 0;
	swprintf(fps_buf, L"Cell ID: %i", id);
	((mgElementText*)m_textCELL_ID->implementation)->text = fps_buf;
	((mgElementText*)m_textCELL_ID->implementation)->textLen = wcslen(fps_buf);
}

void ApplicationGUI::SetTextPosition(const v3f& pos)
{
	static wchar_t fps_buf[100];
	fps_buf[99] = 0;
	swprintf(fps_buf, L"Pos.: %f %f %f", pos.x, pos.y, pos.z);
	((mgElementText*)m_textPOSITION->implementation)->text = fps_buf;
	((mgElementText*)m_textPOSITION->implementation)->textLen = wcslen(fps_buf);
}

void ApplicationGUI::SetTextFPS(s32 fps)
{
	static wchar_t fps_buf[10];
	fps_buf[9] = 0;
	if (fps > 999 || fps < 0)
		fps = 0;
	swprintf(fps_buf, L"FPS: %i", fps);
	((mgElementText*)m_textFPS->implementation)->text = fps_buf;
	((mgElementText*)m_textFPS->implementation)->textLen = wcslen(fps_buf);
}

bool g_gui_oldDepth = false;
void gui_beginDraw()
{
	g_gui_oldDepth = g_app->m_gs->UseDepth(false);
}

mgTexture gui_createTexture(mgImage* img)
{
	mgTexture t;
	
	miImage i;
	i.m_data = img->data;
	i.m_dataSize = img->dataSize;
	i.m_width = img->width;
	i.m_height = img->height;
	i.m_pitch = img->width * 4;

	miGPUTextureInfo ti;
	ti.m_imagePtr = &i;
	ti.m_filter = miTextureFilter::PPP;

	auto gpu_t = g_app->m_gs->CreateTexture(&ti);

	i.m_data = 0;

	t = gpu_t;

	return t;
}

void gui_destroyTexture(mgTexture t)
{
	if (!t)
		return;
	miGPUTexture* gpu_t = (miGPUTexture*)t;
	gpu_t->Release();
}

void gui_endDraw()
{
	g_app->m_gs->UseDepth(g_gui_oldDepth);
}

void gui_drawRectangle(mgElement* element, mgPoint* position, mgPoint* size, mgColor* color1,
	mgColor* color2, mgTexture texture, mgVec4* UVRegion)
{
	v4f corners;
	corners.x = (f32)position->x;
	corners.y = (f32)position->y;
	corners.z = corners.x + (f32)size->x;
	corners.w = corners.y + (f32)size->y;

	miColor c1, c2;

	c1.m_data[0] = color1->r;
	c1.m_data[1] = color1->g;
	c1.m_data[2] = color1->b;
	c1.m_data[3] = color1->a;
	c2.m_data[0] = color2->r;
	c2.m_data[1] = color2->g;
	c2.m_data[2] = color2->b;
	c2.m_data[3] = color2->a;

	g_app->m_gs->DrawRectangle(corners, c1, c2, &g_app->m_GUI->m_proj, 0, 0);
}

void gui_drawText(
	mgPoint* position,
	const wchar_t* text,
	int textLen,
	mgColor* color,
	mgFont* font)
{
	mgPoint _position = *position;
	for (int i = 0; i < textLen; ++i)
	{
		wchar_t character = text[i];
		auto glyph = font->glyphMap[character];
		if (glyph)
		{
			_position.x += glyph->underhang;

			v4f corners;
			corners.x = _position.x;
			corners.y = _position.y;

			corners.z = corners.x + glyph->width;
			corners.w = corners.y + glyph->height;

			miGPUTexture* texture = (miGPUTexture*)((mgFontBitmap*)font->implementation)[glyph->textureSlot].gpuTexture;

			miColor c;
			c.m_data[0] = 1.f;
			c.m_data[1] = 1.f;
			c.m_data[2] = 1.f;
			c.m_data[3] = 1.f;

			v4f uv;
			uv.x = glyph->UV.x;
			uv.y = glyph->UV.y;
			uv.z = glyph->UV.z;
			uv.w = glyph->UV.w;

			g_app->m_gs->DrawRectangle(corners, c, c, &g_app->m_GUI->m_proj, texture, &uv);

			_position.x += glyph->width + glyph->overhang + font->characterSpacing;
			if (character == L' ')
				_position.x += font->spaceSize;
			if (character == L'\t')
				_position.x += font->tabSize;
		}
	}
}

void gui_getTextSize(const wchar_t* text, mgFont* font, mgPoint* sz)
{
	sz->x = 0;
	sz->y = 0;
	int c = wcslen(text);
	if (!c)
		return;
	for (int i = 0; i < c; ++i)
	{
		wchar_t character = text[i];
		auto glyph = font->glyphMap[character];
		if (glyph)
		{
			if (glyph->height > sz->y)
				sz->y = glyph->height;

			sz->x += glyph->width + glyph->overhang + glyph->underhang + font->characterSpacing;
			if (character == L' ')
				sz->x += font->spaceSize;
			if (character == L'\t')
				sz->x += font->tabSize;
		}
	}
}

mgRect gui_setClipRect(mgRect* r)
{
	static mgRect old;


	mgRect ret = old;
	old = *r;
	return ret;
}

