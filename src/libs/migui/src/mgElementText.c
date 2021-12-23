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
#include "mgFunctions.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void
miGUI_onUpdateTransform_text(mgElement* e)
{
	e->buildAreaFinal = e->buildArea;
	e->clipAreaFinal = e->clipArea;
}

void 
miGUI_onUpdate_text(mgElement* e)
{
}

void 
miGUI_onDraw_text(mgElement* e)
{
	mgPoint pos;
	pos.x = e->buildAreaFinal.left;
	pos.y = e->buildAreaFinal.top;

	mgElementText* impl = (mgElementText*)e->implementation;

	if (impl->text && impl->textLen)
	{
		e->context->gpu->setClipRect(&e->clipAreaFinal);
		e->context->gpu->drawText(&pos, impl->text, impl->textLen, &impl->color, impl->font);
	}
}

void
miGUI_onRebuild_text(mgElement* e) {}

MG_API
mgElement* MG_C_DECL
mgCreateText_f(struct mgContext_s* c, mgPoint* position, const wchar_t* text, mgFont* font)
{
	assert(c);
	assert(position);
	assert(text);
	assert(font);
	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_TEXT;
	newElement->buildArea.left = position->x;
	newElement->buildArea.top = position->y;
	newElement->buildArea.right = position->x;
	newElement->buildArea.bottom = position->y;
	newElement->clipArea = newElement->buildArea;
	newElement->context = c;
	newElement->visible = 1;
	newElement->onDraw = miGUI_onDraw_text;
	newElement->onUpdate = miGUI_onUpdate_text;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_text;
	newElement->onRebuild = miGUI_onRebuild_text;

	newElement->implementation = calloc(1, sizeof(mgElementText));
	mgElementText* impl = (mgElementText*)newElement->implementation;
	impl->color.r = 0.f;
	impl->color.g = 0.f;
	impl->color.b = 0.f;
	impl->color.a = 1.f;
	impl->font = font;
	impl->text = text;
	impl->textLen = wcslen(text);

	mgSetParent_f(newElement, 0);

	return newElement;
}
