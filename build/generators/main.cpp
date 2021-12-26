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

#include <cstdio>
#include <cstring>
#include <filesystem>

#include "mi/MainSystem/MainSystem.h"
#include "mi/Mesh/mesh.h"
#include "miGUI.h"
#include "miGUILoader.h"
#include "MapCell.h"
#include "dpk.h"

// создать базовый меш для ячеек.
void create_cell_base()
{
	s32 quadNum = 100;
	f32 sizeHalf = 0.1f * 0.5f;

	miMesh * m_cellTemplate = new miMesh;
	m_cellTemplate->m_indexType = miMeshIndexType::u32;
	m_cellTemplate->m_vertexType = miMeshVertexType::Triangle;
	m_cellTemplate->m_aabb.reset();
	m_cellTemplate->m_vCount = (quadNum + 1) * (quadNum + 1);
	m_cellTemplate->m_iCount = (quadNum * quadNum) * 2 * 3;
	m_cellTemplate->m_stride = sizeof(miVertexTriangle);
	printf("iCount: %i\n", m_cellTemplate->m_iCount);
	m_cellTemplate->m_vertices = (u8*)miMalloc(m_cellTemplate->m_stride * m_cellTemplate->m_vCount);
	m_cellTemplate->m_indices = (u8*)miMalloc(sizeof(u32) * m_cellTemplate->m_iCount);

	miVertexTriangle* vPtr = (miVertexTriangle*)m_cellTemplate->m_vertices;
	u32* iPtr = (u32*)m_cellTemplate->m_indices;

	f32 quadSize = 0.001f;
	u32 vertexIndexCounter = 0;

	v3f pos(-sizeHalf, 0.f, -sizeHalf);	

	for (u32 k = 0; k < quadNum + 1; ++k)
	{
		for (u32 o = 0; o < quadNum + 1; ++o)
		{
			vPtr->Position.x = pos.x;
			vPtr->Position.z = pos.z;
			vPtr++;

			pos.x += quadSize;
		}
		pos.z += quadSize;

		pos.x = -sizeHalf;
	}

	u32 i1 = 0;
	u32 i2 = quadNum + 1;
	for (u32 k = 0; k < quadNum; ++k)
	{
		for (u32 o = 0; o < quadNum; ++o)
		{
			*iPtr = i1; iPtr++;
			*iPtr = i1 + 1; iPtr++;
			*iPtr = i2 + 1; iPtr++;
			
			*iPtr = i1; iPtr++;
			*iPtr = i2 + 1; iPtr++;
			*iPtr = i2; iPtr++;

			i1++;
			i2++;
		}

		i1++;
		i2++;
	}

	FILE* f = fopen("../data/world/cellbase.bin", "wb");
	if (f)
	{
		fwrite(m_cellTemplate, sizeof(miMesh), 1, f);
		fwrite(m_cellTemplate->m_vertices, m_cellTemplate->m_stride * m_cellTemplate->m_vCount, 1, f);
		fwrite(m_cellTemplate->m_indices, sizeof(u32) * m_cellTemplate->m_iCount, 1, f);
		fclose(f);
	}

	delete m_cellTemplate;
}

void gen_basic_cells()
{
	miStringA str;
	str += "../data/world/";
	str += "gen.dpk";

	if (std::filesystem::exists(str.data()))
		std::filesystem::remove(str.data());

	dpk_file dpk;
	memset(&dpk, 0, sizeof(dpk_file));

	auto res = dpk_open(str.data(), &dpk);
	if (res == DPK_ER_OK)
	{
		for (u32 iy = 0; iy < 1000; ++iy)
		{
			for (u32 ix = 0; ix < 1000; ++ix)
			{
				printf("Create cell %i.%i\n", iy, ix);

				miStringA name;
				name += ix;
				int data = 0;
				dpk_add_data(&dpk, &data, sizeof(data), sizeof(data), DPK_CMP_NOCOMPRESS, name.data());
			}
		}
		res = dpk_save(&dpk);
		if (res != DPK_ER_OK)
		{
			printf("DPK SAVE ERROR: %i\n", res);
		}
		dpk_close(&dpk);
	}
}

//void gen_cells_masks()
//{
//	miImage image;
//	image.create(2048, 2048);
//	image.fill(ColorBlack);
//
//	for (u32 iy = 0; iy < 1; ++iy)
//	{
//		printf("Line: %u\n", iy);
//
//		for (u32 ix = 0; ix < 1; ++ix)
//		{
//			miStringA str;
//			str += "../data/world/";
//			str += iy;
//			str += "/";
//			std::filesystem::create_directory(str.data());
//			str += ix;
//			str += "/";
//			std::filesystem::create_directory(str.data());
//			str += ix;
//			str += ".png";
//
//			miString wstr;
//			wstr += str.data();
//			
//			printf("PNG: %u\n", ix);
//
//			miSaveImage(wstr.data(), &image);
//		}
//	}
//}

