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

#ifdef MG_PLATFORM_WINDOWS
#include <Windows.h>
#include "mgCursorWin32.h"
#endif

MG_API
mgCursor* MG_C_DECL
mgCreateCursor_f(const wchar_t* fileName)
{
	assert(fileName);
	mgCursor* newCursor = malloc(sizeof(mgCursor));
#ifdef MG_PLATFORM_WINDOWS
	newCursor->implementation = malloc(sizeof(mgCursorWin32));
	mgCursorWin32* ptr = (mgCursorWin32*)newCursor->implementation;
	ptr->m_handle = LoadImageW(GetModuleHandle(0), fileName, IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
#else
#error For Windows
#endif
	return newCursor;
}

MG_API
void MG_C_DECL
mgDestroyCursor_f(mgCursor* c)
{
	assert(c);
	if (c->implementation)
		free(c->implementation);
	free(c);
}

MG_API
void MG_C_DECL
mgSetCursor_f(mgContext* ctx, mgCursor* c, unsigned int type)
{
	assert(c);
	assert(type < mgCursorType__count);
	ctx->currentCursors[type] = c;
	SetCursor(((mgCursorWin32*)c->implementation)->m_handle);
}

void
mgInitDefaultCursors(mgContext* c)
{
	for (int i = 0; i < mgCursorType__count; ++i)
	{
		c->defaultCursors[i] = malloc(sizeof(mgCursor));
		c->defaultCursors[i]->implementation = malloc(sizeof(mgCursorWin32));
		mgCursorWin32* cur = (mgCursorWin32*)c->defaultCursors[i]->implementation;

		switch (i)
		{
		default:
		case mgCursorType_Arrow: cur->m_handle = LoadCursor(0, IDC_ARROW); break;
		case mgCursorType_Cross: cur->m_handle = LoadCursor(0, IDC_CROSS); break;
		case mgCursorType_Hand: cur->m_handle = LoadCursor(0, IDC_HAND); break;
		case mgCursorType_Help: cur->m_handle = LoadCursor(0, IDC_HELP); break;
		case mgCursorType_IBeam: cur->m_handle = LoadCursor(0, IDC_IBEAM); break;
		case mgCursorType_No: cur->m_handle = LoadCursor(0, IDC_NO); break;
		case mgCursorType_Size: cur->m_handle = LoadCursor(0, IDC_SIZEALL); break;
		case mgCursorType_SizeNESW: cur->m_handle = LoadCursor(0, IDC_SIZENESW); break;
		case mgCursorType_SizeNS: cur->m_handle = LoadCursor(0, IDC_SIZENS); break;
		case mgCursorType_SizeNWSE: cur->m_handle = LoadCursor(0, IDC_SIZENWSE); break;
		case mgCursorType_SizeWE: cur->m_handle = LoadCursor(0, IDC_SIZEWE); break;
		case mgCursorType_UpArrow: cur->m_handle = LoadCursor(0, IDC_UPARROW); break;
		case mgCursorType_Wait: cur->m_handle = LoadCursor(0, IDC_WAIT); break;
		}

		c->currentCursors[i] = c->defaultCursors[i];
	}
}

void
mgDestroyDefaultCursors(mgContext* c)
{
	for (int i = 0; i < mgCursorType__count; ++i)
	{
		free(c->defaultCursors[i]->implementation);
		free(c->defaultCursors[i]);
	}
}