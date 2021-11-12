/*
  Copyright (C) 2021 Basov Artyom
  The authors can be contacted at <artembasov@outlook.com>
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
	 notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
	 notice, this list of conditions and the following disclaimer in
	 the documentation and/or other materials provided with the
	 distribution.
  3. The names of the authors may not be used to endorse or promote
	 products derived from this software without specific prior
	 written permission.
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _MAPCELL_H_
#define _MAPCELL_H_

#include <map>
#include <string>

/*
LOD0 - 50
LOD1 - 25
LOD2 - 12
LOD3 - 8
LOD4 - 4
*/
#define MapCellMaxLOD 2

struct TerrainVertex
{
	TerrainVertex() {}
	TerrainVertex(
		const v3f& position,
		const v3f& normal,
		const v2f& uv1, 
		const v2f& uv2) 
	:
		m_position(position),
		m_normal(normal),
		m_uv1(uv1),
		m_uv2(uv2)
	{}

	v3f m_position;
	v3f m_normal;
	v2f m_uv1;
	v2f m_uv2;
};

struct LODHeader
{
	// for CPU mesh
	u32 m_vCount = 0;
	u32 m_iCount = 0;
	
	u32 m_compressedSize = 0;
	u32 m_uncompressedSize = 0;
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