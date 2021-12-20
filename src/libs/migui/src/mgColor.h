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

#ifndef _MG_COLOR_H_
#define _MG_COLOR_H_

/* from 0.f to 1.f*/
typedef struct mgColor_s {
	float r;
	float g;
	float b;
	float a;

#if defined(__cplusplus)
	mgColor_s()
		:
		r(0.f),
		g(0.f),
		b(0.f),
		a(0.f)
	{}

	mgColor_s(float R, float G, float B, float A)
		:
		r(R),
		g(G),
		b(B),
		a(A)
	{}

	mgColor_s(unsigned int argb)
		:
		r(float(unsigned char(argb >> 16u)) * 0.00392156862745f),
		g(float(unsigned char(argb >> 8u)) * 0.00392156862745f),
		b(float(unsigned char(argb)) * 0.00392156862745f),
		a(float(unsigned char(argb >> 24u)) * 0.00392156862745f)
	{}

	void setAsIntegerARGB(unsigned int argb)
	{
		r = float(unsigned char(argb >> 16u)) * 0.00392156862745f;
		g = float(unsigned char(argb >> 8u)) * 0.00392156862745f;
		b = float(unsigned char(argb)) * 0.00392156862745f;
		a = float(unsigned char(argb >> 24u)) * 0.00392156862745f;
	}

	void setAsIntegerRGB(unsigned int rgb)
	{
		r = float(unsigned char(rgb >> 16u)) * 0.00392156862745f;
		g = float(unsigned char(rgb >> 8u)) * 0.00392156862745f;
		b = float(unsigned char(rgb)) * 0.00392156862745f;
	}

	void setAsIntegerBGR(unsigned int rgb)
	{
		b = float(unsigned char(rgb >> 16u)) * 0.00392156862745f;
		g = float(unsigned char(rgb >> 8u)) * 0.00392156862745f;
		r = float(unsigned char(rgb)) * 0.00392156862745f;
	}

	const unsigned char getAsByteRed() const { return static_cast<unsigned char>(r * 255.); }
	const unsigned char getAsByteGreen() const { return static_cast<unsigned char>(g * 255.); }
	const unsigned char getAsByteBlue() const { return static_cast<unsigned char>(b * 255.); }
	const unsigned char getAsByteAlpha() const { return static_cast<unsigned char>(a * 255.); }

	unsigned int getAsIntegerARGB()
	{
		return MG_MAKEFOURCC(
			this->getAsByteBlue(),
			this->getAsByteGreen(),
			this->getAsByteRed(),
			this->getAsByteAlpha()
		);
	}

#endif

} mgColor;

inline
void mgColorSet(mgColor* c, float R, float G, float B, float A)
{
	c->r = R;
	c->g = G;
	c->b = B;
	c->a = A;
}

/*0xFF112233*/
inline
void mgColorSetAsIntegerARGB(mgColor* c, unsigned int argb)
{
	c->r = (float)((unsigned char)(argb >> 16u)) * 0.00392156862745f;
	c->g = (float)((unsigned char)(argb >> 8u)) * 0.00392156862745f;
	c->b = (float)((unsigned char)(argb)) * 0.00392156862745f;
	c->a = (float)((unsigned char)(argb >> 24u)) * 0.00392156862745f;
}

inline
void mgColorSetAsIntegerRGB(mgColor* c, unsigned int rgb)
{
	c->r = (float)((unsigned char)(rgb >> 16u)) * 0.00392156862745f;
	c->g = (float)((unsigned char)(rgb >> 8u)) * 0.00392156862745f;
	c->b = (float)((unsigned char)(rgb)) * 0.00392156862745f;
}

inline
unsigned int mgColorGetAsIntegerARGB(mgColor* c)
{
	return MG_MAKEFOURCC(
		c->b * 255.,
		c->g * 255.,
		c->r * 255.,
		c->a * 255.
	);
}

inline
unsigned int mgColorGetAsIntegerRGB(mgColor* c)
{
	return MG_MAKEFOURCC(
		c->b * 255.,
		c->g * 255.,
		c->r * 255.,
		0
	);
}

#endif