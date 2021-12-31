#ifndef _GUI_H_
#define _GUI_H_

struct mgVideoDriverAPI_s;
//class GUIShader;

class ApplicationGUI
{
public:
	ApplicationGUI(mgInputContext_s* ic);
	~ApplicationGUI();

	/*miGUIContext* m_context = 0;
	miGUIFont* m_fontDefault = 0;
	miGUIPanel* m_panel_terrain = 0;

	miGUIPanel* m_panel_debug = 0;
	miGUIText* m_debug_text_FPS = 0;
	miGUIText* m_debug_text_position = 0;
	miGUIText* m_debug_text_cameraCellID = 0;*/

	mgVideoDriverAPI_s* m_guiGPU = 0;
	mgContext_s* m_guiContext = 0;
	mgFont* m_font1 = 0;
	mgElement* m_textFPS = 0;
	mgElement* m_textCELL_ID = 0;
	mgElement* m_textPOSITION = 0;

	Mat4 m_proj;
	//miGSDrawCommand m_drawCommand;
	//GUIShader* m_shader = 0;
	void Init();
	void UpdateMatrix(s32 x, s32 y);
	void SetTextFPS(s32 fps);
	void SetTextCellID(s32 id);
	void SetTextPosition(const v3f&);
};


#endif