//void* create_new_cell(size_t* out_size)
//{
//	TerrainVertex vertices[4];
//	u16 indices[6];
//
//	miArray<u8> uncompData;
//	uncompData.reserve(4*sizeof(TerrainVertex) + (6*sizeof(u16)));
//
//	v3f pos[4];
//	
//	LODHeader m_header;
//	m_header.m_vCount = 4;
//	m_header.m_iCount = 6;
//
//	pos[0].set(-0.01f, 0.f, -0.01f);
//	pos[1].set(-0.01f, 0.f, 0.01f);
//	pos[2].set(0.01f, 0.f, 0.01f);
//	pos[3].set(0.01f, 0.f, -0.01f);
//
//	vertices[0] = TerrainVertex(pos[0], v3f(0.f, 1.f, 0.f), v2f(0.f, 0.f), v2f());
//	vertices[1] = TerrainVertex(pos[1], v3f(0.f, 1.f, 0.f), v2f(0.f, 1.f), v2f());
//	vertices[2] = TerrainVertex(pos[2], v3f(0.f, 1.f, 0.f), v2f(1.f, 1.f), v2f());
//	vertices[3] = TerrainVertex(pos[3], v3f(0.f, 1.f, 0.f), v2f(1.f, 0.f), v2f());
//		
//	indices[0] = 0;
//	indices[1] = 1;
//	indices[2] = 2;
//	indices[3] = 0;
//	indices[4] = 2;
//	indices[5] = 3;
//
//	uncompData.clear();
//	for (u32 i = 0; i < 4; ++i)
//	{
//		auto ptr = (u8*)(&vertices[i]);
//		for (u32 i2 = 0; i2 < sizeof(TerrainVertex); ++i2)
//		{
//			uncompData.push_back(ptr[i2]);
//		}
//	}
//	for (u32 i = 0; i < 6; ++i)
//	{
//		auto ptr = (u8*)(&indices[i]);
//		for (u32 i2 = 0; i2 < sizeof(u16); ++i2)
//		{
//			uncompData.push_back(ptr[i2]);
//		}
//	}
//
//	m_header.m_uncompressedSize = uncompData.size();
//
//	auto compressBound = miGetCompressBound(m_header.m_uncompressedSize);
//	u8* compBuf = (u8*)malloc(compressBound);
//	m_header.m_compressedSize = compressBound;
//	miCompress(uncompData.m_data, m_header.m_uncompressedSize, compBuf, &m_header.m_compressedSize);
//
//	miArray<u8> outData;
//	outData.reserve(uncompData.capacity());
//	
//	u8* headerPtr = (u8*)&m_header;
//	for (u32 i = 0; i < sizeof(LODHeader); ++i)
//	{
//		outData.push_back(headerPtr[i]);
//	}
//	for (u32 i = 0; i < m_header.m_compressedSize; ++i)
//	{
//		outData.push_back(compBuf[i]);
//	}
//
//	//wchar_t* wbuf = (wchar_t*)compBuf;
//
//	//dpkCell.m_data = compBuf;
//	free(compBuf);
//
//	*out_size = outData.m_size;
//
//	void* newCell = malloc(outData.m_size);
//	memcpy(newCell, outData.m_data, outData.m_size);
//	/*for (u32 i = 0; i < m_header.m_compressedSize; ++i)
//	{
//		printf("%x ", outData.m_data[i]);
//	}
//	printf("\n\n");*/
//	return newCell;
//}

//void gen_regions()
//{
//	miImage image;
//	image.create(400, 400);
//	image.fill(ColorBlack);
//
//	for (u32 iy = 0; iy < 10; ++iy)
//	{
//		for (u32 ix = 0; ix < 10; ++ix)
//		{
//			miStringA str;
//			str += "../data/world/";
//			str += iy;
//			str += ".";
//			str += ix;
//			str += ".dpk";
//
//			if (std::filesystem::exists(str.data()))
//				std::filesystem::remove(str.data());
//			dpk_file dpk;
//			memset(&dpk, 0, sizeof(dpk_file));
//
//			auto res = dpk_open(str.data(), &dpk);
//			if (res == DPK_ER_OK)
//			{
//				printf("Create region %i.%i\n", iy, ix);
//
//				for (int i1 = 0; i1 < 500; ++i1)
//				{
//					for (int i2 = 0; i2 < 500; ++i2)
//					{
//
//							size_t newCellSize = 0;
//							auto newCell = create_new_cell(&newCellSize);
//							if (newCell)
//							{
//								miStringA n;
//								n += i1;
//								n += ".";
//								n += i2;
//								n += ".c0";
//								dpk_add_data(&dpk, newCell, newCellSize, newCellSize, DPK_CMP_NOCOMPRESS, n.data());
//
//								n.clear();
//								n += i1;
//								n += ".";
//								n += i2;
//								n += ".c1";
//								dpk_add_data(&dpk, newCell, newCellSize, newCellSize, DPK_CMP_NOCOMPRESS, n.data());
//
//								free(newCell);
//							}
//
//					}
//				}
//
//				res = dpk_save(&dpk);
//				if (res != DPK_ER_OK)
//				{
//					printf("DPK SAVE ERROR: %i\n", res);
//				}
//				dpk_close(&dpk);
//			}
//		}
//	}
//}

