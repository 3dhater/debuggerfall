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

struct CellGenInfo
{
	int type;
	int flags;
	int data[8];
};

struct CellBaseData
{
	int ids[9];
	float pos[2];
};

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

class MapCell
{
public:
	MapCell();
	~MapCell();
	
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

	miGPUMesh* m_meshGPU0[16]; // 4x4
	miGPUMesh* m_meshGPU1[16];
	v3f m_position;

	Aabb m_aabb;
	Aabb m_aabbTransformed;
	//v3f m_positionInWorld[16];

	s32 m_id = -1;
	bool m_inView = false;

	bool m_isReady = false;
	void Clear();

	// pos: x, z
	void InitNew(s32 id, f32 * pos);

	// -1 - not visible
	// 0 - lod0
	// 1 - lod1
	s32 m_activeLOD[16];
};


#endif