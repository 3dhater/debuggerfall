#ifndef _MAPCELL_H_
#define _MAPCELL_H_

/*
LOD0 - 50
LOD1 - 25
LOD2 - 12
LOD3 - 8
LOD4 - 4
*/
#define MapCellMaxLOD 3

struct LODsHeader
{
	// for CPU mesh
	u32 m_vBufferSize[MapCellMaxLOD];
	u32 m_iBufferSize[MapCellMaxLOD];
	u32 m_vCount[MapCellMaxLOD];
	u32 m_iCount[MapCellMaxLOD];
	u32 m_quadNum[MapCellMaxLOD];
	
	v3f m_position;

	//Aabb m_aabb; // not transformed
	//v3f m_positionInWorld[4];

	// size of buffers from all LODs
	/*u32 m_vBuffersSizeUncompressed = 0;
	u32 m_iBuffersSizeUncompressed = 0;
	u32 m_vBuffersSizeCompressed = 0;
	u32 m_iBuffersSizeCompressed = 0;*/

	u32 m_globalVertexIndexBufferSize_compressed = 0;
	u32 m_globalVertexIndexBufferSize_uncompressed = 0;
};

class MapCell
{
	//// currentLOD - LOD0, LOD1, LOD2...
	//void GenerateLOD(u32 lodID);
public:
	MapCell();
	~MapCell();
	
	//void GenerateLODs();

	struct Quad
	{
		v3f m_v1;
		v3f m_v2;
		v3f m_v3;
		v3f m_v4;

		void Set(const v3f& position, f32 size)
		{
			m_v1 = position;
			m_v1.x -= size * 0.5f;
			m_v1.z -= size * 0.5f;

			m_v2 = m_v1;
			m_v2.z += size;

			m_v3 = m_v1;
			m_v3.x += size;
			m_v3.z += size;

			m_v4 = m_v1;
			m_v4.x += size;
		}
	};

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
	Aabb m_aabbTransformed;
	v3f m_positionInWorld[4];

	u32 m_id = 0;
	bool m_inView = false;

	u32 m_activeLOD[4] = {0,0,0,0};
	void Generate(u32 ix, u32 iy, std::map<std::string, std::pair<v3f, u32>>& vMap, miImage*, f32 mapSizeX, f32 mapSizeY);
	void DeleteCPUMesh();
	//void WriteToFile(u32 x, u32 y);
};


#endif