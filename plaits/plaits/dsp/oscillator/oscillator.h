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
// Single waveform oscillator. Can optionally do audio-rate linear FM, with
// through-zero capabilities (negative frequencies).

#ifndef PLAITS_DSP_OSCILLATOR_OSCILLATOR_H_
#define PLAITS_DSP_OSCILLATOR_OSCILLATOR_H_

#include "stmlib/dsp/dsp.h"
#include "stmlib/dsp/parameter_interpolator.h"
#include "stmlib/dsp/polyblep.h"

namespace plaits
{

	enum OscillatorShape
	{
		OSCILLATOR_SHAPE_IMPULSE_TRAIN,
		OSCILLATOR_SHAPE_SAW,
		OSCILLATOR_SHAPE_TRIANGLE,
		OSCILLATOR_SHAPE_SLOPE,
		OSCILLATOR_SHAPE_SQUARE,
		OSCILLATOR_SHAPE_SQUARE_BRIGHT,
		OSCILLATOR_SHAPE_SQUARE_DARK,
		OSCILLATOR_SHAPE_SQUARE_TRIANGLE
	};

	float const kMaxFrequency = 0.25f;
	float const kMinFrequency = 0.000001f;

	class Oscillator
	{
	public:
		Oscillator() {
		}
		~Oscillator() {
		}

		void Init() {
			phase_ = 0.5f;
			next_sample_ = 0.0f;
			lp_state_ = 1.0f;
			hp_state_ = 0.0f;
			high_ = true;

			frequency_ = 0.001f;
			pw_ = 0.5f;
		}

		template<OscillatorShape shape>
		void Render(float frequency, float pw, float* out, size_t size) {
			Render<shape, false, false>(frequency, pw, NULL, out, size);
		}

		template<OscillatorShape shape>
		void Render(
		    float frequency,
		    float pw,
		    float const* fm,
		    float* out,
		    size_t size
		) {
			if (!fm) {
				Render<shape, false, false>(frequency, pw, NULL, out, size);
			}
			else {
				Render<shape, true, true>(frequency, pw, fm, out, size);
			}
		}

