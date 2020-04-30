#include "plugin.hpp"


struct Synth : Module {
	enum ParamIds {
		OCTAVE_PARAM,
		DETUNE_PARAM,
		A_PARAM,
		D_PARAM,
		S_PARAM,
		R_PARAM,
		TREMOLO_PARAM,
		HARMO_PARAM,
		SHAPER_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		VOCT_INPUT,
		GATE_INPUT,
		VELOCITY_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		GATELIGHT_LIGHT,
		NUM_LIGHTS
	};

	// Variable and constant declaration
	dsp::SchmittTrigger edgeDetector;
	float phase = 0.f;
	float phaseLfo = 0.f;

	bool isAtk, isRunning;
	float env;
	#define EPSILON 1e-9


	Synth() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(OCTAVE_PARAM, -2.f, 2.f, 0.f, "");
		configParam(DETUNE_PARAM, -1.f, 1.f, 0.f, "Fine tune", " cents", 0.0f, 100.0f);

		configParam(A_PARAM, 0.f, 2.f, 0.1f, "Attack", " s");
		configParam(D_PARAM, 0.f, 2.f, 0.5f, "Decay", " s");
		configParam(S_PARAM, 0.f, 1.f, 0.8f, "Sustain");
		configParam(R_PARAM, 0.f, 2.f, 0.5f, "Release", " s");

		configParam(TREMOLO_PARAM, 0.f, 4.f, 0.f, "");
		configParam(HARMO_PARAM, 1.f, 20.f, 0.f, "");
		configParam(SHAPER_PARAM, 0.f, 10.f, 5.f, "");
	}

	void process(const ProcessArgs& args) override {
		// Sine output
		float pitch = inputs[VOCT_INPUT].getVoltage();

		// Octave
		float oct = params[OCTAVE_PARAM].getValue();
		pitch += oct;

		// Detune
		pitch += params[DETUNE_PARAM].getValue() / 12.0f;;
		pitch = clamp(pitch, -4.f, 4.f);

		// Accumulate the phase
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);
		phase += freq * args.sampleTime;
		if (phase >= 0.5f)
				phase -= 1.f;

		// Compute the sine output
		// Add harmonics for richer sound
		float sine = 0.f;
		int harmoNumber = params[HARMO_PARAM].getValue();
		for (int i = 1; i <= harmoNumber; i++) {
			float harmo = std::sin(2.f* i * M_PI * phase)*(1.f/i);
			sine = sine + harmo;
	}
		// Tremolo
		float lfo = std::sin(2.f * M_PI * phaseLfo);
		float freqLfo = params[TREMOLO_PARAM].getValue();
		phaseLfo += freqLfo * args.sampleTime;
		if (phaseLfo >= 0.5f)
				phaseLfo -= 1.f;
				float offset = 1 - 0.05;
				lfo = lfo * 0.08f + offset;

//////////////////////////////////////////////////////////////
		// Velocity
		float velo = inputs[VELOCITY_INPUT].getVoltage()/8.f;
//////////////////////////////////////////////////////////////
		// WaveShapping
		// https://github.com/lindenbergresearch/LRTRack/blob/master/src/modules/ReShaper.cpp
		float x = clamp(sine * 0.1f, -1.f, 1.f);
		float a = clamp(params[SHAPER_PARAM].getValue(), 1.f, 50.f);
		sine = x * (fabs(x) + a) / (x * x + (a - 1) * fabs(x) + 1);
		sine = sine * 5.f;


//////////////////////////////////////////////////////////////
		// Envelope
		float sus = params[S_PARAM].getValue();
		float Astep = 1.f / (EPSILON + args.sampleRate * params[A_PARAM].getValue());
		float Dstep = (sus - 1.0) / (EPSILON + args.sampleRate * params[D_PARAM].getValue());
		float Rstep = -(sus + EPSILON) / (EPSILON + args.sampleRate * params[R_PARAM].getValue());

		Astep = clamp(Astep, EPSILON, 0.5);
		Dstep = std::max(Dstep, -0.5f);
		Rstep = std::max(Rstep, -1.f);

		bool gate = inputs[GATE_INPUT].getVoltage() >= 1.0;
		if (edgeDetector.process(gate)) {
			isAtk = true;
			isRunning = true;
			lights[GATELIGHT_LIGHT].setSmoothBrightness(1,5e-6f);
		} else {
			lights[GATELIGHT_LIGHT].setSmoothBrightness(0,5e-6f);
		}


		if (isRunning) {
			if (gate) {
				// ATK
				if (isAtk) {
					env += Astep;
					if (env >= 1.0)
						isAtk = false;
				}
				else {
					// DEC
					if (env <= sus + 0.001) {
						env = sus;
					}
					else {
						env += Dstep;
					}
				}
			} else {
				// REL
				env += Rstep;
				if (env <= Rstep)
					isRunning = false;
			}
		} else {
			env = 0.0;
		}

		if (outputs[OUT_OUTPUT].isConnected()) {
			outputs[OUT_OUTPUT].setVoltage(5.f * sine * lfo * velo * env);
		}
	}
};


struct SynthWidget : ModuleWidget {
	SynthWidget(Synth* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/synth.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(42.143, 34.131)), module, Synth::OCTAVE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(66.958, 34.131)), module, Synth::DETUNE_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(39.482, 59.558)), module, Synth::A_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(50.107, 59.558)), module, Synth::D_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(60.732, 59.558)), module, Synth::S_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(71.358, 59.558)), module, Synth::R_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(28.46, 91.86)), module, Synth::TREMOLO_PARAM));
		addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(28.46, 103.177)), module, Synth::HARMO_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(28.46, 114.495)), module, Synth::SHAPER_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.79, 34.131)), module, Synth::VOCT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.79, 59.558)), module, Synth::GATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.79, 76.431)), module, Synth::VELOCITY_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.771, 114.495)), module, Synth::OUT_OUTPUT));

		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(21.054, 56.705)), module, Synth::GATELIGHT_LIGHT));
	}
};


Model* modelSynth = createModel<Synth, SynthWidget>("synth");
