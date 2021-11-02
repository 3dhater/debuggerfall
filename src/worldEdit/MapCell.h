#ifndef _MAPCELL_H_
#define _MAPCELL_H_


/*
LOD0 - 50
LOD1 - 25
LOD2 - 12
LOD3 - 8
LOD4 - 4
*/
#define MapCellMaxLOD 6
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
	void Generate(miImage*, f32 mapSizeX, f32 mapSizeY);
};


#endif