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
#include "miGUILoader.h"
#include <assert.h>

PFNMGCREATECONTEXTPROC mgCreateContext_p;
PFNMGDESTROYCONTEXTPROC mgDestroyContext_p;
PFNMGCREATEFONTPROC mgCreateFont_p;
PFNMGSTARTFRAMEPROC mgStartFrame_p;
PFNMGUPDATEPROC mgUpdate_p;
PFNMGSETPARENTPROC mgSetParent_p;
PFNMGSETVISIBLEPROC mgSetVisible_p;
PFNMGDRAWPROC mgDraw_p;
PFNMGCREATERECTANGLEPROC mgCreateRectangle_p;
PFNMGCREATETEXTPROC mgCreateText_p;
PFNMGCREATEBUTTONPROC mgCreateButton_p;
PFNMGCREATECURSORPROC mgCreateCursor_p;
PFNMGDESTROYCURSORPROC mgDestroyCursor_p;
PFNMGSETCURSORPROC mgSetCursor_p;

void* 
mgGetProc(MG_LIB_HANDLE lib, const char* proc)
{
	void* res = 0;

#ifdef MG_PLATFORM_WINDOWS
	res = GetProcAddress(lib, proc);
#else
#error Need implementation
#endif
	return res;
}

MG_LIB_HANDLE
mgLoad()
{
#ifdef MG_PLATFORM_WINDOWS
	const char* lib_name = "migui.dll";
#else
#error Need implementation
#endif

	MG_LIB_HANDLE lib = MG_LOAD_LIB(lib_name);
	if(!lib)
		return lib;

	mgCreateContext_p = mgGetProc(lib, "mgCreateContext_f");
	mgDestroyContext_p = mgGetProc(lib, "mgDestroyContext_f");
	mgCreateFont_p = mgGetProc(lib, "mgCreateFont_f");
	mgStartFrame_p = mgGetProc(lib, "mgStartFrame_f");
	mgUpdate_p = mgGetProc(lib, "mgUpdate_f");
	mgSetParent_p = mgGetProc(lib, "mgSetParent_f");
	mgSetVisible_p = mgGetProc(lib, "mgSetVisible_f");
	mgDraw_p = mgGetProc(lib, "mgDraw_f");
	mgCreateRectangle_p = mgGetProc(lib, "mgCreateRectangle_f");
	mgCreateText_p = mgGetProc(lib, "mgCreateText_f");
	mgCreateButton_p = mgGetProc(lib, "mgCreateButton_f");
	mgCreateCursor_p = mgGetProc(lib, "mgCreateCursor_f");
	mgDestroyCursor_p = mgGetProc(lib, "mgDestroyCursor_f");
	mgSetCursor_p = mgGetProc(lib, "mgSetCursor_f");

	return lib;
}

void
mgUnload(MG_LIB_HANDLE lib)
{
	assert(lib);
	MG_FREE_LIB(lib);
}