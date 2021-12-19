#ifndef _APP_H_
#define _APP_H_

#include <map>
#include <string>

template <typename M, typename V>
void MapToVec(const  M& m, V& v) {
	for (typename M::const_iterator it = m.begin(); it != m.end(); ++it) {
		v.push_back(it->second.first);
	}
}

class MapCell;
class Player;

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

	void Init();
};






class Application
{
public:
	Application();
	~Application();
	
	miMainSystem* m_mainSystem = 0;
	miWindow* m_mainWindow = nullptr;
	mgInputContext_s* m_inputContext = 0;
	miGraphicsSystem* m_gs = 0;

	f32 m_dt = 0.f;
	v2f m_gpuDepthRange;
	miCameraFly* m_activeCamera = 0;
	ApplicationGUI* m_GUI = 0;

	FILE* m_file_gen = 0;
	FILE* m_file_ids = 0;


	bool OnCreate(const char*);
	void MainLoop();
	void WriteLog(const char* message);
	//miPopup* ShowPopup();
	void ShowGUITab(u32);
	bool OpenMap();
	void GenerateWorld();
	void ReadWorld();

	// need to find cell ID for player, and init cell
	bool m_needUpdateMapCell = true;
	void _updateMapCell();

	//MapCell* m_testMapCell = 0;
	miArray<MapCell*> m_mapCells;
	//miArray<MapCell*> m_visibleMapCells;
	void DrawMapCell(MapCell*);
	void FrustumCullMap();

	Player* m_player = 0;
	//void FindCurrentCellID();

	bool m_cameraWasMoved = true;
	void FindLODs();
};

#endif