		template<OscillatorShape shape, bool has_external_fm, bool through_zero_fm>
		void Render(
		    float frequency0,
		    float pw0,
		    float const* external_fm,
		    float* out,
		    size_t size
		) {
			if constexpr (!has_external_fm) {
				if constexpr (!through_zero_fm) {
					CONSTRAIN(frequency0, kMinFrequency, kMaxFrequency);
				}
				else {
					CONSTRAIN(frequency0, -kMaxFrequency, kMaxFrequency);
				}
				CONSTRAIN(pw0, fabsf(frequency0) * 2.0f, 1.0f - 2.0f * fabsf(frequency0));
			}

			stmlib::ParameterInterpolator fm(&frequency_, frequency0, size);
			stmlib::ParameterInterpolator pwm(&pw_, pw0, size);

			float next_sample = next_sample_;

			while (size--) {
				float this_sample = next_sample;
				next_sample = 0.0f;

				float frequency = fm.Next();
				if (has_external_fm) {
					frequency *= (1.0f + *external_fm++);
					if (!through_zero_fm) {
						CONSTRAIN(frequency, kMinFrequency, kMaxFrequency);
					}
					else {
						CONSTRAIN(frequency, -kMaxFrequency, kMaxFrequency);
					}
				}
				float pw = (shape == OSCILLATOR_SHAPE_SQUARE_TRIANGLE ||
				            shape == OSCILLATOR_SHAPE_TRIANGLE)
				               ? 0.5f
				               : pwm.Next();
				if (has_external_fm) {
					CONSTRAIN(pw, fabsf(frequency) * 2.0f, 1.0f - 2.0f * fabsf(frequency));
				}
				phase_ += frequency;

				if constexpr (shape <= OSCILLATOR_SHAPE_SAW) {
					if (phase_ >= 1.0f) {
						phase_ -= 1.0f;
						float t = phase_ / frequency;
						this_sample -= stmlib::ThisBlepSample(t);
						next_sample -= stmlib::NextBlepSample(t);
					}
					else if constexpr (through_zero_fm) {
						if (phase_ < 0.0f) {
							float t = phase_ / frequency;
							phase_ += 1.0f;
							this_sample += stmlib::ThisBlepSample(t);
							next_sample += stmlib::NextBlepSample(t);
						}
					}
					next_sample += phase_;

					if constexpr (shape == OSCILLATOR_SHAPE_SAW) {
						*out++ = 2.0f * this_sample - 1.0f;
					}
					else {
						lp_state_ += 0.25f * ((hp_state_ - this_sample) - lp_state_);
						*out++ = 4.0f * lp_state_;
						hp_state_ = this_sample;
					}
				}
				else if constexpr (shape <= OSCILLATOR_SHAPE_SLOPE) {
					float slope_up = 2.0f;
					float slope_down = 2.0f;
					if constexpr (shape == OSCILLATOR_SHAPE_SLOPE) {
						slope_up = 1.0f / (pw);
						slope_down = 1.0f / (1.0f - pw);
					}
					if (high_ != (phase_ < pw)) {
						float t = (phase_ - pw) / frequency;
						float discontinuity = (slope_up + slope_down) * frequency;
						if constexpr (through_zero_fm) {
							if (frequency < 0.0f) {
								discontinuity = -discontinuity;
							}
						}
						this_sample -= stmlib::ThisIntegratedBlepSample(t) * discontinuity;
						next_sample -= stmlib::NextIntegratedBlepSample(t) * discontinuity;
						high_ = phase_ < pw;
					}
					if (phase_ >= 1.0f) {
						phase_ -= 1.0f;
						float t = phase_ / frequency;
						float discontinuity = (slope_up + slope_down) * frequency;
						this_sample += stmlib::ThisIntegratedBlepSample(t) * discontinuity;
						next_sample += stmlib::NextIntegratedBlepSample(t) * discontinuity;
						high_ = true;
					}
					else if (through_zero_fm && phase_ < 0.0f) {
						float t = phase_ / frequency;
						phase_ += 1.0f;
						float discontinuity = (slope_up + slope_down) * frequency;
						this_sample -= stmlib::ThisIntegratedBlepSample(t) * discontinuity;
						next_sample -= stmlib::NextIntegratedBlepSample(t) * discontinuity;
						high_ = false;
					}
					next_sample += high_
					                   ? phase_ * slope_up
					                   : 1.0f - (phase_ - pw) * slope_down;
					*out++ = 2.0f * this_sample - 1.0f;
				}
				else {
					if (high_ != (phase_ >= pw)) {
						float t = (phase_ - pw) / frequency;
						float discontinuity = 1.0f;
						if constexpr (through_zero_fm) {
							if (frequency < 0.0f) {
								discontinuity = -discontinuity;
							}
						}
						this_sample += stmlib::ThisBlepSample(t) * discontinuity;
						next_sample += stmlib::NextBlepSample(t) * discontinuity;
						high_ = phase_ >= pw;
					}
					if (phase_ >= 1.0f) {
						phase_ -= 1.0f;
						float t = phase_ / frequency;
						this_sample -= stmlib::ThisBlepSample(t);
						next_sample -= stmlib::NextBlepSample(t);
						high_ = false;
					}
					else if constexpr (through_zero_fm) {
						if (phase_ < 0.0f) {
							float t = phase_ / frequency;
							phase_ += 1.0f;
							this_sample += stmlib::ThisBlepSample(t);
							next_sample += stmlib::NextBlepSample(t);
							high_ = true;
						}
					}
					next_sample += phase_ < pw ? 0.0f : 1.0f;

					if constexpr (shape == OSCILLATOR_SHAPE_SQUARE_TRIANGLE) {
						float const integrator_coefficient = frequency * 0.0625f;
						this_sample = 128.0f * (this_sample - 0.5f);
						lp_state_ += integrator_coefficient * (this_sample - lp_state_);
						*out++ = lp_state_;
					}
					else if constexpr (shape == OSCILLATOR_SHAPE_SQUARE_DARK) {
						float const integrator_coefficient = frequency * 2.0f;
						this_sample = 4.0f * (this_sample - 0.5f);
						lp_state_ += integrator_coefficient * (this_sample - lp_state_);
						*out++ = lp_state_;
					}
					else if constexpr (shape == OSCILLATOR_SHAPE_SQUARE_BRIGHT) {
						float const integrator_coefficient = frequency * 2.0f;
						this_sample = 2.0f * this_sample - 1.0f;
						lp_state_ += integrator_coefficient * (this_sample - lp_state_);
						*out++ = (this_sample - lp_state_) * 0.5f;
					}
					else {
						this_sample = 2.0f * this_sample - 1.0f;
						*out++ = this_sample;
					}
				}
			}
			next_sample_ = next_sample;
		}

	private:
		// Oscillator state.
		float phase_;
		float next_sample_;
		float lp_state_;
		float hp_state_;
		bool high_;

		// For interpolation of parameters.
		float frequency_;
		float pw_;

		DISALLOW_COPY_AND_ASSIGN(Oscillator);
	};

} // namespace plaits

#endif // PLAITS_DSP_OSCILLATOR_OSCILLATOR_H_