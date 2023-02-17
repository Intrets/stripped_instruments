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
// Main synthesis voice.

#ifndef PLAITS_DSP_VOICE_H_
#define PLAITS_DSP_VOICE_H_

#include "stmlib/stmlib.h"

#include "stmlib/dsp/filter.h"
#include "stmlib/dsp/limiter.h"
#include "stmlib/utils/buffer_allocator.h"

#include "plaits/dsp/engine/additive_engine.h"
#include "plaits/dsp/engine/chord_engine.h"
#include "plaits/dsp/engine/engine.h"
#include "plaits/dsp/engine/fm_engine.h"
#include "plaits/dsp/engine/grain_engine.h"
#include "plaits/dsp/engine/modal_engine.h"
#include "plaits/dsp/engine/noise_engine.h"
#include "plaits/dsp/engine/particle_engine.h"
#include "plaits/dsp/engine/string_engine.h"
#include "plaits/dsp/engine/swarm_engine.h"
#include "plaits/dsp/engine/virtual_analog_engine.h"
#include "plaits/dsp/engine/waveshaping_engine.h"
#include "plaits/dsp/engine/wavetable_engine.h"

#include "plaits/dsp/envelope.h"

#include "plaits/dsp/fx/low_pass_gate.h"

namespace plaits
{

	int const kMaxEngines = 16;
	int const kMaxTriggerDelay = 8;
	int const kTriggerDelay = 5;

	class ChannelPostProcessor
	{
	public:
		ChannelPostProcessor() {
		}
		~ChannelPostProcessor() {
		}

		void Init() {
			Reset();
		}

		void Reset() {
			limiter_.Init();
		}

		float Process(
		    float gain,
		    float in
		) {
			if (gain < 0.0f) {
				in = limiter_.Process(-gain, in);
			}

			return in;
		}

	private:
		stmlib::Limiter limiter_;

		DISALLOW_COPY_AND_ASSIGN(ChannelPostProcessor);
	};

	struct Patch
	{
		float note;
		float harmonics;
		float timbre;
		float morph;
		float frequency_modulation_amount;
		float timbre_modulation_amount;
		float morph_modulation_amount;
		float samplePeriod;

		int engine;
		float decay;
		float lpg_colour;
	};

	struct Modulations
	{
		float engine;
		float note;
		float frequency;
		float harmonics;
		float timbre;
		float morph;
		float trigger;
		float level;

		bool frequency_patched;
		bool timbre_patched;
		bool morph_patched;
		bool trigger_patched;
		bool level_patched;
		bool sustain;
		bool trigger2;
	};

	class Voice
	{
	public:
		Voice() {
		}
		~Voice() {
		}

		struct Frame
		{
			float out;
			float aux;
		};

		void Init(stmlib::BufferAllocator* allocator);
		Frame Render(
		    Patch const& patch,
		    Modulations const& modulations
		);
		inline int active_engine() const {
			return previous_engine_index_;
		}

	private:
		void ComputeDecayParameters(Patch const& settings);

		AdditiveEngine additive_engine_;
		ChordEngine chord_engine_;
		FMEngine fm_engine_;
		GrainEngine grain_engine_;
		ModalEngine modal_engine_;
		NoiseEngine noise_engine_;
		ParticleEngine particle_engine_;
		StringEngine string_engine_;
		SwarmEngine swarm_engine_;
		VirtualAnalogEngine virtual_analog_engine_;
		WaveshapingEngine waveshaping_engine_;
		WavetableEngine wavetable_engine_;

		stmlib::HysteresisQuantizer engine_quantizer_;

		int previous_engine_index_;
		float engine_cv_;

		ChannelPostProcessor out_post_processor_;
		ChannelPostProcessor aux_post_processor_;

		EngineRegistry<kMaxEngines> engines_;

		float out_buffer_[kMaxBlockSize];
		float aux_buffer_[kMaxBlockSize];

		DISALLOW_COPY_AND_ASSIGN(Voice);
	};

} // namespace plaits

#endif // PLAITS_DSP_VOICE_H_
