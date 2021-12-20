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

#ifndef _MG_IC_H_
#define _MG_IC_H_

enum {
	MG_KEY_NONE = 0,
	MG_KEY_BACKSPACE = 8,
	MG_KEY_TAB = 9,
	MG_KEY_ENTER = 13,
	MG_KEY_SHIFT = 16,
	MG_KEY_CTRL = 17,
	MG_KEY_ALT = 18,
	MG_KEY_PAUSE = 19,
	MG_KEY_CAPS_LOCK = 20,
	MG_KEY_ESCAPE = 27,
	MG_KEY_SPACE = 32,
	MG_KEY_PGUP = 33,
	MG_KEY_PGDOWN = 34,
	MG_KEY_END = 35,
	MG_KEY_HOME = 36,
	MG_KEY_LEFT = 37,
	MG_KEY_UP = 38,
	MG_KEY_RIGHT = 39,
	MG_KEY_DOWN = 40,
	MG_KEY_PRTSCR = 44,
	MG_KEY_INSERT = 45,
	MG_KEY_DELETE = 46,
	MG_KEY_0 = 48,
	MG_KEY_1 = 49,
	MG_KEY_2 = 50,
	MG_KEY_3 = 51,
	MG_KEY_4 = 52,
	MG_KEY_5 = 53,
	MG_KEY_6 = 54,
	MG_KEY_7 = 55,
	MG_KEY_8 = 56,
	MG_KEY_9 = 57,
	MG_KEY_A = 65,
	MG_KEY_B = 66,
	MG_KEY_C = 67,
	MG_KEY_D = 68,
	MG_KEY_E = 69,
	MG_KEY_F = 70,
	MG_KEY_G = 71,
	MG_KEY_H = 72,
	MG_KEY_I = 73,
	MG_KEY_J = 74,
	MG_KEY_K = 75,
	MG_KEY_L = 76,
	MG_KEY_M = 77,
	MG_KEY_N = 78,
	MG_KEY_O = 79,
	MG_KEY_P = 80,
	MG_KEY_Q = 81,
	MG_KEY_R = 82,
	MG_KEY_S = 83,
	MG_KEY_T = 84,
	MG_KEY_U = 85,
	MG_KEY_V = 86,
	MG_KEY_W = 87,
	MG_KEY_X = 88,
	MG_KEY_Y = 89,
	MG_KEY_Z = 90,
	MG_KEY_WIN = 91,
	MG_KEY_CONTEXT = 93,
	MG_KEY_NUM_0 = 96,
	MG_KEY_NUM_1 = 97,
	MG_KEY_NUM_2 = 98,
	MG_KEY_NUM_3 = 99,
	MG_KEY_NUM_4 = 100,
	MG_KEY_NUM_5 = 101,
	MG_KEY_NUM_6 = 102,
	MG_KEY_NUM_7 = 103,
	MG_KEY_NUM_8 = 104,
	MG_KEY_NUM_9 = 105,
	MG_KEY_NUM_MUL = 106, // *
	MG_KEY_NUM_ADD = 107, // +
	MG_KEY_NUM_SUB = 109, // -
	MG_KEY_NUM_DOT = 110, // .
	MG_KEY_NUM_SLASH = 111, // /
	MG_KEY_F1 = 112,
	MG_KEY_F2 = 113,
	MG_KEY_F3 = 114,
	MG_KEY_F4 = 115,
	MG_KEY_F5 = 116,
	MG_KEY_F6 = 117,
	MG_KEY_F7 = 118,
	MG_KEY_F8 = 119,
	MG_KEY_F9 = 120,
	MG_KEY_F10 = 121,
	MG_KEY_F11 = 122,
	MG_KEY_F12 = 123,
	MG_KEY_NUM_LOCK = 144,
	MG_KEY_SCROLL_LOCK = 145,
	MG_KEY_LSHIFT = 160,
	MG_KEY_RSHIFT = 161,
	MG_KEY_LCTRL = 162,
	MG_KEY_RCTRL = 163,
	MG_KEY_LALT = 164,
	MG_KEY_RALT = 165,
	MG_KEY_FUNC_MY_COMP = 182, // Function key. open My computer
	MG_KEY_FUNC_CALC = 183, // Function key. open Calculator
	MG_KEY_COLON = 186, //;:
	MG_KEY_ADD = 187, //=+
	MG_KEY_COMMA = 188, //,<
	MG_KEY_SUB = 189, //-_
	MG_KEY_DOT = 190, //.>
	MG_KEY_SLASH = 191, // /?
	MG_KEY_TILDE = 192, //`~
	MG_KEY_FIGURE_OPEN = 219, //[{
	MG_KEY_BACKSLASH = 220, //\|
	MG_KEY_FIGURE_CLOSE = 221, //]}
	MG_KEY_QUOTE = 222, //'"

	MG_KEY_end__,
};

enum {
	MG_MBFL_LMBDOWN = 0x1,
	MG_MBFL_LMBUP = 0x2,
	MG_MBFL_RMBDOWN = 0x4,
	MG_MBFL_RMBUP = 0x8,
	MG_MBFL_MMBDOWN = 0x10,
	MG_MBFL_MMBUP = 0x20,
	MG_MBFL_X1MBDOWN = 0x40,
	MG_MBFL_X1MBUP = 0x80,
	MG_MBFL_X2MBDOWN = 0x100,
	MG_MBFL_X2MBUP = 0x200,

	MG_MBFL_LMBHOLD = 0x400,
	MG_MBFL_RMBHOLD = 0x800,
	MG_MBFL_MMBHOLD = 0x1000,
	MG_MBFL_X1MBHOLD = 0x2000,
	MG_MBFL_X2MBHOLD = 0x4000,
};

enum {
	MG_KEYFL_HOLD = 0x1,
	MG_KEYFL_HIT = 0x2,
	MG_KEYFL_RELEASE = 0x4,
};

enum {
	MG_KBMOD_clear = 0x0,
	MG_KBMOD_CTRL = 0x1,
	MG_KBMOD_SHIFT = 0x2,
	MG_KBMOD_ALT = 0x4,
	MG_KBMOD_CTRLSHIFT = 0x8,
	MG_KBMOD_CTRLALT = 0x10,
	MG_KBMOD_SHIFTALT = 0x20,
	MG_KBMOD_CTRLSHIFTALT = 0x40
};

/* You must update states by yourself */
typedef struct mgInputContext_s {
	mgPoint mousePosition;
	mgPoint mousePositionOld;

	mgPoint mouseMoveDelta;
	mgPoint mouseMoveDeltaOld;

	float   mouseWheelDelta;
	float   mouseWheelDeltaOld;

	unsigned int mouseButtonFlags1;/*MG_MBFL...*/
	unsigned int mouseButtonFlags2;/*MG_MBFL... HOLD*/

	wchar_t character;
	unsigned char keyFlags[256]; /**/

	unsigned int keyboardModifier; /*MG_KBMOD...*/

} mgInputContext;

inline
int mgIsKeyHit(mgInputContext* c, unsigned char key) {
	return (c->keyFlags[key] & MG_KEYFL_HIT) == MG_KEYFL_HIT;
}

inline
int mgIsKeyHold(mgInputContext* c, unsigned char key) {
	return (c->keyFlags[key] & MG_KEYFL_HOLD) == MG_KEYFL_HOLD;
}

inline
int mgIsKeyRelease(mgInputContext* c, unsigned char key) {
	return (c->keyFlags[key] & MG_KEYFL_RELEASE) == MG_KEYFL_RELEASE;
}

#endif