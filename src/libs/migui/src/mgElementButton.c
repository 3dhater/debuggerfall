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

void miGUI_onUpdate_rectangle(mgElement* e);

void
miGUI_onUpdateTransform_button(mgElement* e)
{
	e->buildAreaFinal = e->buildArea;
	e->clipAreaFinal = e->clipArea;
}

void 
miGUI_onUpdate_button(mgElement* e)
{
	mgElementButton* impl = (mgElementButton*)e->implementation;
	if(impl->enabled)
		miGUI_onUpdate_rectangle(e);
}

void 
miGUI_onDraw_button(mgElement* e)
{
	mgPoint pos;
	pos.x = e->buildAreaFinal.left;
	pos.y = e->buildAreaFinal.top;

	mgPoint sz;
	sz.x = e->buildAreaFinal.right - e->buildAreaFinal.left;
	sz.y = e->buildAreaFinal.bottom - e->buildAreaFinal.top;

	mgElementButton* impl = (mgElementButton*)e->implementation;
	
	if (impl->enabled)
	{
		if (e->elementState & 0x1)
		{
			impl->colorFinal1 = impl->colorHover1;
			impl->colorFinal2 = impl->colorHover2;
		}
		else
		{
			impl->colorFinal1 = impl->color1;
			impl->colorFinal2 = impl->color2;
		}

		if (e->elementState & 0x2)
		{
			impl->colorFinal1 = impl->colorPress1;
			impl->colorFinal2 = impl->colorPress2;
		}
	}
	else
	{
		impl->colorFinal1 = impl->colorDisabled1;
		impl->colorFinal2 = impl->colorDisabled2;
	}

	e->context->gpu->setClipRect(&e->clipAreaFinal);
	e->context->gpu->drawRectangle(e, &pos, &sz, &impl->colorFinal1, &impl->colorFinal2, 0, 0);

	if (impl->text && impl->textLen)
	{
		if (impl->enabled)
		{
			if (e->elementState & 0x1)
				impl->textColorFinal = impl->textColorHover;
			else
				impl->textColorFinal = impl->textColor;

			if (e->elementState & 0x2)
				impl->textColorFinal = impl->textColorPress;
		}
		else
		{
			impl->textColorFinal = impl->textColorDisabled;
		}

		pos.x += impl->textIndentInternal.x;
		pos.y += impl->textIndentInternal.y;
		e->context->gpu->drawText(&pos, impl->text, impl->textLen, &impl->textColorFinal, impl->font);
	}
}

void
miGUI_onRebuild_button(mgElement* e) 
{
	mgElementButton* impl = (mgElementButton*)e->implementation;
	
	impl->textIndentInternal.x = 0;
	impl->textIndentInternal.y = 0;

	if (impl->text && e->context->getTextSize)
	{
		mgPoint p;
		e->context->getTextSize(impl->text, impl->font, &p);

		mgPoint buttonCenter;
		buttonCenter.x = (int)((float)(e->buildAreaFinal.right - e->buildAreaFinal.left) * 0.5f);
		buttonCenter.y = (int)((float)(e->buildAreaFinal.bottom - e->buildAreaFinal.top) * 0.5f);

		mgPoint textHalfLen;
		textHalfLen.x = (int)((float)p.x * 0.5f);
		textHalfLen.y = (int)((float)p.y * 0.5f);

		impl->textIndentInternal.x += buttonCenter.x - textHalfLen.x;
		impl->textIndentInternal.y += buttonCenter.y - textHalfLen.y;
	}
}

MG_API
mgElement* MG_C_DECL
mgCreateButton_f(struct mgContext_s* c, mgPoint* position, mgPoint* size, const wchar_t* text, mgFont* font)
{
	assert(c);
	assert(position);
	assert(text);
	assert(font);
	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_BUTTON;
	newElement->buildArea.left = position->x;
	newElement->buildArea.top = position->y;
	newElement->buildArea.right = position->x + size->x;
	newElement->buildArea.bottom = position->y + size->y;
	newElement->clipArea = newElement->buildArea;
	newElement->context = c;
	newElement->visible = 1;
	newElement->onDraw = miGUI_onDraw_button;
	newElement->onUpdate = miGUI_onUpdate_button;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_button;
	newElement->onRebuild = miGUI_onRebuild_button;

	newElement->implementation = calloc(1, sizeof(mgElementButton));
	mgElementButton* impl = (mgElementButton*)newElement->implementation;
	impl->enabled = 1;
	mgColorSetAsIntegerRGB(&impl->color1, 0x999999);
	mgColorSetAsIntegerRGB(&impl->color2, 0x666666);
	mgColorSetAsIntegerRGB(&impl->colorDisabled1, 0x999999);
	mgColorSetAsIntegerRGB(&impl->colorDisabled2, 0x666666);
	mgColorSetAsIntegerRGB(&impl->colorHover1, 0xAAAAAA);
	mgColorSetAsIntegerRGB(&impl->colorHover2, 0x777777);
	mgColorSetAsIntegerRGB(&impl->colorPress1, 0x777777);
	mgColorSetAsIntegerRGB(&impl->colorPress2, 0x444444);

	impl->font = font;
	impl->text = text;
	impl->textLen = wcslen(text);
	mgColorSetAsIntegerRGB(&impl->textColor, 0x0);
	mgColorSetAsIntegerRGB(&impl->textColorHover, 0x222222);
	mgColorSetAsIntegerRGB(&impl->textColorPress, 0x0);
	mgColorSetAsIntegerRGB(&impl->textColorDisabled, 0x555555);

	mgSetParent_f(newElement, 0);

	miGUI_onRebuild_button(newElement);

	return newElement;
}
