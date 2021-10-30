#include "mixer.lib.h"
#include "mixer.lib.mesh.h"
#include "mixer.lib.gpuMesh.h"

#include "MapCell.h"


MapCell::MapCell() 
{
	for (u32 i = 0; i < MapCellMaxLOD; ++i)
	{
		m_meshGPU0[i] = 0;
		m_meshGPU1[i] = 0;
		m_meshGPU2[i] = 0;
		m_meshGPU3[i] = 0;

		m_meshCPU0[i] = 0;
		m_meshCPU1[i] = 0;
		m_meshCPU2[i] = 0;
		m_meshCPU3[i] = 0;
	}
}

MapCell::~MapCell()
{
	for (u32 i = 0; i < MapCellMaxLOD; ++i)
	{
		if (m_meshGPU0[i]) miDestroy(m_meshGPU0[i]);
		if (m_meshGPU1[i]) miDestroy(m_meshGPU1[i]);
		if (m_meshGPU2[i]) miDestroy(m_meshGPU2[i]);
		if (m_meshGPU3[i]) miDestroy(m_meshGPU3[i]);

		if (m_meshCPU0[i]) miDestroy(m_meshCPU0[i]);
		if (m_meshCPU1[i]) miDestroy(m_meshCPU1[i]);
		if (m_meshCPU2[i]) miDestroy(m_meshCPU2[i]);
		if (m_meshCPU3[i]) miDestroy(m_meshCPU3[i]);
	}
}

