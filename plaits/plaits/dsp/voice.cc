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

#include "plaits/dsp/voice.h"

namespace plaits
{

	using namespace std;
	using namespace stmlib;

	void Voice::Init(BufferAllocator* allocator) {
		engines_.Init();
		engines_.RegisterInstance(&virtual_analog_engine_, false, 0.8f, 0.8f);
		engines_.RegisterInstance(&waveshaping_engine_, false, 0.7f, 0.6f);
		engines_.RegisterInstance(&fm_engine_, false, 0.6f, 0.6f);
		engines_.RegisterInstance(&grain_engine_, false, 0.7f, 0.6f);
		engines_.RegisterInstance(&additive_engine_, false, 0.8f, 0.8f);
		engines_.RegisterInstance(&wavetable_engine_, false, 0.6f, 0.6f);
		engines_.RegisterInstance(&chord_engine_, false, 0.8f, 0.8f);

		engines_.RegisterInstance(&swarm_engine_, false, -3.0f, 1.0f);
		engines_.RegisterInstance(&noise_engine_, false, -1.0f, -1.0f);
		engines_.RegisterInstance(&particle_engine_, false, -2.0f, 1.0f);
		engines_.RegisterInstance(&string_engine_, true, -1.0f, 0.8f);
		engines_.RegisterInstance(&modal_engine_, true, -1.0f, 0.8f);
		engines_.RegisterInstance(&bass_drum_engine_, true, 0.8f, 0.8f);
		engines_.RegisterInstance(&snare_drum_engine_, true, 0.8f, 0.8f);
		engines_.RegisterInstance(&hi_hat_engine_, true, 0.8f, 0.8f);
		for (int i = 0; i < engines_.size(); ++i) {
			// All engines will share the same RAM space.
			allocator->Free();
			engines_.get(i)->Init(allocator);
		}

		engine_quantizer_.Init();
		previous_engine_index_ = -1;
		engine_cv_ = 0.0f;

		out_post_processor_.Init();
		aux_post_processor_.Init();
	}

	Voice::Frame Voice::Render(
	    Patch const& patch,
	    Modulations const& modulations
	) {
		using math = crack::audio::StdContext;
		Frame result{};
		// Trigger, LPG, internal envelope.

		// Engine selection.
		int engine_index = engine_quantizer_.Process(
		    patch.engine,
		    modulations.engine,
		    engines_.size(),
		    0.25f
		);

		Engine* e = engines_.get(engine_index);

		if (engine_index != previous_engine_index_) {
			e->Reset();
			out_post_processor_.Reset();
			previous_engine_index_ = engine_index;
		}
		EngineParameters p;
		p.samplePeriod = patch.samplePeriod;
		p.trigger2 = modulations.trigger2;
		p.sustain = modulations.sustain;

		PostProcessingSettings const& pp_s = e->post_processing_settings;

		float const compressed_level = max(
		    1.3f * modulations.level / (0.3f + fabsf(modulations.level)),
		    0.0f
		);
		p.accent = modulations.level_patched ? compressed_level : 0.8f;
		p.harmonics = math::clamp(patch.harmonics + modulations.harmonics, 0.0f, 1.0f);
		p.note = math::clamp(patch.note, -119.0f, 120.0f);
		p.timbre = math::clamp(patch.timbre, 0.0f, 1.0f);
		p.morph = math::clamp(patch.morph, 0.0f, 1.0f);

		bool already_enveloped = pp_s.already_enveloped;
		e->Render(p, &result.out, &result.aux, 1, &already_enveloped);

		result.out = out_post_processor_.Process(
		    pp_s.out_gain,
		    result.out
		);

		result.aux = aux_post_processor_.Process(
		    pp_s.aux_gain,
		    result.aux
		);

		return result;
	}

} // namespace plaits
