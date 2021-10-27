#ifndef _MAPCELL_H_
#define _MAPCELL_H_

//class TerrainVertex
//{
//	v3f m_position;
//	v2f m_uv;
//};

class MapCell
{
public:
	MapCell();
	~MapCell();

	miGPUMesh* m_meshGPU0 = 0;
	miGPUMesh* m_meshGPU1 = 0;
	miGPUMesh* m_meshGPU2 = 0;
	miGPUMesh* m_meshGPU3 = 0;
	v3f m_position;

	void Generate();
};


#endif