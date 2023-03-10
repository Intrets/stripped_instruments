// Copyright 2016 Emilie Gillet.
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
// Resources definitions.
//
// Automatically generated with:
// make resources

#ifndef PLAITS_RESOURCES_H_
#define PLAITS_RESOURCES_H_

#include "stmlib/stmlib.h"

namespace plaits
{

	typedef uint8_t ResourceId;

	extern float const* lookup_table_table[];

	extern int16_t const* lookup_table_i16_table[];

	extern int8_t const* lookup_table_i8_table[];

	extern int16_t const* wavetables_table[];

	extern float const lut_sine[];
	extern float const lut_fm_frequency_quantizer[];
	extern float const lut_fold[];
	extern float const lut_fold_2[];
	extern float const lut_stiffness[];
	extern float const lut_svf_shift[];
	extern const int16_t lut_ws_inverse_tan[];
	extern const int16_t lut_ws_inverse_sin[];
	extern const int16_t lut_ws_linear[];
	extern const int16_t lut_ws_bump[];
	extern const int16_t lut_ws_double_bump[];
	extern const int8_t lut_lpc_excitation_pulse[];
	extern const int16_t wav_integrated_waves[];
	constexpr auto LUT_SINE = 0;
	constexpr auto LUT_SINE_SIZE = 1281;
	constexpr auto LUT_FM_FREQUENCY_QUANTIZER = 1;
	constexpr auto LUT_FM_FREQUENCY_QUANTIZER_SIZE = 129;
	constexpr auto LUT_FOLD = 2;
	constexpr auto LUT_FOLD_SIZE = 516;
	constexpr auto LUT_FOLD_2 = 3;
	constexpr auto LUT_FOLD_2_SIZE = 516;
	constexpr auto LUT_STIFFNESS = 4;
	constexpr auto LUT_STIFFNESS_SIZE = 65;
	constexpr auto LUT_SVF_SHIFT = 5;
	constexpr auto LUT_SVF_SHIFT_SIZE = 257;
	constexpr auto LUT_WS_INVERSE_TAN = 0;
	constexpr auto LUT_WS_INVERSE_TAN_SIZE = 257;
	constexpr auto LUT_WS_INVERSE_SIN = 1;
	constexpr auto LUT_WS_INVERSE_SIN_SIZE = 257;
	constexpr auto LUT_WS_LINEAR = 2;
	constexpr auto LUT_WS_LINEAR_SIZE = 257;
	constexpr auto LUT_WS_BUMP = 3;
	constexpr auto LUT_WS_BUMP_SIZE = 257;
	constexpr auto LUT_WS_DOUBLE_BUMP = 4;
	constexpr auto LUT_WS_DOUBLE_BUMP_SIZE = 257;
	constexpr auto LUT_WS_DOUBLE_BUMP_SENTINEL = 5;
	constexpr auto LUT_WS_DOUBLE_BUMP_SENTINEL_SIZE = 257;
	constexpr auto LUT_LPC_EXCITATION_PULSE = 0;
	constexpr auto LUT_LPC_EXCITATION_PULSE_SIZE = 640;
	constexpr auto WAV_INTEGRATED_WAVES = 0;
	constexpr auto WAV_INTEGRATED_WAVES_SIZE = 49920;

} // namespace plaits

#endif // PLAITS_RESOURCES_H_
