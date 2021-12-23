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

void mgInitDefaultCursors(mgContext* c);
void mgDestroyDefaultCursors(mgContext* c);

void 
mgDestroyElement_f(mgElement* e)
{
	assert(e);
	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgDestroyElement_f(e->children[i].pointer);
	}

	if (e->implementation)
		free(e->implementation);
	free(e);
}

void mgrootobject_cb(mgElement*e) {}

MG_API 
mgContext* MG_C_DECL
mgCreateContext_f(mgVideoDriverAPI* gpu, mgInputContext* input)
{
	assert(gpu);
	assert(input);

	mgContext* c = calloc(1, sizeof(mgContext));
	c->gpu = gpu;
	c->input = input;
	c->needUpdateTransform = 1;
	c->needRebuild = 1;
	c->rootElement = calloc(1, sizeof(mgElement));
	c->rootElement->context = c;
	c->rootElement->visible = 1;
	c->rootElement->onDraw = mgrootobject_cb;
	c->rootElement->onUpdate = mgrootobject_cb;
	c->rootElement->onUpdateTransform = mgrootobject_cb;
	c->rootElement->onRebuild = mgrootobject_cb;

	c->functions.CreateButton_p = mgCreateButton_f;
	c->functions.CreateContext_p = mgCreateContext_f;
	c->functions.CreateCursor_p = mgCreateCursor_f;
	c->functions.CreateFont_p = mgCreateFont_f;
	c->functions.CreateRectangle_p = mgCreateRectangle_f;
	c->functions.CreateText_p = mgCreateText_f;
	c->functions.DestroyContext_p = mgDestroyContext_f;
	c->functions.DestroyCursor_p = mgDestroyCursor_f;
	c->functions.DestroyFont_p = mgDestroyFont_f;
	c->functions.Draw_p = mgDraw_f;
	c->functions.SetCursor_p = mgSetCursor_f;
	c->functions.SetParent_p = mgSetParent_f;
	c->functions.SetVisible_p = mgSetVisible_f;
	c->functions.StartFrame_p = mgStartFrame_f;
	c->functions.Update_p = mgUpdate_f;

	mgInitDefaultCursors(c);

	return c;
}

MG_API 
void MG_C_DECL
mgDestroyContext_f(mgContext* c)
{
	assert(c);

	mgDestroyDefaultCursors(c);

	/*destroy everything here*/
	/*...*/
	if (c->rootElement)
		mgDestroyElement_f(c->rootElement);

	free(c);
}

void
mgUpdateElement(mgElement* e) 
{
	e->onUpdate(e);
	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgUpdateElement(e->children[i].pointer);
	}
}

void
mgUpdateTransformElement(mgElement* e)
{
	if (!e->visible)
		return;

	e->onUpdateTransform(e);
	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgUpdateTransformElement(e->children[i].pointer);
	}
}

void
mgRebuildElement(mgElement* e)
{
	/*if (!e->visible)
		return;*/

	e->onRebuild(e);
	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgRebuildElement(e->children[i].pointer);
	}
}

MG_API
void MG_C_DECL
mgUpdate_f(mgContext* c)
{
	assert(c);

	unsigned int ctrl_shift_alt = 0;
	if (mgIsKeyHold(c->input, MG_KEY_LALT) || mgIsKeyHold(c->input, MG_KEY_RALT))
		ctrl_shift_alt |= 1;

	if (mgIsKeyHold(c->input, MG_KEY_LSHIFT) || mgIsKeyHold(c->input, MG_KEY_RSHIFT))
		ctrl_shift_alt |= 2;

	if (mgIsKeyHold(c->input, MG_KEY_LCTRL) || mgIsKeyHold(c->input, MG_KEY_RCTRL))
		ctrl_shift_alt |= 4;

	switch (ctrl_shift_alt)
	{
	default:
	case 0:  c->input->keyboardModifier = MG_KBMOD_clear;         break;
	case 1:  c->input->keyboardModifier = MG_KBMOD_ALT;           break;
	case 2:  c->input->keyboardModifier = MG_KBMOD_SHIFT;         break;
	case 3:  c->input->keyboardModifier = MG_KBMOD_SHIFTALT;      break;
	case 4:  c->input->keyboardModifier = MG_KBMOD_CTRL;          break;
	case 5:  c->input->keyboardModifier = MG_KBMOD_CTRLALT;       break;
	case 6:  c->input->keyboardModifier = MG_KBMOD_CTRLSHIFT;     break;
	case 7:  c->input->keyboardModifier = MG_KBMOD_CTRLSHIFTALT;  break;
	}

	mgUpdateElement(c->rootElement);

	if (c->needUpdateTransform)
	{
		mgUpdateTransformElement(c->rootElement);
		c->needUpdateTransform = 0;
	}

	if (c->needRebuild)
	{
		mgRebuildElement(c->rootElement);
		c->needRebuild = 0;
	}
}

