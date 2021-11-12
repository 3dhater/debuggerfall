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

#include "mixer.lib.h"
#include "mixer.lib.inputContext.h"
#include "MapCell.h"
#include "dpk.h"

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

void* create_new_cell(size_t* out_size)
{
	TerrainVertex vertices[4];
	u16 indices[6];

	miArray<u8> uncompData;
	uncompData.reserve(4*sizeof(TerrainVertex) + (6*sizeof(u16)));

	v3f pos[4];
	
	LODHeader m_header;
	m_header.m_vCount = 4;
	m_header.m_iCount = 6;

	pos[0].set(-0.05f, 0.f, -0.05f);
	pos[1].set(-0.05f, 0.f, 0.05f);
	pos[2].set(0.05f, 0.f, 0.05f);
	pos[3].set(0.05f, 0.f, -0.05f);

	vertices[0] = TerrainVertex(pos[0], v3f(0.f, 1.f, 0.f), v2f(0.f, 0.f), v2f());
	vertices[1] = TerrainVertex(pos[1], v3f(0.f, 1.f, 0.f), v2f(0.f, 1.f), v2f());
	vertices[2] = TerrainVertex(pos[2], v3f(0.f, 1.f, 0.f), v2f(1.f, 1.f), v2f());
	vertices[3] = TerrainVertex(pos[3], v3f(0.f, 1.f, 0.f), v2f(1.f, 0.f), v2f());
		
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	uncompData.clear();
	for (u32 i = 0; i < 4; ++i)
	{
		auto ptr = (u8*)(&vertices[i]);
		for (u32 i2 = 0; i2 < sizeof(TerrainVertex); ++i2)
		{
			uncompData.push_back(ptr[i2]);
		}
	}
	for (u32 i = 0; i < 6; ++i)
	{
		auto ptr = (u8*)(&indices[i]);
		for (u32 i2 = 0; i2 < sizeof(u16); ++i2)
		{
			uncompData.push_back(ptr[i2]);
		}
	}

	m_header.m_uncompressedSize = uncompData.size();

	auto compressBound = miGetCompressBound(m_header.m_uncompressedSize);
	u8* compBuf = (u8*)malloc(compressBound);
	m_header.m_compressedSize = compressBound;
	miCompress(uncompData.m_data, m_header.m_uncompressedSize, compBuf, &m_header.m_compressedSize);

	miArray<u8> outData;
	outData.reserve(uncompData.capacity());
	
	u8* headerPtr = (u8*)&m_header;
	for (u32 i = 0; i < sizeof(LODHeader); ++i)
	{
		outData.push_back(headerPtr[i]);
	}
	for (u32 i = 0; i < m_header.m_compressedSize; ++i)
	{
		outData.push_back(compBuf[i]);
	}

	//wchar_t* wbuf = (wchar_t*)compBuf;

	//dpkCell.m_data = compBuf;
	free(compBuf);

	*out_size = outData.m_size;

	void* newCell = malloc(outData.m_size);
	memcpy(newCell, outData.m_data, outData.m_size);
	/*for (u32 i = 0; i < m_header.m_compressedSize; ++i)
	{
		printf("%x ", outData.m_data[i]);
	}
	printf("\n\n");*/
	return newCell;
}

void gen_regions()
{
	for (u32 iy = 0; iy < 10; ++iy)
	{
		for (u32 ix = 0; ix < 10; ++ix)
		{
			miStringA str;
			str += "../data/world/";
			str += iy;
			str += ".";
			str += ix;
			str += ".dpk";
	
			dpk_file dpk;
			dpk_init(&dpk);

			auto res = dpk_open(str.data(), &dpk);
			if (res == DPK_ER_OK)
			{

				size_t newCellSize = 0;
				auto newCell = create_new_cell(&newCellSize);
				if (newCell)
				{
					dpk_add_data(&dpk, newCell, newCellSize, newCellSize, DPK_CMP_NOCOMPRESS, "0.cll");
					free(newCell);
				}

				res = dpk_save(&dpk);
				if (res != DPK_ER_OK)
				{
					printf("DPK SAVE ERROR: %i\n", res);
				}
				dpk_close(&dpk);
			}
		}
	}
	
}

int main(int argc, char* argv[])
{
	printf("Commands:\n");
	//printf("\"-gen_basic_cells\" - create 10'00x10'00 cells. It will erase old cells.\n");
	//printf("\"-gen_cells_masks\" - create mask/PNG files for each cell.\n");
	printf("\"-gen_regions\" - create regions.\n");
	printf("\n");

	miInputContext* m_inputContext = nullptr;
	miLibContext* m_libContext = nullptr;

	m_inputContext = miCreate<miInputContext>();
	m_libContext = miCreate<miLibContext>();
	m_libContext->Start(m_inputContext);

	for (int i = 0; i < argc; ++i)
	{
		/*if (strcmp(argv[i], "-vid") == 0)
		{
			++i;
			if (i < argc)
			{
				videoDriverTypeStr = argv[i];
			}
		}*/

		if (strcmp(argv[i], "-gen_regions") == 0)
		{
			gen_regions();
		}

		/*if (strcmp(argv[i], "-gen_basic_cells") == 0)
		{
			gen_basic_cells();
		}
		else if (strcmp(argv[i], "-gen_cells_masks") == 0)
		{
			gen_cells_masks();
		}*/


	}

	if (m_libContext)
		miDestroy(m_libContext);
	if (m_inputContext)
		miDestroy(m_inputContext);

	return 0;
}