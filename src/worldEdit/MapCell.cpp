#include "mi/MainSystem/MainSystem.h"
#include "mi/Mesh/mesh.h"
#include "mi/GraphicsSystem/util.h"
#include "mi/Scene/common.h"
#include "mi/Scene/cameraFly.h"

#include "application.h"
#include "MapCell.h"
#include "ShaderTerrain.h"

#include <filesystem>

extern Application* g_app;

MapCell::MapCell() 
{
	m_meshGPU0 = 0;
	m_meshGPU1 = 0;
	m_activeLOD = -1;
}

MapCell::~MapCell()
{
	Clear();
}

void MapCell::Clear()
{
	m_id = -1;
	if (m_meshGPU0)
	{
		miDestroy(m_meshGPU0);
		m_meshGPU0 = 0;
	}
		
	if (m_meshGPU1)
	{
		miDestroy(m_meshGPU1);
		m_meshGPU1 = 0;
	}
}

void MapCell::InitNew()
{
	m_position.x = m_cellData.pos.x;
	m_position.y = 0.f;
	m_position.z = m_cellData.pos.y;

	m_aabb.reset();
	m_aabb.m_min.set(-0.125f, -10.f, -0.125f, 0.f);
	m_aabb.m_max.set(0.125f, 10.f, 0.125f, 0.f);
	
	m_aabbTransformed = m_aabb;
	m_aabbTransformed.m_min.x += m_cellData.pos.x;
	m_aabbTransformed.m_min.z += m_cellData.pos.y;
	m_aabbTransformed.m_max.x += m_cellData.pos.x;
	m_aabbTransformed.m_max.z += m_cellData.pos.y;

	m_worldMatrix.setTranslation(v3f(m_cellData.pos.x, 0.f, m_cellData.pos.y));

	//g_app->m_file_gen

	miGPUMeshInfo mi;
	mi.m_meshPtr = g_app->m_cellbase;

	auto vPtr = (miVertexTriangle*)mi.m_meshPtr->m_vertices;
	for (u32 i = 0; i < mi.m_meshPtr->m_vCount; ++i)
	{
		vPtr->Position.y = 0.f;

	//	printf("g_app->m_genData.m_size: %i\n", g_app->m_genData.m_size);
		for (s32 i3 = 0; i3 < g_app->m_genData.m_size; ++i3)
		{
			auto gd = &g_app->m_genData.m_data[i3];
			f64 distance = 0.f;
			{
				f64 _x1 = (f64)vPtr->Position.x + (f64)m_position.x;
				f64 _z1 = (f64)vPtr->Position.z + (f64)m_position.z;

				f64 _x2 = (f64)gd->worldPosition.x;
				f64 _z2 = (f64)gd->worldPosition.z;

				f64 _xx = _x2 - _x1;
				f64 _zz = _z2 - _z1;

				distance = std::sqrt((_xx * _xx) + (_zz * _zz));
			}

			/*f32 distance = v2f(
				vPtr->Position.x + m_position.x,
				vPtr->Position.z + m_position.z
			).distance(
				v2f((f32)cd->genData[genDataIndex].pos[0] + offset.x,
					(f32)cd->genData[genDataIndex].pos[2] + offset.z));*/

			f64 infl = 0.f;

			if (distance == 0.f)
				infl = 1.f;

			switch (gd->data.genType)
			{
			case CellGenType::CellGenType_MoveDown:
				if (gd->data.f32Data1)
				{
					if (distance > gd->data.f32Data1)
						infl = 0.f;
					else
					{
						infl = 1.0 - (distance / (f64)gd->data.f32Data1);

						// пусть если infl == 1 то это 90градусов 
						f32 alfa = infl * 1.5707963268;
						f32 sn = std::sin(alfa);

						infl = sn;
					}
				}
				vPtr->Position.y -= (f32)gd->data.pos[1] * infl;
				break;
			default:
			case CellGenType::CellGenType_MoveUp:
				if (gd->data.f32Data1)
				{
					if (distance > gd->data.f32Data1)
						infl = 0.f;
					else
					{
						infl = 1.0 - (distance / (f64)gd->data.f32Data1);
						f32 alfa = infl * 1.5707963268;
						f32 sn = std::sin(alfa);
						infl = sn;
					}
				}
				vPtr->Position.y += (f32)gd->data.pos[1] * infl;
				break;
			}
		}
		
		vPtr++;
	}
	
	m_meshGPU0 = g_app->m_gs->CreateMesh(&mi);
	m_material.m_wireframe = true;
	m_material.m_maps[0].m_GPUTexture = g_app->m_mainSystem->GetWhiteTexture();
	m_drawCommand.m_shader = g_app->m_shaderTerrain->m_GPUShader;
	m_drawCommand.m_material = &m_material;
	m_drawCommand.m_mesh = m_meshGPU0;
	m_drawCommand.m_matWorld = &m_worldMatrix;
	m_drawCommand.m_matView = &g_app->m_activeCamera->m_view;
	m_drawCommand.m_matProjection = &g_app->m_activeCamera->m_projection;
	m_drawCommand.m_matWVP = &m_WVP;
}

