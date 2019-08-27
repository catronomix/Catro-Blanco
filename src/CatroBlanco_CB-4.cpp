#include "CatroBlanco.hpp"


//Catro-Blanco CB-4: 4hp blank

struct CB4Module : Module {

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
    float sig3 = 0.0;
	bool input_active = false;
	int quantstep = 0;
	int mode = 0;
	dsp::SchmittTrigger modetrigger;

	CB4Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CB4Module::PARAM_MODE, 0.0, 1.0, 0.0, "MODE");
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

void CB4Module::process(const ProcessArgs &args) {
	if (modetrigger.process(params[PARAM_MODE].getValue())){
		mode = (mode < 4) ? mode + 1 : 0;
	}

	// if (quantstep < (APP->engine->getSampleRate() * 0.01)){
	// 	quantstep++;
	// }else{
		if (inputs[INPUT_SIG].active){
			input_active = true;
			sig = inputs[INPUT_SIG].getVoltageSum();
            sig2 = 10 / (((sig < -0.00000001 || sig > 0.00000001) ? sig * 0.1 : 0.00000001) * 10);
            sig3 = 10 - (pow(sig * 0.1, 2 ) * 10.0);
		}else{
			input_active = false;
		}
	// 	quantstep = 0;
	// }
}

struct CB4ModuleWidget : ModuleWidget {

	CB4ModuleWidget(CB4Module *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-4.svg")));

		if (module)
		{
			//Visualisations
            CB_ColorCycler *CCycler1 = new CB_ColorCycler();
            CCycler1->box.pos = Vec(0.0, 0.0);
            CCycler1->box.size = Vec(60.0, 301.0);
            CCycler1->sig = &module->sig;
			CCycler1->active = &module->input_active;
			CCycler1->mode = &module->mode;
            addChild(CCycler1);

            CB_ColorCycler *CCycler2 = new CB_ColorCycler(0xA0);
            CCycler2->box.pos = Vec(0.0, 94.0);
            CCycler2->box.size = Vec(60.0, 207.0);
            CCycler2->sig = &module->sig2;
			CCycler2->active = &module->input_active;
			CCycler2->mode = &module->mode;
            addChild(CCycler2);

            CB_ColorCycler *CCycler3 = new CB_ColorCycler(0x5F);
            CCycler3->box.pos = Vec(0.0, 301.0);
            CCycler3->box.size = Vec(60.0, 78.0);
            CCycler3->sig = &module->sig3;
			CCycler3->active = &module->input_active;
			CCycler3->mode = &module->mode;
            addChild(CCycler3);

			//Front panel
			CB4_FrontPanel *frontpanel = new CB4_FrontPanel();
			addChild(frontpanel);
		}

		//mode button
		addParam(createParam<CB_Ledbutton_mini>(Vec(26 , 368), module, CB4Module::PARAM_MODE));

		//input
		addInput(createInput<CB_Input_small>(Vec(40, 360), module, CB4Module::INPUT_SIG));

	}
};

Model *modelCB4Module = createModel<CB4Module, CB4ModuleWidget>("CatroBlanco_CB-4");
