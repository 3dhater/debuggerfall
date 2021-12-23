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

#include "miGUI.h"
#include <stdio.h>
#include <string.h>

typedef struct TextureInfo_s
{
	/*range*/
	mgFontGlyph* fromThis;
	int num;
} TextureInfo;

struct dds_pixelformat
{
	unsigned int size;
	unsigned int flags;
	unsigned int fourCC;
	unsigned int RGBBitCount;
	unsigned int RBitMask;
	unsigned int GBitMask;
	unsigned int BBitMask;
	unsigned int ABitMask;
};

struct dds_header
{
	unsigned int size;
	unsigned int flags;
	unsigned int height;
	unsigned int width;
	unsigned int pitchOrLinearSize;
	unsigned int depth;
	unsigned int mipMapCount;
	unsigned int reserved1[11];
	struct dds_pixelformat ddspf;
	unsigned int caps;
	unsigned int caps2;
	unsigned int caps3;
	unsigned int caps4;
	unsigned int reserved2;
};

void
WriteBYTE(unsigned char ch, FILE* f)
{
	fwrite(&ch, sizeof(unsigned char), 1, f);
}

void
WriteWCHAR(wchar_t ch, FILE* f)
{
	fwrite(&ch, sizeof(wchar_t), 1, f);
}

void
WriteINT(int i, FILE* f)
{
	char itoaBuf[20];
	_itoa(i, itoaBuf, 10);
	itoaBuf[19] = 0;
	int sz = strlen(itoaBuf);
	for (int i = 0; i < sz; ++i)
	{
		wchar_t ch = itoaBuf[i];
		fwrite(&ch, sizeof(wchar_t), 1, f);
	}
}

void
mgSaveImageToFile(const char* fn, mgImage* img)
{
	struct dds_header head;
	memset(&head, 0, sizeof(struct dds_header));
	head.size = sizeof(struct dds_header);
	head.flags = 0x00000002 | 0x00000004;
	head.width = img->width;
	head.height = img->height;
	head.pitchOrLinearSize = img->dataSize;
	head.mipMapCount = 1;
	head.ddspf.size = sizeof(struct dds_pixelformat);
	head.ddspf.flags = 0x00000041; /*DDS_RGBA*/
	head.ddspf.RGBBitCount = 32;
	head.ddspf.RBitMask = 0x000000ff;
	head.ddspf.GBitMask = 0x0000ff00;
	head.ddspf.BBitMask = 0x00ff0000;
	head.ddspf.ABitMask = 0xff000000;

	FILE* f = fopen(fn, "wb");
	if (f)
	{
		const unsigned int dds_magic = 0x20534444;
		fwrite(&dds_magic, sizeof(unsigned int), 1, f);
		fwrite(&head, head.size, 1, f);
		fwrite(img->data, img->dataSize, 1, f);
		fclose(f);
	}
}

