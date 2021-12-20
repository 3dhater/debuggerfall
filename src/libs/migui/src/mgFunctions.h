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

#ifndef _MG_FUNCTIONS_H_
#define _MG_FUNCTIONS_H_

/*
* For DLL
*/

MG_API mgElement* MG_C_DECL mgCreateButton_f(struct mgContext_s* c, mgPoint* position, mgPoint* size, const wchar_t* text, mgFont* font);
MG_API mgContext* MG_C_DECL mgCreateContext_f(mgVideoDriverAPI* gpu, mgInputContext* input);
MG_API mgCursor* MG_C_DECL mgCreateCursor_f(const wchar_t* fileName);
MG_API mgFont* MG_C_DECL mgCreateFont_f(mgContext* c, const char* fn, unsigned int flags, int size);
MG_API mgElement* MG_C_DECL mgCreateRectangle_f(struct mgContext_s* c, mgPoint* position, mgPoint* size, mgColor* color1, mgColor* color2);
MG_API mgElement* MG_C_DECL mgCreateText_f(struct mgContext_s* c, mgPoint* position, const wchar_t* text, mgFont* font);
MG_API void MG_C_DECL mgDestroyContext_f(mgContext* c);
MG_API void MG_C_DECL mgDestroyCursor_f(mgCursor* c);
MG_API void MG_C_DECL mgDraw_f(mgContext* c);
MG_API void MG_C_DECL mgSetCursor_f(mgContext* ctx, mgCursor* c, unsigned int type);
MG_API void MG_C_DECL mgSetParent_f(mgElement* object, mgElement* parent);
MG_API void MG_C_DECL mgSetVisible_f(mgElement* e, int v);
MG_API void MG_C_DECL mgStartFrame_f(mgContext* c);
MG_API void MG_C_DECL mgUpdate_f(mgContext* c);

#endif