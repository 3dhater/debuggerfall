#ifndef _GUI_H_
#define _GUI_H_

struct mgVideoDriverAPI_s;

class ApplicationGUI
{
public:
	ApplicationGUI();
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

	void Init(mgInputContext_s* ic);
};


#endif