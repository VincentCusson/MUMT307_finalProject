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

	PlayGround() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(PARAM1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM2_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM3_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM4_PARAM, 0.f, 1.f, 0.f, "");
	}

	void process(const ProcessArgs& args) override {
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(13.386, 75.601)), module, PlayGround::PARAM1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.24, 75.601)), module, PlayGround::PARAM2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(37.093, 75.601)), module, PlayGround::PARAM3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(48.946, 75.601)), module, PlayGround::PARAM4_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(13.386, 32.139)), module, PlayGround::IN1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.24, 32.139)), module, PlayGround::IN2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.093, 32.139)), module, PlayGround::IN3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.946, 32.139)), module, PlayGround::IN4_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(13.386, 97.05)), module, PlayGround::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.24, 97.05)), module, PlayGround::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(37.093, 97.05)), module, PlayGround::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.946, 97.05)), module, PlayGround::OUT4_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(13.386, 54.152)), module, PlayGround::LIGHT1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(25.24, 54.152)), module, PlayGround::LIGHT2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(37.093, 54.152)), module, PlayGround::LIGHT3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(48.946, 54.152)), module, PlayGround::LIGHT4_LIGHT));
	}
};


Model* modelPlayGround = createModel<PlayGround, PlayGroundWidget>("playGround");