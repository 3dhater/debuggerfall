#ifndef _MAPCELL_H_
#define _MAPCELL_H_

//class TerrainVertex
//{
//	v3f m_position;
//	v2f m_uv;
//};


/*
LOD0 - 50
LOD1 - 25
LOD2 - 12
LOD3 - 6
LOD4 - 3
*/
#define MapCellMaxLOD 5
class MapCell
{
public:
	MapCell();
	~MapCell();

	miGPUMesh* m_meshGPU0[MapCellMaxLOD];
	miGPUMesh* m_meshGPU1[MapCellMaxLOD];
	miGPUMesh* m_meshGPU2[MapCellMaxLOD];
	miGPUMesh* m_meshGPU3[MapCellMaxLOD];
	v3f m_position;

	miMesh* m_meshCPU0[MapCellMaxLOD];
	miMesh* m_meshCPU1[MapCellMaxLOD];
	miMesh* m_meshCPU2[MapCellMaxLOD];
	miMesh* m_meshCPU3[MapCellMaxLOD];

	Aabb m_aabb;

	u32 m_id = 0;

	void Generate();
};


#endif