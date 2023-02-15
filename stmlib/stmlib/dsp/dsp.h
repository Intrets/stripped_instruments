// Copyright 2012 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// DSP utility routines.

#ifndef STMLIB_UTILS_DSP_DSP_H_
#define STMLIB_UTILS_DSP_DSP_H_

#include "stmlib/stmlib.h"

#include <cmath>
#include <math.h>

#include <crack/audio/MathContext.h>

namespace stmlib
{

#define MAKE_INTEGRAL_FRACTIONAL(x) \
	int32_t x##_integral = static_cast<int32_t>(x); \
	float x##_fractional = x - static_cast<float>(x##_integral);

	inline float Interpolate(float const* table, float index, float size) {
		index *= size;
		MAKE_INTEGRAL_FRACTIONAL(index)
		float a = table[index_integral];
		float b = table[index_integral + 1];
		return a + (b - a) * index_fractional;
	}

	inline float InterpolateHermite(float const* table, float index, float size) {
		index *= size;
		MAKE_INTEGRAL_FRACTIONAL(index)
		float const xm1 = table[index_integral - 1];
		float const x0 = table[index_integral + 0];
		float const x1 = table[index_integral + 1];
		float const x2 = table[index_integral + 2];
		float const c = (x1 - xm1) * 0.5f;
		float const v = x0 - x1;
		float const w = c + v;
		float const a = w + v + (x2 - x0) * 0.5f;
		float const b_neg = w + a;
		float const f = index_fractional;
		return (((a * f) - b_neg) * f + c) * f + x0;
	}

	inline float InterpolateWrap(float const* table, float index, float size) {
		index -= static_cast<float>(static_cast<int32_t>(index));
		index *= size;
		MAKE_INTEGRAL_FRACTIONAL(index)
		float a = table[index_integral];
		float b = table[index_integral + 1];
		return a + (b - a) * index_fractional;
	}

	inline float SmoothStep(float value) {
		return value * value * (3.0f - 2.0f * value);
	}

#define ONE_POLE(out, in, coefficient) out += (coefficient) * ((in)-out);
#define SLOPE(out, in, positive, negative) \
	{ \
		float error = (in)-out; \
		out += (error > 0 ? positive : negative) * error; \
	}
#define SLEW(out, in, delta) \
	{ \
		float error = (in)-out; \
		float d = (delta); \
		if (error > d) { \
			error = d; \
		} \
		else if (error < -d) { \
			error = -d; \
		} \
		out += error; \
	}

	inline float Crossfade(float a, float b, float fade) {
		return a + (b - a) * fade;
	}

	constexpr inline float SoftLimit(float x) {
		return x * (27.0f + x * x) / (27.0f + 9.0f * x * x);
	}

	inline float SoftClip(float x) {
		using math = crack::audio::StdContext;

		return math::clamp(SoftLimit(x), -1.0f, 1.0f);
	}

	inline int32_t clamp_i32(int32_t x, int32_t min, int32_t max) {
		return std::max(std::min(x, max), min);
	}

	inline int16_t Clip16(int32_t x) {
		return static_cast<int16_t>(clamp_i32(x, -32768, 32767));
	}

	inline uint16_t ClipU16(int32_t x) {
		return static_cast<uint16_t>(clamp_i32(x, 0, 65535));
	}

	inline float Sqrt(float x) {
		using math = crack::audio::StdContext;

		return math::sqrt0(x);
	}

	inline int16_t SoftConvert(float x) {
		return Clip16(static_cast<int32_t>(SoftLimit(x * 0.5f) * 32768.0f));
	}

} // namespace stmlib

#endif // STMLIB_UTILS_DSP_DSP_H_