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

void
miGUI_onUpdateTransform_rectangle(mgElement* e)
{
	e->buildAreaFinal = e->buildArea;
	e->clipAreaFinal = e->clipArea;
}

void 
miGUI_onUpdate_rectangle(mgElement* e)
{
	int inRect = mgPointInRect(&e->buildAreaFinal, &e->context->input->mousePosition);

	if (inRect)
	{
		if (!e->elementState & 0x1)
		{
			if (e->onMouseEnter)
				e->onMouseEnter(e);
		}

		if (e->context->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN)
		{
			e->elementState |= 0x2;

			if (e->onClickLMB)
				e->onClickLMB(e);
		}
		else if (e->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
		{
			if (e->onReleaseLMB && e->elementState & 0x2)
				e->onReleaseLMB(e);
		}

		if (e->context->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN)
		{
			e->elementState |= 0x4;

			if (e->onClickRMB)
				e->onClickRMB(e);
		}
		else if (e->context->input->mouseButtonFlags1 & MG_MBFL_RMBUP)
		{
			if (e->onReleaseRMB && e->elementState & 0x4)
				e->onReleaseRMB(e);
		}

		if (e->context->input->mouseButtonFlags1 & MG_MBFL_MMBDOWN)
		{
			e->elementState |= 0x8;

			if (e->onClickMMB)
				e->onClickMMB(e);
		}
		else if (e->context->input->mouseButtonFlags1 & MG_MBFL_MMBUP)
		{
			if (e->onReleaseMMB && e->elementState & 0x8)
				e->onReleaseMMB(e);
		}

		if (e->context->input->mouseButtonFlags1 & MG_MBFL_X1MBDOWN)
		{
			e->elementState |= 0x10;

			if (e->onClickX1MB)
				e->onClickX1MB(e);
		}
		else if (e->context->input->mouseButtonFlags1 & MG_MBFL_X1MBUP)
		{
			if (e->onReleaseX1MB && e->elementState & 0x10)
				e->onReleaseX1MB(e);
		}

		if (e->context->input->mouseButtonFlags1 & MG_MBFL_X2MBDOWN)
		{
			e->elementState |= 0x20;

			if (e->onClickX2MB)
				e->onClickX2MB(e);
		}
		else if (e->context->input->mouseButtonFlags1 & MG_MBFL_X2MBUP)
		{
			if (e->onReleaseX2MB && e->elementState & 0x20)
				e->onReleaseX2MB(e);
		}
	}
	else
	{
		if (e->elementState & 0x1)
		{
			if (e->onMouseLeave)
				e->onMouseLeave(e);
		}
	}

	if (e->context->input->mouseButtonFlags1 & MG_MBFL_LMBUP)
	{
		if (e->elementState & 0x2)
			e->elementState ^= 0x2;
	}

	if (e->context->input->mouseButtonFlags1 & MG_MBFL_RMBUP)
	{
		if (e->elementState & 0x4)
			e->elementState ^= 0x4;
	}

	if (e->context->input->mouseButtonFlags1 & MG_MBFL_MMBUP)
	{
		if (e->elementState & 0x8)
			e->elementState ^= 0x8;
	}

	if (e->context->input->mouseButtonFlags1 & MG_MBFL_X1MBUP)
	{
		if (e->elementState & 0x10)
			e->elementState ^= 0x10;
	}

	if (e->context->input->mouseButtonFlags1 & MG_MBFL_X2MBUP)
	{
		if (e->elementState & 0x20)
			e->elementState ^= 0x20;
	}

	if (inRect)
		e->elementState |= 0x1;
	else if(e->elementState & 0x1)
		e->elementState ^= 0x1;
}

void 
miGUI_onDraw_rectangle(mgElement* e)
{
	mgPoint pos;
	pos.x = e->buildAreaFinal.left;
	pos.y = e->buildAreaFinal.top;

	mgPoint sz;
	sz.x = e->buildAreaFinal.right - e->buildAreaFinal.left;
	sz.y = e->buildAreaFinal.bottom - e->buildAreaFinal.top;

	mgElementRectangle* impl = (mgElementRectangle*)e->implementation;

	e->context->gpu->setClipRect(&e->clipAreaFinal);
	e->context->gpu->drawRectangle(e, &pos, &sz, &impl->color1, &impl->color2, 0, 0);
}

void
miGUI_onRebuild_rectangle(mgElement* e) {}

MG_API
mgElement* MG_C_DECL
mgCreateRectangle_f(struct mgContext_s* c, mgPoint* position, mgPoint* size, mgColor* color1, mgColor* color2)
{
	assert(c);
	assert(position);
	assert(size);
	assert(color1);
	assert(color2);
	mgElement* newElement = calloc(1, sizeof(mgElement));
	newElement->type = MG_TYPE_RECTANGLE;
	newElement->buildArea.left = position->x;
	newElement->buildArea.top = position->y;
	newElement->buildArea.right = position->x + size->x;
	newElement->buildArea.bottom = position->y + size->y;
	newElement->clipArea = newElement->buildArea;
	newElement->context = c;
	newElement->visible = 1;
	newElement->onDraw = miGUI_onDraw_rectangle;
	newElement->onUpdate = miGUI_onUpdate_rectangle;
	newElement->onUpdateTransform = miGUI_onUpdateTransform_rectangle;
	newElement->onRebuild = miGUI_onRebuild_rectangle;

	newElement->implementation = calloc(1, sizeof(mgElementRectangle));
	mgElementRectangle* impl = (mgElementRectangle*)newElement->implementation;
	impl->color1 = *color1;
	impl->color2 = *color2;

	mgSetParent_f(newElement, 0);

	return newElement;
}
