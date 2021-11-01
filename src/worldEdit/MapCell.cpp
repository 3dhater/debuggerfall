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

f32 MapCell_getYForLOD(const v3f& vertexPosition, miMesh* prevLOD)
{
	auto vPtr = (miVertexTriangle*)prevLOD->m_vertices;
	f32 minD = 999999.f;
	f32 y = 0.f;
	for (u32 i = 0; i < prevLOD->m_vCount; ++i)
	{
		auto d = vPtr[i].Position.distance(vertexPosition);
		if (d < minD)
		{
			minD = d;
			y = vPtr[i].Position.y;
		}
	}

	return y;
}

f32 MapCell_getY(miVertexTriangle* vPtr, const v3f& m_position, const v2f& leftTop, 
	f32 mapszX1,
	f32 mapszY1,
	f32 imgX1,
	f32 imgY1,
	miImage* hm)
{
	f32 X = vPtr->Position.x + m_position.x - leftTop.x;
	f32 Y = vPtr->Position.z + m_position.z - leftTop.y;

	X *= mapszX1;
	Y *= mapszY1;

	if (X != 0.f)
		X = X / imgX1;

	if (Y != 0.f)
		Y = Y / imgY1;

	auto c = hm->getPixelColor(X, Y);
	return c.m_data[0] * 0.002f;
	//return 0.f;
}

