#include "plugin.hpp"


struct PlayGround : Module {
	enum ParamIds {
		PARAM1_PARAM,
		PARAM2_PARAM,
		PARAM3_PARAM,
		PARAM4_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		LIGHT1_LIGHT,
		LIGHT2_LIGHT,
		LIGHT3_LIGHT,
		LIGHT4_LIGHT,
		NUM_LIGHTS
	};

	dsp::SchmittTrigger edgeDetector;
	float phase = 0.f;
	float phaseLfo = 0.f;

	bool isAtk, isRunning;
	float env;
	#define EPSILON 1e-9

	PlayGround() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM1_PARAM, 0.f, 5.f, 0.5f, "");
		configParam(PARAM2_PARAM, 0.f, 4.f, 2.f, "");
		configParam(PARAM3_PARAM, 0.f, 1.f, 1.f, "");
		configParam(PARAM4_PARAM, 1.f, 20.f, 1.f, "");
	}

	void process(const ProcessArgs& args) override {

		float oct = params[PARAM3_PARAM].getValue()+1;
		// Sine output
		float pitch = inputs[IN2_INPUT].getVoltage();
		pitch = clamp(pitch, -4.f, 4.f);
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);
		freq = freq * oct;
		// Accumulate the phase
		phase += freq * args.sampleTime;
		if (phase >= 0.5f)
				phase -= 1.f;

		// Compute the sine output
		// Add harmonics for richer sound
		float sine = 0.f;//std::sin(2.f * M_PI * phase);
		int harmoNumber = params[PARAM4_PARAM].getValue();
		for (int i = 1; i <= harmoNumber; i++) {
			float harmo = std::sin(2.f* i * M_PI * phase)*(1.f/i);
			sine = sine + harmo;
	}

		// Audio signals are typically +/-5V
		// https://vcvrack.com/manual/VoltageStandards.html
		outputs[OUT1_OUTPUT].setVoltage(5.f * sine);
//////////////////////////////////////////////////////////////
	// Tremolo
	float lfo = std::sin(2.f * M_PI * phaseLfo);
	float pitchLfo = inputs[IN2_INPUT].getVoltage();
	pitchLfo = clamp(pitchLfo, -4.f, 4.f);
	float freqLfo = dsp::FREQ_C4 * std::pow(2.f, pitchLfo);

	// Accumulate the phase
	phaseLfo += freqLfo * args.sampleTime;
	if (phaseLfo >= 0.5f)
			phaseLfo -= 1.f;

	outputs[OUT4_OUTPUT].setVoltage(5.f * lfo);
//////////////////////////////////////////////////////////////
	// Velocity
	float velo = inputs[IN3_INPUT].getVoltage()/8.f;
//////////////////////////////////////////////////////////////
	// WaveShapping
	float x = clamp(sine * 0.1f, -1.f, 1.f);
	float a = clamp(params[PARAM2_PARAM].getValue(), 1.f, 50.f);
	sine = x * (fabs(x) + a) / (x * x + (a - 1) * fabs(x) + 1);
//////////////////////////////////////////////////////////////
	// Envelope
	float sus = 0.8f;//params[PARAM_SUS].getValue();
	float Astep = 1.f / (EPSILON + args.sampleRate * params[PARAM1_PARAM].getValue());
	float Dstep = (sus - 1.0) / (EPSILON + args.sampleRate * 0.2f);//params[PARAM_DEC].getValue());
	float Rstep = -(sus + EPSILON) / (EPSILON + args.sampleRate * 0.2f);//params[PARAM_REL].getValue());

	Astep = clamp(Astep, EPSILON, 0.5);
	Dstep = std::max(Dstep, -0.5f);
	Rstep = std::max(Rstep, -1.f);

	bool gate = inputs[IN1_INPUT].getVoltage() >= 1.0;
	if (edgeDetector.process(gate)) {
		isAtk = true;
		isRunning = true;
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

	if (outputs[OUT2_OUTPUT].isConnected()) {
		outputs[OUT2_OUTPUT].setVoltage(10.f * env);
		outputs[OUT3_OUTPUT].setVoltage(5.f * sine * env * velo);
	}

//////////////////////////////////////////////////////////////
		// Trigger lights a light
		if (edgeDetector.process(inputs[IN1_INPUT].getVoltage())){
			lights[LIGHT1_LIGHT].setSmoothBrightness(1,5e-6f);

		} else {
			lights[LIGHT1_LIGHT].setSmoothBrightness(0,5e-6f);
		}


	}
};


struct PlayGroundWidget : ModuleWidget {
	PlayGroundWidget(PlayGround* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/playGround.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(13.386, 75.601)), module, PlayGround::PARAM1_PARAM));
		addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(25.24, 75.601)), module, PlayGround::PARAM2_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(37.093, 75.601)), module, PlayGround::PARAM3_PARAM));
		addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(48.946, 75.601)), module, PlayGround::PARAM4_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.386, 32.139)), module, PlayGround::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.24, 32.139)), module, PlayGround::IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.093, 32.139)), module, PlayGround::IN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.946, 32.139)), module, PlayGround::IN4_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.386, 97.05)), module, PlayGround::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.24, 97.05)), module, PlayGround::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.093, 97.05)), module, PlayGround::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.946, 97.05)), module, PlayGround::OUT4_OUTPUT));

		addChild(createLightCentered<TinyLight<RedLight>>(mm2px(Vec(13.386, 54.152)), module, PlayGround::LIGHT1_LIGHT));
		addChild(createLightCentered<SmallLight<RedLight>>(mm2px(Vec(25.24, 54.152)), module, PlayGround::LIGHT2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.093, 54.152)), module, PlayGround::LIGHT3_LIGHT));
		addChild(createLightCentered<LargeLight<RedLight>>(mm2px(Vec(48.946, 54.152)), module, PlayGround::LIGHT4_LIGHT));
	}
};


Model* modelPlayGround = createModel<PlayGround, PlayGroundWidget>("playGround");