/*Code based on irrlicht font tool*/
#ifdef MG_PLATFORM_WINDOWS
#include <Windows.h>
mgFont*
mgCreateFont_generate_win32(mgContext* c, const char* fn, unsigned int flags, int size, const char* saveIt)
{
	mgFont* newFont = 0;
	LPGLYPHSET glyphset = 0;
	
	mgFontGlyph* fontGlyphs = 0;
	int textureInfoCount = 1;
	
	int textureSize = 512;
	mgImage image;
	image.width = textureSize;
	image.height = textureSize;
	image.dataSize = (image.width * 4) * image.height;
	image.data = malloc(image.dataSize);

	TextureInfo* textureInfoArray = 0;

	FILE* textFile = 0;

	if (saveIt)
	{
		char pathBuffer[2000];
		sprintf(pathBuffer, "../data/fonts/%s/", saveIt);
	
		CreateDirectoryA("../data/", 0);
		CreateDirectoryA("../data/fonts/", 0);
		CreateDirectoryA(pathBuffer, 0);
		
		sprintf(pathBuffer, "../data/fonts/%s/%s.txt", saveIt, saveIt);

		textFile = fopen(pathBuffer, "wb");
		if (textFile)
		{
			/*UTF-16 BOM, little endian*/
			WriteBYTE(0xFF, textFile);
			WriteBYTE(0xFE, textFile);
		}
	}

	HDC hDC = CreateDC(L"DISPLAY", L"DISPLAY", 0, 0);
	HFONT font = CreateFontA(
		-MulDiv(size, GetDeviceCaps(hDC, LOGPIXELSY), 72), // cHeight
		0, // cWidth
		0, // cEscapement
		0, // cOrientation
		(flags & MG_FNTFL_BOLD) ? FW_BOLD : FW_NORMAL,
		(flags & MG_FNTFL_ITALIC) ? 1 : 0,
		(flags & MG_FNTFL_UNDERLINE) ? 1 : 0,
		(flags & MG_FNTFL_STRIKEOUT) ? 1 : 0,
		OEM_CHARSET, // iCharSet
		OUT_DEFAULT_PRECIS, // iOutPrecision
		CLIP_DEFAULT_PRECIS, // iClipPrecision
		ANTIALIASED_QUALITY, // iQuality
		VARIABLE_PITCH | FF_DONTCARE, // iPitchAndFamily
		fn);

	if (!font)
		goto end;

	SelectObject(hDC, font);
	SetTextAlign(hDC, TA_LEFT | TA_TOP | TA_NOUPDATECP);

	DWORD glyphsetSize = GetFontUnicodeRanges(hDC, 0);
	if (!glyphsetSize)
		goto end;

	glyphset = malloc(glyphsetSize * sizeof(LPGLYPHSET));
	GetFontUnicodeRanges(hDC, glyphset);
	
	/*int maxWidth = 0;*/
	int numOfChars = 0;
	for (DWORD range = 0; range < glyphset->cRanges; ++range)
	{
		WCRANGE* currentRange = &glyphset->ranges[range];
		for (WCHAR ch = currentRange->wcLow; ch < currentRange->wcLow + currentRange->cGlyphs; ++ch)
		{
			if (IsDBCSLeadByte((BYTE)ch))
				continue;

			ABC abc;
			if (GetCharABCWidthsW(hDC, ch, ch, &abc))
			{
				if (abc.abcB - abc.abcA + abc.abcC < 1)
					continue;
				/*if (abc.abcB > maxWidth)
					maxWidth = abc.abcB;*/
			}
			
			SIZE size;
			GetTextExtentPoint32W(hDC, &ch, 1, &size);
			if (size.cy < 1)
				continue;

			++numOfChars;
		}
	}

	int curPosX = 0;
	int curPosY = 0;
	int textureSlot = 0;
	int maxY = 0;
	
	fontGlyphs = calloc(1, sizeof(mgFontGlyph) * numOfChars);
	mgFontGlyph* currGlyph = fontGlyphs;

	textureInfoArray = calloc(1, sizeof(TextureInfo) * 100);
	TextureInfo* currTextureInfo = textureInfoArray;
	currTextureInfo->fromThis = fontGlyphs;
	
	float uvPerPixel = 1.f / (float)textureSize;

	for (DWORD range = 0; range < glyphset->cRanges; ++range)
	{
		WCRANGE* currentRange = &glyphset->ranges[range];
		for (WCHAR ch = currentRange->wcLow; ch < currentRange->wcLow + currentRange->cGlyphs; ++ch)
		{
			if (IsDBCSLeadByte((BYTE)ch))
				continue;

			ABC abc;
			if (GetCharABCWidthsW(hDC, ch, ch, &abc))
			{
				if (abc.abcB - abc.abcA + abc.abcC < 1)
					continue;
			}

			SIZE size;
			GetTextExtentPoint32W(hDC, &ch, 1, &size);
			if (size.cy < 1)
				continue;
			
			size.cx = abc.abcB;

			if (curPosX + size.cx > textureSize)
			{
				curPosY += maxY;
				curPosX = 0;
				if ((curPosY + maxY) > textureSize)
				{
					textureSlot++;
					currTextureInfo++;
					currTextureInfo->fromThis = currGlyph;
					textureInfoCount++;
					curPosY = 0;
				}
				maxY = 0;
			}

			currGlyph->underhang = abc.abcA;
			currGlyph->overhang = abc.abcC;
			currGlyph->width = abc.abcB;
			currGlyph->height = size.cy;
			currGlyph->symbol = ch;
			currGlyph->textureSlot = textureSlot;
			//if (ch == L'K')
			//	printf(".");

			currGlyph->rect.left = curPosX;
			currGlyph->rect.top = curPosY;
			currGlyph->rect.right = curPosX + size.cx;
			currGlyph->rect.bottom = curPosY + size.cy;

			currGlyph->UV.x = (float)currGlyph->rect.left * uvPerPixel;
			currGlyph->UV.y = (float)currGlyph->rect.top * uvPerPixel;
			currGlyph->UV.z = (float)currGlyph->rect.right * uvPerPixel;
			currGlyph->UV.w = (float)currGlyph->rect.bottom * uvPerPixel;

			curPosX += size.cx + 1;

			if (size.cy + 1 > maxY)
				maxY = size.cy + 1;

			currTextureInfo->num++;
			currGlyph++;
		}
	}
	newFont = calloc(1, sizeof(mgFont));
	newFont->glyphNum = numOfChars;
	newFont->glyphs = fontGlyphs;
	newFont->implementation = malloc(textureInfoCount * sizeof(mgFontBitmap));
	newFont->textureCount = textureInfoCount;
	

	for (int i = 0; i < textureInfoCount; ++i)
	{
		HBITMAP bmp = CreateCompatibleBitmap(hDC, textureSize, textureSize);
		HDC bmpdc = CreateCompatibleDC(hDC);
		LOGBRUSH lbrush;
		lbrush.lbHatch = 0;
		lbrush.lbStyle = BS_SOLID;
		lbrush.lbColor = RGB(0, 0, 0);
		HBRUSH brush = CreateBrushIndirect(&lbrush);
		HPEN pen = CreatePen(PS_NULL, 0, 0);

		SelectObject(bmpdc, bmp);
		SelectObject(bmpdc, pen);
		SelectObject(bmpdc, brush);
		SelectObject(bmpdc, font);

		SetTextColor(bmpdc, RGB(255, 255, 255));
		Rectangle(bmpdc, 0, 0, textureSize, textureSize);
		SetBkMode(bmpdc, TRANSPARENT);

		
		for (int j = 0; j < textureInfoArray[i].num; ++j)
		{
			mgFontGlyph* currGlyph = &textureInfoArray[i].fromThis[j];
			newFont->glyphMap[currGlyph->symbol] = currGlyph;

			TextOutW(
				bmpdc, 
				currGlyph->rect.left - currGlyph->underhang,
				currGlyph->rect.top,
				&currGlyph->symbol,
				1);

			if (textFile)
			{
				wchar_t space = ' ';
				wchar_t newLine = '\n';

				WriteWCHAR(currGlyph->symbol, textFile);
				WriteWCHAR(L' ', textFile);
				
				WriteINT(currGlyph->rect.left, textFile);
				WriteWCHAR(L' ', textFile);

				WriteINT(currGlyph->rect.top, textFile);
				WriteWCHAR(L' ', textFile);

				WriteINT(currGlyph->rect.right, textFile);
				WriteWCHAR(L' ', textFile);

				WriteINT(currGlyph->rect.bottom, textFile);
				WriteWCHAR(L' ', textFile);

				WriteINT(currGlyph->underhang, textFile);
				WriteWCHAR(L' ', textFile);

				WriteINT(currGlyph->overhang, textFile);
				WriteWCHAR(L' ', textFile);

				WriteINT(currGlyph->textureSlot, textFile);
				WriteWCHAR(L' ', textFile);
				
				WriteWCHAR(L'\r', textFile);
				WriteWCHAR(L'\n', textFile);
			}
		}

		BITMAP b;
		GetObject(bmp, sizeof(BITMAP), (LPSTR)&b);
		WORD cClrBits = (WORD)(b.bmPlanes * b.bmBitsPixel);
		PBITMAPINFO pbmi = (PBITMAPINFO)LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = b.bmWidth;
		pbmi->bmiHeader.biHeight = b.bmHeight;
		pbmi->bmiHeader.biPlanes = b.bmPlanes;
		pbmi->bmiHeader.biBitCount = b.bmBitsPixel;
		pbmi->bmiHeader.biCompression = BI_RGB;
		pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8 * pbmi->bmiHeader.biHeight;
		pbmi->bmiHeader.biClrImportant = 0;

		LPBYTE lpBits;
		PBITMAPINFOHEADER pbih = (PBITMAPINFOHEADER)pbmi;
		lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);
		auto r = GetDIBits(hDC, bmp, 0, (WORD)pbih->biHeight, lpBits, pbmi, DIB_RGB_COLORS);
		int rowsize = ((pbmi->bmiHeader.biWidth * cClrBits + 31) & ~31) / 8;
		char* row = malloc(rowsize);
		for (int j = 0; j < (pbih->biHeight / 2); ++j)
		{
			memcpy(row, lpBits + (rowsize * j), rowsize);
			memcpy(lpBits + (rowsize * j), lpBits + ((pbih->biHeight - 1 - j) * rowsize), rowsize);
			memcpy(lpBits + ((pbih->biHeight - 1 - j) * rowsize), row, rowsize);
		}
		free(row);

		if (cClrBits > 24)
		{
			memset(image.data, 0xFF, image.dataSize);
			unsigned char * imageData = image.data;

			for (LPBYTE m = lpBits; m < lpBits + pbih->biSizeImage; m += 4)
			{
				*imageData = 255; ++imageData;
				*imageData = 255; ++imageData;
				*imageData = 255; ++imageData;

				*imageData = m[0]; ++imageData; /*A*/
			}
		}

		if(saveIt)
		{
			char pathBuffer[2000];
			sprintf(pathBuffer, "../data/fonts/%s/%i.dds", saveIt, i);
			mgSaveImageToFile(pathBuffer, &image);
		}

		((mgFontBitmap*)newFont->implementation)[i].gpuTexture = c->gpu->createTexture(&image);

		LocalFree(pbmi);
		GlobalFree(lpBits);
		DeleteDC(bmpdc);
		DeleteObject(brush);
		DeleteObject(pen);
		DeleteObject(bmp);
	}

