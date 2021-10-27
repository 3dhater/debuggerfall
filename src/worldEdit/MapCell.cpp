#include "mixer.lib.h"
#include "mixer.lib.mesh.h"
#include "mixer.lib.gpuMesh.h"

#include "MapCell.h"


MapCell::MapCell() 
{

}

MapCell::~MapCell()
{
	if (m_meshGPU0) miDestroy(m_meshGPU0);
	if (m_meshGPU1) miDestroy(m_meshGPU1);
	if (m_meshGPU2) miDestroy(m_meshGPU2);
	if (m_meshGPU3) miDestroy(m_meshGPU3);
}

void MapCell::Generate()
{
	if (m_meshGPU0) miDestroy(m_meshGPU0);
	if (m_meshGPU1) miDestroy(m_meshGPU1);
	if (m_meshGPU2) miDestroy(m_meshGPU2);
	if (m_meshGPU3) miDestroy(m_meshGPU3);

	miMesh m_meshCPU;
	m_meshCPU.m_vertexType = miMeshVertexType::Triangle;
	m_meshCPU.m_indexType = miMeshIndexType::u16;
	m_meshCPU.m_stride = sizeof(miVertexTriangle);
	m_meshCPU.m_vCount = 40000;
	m_meshCPU.m_vertices = (u8*)miMalloc(m_meshCPU.m_stride * m_meshCPU.m_vCount);
	m_meshCPU.m_iCount = 60000;
	m_meshCPU.m_indices = (u8*)miMalloc(sizeof(u16) * m_meshCPU.m_iCount);

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

	Quad quad;

	v3f pos;
	f32 quadSize = 0.00005f;
	f32 quadSizeHalf = quadSize * 0.5f;
	v3f hsz(quadSizeHalf, 0.f, quadSizeHalf);
	
	auto vPtr = (miVertexTriangle*)m_meshCPU.m_vertices;
	auto iPtr = (u16*)m_meshCPU.m_indices;

	u32 vertexIndexCounter = 0;

	for (u32 i = 0; i < 100; ++i)
	{
		for (u32 o = 0; o < 100; ++o)
		{
			quad.Set(pos, quadSize);

			vPtr->Position = quad.m_v1 + hsz;
			vPtr->UV.set(0.f, 0.f);
			vPtr->Color.set(1.f);
			vPtr++;
			
			vPtr->Position = quad.m_v2 + hsz;
			vPtr->UV.set(1.f, 0.f);
			vPtr->Color.set(1.f);
			vPtr++;

			vPtr->Position = quad.m_v3 + hsz;
			vPtr->UV.set(1.f, 1.f);
			vPtr->Color.set(1.f);
			vPtr++;

			vPtr->Position = quad.m_v4 + hsz;
			vPtr->UV.set(0.f, 1.f);
			vPtr->Color.set(1.f);
			vPtr++;

			*iPtr = vertexIndexCounter; 
			iPtr++;
			*iPtr = vertexIndexCounter + 1;
			iPtr++;
			*iPtr = vertexIndexCounter + 2;
			iPtr++;

			*iPtr = vertexIndexCounter;
			iPtr++;
			*iPtr = vertexIndexCounter + 2;
			iPtr++;
			*iPtr = vertexIndexCounter + 3;
			iPtr++;

			vertexIndexCounter += 4;

			pos.x += quadSize;
		}
		pos.x = 0.f;
		pos.z += quadSize;
	}

	miGPUMeshInfo mi;
	mi.m_meshPtr = &m_meshCPU;
	m_meshGPU0 = miCreateGPUMesh(&mi);
	
	f32 width = quadSize * 100.f;

	vPtr = (miVertexTriangle*)m_meshCPU.m_vertices;
	for (u32 i = 0; i < m_meshCPU.m_vCount; ++i)
	{
		vPtr[i].Position.x -= width;
	}
	m_meshGPU1 = miCreateGPUMesh(&mi);

	vPtr = (miVertexTriangle*)m_meshCPU.m_vertices;
	for (u32 i = 0; i < m_meshCPU.m_vCount; ++i)
	{
		vPtr[i].Position.z -= width;
	}
	m_meshGPU2 = miCreateGPUMesh(&mi);

	vPtr = (miVertexTriangle*)m_meshCPU.m_vertices;
	for (u32 i = 0; i < m_meshCPU.m_vCount; ++i)
	{
		vPtr[i].Position.x += width;
	}
	m_meshGPU3 = miCreateGPUMesh(&mi);
}