#include "plugin.hpp"


struct Mirror : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		IN1_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT1_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Mirror() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
	}

	void process(const ProcessArgs& args) override {
		// Mirror the input signal to the output
		float in = 0.f;
		in = inputs[IN1_INPUT].getVoltage();
		outputs[OUT1_OUTPUT].setVoltage(in);
	}
};


struct MirrorWidget : ModuleWidget {
	MirrorWidget(Mirror* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/mirror.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.96, 15.451)), module, Mirror::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.96, 96.329)), module, Mirror::OUT1_OUTPUT));
	}
};


Model* modelMirror = createModel<Mirror, MirrorWidget>("Mirror");
