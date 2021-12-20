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

#ifndef _MG_ELEMENT_H_
#define _MG_ELEMENT_H_

enum {
	MG_TYPE_RECTANGLE = 1,
	MG_TYPE_TEXT,
	MG_TYPE_BUTTON
};

struct mgElement_s;

/*I don't want to use manystars** magic*/
struct mgElementNode_s {
	struct mgElement_s* pointer;
};


/* base data for all GUI widgets*/
typedef struct mgElement_s {
	unsigned int type; /*MG_TYPE...*/
	void* implementation;

	struct mgContext_s* context;

	int id;
	void* userData;

	int visible;/* != 0 - visible*/
	
	struct mgElement_s* parent;
	struct mgElementNode_s* children;
	int childrenCount;

	mgRect buildArea;
	mgRect clipArea;
	mgRect buildAreaFinal;
	mgRect clipAreaFinal;

	void(*onDraw)(struct mgElement_s* e);
	void(*onUpdate)(struct mgElement_s* e);
	void(*onUpdateTransform)(struct mgElement_s* e);
	void(*onRebuild)(struct mgElement_s* e);/*must call when change window size*/

	void(*onMouseEnter)(struct mgElement_s* e);
	void(*onMouseLeave)(struct mgElement_s* e);
	
	void(*onClickLMB)(struct mgElement_s* e);
	void(*onClickRMB)(struct mgElement_s* e);
	void(*onClickMMB)(struct mgElement_s* e);
	void(*onClickX1MB)(struct mgElement_s* e);
	void(*onClickX2MB)(struct mgElement_s* e);
	void(*onReleaseLMB)(struct mgElement_s* e);
	void(*onReleaseRMB)(struct mgElement_s* e);
	void(*onReleaseMMB)(struct mgElement_s* e);
	void(*onReleaseX1MB)(struct mgElement_s* e);
	void(*onReleaseX2MB)(struct mgElement_s* e);
	
	/* 
	*  0x1 when cursor in buildAreaFinal
	*  0x2 LMB click
	*  0x4 RMB click
	*  0x8 MMB click
	*  0x10 X1MB click
	*  0x20 X2MB click
	*/
	int elementState;

} mgElement;

#include "mgElementRectangle.h"
#include "mgElementText.h"
#include "mgElementButton.h"

#endif