void print_dpk(const char* fileName)
{
	dpk_file dpk;
	memset(&dpk, 0, sizeof(dpk_file));

	auto res = dpk_open(fileName, &dpk);
	if (res == DPK_ER_OK)
	{
		printf("DPK file name: %s\n", dpk.file_name);
		printf("DPK file version: %i\n", dpk.header.version);		
		printf("DPK data num: %i\n", dpk.header.data_num);
		
		dpk_data_node* curr = dpk.data;
		dpk_data_node* last = dpk.data->left;
		while (true)
		{
			printf("\t-data: %s, size: %i\n", curr->header.name, curr->header.comp_size);
			
			if (curr == last)
				break;
			curr = curr->right;
		}
		dpk_close(&dpk);
	}
}

void create_cells_bdata()
{
	FILE* f = fopen("../data/world/b.bin", "wb");
	if (f)
	{
		CellBaseData bd;
		int id = 0;
		f32 pos_x = -500.f;
		f32 pos_z = -500.f;
		for (int y = 0; y < 1000; ++y)
		{
			for (int x = 0; x < 1000; ++x)
			{
				bd.ids[0] = id;
				bd.ids[1] = bd.ids[0] + 1;
				bd.ids[2] = bd.ids[0] - 1;

				bd.ids[3] = bd.ids[0] + 1000 + 1;
				bd.ids[4] = bd.ids[3] - 1;
				bd.ids[5] = bd.ids[4] - 1;

				bd.ids[6] = bd.ids[0] - 1000 + 1;
				bd.ids[7] = bd.ids[6] - 1;
				bd.ids[8] = bd.ids[7] - 1;

				if (x == 0)
				{
					bd.ids[5] = -1;
					bd.ids[2] = -1;
					bd.ids[8] = -1;
				}
				else if (x == 999)
				{
					bd.ids[3] = -1;
					bd.ids[1] = -1;
					bd.ids[6] = -1;
				}

				if (y == 0)
				{
					bd.ids[6] = -1;
					bd.ids[7] = -1;
					bd.ids[8] = -1;
				}
				else if (y == 999)
				{
					bd.ids[3] = -1;
					bd.ids[4] = -1;
					bd.ids[5] = -1;
				}

				// позиции ячеек начинаются с самого отрицательного значения.
				bd.pos[0] = pos_x;
				bd.pos[1] = pos_z;

				pos_x += 1.f;

				++id;
				fwrite(&bd, sizeof(CellBaseData), 1, f);
			}
			pos_x = -500.f;
			pos_z += 1.f;
		}

		fclose(f);
	}
}

int main(int argc, char* argv[])
{
	printf("Commands:\n");
	printf("\"-gen_basic_cells\" - create 1km x 1km cells. It will erase old cells.\n");
	//printf("\"-gen_cells_masks\" - create mask/PNG files for each cell.\n");
	//printf("\"-gen_regions\" - create regions.\n");
	printf("\"-print_dpk \"dpk file\" \" - print information about dpk file.\n");
	printf("\"-create_cells_bdata - create base data b.bin .\n");
	printf("\"-create_cell_base - create base mesh for cells.\n");
	printf("\n");

	MG_LIB_HANDLE gui_lib = mgLoad();
	if (!gui_lib)
	{
		MessageBoxA(0, "Can't load migui.dll", "Error", MB_OK);
		return 0;
	}

	mgInputContext* m_inputContext = miCreate<mgInputContext>();
	memset(m_inputContext, 0, sizeof(mgInputContext));

	mgVideoDriverAPI * m_guiGPU = new mgVideoDriverAPI;
	m_guiGPU->createTexture = 0;
	m_guiGPU->destroyTexture = 0;
	m_guiGPU->beginDraw = 0;
	m_guiGPU->endDraw = 0;
	m_guiGPU->drawRectangle = 0;
	m_guiGPU->drawText = 0;
	m_guiGPU->setClipRect = 0;

	auto m_guiContext = mgCreateContext(m_guiGPU, m_inputContext);

	auto m_mainSystem = miGetMainSystem(m_guiContext, m_inputContext);

	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "-print_dpk") == 0)
		{
			++i;
			if (i < argc)
				print_dpk(argv[i]);
		}
		//else if (strcmp(argv[i], "-gen_regions") == 0)
			//gen_regions();

		if (strcmp(argv[i], "-gen_basic_cells") == 0)
		{
			gen_basic_cells();
		}
		else if (strcmp(argv[i], "-create_cells_bdata") == 0)
		{
			create_cells_bdata();
		}
		else if (strcmp(argv[i], "-create_cell_base") == 0)
		{
			create_cell_base();
		}
		/*else if (strcmp(argv[i], "-gen_cells_masks") == 0)
		{
			gen_cells_masks();
		}*/
		char x = 'z';

	}
	
	if (m_mainSystem)
		m_mainSystem->Release();

	if (m_guiContext)
		mgDestroyContext(m_guiContext);

	if (m_guiGPU)
		delete m_guiGPU;

	if (m_inputContext)
		miDestroy(m_inputContext);
	
	mgUnload(gui_lib);

	return 0;
}