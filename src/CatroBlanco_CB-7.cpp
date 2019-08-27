#include "CatroBlanco.hpp"


//Catro-Blanco CB-7: 10hp blank alternate

struct CB7Module : Module {

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

	CB7Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CB7Module::PARAM_MODE, 0.0, 1.0, 0.0, "MODE");
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

void CB7Module::process(const ProcessArgs &args) {
	if (modetrigger.process(params[PARAM_MODE].getValue())){
		mode = (mode < 4) ? mode + 1 : 0;
	}

	// if (quantstep < (APP->engine->getSampleRate() * 0.01)){
	// 	quantstep++;
	// }else{
		if (inputs[INPUT_SIG].active){
			input_active = true;
			sig2 = inputs[INPUT_SIG].getVoltageSum();
            sig = (sig + sig2) / 3.0;
		}else{
			input_active = false;
		}
	// 	quantstep = 0;
	// }
}

struct CB7ModuleWidget : ModuleWidget {

	CB7ModuleWidget(CB7Module *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-7.svg")));

		if (module)
		{
			//Visualisations
            CB_ColorCycler *CCycler1 = new CB_ColorCycler(0x8A);
            CCycler1->box.pos = Vec(0.0, 0.0);
            CCycler1->box.size = Vec(192.0, 485.0);
            CCycler1->sig = &module->sig;
			CCycler1->active = &module->input_active;
			CCycler1->mode = &module->mode;
            addChild(CCycler1);

            CB_ColorCycler *CCycler2 = new CB_ColorCyclerCircle();
            CCycler2->box.pos = Vec(75.0, 20.5);
            CCycler2->box.size = Vec(15.4, 15.4);
            CCycler2->sig = &module->sig2;
			CCycler2->active = &module->input_active;
			CCycler2->mode = &module->mode;
            addChild(CCycler2);

            CB_ColorCycler *CCycler3 = new CB_ColorCyclerCircle();
            CCycler3->box.pos = Vec(75.0, 150.4);
            CCycler3->box.size = Vec(12.0, 12.0);
            CCycler3->sig = &module->sig2;
			CCycler3->active = &module->input_active;
			CCycler3->mode = &module->mode;
            addChild(CCycler3);

			CB_ColorCycler *CCycler4 = new CB_ColorCycler();
            CCycler4->box.pos = Vec(43.0, 44.0);
            CCycler4->box.size = Vec(64.0, 4.2);
            CCycler4->sig = &module->sig2;
			CCycler4->active = &module->input_active;
			CCycler4->mode = &module->mode;
            addChild(CCycler4);

			CB_ColorCycler *CCycler5 = new CB_ColorCycler();
            CCycler5->box.pos = Vec(69.6, 51.4);
            CCycler5->box.size = Vec(11.0, 45.0);
            CCycler5->sig = &module->sig2;
			CCycler5->active = &module->input_active;
			CCycler5->mode = &module->mode;
            addChild(CCycler5);


			//Front panel
			CB7_FrontPanel *frontpanel = new CB7_FrontPanel();
			addChild(frontpanel);
		}

		//mode button
		addParam(createParam<CB_Ledbutton_mini>(Vec(21 , 362), module, CB7Module::PARAM_MODE));

		//input
		addInput(createInput<CB_Input_small>(Vec(2, 360), module, CB7Module::INPUT_SIG));

	}
};

Model *modelCB7Module = createModel<CB7Module, CB7ModuleWidget>("CatroBlanco_CB-7");