void MapCell::Generate(miImage* hm, f32 mapSizeX, f32 mapSizeY)
{
	f32 mapSizeHalfX = mapSizeX * 0.5f;
	f32 mapSizeHalfY = mapSizeY * 0.5f;
	v2f leftTop(-mapSizeHalfX, -mapSizeHalfY);

	f32 imgX1 = 1.f / (f32)hm->m_width;
	f32 imgY1 = 1.f / (f32)hm->m_height;

	f32 mapszX1 = 1.f / mapSizeX;
	f32 mapszY1 = 1.f / mapSizeY;

	for (u32 i = 0; i < MapCellMaxLOD; ++i)
	{
		if (m_meshGPU0[i]) miDestroy(m_meshGPU0[i]); m_meshGPU0[i] = 0;
		if (m_meshGPU1[i]) miDestroy(m_meshGPU1[i]); m_meshGPU1[i] = 0;
		if (m_meshGPU2[i]) miDestroy(m_meshGPU2[i]); m_meshGPU2[i] = 0;
		if (m_meshGPU3[i]) miDestroy(m_meshGPU3[i]); m_meshGPU3[i] = 0;

		if (m_meshCPU0[i]) miDestroy(m_meshCPU0[i]); m_meshCPU0[i] = 0;
		if (m_meshCPU1[i]) miDestroy(m_meshCPU1[i]); m_meshCPU1[i] = 0;
		if (m_meshCPU2[i]) miDestroy(m_meshCPU2[i]); m_meshCPU2[i] = 0;
		if (m_meshCPU3[i]) miDestroy(m_meshCPU3[i]); m_meshCPU3[i] = 0;
	}

	u32 quadNum[] = { 100, 50, 24, 12, 8, 4 };

	for (u32 i = 0; i < MapCellMaxLOD; ++i)
	{
		m_meshCPU0[i] = miCreate<miMesh>();
		miMesh* meshCPU = m_meshCPU0[i];
		meshCPU->m_vertexType = miMeshVertexType::Triangle;
		meshCPU->m_indexType = miMeshIndexType::u16;
		meshCPU->m_stride = sizeof(miVertexTriangle);
		meshCPU->m_vCount = quadNum[i] * quadNum[i] * 4; // 100 квадов по пол метра = 50 метров
		meshCPU->m_iCount = ((quadNum[i] * quadNum[i]) * 2) * 3; // ((100*100)*2 triangles)*3 vertices per triangle

		meshCPU->m_vertices = (u8*)miMalloc(meshCPU->m_stride * meshCPU->m_vCount);
		meshCPU->m_indices = (u8*)miMalloc(sizeof(u16) * meshCPU->m_iCount);
		
		auto vPtr = (miVertexTriangle*)meshCPU->m_vertices;
		auto iPtr = (u16*)meshCPU->m_indices;

		Quad quad;
		v3f pos;
		f32 quadSize = 0.005f / (f32)quadNum[i];
		f32 quadSizeHalf = quadSize * 0.5f;
		v3f hsz(quadSizeHalf, 0.f, quadSizeHalf);
		u32 vertexIndexCounter = 0;

		for (u32 k = 0; k < quadNum[i]; ++k)
		{
			for (u32 o = 0; o < quadNum[i]; ++o)
			{
				quad.Set(pos, quadSize);

				vPtr->Position = quad.m_v1 + hsz;
				vPtr->Position.y = MapCell_getY(vPtr, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
				vPtr->UV.set(0.f, 0.f);
				vPtr->Color.set(1.f);
				m_aabb.add(vPtr->Position);
				vPtr++;
			
				vPtr->Position = quad.m_v2 + hsz;
				vPtr->Position.y = MapCell_getY(vPtr, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
				vPtr->UV.set(1.f, 0.f);
				vPtr->Color.set(1.f);
				m_aabb.add(vPtr->Position);
				vPtr++;

				vPtr->Position = quad.m_v3 + hsz;
				vPtr->Position.y = MapCell_getY(vPtr, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
				vPtr->UV.set(1.f, 1.f);
				vPtr->Color.set(1.f);
				m_aabb.add(vPtr->Position);
				vPtr++;

				vPtr->Position = quad.m_v4 + hsz;
				vPtr->Position.y = MapCell_getY(vPtr, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
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
			pos.x = 0.f;
			pos.z += quadSize;
		}
	
		miGPUMeshInfo mi;
		mi.m_meshPtr = meshCPU;
		m_meshGPU0[i] = miCreateGPUMesh(&mi);
		
		f32 width = 0.00005f * 100.f;
		
		//m_meshCPU1[i] = miCreate<miMesh>();
		//meshCPU = m_meshCPU1[i];
		vPtr = (miVertexTriangle*)meshCPU->m_vertices;
	
		for (u32 k = 0; k < meshCPU->m_vCount; ++k)
		{
			vPtr[k].Position.x += width;
			vPtr[k].Position.y = MapCell_getY(&vPtr[k], m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
			m_aabb.add(vPtr[k].Position);
		}
		m_meshGPU1[i] = miCreateGPUMesh(&mi);

		//m_meshCPU2[i] = miCreate<miMesh>();
		//meshCPU = m_meshCPU2[i];
		vPtr = (miVertexTriangle*)meshCPU->m_vertices;
		for (u32 k = 0; k < meshCPU->m_vCount; ++k)
		{
			vPtr[k].Position.z += width;
			vPtr[k].Position.y = MapCell_getY(&vPtr[k], m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
			m_aabb.add(vPtr[k].Position);
		}
		m_meshGPU2[i] = miCreateGPUMesh(&mi);

		//m_meshCPU3[i] = miCreate<miMesh>();
		//meshCPU = m_meshCPU3[i];
		vPtr = (miVertexTriangle*)meshCPU->m_vertices;
		for (u32 k = 0; k < meshCPU->m_vCount; ++k)
		{
			vPtr[k].Position.x -= width;
			vPtr[k].Position.y = MapCell_getY(&vPtr[k], m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
			m_aabb.add(vPtr[k].Position);
		}
		m_meshGPU3[i] = miCreateGPUMesh(&mi);
		//break;
	}
	
	m_aabbTransformed = m_aabb;
	m_aabbTransformed.m_min += m_position;
	m_aabbTransformed.m_max += m_position;

	//GenerateLODs();
}

//void MapCell::GenerateLOD(u32 lodID)
//{
//	assert(lodID > 0 && lodID < MapCellMaxLOD);
//
//	m_meshCPU0[lodID] = miCreate<miMesh>();
//
//	miMesh * meshCPU = m_meshCPU0[lodID];
//	meshCPU->m_vertexType = miMeshVertexType::Triangle;
//	meshCPU->m_indexType = miMeshIndexType::u16;
//	meshCPU->m_stride = sizeof(miVertexTriangle);
//	
//	miMesh* prevLOD0 = m_meshCPU0[lodID - 1];
//	miMesh* prevLOD1 = m_meshCPU1[lodID - 1];
//	miMesh* prevLOD2 = m_meshCPU2[lodID - 1];
//	miMesh* prevLOD3 = m_meshCPU3[lodID - 1];
//
//	u32 quadNum = 0;
//	if (lodID == 1)
//	{
//		quadNum = 50;
//	}
//	else if (lodID == 2)
//	{
//		quadNum = 24;
//	}
//	else if (lodID == 3)
//	{
//		quadNum = 12;
//	}
//	else if (lodID == 4)
//	{
//		quadNum = 8;
//	}
//	else if (lodID == 5)
//	{
//		quadNum = 4;
//	}
//
//	meshCPU->m_vCount = quadNum * quadNum * 4;
//	meshCPU->m_iCount = ((quadNum * quadNum) * 2) * 3;
//
//	meshCPU->m_vertices = (u8*)miMalloc(meshCPU->m_stride * meshCPU->m_vCount);
//	meshCPU->m_indices = (u8*)miMalloc(sizeof(u16) * meshCPU->m_iCount);
//	auto vPtr = (miVertexTriangle*)meshCPU->m_vertices;
//	auto iPtr = (u16*)meshCPU->m_indices;
//
//	Quad quad;
//	v3f pos;
//	f32 quadSize = 0.005f / (f32)quadNum;// 0.00005f;
//	f32 quadSizeHalf = quadSize * 0.5f;
//	v3f hsz(quadSizeHalf, 0.f, quadSizeHalf);
//	u32 vertexIndexCounter = 0;
//
//	// LAST VERTEX must be in position 0.005f
//	for (u32 i = 0; i < quadNum; ++i)
//	{
//		for (u32 o = 0; o < quadNum; ++o)
//		{
//			quad.Set(pos, quadSize);
//
//			vPtr->Position = quad.m_v1 + hsz;
//			vPtr->UV.set(0.f, 0.f);
//			vPtr->Color.set(1.f);
//			vPtr++;
//
//			vPtr->Position = quad.m_v2 + hsz;
//			vPtr->UV.set(1.f, 0.f);
//			vPtr->Color.set(1.f);
//			vPtr++;
//
//			vPtr->Position = quad.m_v3 + hsz;
//			vPtr->UV.set(1.f, 1.f);
//			vPtr->Color.set(1.f);
//			vPtr++;
//
//			vPtr->Position = quad.m_v4 + hsz;
//			vPtr->UV.set(0.f, 1.f);
//			vPtr->Color.set(1.f);
//			vPtr++;
//
//			*iPtr = vertexIndexCounter;
//			iPtr++;
//			*iPtr = vertexIndexCounter + 1;
//			iPtr++;
//			*iPtr = vertexIndexCounter + 2;
//			iPtr++;
//
//			*iPtr = vertexIndexCounter;
//			iPtr++;
//			*iPtr = vertexIndexCounter + 2;
//			iPtr++;
//			*iPtr = vertexIndexCounter + 3;
//			iPtr++;
//
//			vertexIndexCounter += 4;
//
//			pos.x += quadSize;
//		}
//
//		pos.x = 0.f;
//		pos.z += quadSize;
//	}
//
//	miGPUMeshInfo mi;
//	mi.m_meshPtr = meshCPU;
//	m_meshGPU0[lodID] = miCreateGPUMesh(&mi);
//
//	f32 width = 0.00005f * 100.f;
//
//	vPtr = (miVertexTriangle*)meshCPU->m_vertices;
//	for (u32 i = 0; i < meshCPU->m_vCount; ++i)
//	{
//		vPtr[i].Position.x -= width;
//		m_aabb.add(vPtr[i].Position);
//	}
//	m_meshGPU1[lodID] = miCreateGPUMesh(&mi);
//
//	vPtr = (miVertexTriangle*)meshCPU->m_vertices;
//	for (u32 i = 0; i < meshCPU->m_vCount; ++i)
//	{
//		vPtr[i].Position.z -= width;
//		m_aabb.add(vPtr[i].Position);
//	}
//	m_meshGPU2[lodID] = miCreateGPUMesh(&mi);
//
//	vPtr = (miVertexTriangle*)meshCPU->m_vertices;
//	for (u32 i = 0; i < meshCPU->m_vCount; ++i)
//	{
//		vPtr[i].Position.x += width;
//		m_aabb.add(vPtr[i].Position);
//	}
//	m_meshGPU3[lodID] = miCreateGPUMesh(&mi);
//}
//
//void MapCell::GenerateLODs()
//{
//	for (u32 i = 1; i < MapCellMaxLOD; ++i)
//	{
//		if (m_meshGPU0[i]) miDestroy(m_meshGPU0[i]);
//		if (m_meshGPU1[i]) miDestroy(m_meshGPU1[i]);
//		if (m_meshGPU2[i]) miDestroy(m_meshGPU2[i]);
//		if (m_meshGPU3[i]) miDestroy(m_meshGPU3[i]);
//
//		/*if (m_meshCPU0[i]) miDestroy(m_meshCPU0[i]);
//		if (m_meshCPU1[i]) miDestroy(m_meshCPU1[i]);
//		if (m_meshCPU2[i]) miDestroy(m_meshCPU2[i]);
//		if (m_meshCPU3[i]) miDestroy(m_meshCPU3[i]);*/
//	}
//	for (u32 i = 1; i < MapCellMaxLOD; ++i)
//	{
//		GenerateLOD(i);
//	}
//
//	for (u32 i = 1; i < MapCellMaxLOD; ++i)
//	{
//		if (m_meshCPU0[i]) miDestroy(m_meshCPU0[i]); m_meshCPU0[i] = 0;
//		if (m_meshCPU1[i]) miDestroy(m_meshCPU1[i]); m_meshCPU1[i] = 0;
//		if (m_meshCPU2[i]) miDestroy(m_meshCPU2[i]); m_meshCPU2[i] = 0;
//		if (m_meshCPU3[i]) miDestroy(m_meshCPU3[i]); m_meshCPU3[i] = 0;
//	}
//}