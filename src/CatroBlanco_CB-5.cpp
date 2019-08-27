#include "CatroBlanco.hpp"


//Catro-Blanco CB-5: 4hp blank alternate

struct CB5Module : Module {

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

	CB5Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CB5Module::PARAM_MODE, 0.0, 1.0, 0.0, "MODE");
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

void CB5Module::process(const ProcessArgs &args) {
	if (modetrigger.process(params[PARAM_MODE].getValue())){
		mode = (mode < 4) ? mode + 1 : 0;
	}

	// if (quantstep < (APP->engine->getSampleRate() * 0.01)){
	// 	quantstep++;
	// }else{
		if (inputs[INPUT_SIG].active){
			input_active = true;
			sig = inputs[INPUT_SIG].getVoltageSum();
            sig2 = (mode == 1 || mode == 3) ? -sig : -sig * 0.5;
		}else{
			input_active = false;
		}
	// 	quantstep = 0;
	// }
}

struct CB5ModuleWidget : ModuleWidget {

	CB5ModuleWidget(CB5Module *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-5.svg")));

		if (module)
		{
			//Visualisations
            CB_ColorCycler *CCycler1 = new CB_ColorCycler(0x80);
            CCycler1->box.pos = Vec(0.0, 0.0);
            CCycler1->box.size = Vec(60.0,378.0);
            CCycler1->sig = &module->sig;
			CCycler1->active = &module->input_active;
			CCycler1->mode = &module->mode;
            addChild(CCycler1);

            CB_ColorCycler *CCycler2 = new CB_ColorCycler();
            CCycler2->box.pos = Vec(14.0, 181.0);
            CCycler2->box.size = Vec(33.0, 33.0);
            CCycler2->sig = &module->sig2;
			CCycler2->active = &module->input_active;
			CCycler2->mode = &module->mode;
            addChild(CCycler2);


			//Front panel
			CB5_FrontPanel *frontpanel = new CB5_FrontPanel();
			addChild(frontpanel);
		}

		//mode button
		addParam(createParam<CB_Ledbutton_mini>(Vec(26 , 366), module, CB5Module::PARAM_MODE));

		//input
		addInput(createInput<CB_Input_small>(Vec(7, 354), module, CB5Module::INPUT_SIG));

	}
};

Model *modelCB5Module = createModel<CB5Module, CB5ModuleWidget>("CatroBlanco_CB-5");