end:
	if (image.data) free(image.data);
	if (textureInfoArray) free(textureInfoArray);
	if (fontGlyphs && !newFont) free(fontGlyphs);
	if (glyphset) free(glyphset);
	if (font) DeleteObject(font);
	if (hDC) DeleteDC(hDC);
	if (textFile) fclose(textFile);
	return newFont;
}
#endif

mgFont*
mgCreateFont_generate(mgContext* c, const char* fn, unsigned int flags, int size, const char* saveIt)
{
#ifdef MG_PLATFORM_WINDOWS
	return mgCreateFont_generate_win32(c, fn, flags, size, saveIt);
#endif
}

mgFont*
mgCreateFont_read_file(mgContext* c, const char* fn, unsigned int flags, int size, const char* saveIt)
{
	return 0;
}

mgFont*
mgCreateFont_from_file(mgContext* c, const char* fn, unsigned int flags, int size, const char* saveIt)
{
	char * pch = strrchr(fn, '.');
	
	if (!pch)
		return mgCreateFont_generate(c, fn, flags, size, saveIt);

	char* pfn = pch;

	pch = strtok(pfn, " .");
	while (pch != 0)
	{
		if (strcmp(pch, "txt") == 0)
		{
			return mgCreateFont_read_file(c, fn, flags, size, saveIt);
		}
		pch = strtok(0, " .");
	}

	return mgCreateFont_generate(c, fn, flags, size, saveIt);
}

MG_API
mgFont* MG_C_DECL
mgCreateFont_f(mgContext* c, const char* fn, unsigned int flags, int size, const char* saveIt)
{
	FILE* f = fopen(fn, "rb");
	if (f)
	{
		fclose(f);
		return mgCreateFont_from_file(c, fn, flags, size, saveIt);
	}
	else
	{
		return mgCreateFont_generate(c, fn, flags, size, saveIt);
	}
}

MG_API
void MG_C_DECL
mgDestroyFont_f(struct mgContext_s* c, mgFont* f)
{
	assert(f);
	if (f->glyphs) free(f->glyphs);
	if (f->implementation)
	{
		for (int i = 0; i < f->textureCount; ++i)
		{
			c->gpu->destroyTexture(((mgFontBitmap*)f->implementation)[i].gpuTexture);
		}
		free(f->implementation);
	}
}