f32 MapCell_getY(
	//TerrainVertex* vPtr, 
	const v3f& vPosition,
	const v3f& m_position, const v2f& leftTop,
	f32 mapszX1,
	f32 mapszY1,
	f32 imgX1,
	f32 imgY1,
	miImage* hm)
{
	f32 X = vPosition.x + m_position.x - leftTop.x;
	f32 Y = vPosition.z + m_position.z - leftTop.y;

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


//void MapCell_PutPositionInMap(
//	std::map<std::string, std::pair<v3f,u32>>& vMap,
//	const v3f& position, 
//	std::string& stdstr,
//	miStringA& mystr1f,
//	miStringA& mystr3f,
//	miArray<u32>& viArray)
//{
//	mystr3f.clear();
//	stdstr.clear();
//
//	mystr1f.clear();
//	mystr1f.append_float(position.x);
//	mystr1f.pop_back();
//	mystr1f.pop_back();
//	mystr1f.pop_back();
//	mystr3f += mystr1f;
//
//	mystr1f.clear();
//	mystr1f.append_float(position.y);
//	mystr1f.pop_back();
//	mystr1f.pop_back();
//	mystr1f.pop_back();
//	mystr3f += mystr1f;
//
//	mystr1f.clear();
//	mystr1f.append_float(position.z);
//	mystr1f.pop_back();
//	mystr1f.pop_back();
//	mystr1f.pop_back();
//	mystr3f += mystr1f;
//
//	stdstr = mystr3f.c_str();
//		
//	auto it = vMap.find(stdstr);
//	if (it == vMap.end())
//	{
//		auto index = vMap.size();
//		vMap[stdstr] = std::pair<v3f, u32>(position, index);
//		viArray.push_back(index);
//	}
//	else
//	{
//		viArray.push_back(it->second.second);
//	}
//}

//void MapCell::Generate(
//	u32 ix, 
//	u32 iy,
//	std::map<std::string, std::pair<v3f, u32>>& vMap,
//	miImage* hm, 
//	f32 mapSizeX, 
//	f32 mapSizeY)
//{
//	LODsHeader head;
//	
//	f32 mapSizeHalfX = mapSizeX * 0.5f;
//	f32 mapSizeHalfY = mapSizeY * 0.5f;
//	v2f leftTop(-mapSizeHalfX, -mapSizeHalfY);
//
//	f32 imgX1 = 1.f / (f32)hm->m_width;
//	f32 imgY1 = 1.f / (f32)hm->m_height;
//
//	f32 mapszX1 = 1.f / mapSizeX;
//	f32 mapszY1 = 1.f / mapSizeY;
//
//	u32 quadNum[] = { 50, 24, 4, 4, 4, 4 };
//	
//	f32 width = 0.00005f * 100.f;
//
//	std::string stdstr;
//	miStringA mystr1f;
//	miStringA mystr3f;
//
//	miArray<u32> viArray;
//
//	for (u32 i = 0; i < MapCellMaxLOD; ++i)
//	{
//	//	m_meshCPU0[i] = miCreate<miMesh>();
//		//miMesh* meshCPU = m_meshCPU0[i];
//		//meshCPU->m_vertexType = miMeshVertexType::Triangle;
//		//meshCPU->m_indexType = miMeshIndexType::u16;
//		//meshCPU->m_stride = sizeof(TerrainVertex);
//		//meshCPU->m_vCount = quadNum[i] * quadNum[i] * 4; // 100 квадов по пол метра = 50 метров
//		//meshCPU->m_iCount = ((quadNum[i] * quadNum[i]) * 2) * 3; // ((100*100)*2 triangles)*3 vertices per triangle
//
//		//meshCPU->m_vertices = (u8*)miMalloc(meshCPU->m_stride * meshCPU->m_vCount);
//		//meshCPU->m_indices = (u8*)miMalloc(sizeof(u16) * meshCPU->m_iCount);
//		//
//		head.m_vCount[i] = quadNum[i] * quadNum[i] * 4;// m_meshCPU0[i]->m_vCount;
//		head.m_iCount[i] = ((quadNum[i] * quadNum[i]) * 2) * 3;// m_meshCPU0[i]->m_iCount;
//		head.m_vBufferSize[i] = head.m_vCount[i] * sizeof(TerrainVertex);
//		head.m_iBufferSize[i] = head.m_iCount[i] * sizeof(u16);
//		head.m_quadNum[i] = quadNum[i];
//
//		//auto vPtr = (TerrainVertex*)meshCPU->m_vertices;
//		//auto iPtr = (u16*)meshCPU->m_indices;
//
//		Quad quad;
//		v3f pos;
//		f32 quadSize = 0.005f / (f32)quadNum[i];
//		f32 quadSizeHalf = quadSize * 0.5f;
//		v3f hsz(quadSizeHalf, 0.f, quadSizeHalf);
//		u32 vertexIndexCounter = 0;
//		
//		miArray<v3f> currLODVertices;
//		currLODVertices.reserve(0xfff); // for other parts: 1,2,3
//
//		for (u32 k = 0; k < quadNum[i]; ++k)
//		{
//			for (u32 o = 0; o < quadNum[i]; ++o)
//			{
//				quad.Set(pos, quadSize);
//
//				v3f position = quad.m_v1 + hsz;
//				position.y = MapCell_getY(position, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//				MapCell_PutPositionInMap(vMap, position, stdstr, mystr1f, mystr3f, viArray);
//			//	m_aabb.add(position);
//				currLODVertices.push_back(position);
//
//				position = quad.m_v2 + hsz;
//				position.y = MapCell_getY(position, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//				MapCell_PutPositionInMap(vMap, position, stdstr, mystr1f, mystr3f, viArray);
//			//	m_aabb.add(position);
//				currLODVertices.push_back(position);
//
//				position = quad.m_v3 + hsz;
//				position.y = MapCell_getY(position, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//				MapCell_PutPositionInMap(vMap, position, stdstr, mystr1f, mystr3f, viArray);
//			//	m_aabb.add(position);
//				currLODVertices.push_back(position);
//
//				position = quad.m_v4 + hsz;
//				position.y = MapCell_getY(position, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//				MapCell_PutPositionInMap(vMap, position, stdstr, mystr1f, mystr3f, viArray);
//			//	m_aabb.add(position);
//				currLODVertices.push_back(position);
//
//		//		vPtr->Position = quad.m_v1 + hsz;
//		//		vPtr->Position.y = MapCell_getY(vPtr, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//				//vPtr->UV.set(0.f, 0.f);
//				//vPtr->Color.set(1.f);
//			//	vArray.push_back(*vPtr);
//		//		m_aabb.add(vPtr->Position);
//		//		vPtr++;
//
//		//		vPtr->Position = quad.m_v2 + hsz;
//		//		vPtr->Position.y = MapCell_getY(vPtr, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//				//vPtr->UV.set(1.f, 0.f);
//				//vPtr->Color.set(1.f);
//			//	vArray.push_back(*vPtr);
//		//		m_aabb.add(vPtr->Position);
//		//		vPtr++;
//
//		//		vPtr->Position = quad.m_v3 + hsz;
//		//		vPtr->Position.y = MapCell_getY(vPtr, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//				//vPtr->UV.set(1.f, 1.f);
//				//vPtr->Color.set(1.f);
//			//	vArray.push_back(*vPtr);
//		//		m_aabb.add(vPtr->Position);
//		//		vPtr++;
//
//		//		vPtr->Position = quad.m_v4 + hsz;
//		//		vPtr->Position.y = MapCell_getY(vPtr, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//				//vPtr->UV.set(0.f, 1.f);
//				//vPtr->Color.set(1.f);
//			//	vArray.push_back(*vPtr);
//		//		m_aabb.add(vPtr->Position);
//		//		vPtr++;
//			
//		//		*iPtr = vertexIndexCounter; 
//			//	iArray.push_back(*iPtr);
//		//		iPtr++;
//		//		*iPtr = vertexIndexCounter + 1;
//			//	iArray.push_back(*iPtr);
//		//		iPtr++;
//		//		*iPtr = vertexIndexCounter + 2;
//			//	iArray.push_back(*iPtr);
//		//		iPtr++;
//
//		//		*iPtr = vertexIndexCounter;
//			//	iArray.push_back(*iPtr);
//		//		iPtr++;
//		//		*iPtr = vertexIndexCounter + 2;
//			//	iArray.push_back(*iPtr);
//		//		iPtr++;
//		//		*iPtr = vertexIndexCounter + 3;
//			//	iArray.push_back(*iPtr);
//		//		iPtr++;
//
//				vertexIndexCounter += 4;
//				pos.x += quadSize;
//			}
//			pos.x = 0.f;
//			pos.z += quadSize;
//		}
//	
//		for (u32 k = 0; k < currLODVertices.m_size; ++k)
//		{
//			v3f pos = currLODVertices.m_data[k];
//			pos.x += width;
//			currLODVertices.m_data[k].y = MapCell_getY(pos, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//			MapCell_PutPositionInMap(vMap, currLODVertices.m_data[k], stdstr, mystr1f, mystr3f, viArray);
//		}
//
//		for (u32 k = 0; k < currLODVertices.m_size; ++k)
//		{
//			v3f pos = currLODVertices.m_data[k];
//			pos.z += width;
//			currLODVertices.m_data[k].y = MapCell_getY(pos, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//			MapCell_PutPositionInMap(vMap, currLODVertices.m_data[k], stdstr, mystr1f, mystr3f, viArray);
//		}
//
//		for (u32 k = 0; k < currLODVertices.m_size; ++k)
//		{
//			v3f pos = currLODVertices.m_data[k];
//			pos.x -= width;
//			currLODVertices.m_data[k].y = MapCell_getY(pos, m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//			MapCell_PutPositionInMap(vMap, currLODVertices.m_data[k], stdstr, mystr1f, mystr3f, viArray);
//		}
//
//
//		//miGPUMeshInfo mi;
//		//mi.m_meshPtr = meshCPU;
//		//m_meshGPU0[i] = miCreateGPUMesh(&mi);
//		
//		//m_meshCPU1[i] = miCreate<miMesh>();
//		//meshCPU = m_meshCPU1[i];
//	//	vPtr = (TerrainVertex*)meshCPU->m_vertices;
//	
//	//	for (u32 k = 0; k < meshCPU->m_vCount; ++k)
//	//	{
//	//		vPtr[k].Position.x += width;
//	//		vPtr[k].Position.y = MapCell_getY(&vPtr[k], m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//		//	vArray.push_back(vPtr[k]);
//	//		m_aabb.add(vPtr[k].Position);
//	//	}
//		//m_meshGPU1[i] = miCreateGPUMesh(&mi);
//
//		//m_meshCPU2[i] = miCreate<miMesh>();
//		//meshCPU = m_meshCPU2[i];
//	//	vPtr = (TerrainVertex*)meshCPU->m_vertices;
//	//	for (u32 k = 0; k < meshCPU->m_vCount; ++k)
//	//	{
//	//		vPtr[k].Position.z += width;
//	//		vPtr[k].Position.y = MapCell_getY(&vPtr[k], m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//		//	vArray.push_back(vPtr[k]);
//	//		m_aabb.add(vPtr[k].Position);
//	//	}
//		//m_meshGPU2[i] = miCreateGPUMesh(&mi);
//
//		//m_meshCPU3[i] = miCreate<miMesh>();
//		//meshCPU = m_meshCPU3[i];
//	//	vPtr = (TerrainVertex*)meshCPU->m_vertices;
//	//	for (u32 k = 0; k < meshCPU->m_vCount; ++k)
//	//	{
//	//		vPtr[k].Position.x -= width;
//	//		vPtr[k].Position.y = MapCell_getY(&vPtr[k], m_position, leftTop, mapszX1, mapszY1, imgX1, imgY1, hm);
//		//	vArray.push_back(vPtr[k]);
//	//		m_aabb.add(vPtr[k].Position);
//	//	}
//		//m_meshGPU3[i] = miCreateGPUMesh(&mi);
//	}
//	
//
//	miStringA str;
//	str += "../data/world/";
//	str += iy;
//	str += "/";
//	std::filesystem::create_directory(str.data());
//	str += ix;
//	str += "/";
//	std::filesystem::create_directory(str.data());
//	str += ix;
//	str += ".lods";
//
//	
//	FILE* f = fopen(str.data(), "wb");
//	//head.m_aabb = m_aabb;
//	head.m_position = m_position;
//	/*for (u32 i = 0; i < 4; ++i) {
//		head.m_positionInWorld[i] = m_positionInWorld[i];
//	}
//
//	head.m_vBuffersSizeUncompressed = vArray.size() * sizeof(TerrainVertex);
//	head.m_iBuffersSizeUncompressed = iArray.size() * sizeof(u16);*/
//
//	{
//		head.m_globalVertexIndexBufferSize_uncompressed = viArray.m_size * sizeof(u32);
//		u32 srcSize = head.m_globalVertexIndexBufferSize_uncompressed;
//		auto compressBound = miGetCompressBound(srcSize);
//		u8* compBufV = (u8*)malloc(compressBound);
//		u32 compSize = compressBound;
//		miCompress(viArray.m_data, srcSize, compBufV, &compSize);
//		head.m_globalVertexIndexBufferSize_compressed = compSize;
//
//		fwrite(&head, sizeof(LODsHeader), 1, f);
//		fwrite(compBufV, head.m_globalVertexIndexBufferSize_compressed, 1, f);
//
//		free(compBufV);
//	}
//
//	fclose(f);
//}