void MapCell::Generate()
{
	for (u32 i = 0; i < MapCellMaxLOD; ++i)
	{
		if (m_meshGPU0[i]) miDestroy(m_meshGPU0[i]);
		if (m_meshGPU1[i]) miDestroy(m_meshGPU1[i]);
		if (m_meshGPU2[i]) miDestroy(m_meshGPU2[i]);
		if (m_meshGPU3[i]) miDestroy(m_meshGPU3[i]);

		if (m_meshCPU0[i]) miDestroy(m_meshCPU0[i]);
		if (m_meshCPU1[i]) miDestroy(m_meshCPU1[i]);
		if (m_meshCPU2[i]) miDestroy(m_meshCPU2[i]);
		if (m_meshCPU3[i]) miDestroy(m_meshCPU3[i]);
	}

	m_meshCPU0[0] = miCreate<miMesh>();

	miMesh * meshCPU = m_meshCPU0[0];
	meshCPU->m_vertexType = miMeshVertexType::Triangle;
	meshCPU->m_indexType = miMeshIndexType::u16;
	meshCPU->m_stride = sizeof(miVertexTriangle);
	meshCPU->m_vCount = 100*100*4; // 100 квадов по пол метра = 50 метров
	meshCPU->m_vertices = (u8*)miMalloc(meshCPU->m_stride * meshCPU->m_vCount);
	meshCPU->m_iCount = 60000; // ((100*100)*2 triangles)*3 vertices per triangle
	meshCPU->m_indices = (u8*)miMalloc(sizeof(u16) * meshCPU->m_iCount);

	Quad quad;

	v3f pos;
	f32 quadSize = 0.00005f;
	f32 quadSizeHalf = quadSize * 0.5f;
	v3f hsz(quadSizeHalf, 0.f, quadSizeHalf);
	
	auto vPtr = (miVertexTriangle*)meshCPU->m_vertices;
	auto iPtr = (u16*)meshCPU->m_indices;

	u32 vertexIndexCounter = 0;

	/*
		0.005 = 50 m

		LOD0 0.005 : 100 = 0.00005
		LOD1 0.005 : 50  = 0.0001
		LOD2 0.005 : 25  = 0.0002
		LOD3 0.005 : 12  = 0.0004+
		LOD4 0.005 : 6   = 0.0008+
	*/

	for (u32 i = 0; i < 100; ++i)
	{
		for (u32 o = 0; o < 100; ++o)
		{
			quad.Set(pos, quadSize);

			vPtr->Position = quad.m_v1 + hsz;
			vPtr->UV.set(0.f, 0.f);
			vPtr->Color.set(1.f);
			m_aabb.add(vPtr->Position);
			vPtr++;
			

			vPtr->Position = quad.m_v2 + hsz;
			vPtr->UV.set(1.f, 0.f);
			vPtr->Color.set(1.f);
			m_aabb.add(vPtr->Position);
			vPtr++;

			vPtr->Position = quad.m_v3 + hsz;
			vPtr->UV.set(1.f, 1.f);
			vPtr->Color.set(1.f);
			m_aabb.add(vPtr->Position);
			vPtr++;

			vPtr->Position = quad.m_v4 + hsz;
			vPtr->UV.set(0.f, 1.f);
			vPtr->Color.set(1.f);
			m_aabb.add(vPtr->Position);
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
		//printf("pos.x = %f\n", pos.x);
		pos.x = 0.f;
		pos.z += quadSize;
	}

	miGPUMeshInfo mi;
	mi.m_meshPtr = meshCPU;
	m_meshGPU0[0] = miCreateGPUMesh(&mi);
	
	f32 width = quadSize * 100.f;

	vPtr = (miVertexTriangle*)meshCPU->m_vertices;
	for (u32 i = 0; i < meshCPU->m_vCount; ++i)
	{
		vPtr[i].Position.x -= width;
		m_aabb.add(vPtr[i].Position);
	}
	m_meshGPU1[0] = miCreateGPUMesh(&mi);

	vPtr = (miVertexTriangle*)meshCPU->m_vertices;
	for (u32 i = 0; i < meshCPU->m_vCount; ++i)
	{
		vPtr[i].Position.z -= width;
		m_aabb.add(vPtr[i].Position);
	}
	m_meshGPU2[0] = miCreateGPUMesh(&mi);

	vPtr = (miVertexTriangle*)meshCPU->m_vertices;
	for (u32 i = 0; i < meshCPU->m_vCount; ++i)
	{
		vPtr[i].Position.x += width;
		m_aabb.add(vPtr[i].Position);
	}
	m_meshGPU3[0] = miCreateGPUMesh(&mi);
	
	m_aabbTransformed = m_aabb;
	m_aabbTransformed.m_min += m_position;
	m_aabbTransformed.m_max += m_position;

	GenerateLODs();
}

void MapCell::GenerateLOD(u32 lodID)
{
	assert(lodID > 0 && lodID < MapCellMaxLOD);

	miMesh meshCPU;
	meshCPU.m_vertexType = miMeshVertexType::Triangle;
	meshCPU.m_indexType = miMeshIndexType::u16;
	meshCPU.m_stride = sizeof(miVertexTriangle);
	
	u32 quadNum = 0;
	if (lodID == 1)
	{
		quadNum = 50;
	}
	else if (lodID == 2)
	{
		quadNum = 24;
	}
	else if (lodID == 3)
	{
		quadNum = 12;
	}
	else if (lodID == 4)
	{
		quadNum = 8;
	}
	else if (lodID == 5)
	{
		quadNum = 4;
	}

	meshCPU.m_vCount = quadNum * quadNum * 4;
	meshCPU.m_iCount = ((quadNum * quadNum) * 2) * 3;

	meshCPU.m_vertices = (u8*)miMalloc(meshCPU.m_stride * meshCPU.m_vCount);
	meshCPU.m_indices = (u8*)miMalloc(sizeof(u16) * meshCPU.m_iCount);
	auto vPtr = (miVertexTriangle*)meshCPU.m_vertices;
	auto iPtr = (u16*)meshCPU.m_indices;

	Quad quad;
	v3f pos;
	f32 quadSize = 0.005f / (f32)quadNum;// 0.00005f;
	f32 quadSizeHalf = quadSize * 0.5f;
	v3f hsz(quadSizeHalf, 0.f, quadSizeHalf);
	u32 vertexIndexCounter = 0;

	// LAST VERTEX must be in position 0.005f
	for (u32 i = 0; i < quadNum; ++i)
	{
		for (u32 o = 0; o < quadNum; ++o)
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
	mi.m_meshPtr = &meshCPU;
	m_meshGPU0[lodID] = miCreateGPUMesh(&mi);

	f32 width = 0.00005f * 100.f;

	vPtr = (miVertexTriangle*)meshCPU.m_vertices;
	for (u32 i = 0; i < meshCPU.m_vCount; ++i)
	{
		vPtr[i].Position.x -= width;
		m_aabb.add(vPtr[i].Position);
	}
	m_meshGPU1[lodID] = miCreateGPUMesh(&mi);

	vPtr = (miVertexTriangle*)meshCPU.m_vertices;
	for (u32 i = 0; i < meshCPU.m_vCount; ++i)
	{
		vPtr[i].Position.z -= width;
		m_aabb.add(vPtr[i].Position);
	}
	m_meshGPU2[lodID] = miCreateGPUMesh(&mi);

	vPtr = (miVertexTriangle*)meshCPU.m_vertices;
	for (u32 i = 0; i < meshCPU.m_vCount; ++i)
	{
		vPtr[i].Position.x += width;
		m_aabb.add(vPtr[i].Position);
	}
	m_meshGPU3[lodID] = miCreateGPUMesh(&mi);
}

void MapCell::GenerateLODs()
{
	for (u32 i = 1; i < MapCellMaxLOD; ++i)
	{
		if (m_meshGPU0[i]) miDestroy(m_meshGPU0[i]);
		if (m_meshGPU1[i]) miDestroy(m_meshGPU1[i]);
		if (m_meshGPU2[i]) miDestroy(m_meshGPU2[i]);
		if (m_meshGPU3[i]) miDestroy(m_meshGPU3[i]);

		if (m_meshCPU0[i]) miDestroy(m_meshCPU0[i]);
		if (m_meshCPU1[i]) miDestroy(m_meshCPU1[i]);
		if (m_meshCPU2[i]) miDestroy(m_meshCPU2[i]);
		if (m_meshCPU3[i]) miDestroy(m_meshCPU3[i]);
	}
	for (u32 i = 1; i < MapCellMaxLOD; ++i)
	{
		GenerateLOD(i);
	}
}