MG_API
void MG_C_DECL
mgStartFrame_f(mgContext* c)
{
	assert(c);
	
	c->input->mouseMoveDeltaOld = c->input->mouseMoveDelta;
	c->input->mouseWheelDeltaOld = c->input->mouseWheelDelta;
	c->input->mousePositionOld = c->input->mousePosition;

	if ((c->input->mouseButtonFlags1 & MG_MBFL_LMBDOWN) == MG_MBFL_LMBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_LMBDOWN;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_RMBDOWN) == MG_MBFL_RMBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_RMBDOWN;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_MMBDOWN) == MG_MBFL_MMBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_MMBDOWN;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_X1MBDOWN) == MG_MBFL_X1MBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_X1MBDOWN;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_X2MBDOWN) == MG_MBFL_X2MBDOWN)
		c->input->mouseButtonFlags1 ^= MG_MBFL_X2MBDOWN;

	if ((c->input->mouseButtonFlags1 & MG_MBFL_LMBUP) == MG_MBFL_LMBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_LMBUP;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_RMBUP) == MG_MBFL_RMBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_RMBUP;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_MMBUP) == MG_MBFL_MMBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_MMBUP;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_X1MBUP) == MG_MBFL_X1MBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_X1MBUP;
	if ((c->input->mouseButtonFlags1 & MG_MBFL_X2MBUP) == MG_MBFL_X2MBUP)
		c->input->mouseButtonFlags1 ^= MG_MBFL_X2MBUP;
	
	for (int i = 0; i < 256; ++i)
	{
		if ((c->input->keyFlags[i] & MG_KEYFL_HIT) == MG_KEYFL_HIT)
			c->input->keyFlags[i] ^= MG_KEYFL_HIT;

		if ((c->input->keyFlags[i] & MG_KEYFL_RELEASE) == MG_KEYFL_RELEASE)
			c->input->keyFlags[i] ^= MG_KEYFL_RELEASE;
	}
}


MG_API
void MG_C_DECL
mgSetParent_f(mgElement* object, mgElement* parent)
{
	assert(object);

	if (object->parent)
	{
		int chCount = 0;
		for (int i = 0; i < object->parent->childrenCount; ++i)
		{
			if (object->parent->children[i].pointer != object)
				++chCount;
		}
		
		struct mgElementNode_s* newChildren = 0;
		
		if (chCount)
		{
			newChildren = malloc(chCount * sizeof(struct mgElementNode_s));

			int i2 = 0;
			for (int i = 0; i < object->parent->childrenCount; ++i)
			{
				if (object->parent->children[i].pointer != object)
				{
					newChildren[i2].pointer = object->parent->children[i].pointer;
					++i2;
				}
			}
		}

		object->parent->childrenCount = chCount;
		if (object->parent->children)
			free(object->parent->children);

		object->parent->children = newChildren;
	}

	object->parent = parent;
	if (!parent)
		object->parent = object->context->rootElement;

	{
		int chCount = object->parent->childrenCount + 1;
		struct mgElementNode_s* newChildren = malloc(chCount * sizeof(struct mgElementNode_s));

		for (int i = 0; i < object->parent->childrenCount; ++i)
		{
			newChildren[i].pointer = object->parent->children[i].pointer;
		}

		newChildren[object->parent->childrenCount].pointer = object;

		object->parent->childrenCount++;

		if (object->parent->children)
			free(object->parent->children);

		object->parent->children = newChildren;
	}
}

MG_API
void MG_C_DECL
mgSetVisible_f(mgElement* e, int v)
{
	assert(e);
	e->visible = v;
	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgSetVisible_f(e->children[i].pointer, v);
	}
}

void 
mgDrawElement(mgElement* e)
{
	if (!e->visible)
		return;

	e->onDraw(e);

	for (int i = 0; i < e->childrenCount; ++i)
	{
		mgDrawElement(e->children[i].pointer);
	}
}

MG_API
void MG_C_DECL
mgDraw_f(mgContext* c)
{
	assert(c);
	mgDrawElement(c->rootElement);
}
