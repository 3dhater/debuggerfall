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

#include "MapCell.h"

class Player;
class ApplicationGUI;
class miMesh;
class ShaderTerrain;
struct miGPUDrawCommand;

class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;
class btBroadphaseInterface;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
struct PhysicsEngine
{
	PhysicsEngine();
	~PhysicsEngine();
	btDefaultCollisionConfiguration* m_collisionConfiguration = 0;
	btCollisionDispatcher* m_dispatcher = 0;
	btBroadphaseInterface* m_overlappingPairCache = 0;
	btSequentialImpulseConstraintSolver* m_solver = 0;
	btDiscreteDynamicsWorld* m_world = 0;
};

class Application
{
public:
	Application();
	~Application();
	
	miMainSystem* m_mainSystem = 0;
	miWindow* m_windowMain = nullptr;
	mgInputContext_s* m_inputContext = 0;
	miGraphicsSystem* m_gs = 0;

	f32 m_dt = 0.f;
	v2f m_gpuDepthRange;
	miCameraFly* m_activeCamera = 0;
	ApplicationGUI* m_GUI = 0;

	PhysicsEngine* m_physics = 0;

	FILE* m_file_land = 0;
	//FILE* m_file_ids = 0; // base data


	bool OnCreate(const char*);
	void MainLoop();
	void WriteLog(const char* message);


	//miPopup* ShowPopup();
	void ShowGUITab(u32);
	bool OpenMap();
	void GenerateWorld();
	void ReadWorld();

	bool m_needUpdateMapCell = true;// need to find cell ID for player, and init cell
	bool m_needUpdateMapView = true;
	void _updateMapCell();

	miArray<CellGenData2> m_genData;

	//MapCell* m_testMapCell = 0;
	miArray<MapCell*> m_mapCells;
	miArray<miGPUDrawCommand*> m_mapDrawCommands;
	void DrawMapCell(MapCell*);
	void FrustumCullMap();

	Player* m_player = 0;
	//void FindCurrentCellID();

	bool m_cameraWasMoved = true;
	void FindLODs();

	miMesh* m_cellbase = 0; // square 250x250 meters
	//miGPUMesh* m_cellbaseGPU = 0;
	ShaderTerrain* m_shaderTerrain = 0;
};

#endif