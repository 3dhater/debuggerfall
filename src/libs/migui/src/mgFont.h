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

#ifndef _MG_FONT_H_
#define _MG_FONT_H_

enum {
	MG_FNTFL_BOLD = 1,
	MG_FNTFL_ITALIC = 2,
	MG_FNTFL_UNDERLINE = 4,
	MG_FNTFL_STRIKEOUT = 8,
};

/*bitmap fonts implementation
* It must be inside mgFont_s
*/
typedef struct mgFontBitmap_s {
	void * gpuTexture;
} mgFontBitmap;

typedef struct mgFontGlyph_s
{
	wchar_t symbol;
	mgVec4 UV;
	mgRect rect;
	int underhang;
	int overhang;
	int width;
	int height;
	int textureSlot;
} mgFontGlyph;

typedef struct mgFont_s {

	/*miGUI will create mgFontBitmap_s and will put address here.
	* (this is array, textureCount for size)
	* 
	* but you can create your own font inside your app, just put
	* address here, and then use it as you wish (don't forget to
	* delete your own font).
	*/
	void* implementation; /*it can be HFONT from windows*/
	int textureCount;

	int characterSpacing;
	int spaceSize;
	int tabSize;

	int glyphNum; /*how many chars in this font*/
	mgFontGlyph* glyphs;

	mgFontGlyph* glyphMap[0xFFFF];
} mgFont;

extern mgFont* mgCreateFont(struct mgContext_s*, const char*, unsigned int flags, int size);

#endif