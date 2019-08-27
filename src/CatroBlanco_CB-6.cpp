#include "CatroBlanco.hpp"


//Catro-Blanco CB-6: 5hp blank alternate

struct CB6Module : Module {

	enum ParamIds {
		PARAM_MODE,
		NUM_PARAMS
	};
	enum InputIds {
		INPUT_SIG,
		NUM_INPUTS
	};
	enum OutputIds {
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	float sig = 0.0;
    float sig2 = 0.0;
	bool input_active = false;
	int quantstep = 0;
	int mode = 0;
	dsp::SchmittTrigger modetrigger;

	CB6Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CB6Module::PARAM_MODE, 0.0, 1.0, 0.0, "MODE");
	}
	void process(const ProcessArgs &args) override;

    	json_t *dataToJson() override {

		json_t *rootJ = json_object();

		json_t *settingsJ = json_array();
		json_array_append_new(settingsJ, json_real(mode));	
		json_object_set_new(rootJ, "settings", settingsJ);
	
		return rootJ;
	}


	void dataFromJson(json_t *rootJ) override {
		// running
		json_t *settingsJ = json_object_get(rootJ, "settings");
		mode = json_real_value(json_array_get(settingsJ, 0));			
	}
};

void CB6Module::process(const ProcessArgs &args) {
	if (modetrigger.process(params[PARAM_MODE].getValue())){
		mode = (mode < 4) ? mode + 1 : 0;
	}

	// if (quantstep < (APP->engine->getSampleRate() * 0.01)){
	// 	quantstep++;
	// }else{
		if (inputs[INPUT_SIG].active){
			input_active = true;
			sig2 = -inputs[INPUT_SIG].getVoltageSum();
            sig = -sig2 * 2;
		}else{
			input_active = false;
		}
	// 	quantstep = 0;
	// }
}

struct CB6ModuleWidget : ModuleWidget {

	CB6ModuleWidget(CB6Module *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-6.svg")));

		if (module)
		{
			//Visualisations
            CB_ColorCycler *CCycler1 = new CB_ColorCycler();
            CCycler1->box.pos = Vec(30.0, 39.0);
            CCycler1->box.size = Vec(15.0,26.0);
            CCycler1->sig = &module->sig;
			CCycler1->active = &module->input_active;
			CCycler1->mode = &module->mode;
            addChild(CCycler1);

            CB_ColorCycler *CCycler2 = new CB_ColorCycler(0x80);
            CCycler2->box.pos = Vec(0.0, 0.0);
            CCycler2->box.size = Vec(75.0, 378.0);
            CCycler2->sig = &module->sig2;
			CCycler2->active = &module->input_active;
			CCycler2->mode = &module->mode;
            addChild(CCycler2);


			//Front panel
			CB6_FrontPanel *frontpanel = new CB6_FrontPanel();
			addChild(frontpanel);
		}

		//mode button
		addParam(createParam<CB_Ledbutton_mini>(Vec(63 , 367), module, CB6Module::PARAM_MODE));

		//input
		addInput(createInput<CB_Input_small>(Vec(4, 359), module, CB6Module::INPUT_SIG));

	}
};

Model *modelCB6Module = createModel<CB6Module, CB6ModuleWidget>("CatroBlanco_CB-6");
