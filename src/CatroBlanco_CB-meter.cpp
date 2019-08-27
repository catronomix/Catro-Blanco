#include "CatroBlanco.hpp"


//Catro-Blanco CB-meter: 2hp meter

struct CBmeterModule : Module {

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

    float sig = 9.9;
    float sig2 = 9.9;
	float ysize = 314.0;
    float ystart = 41.0;
    bool clip = 0.0;
	bool input_active = false;
	int quantstep = 0;
	int mode = 1;
	dsp::SchmittTrigger modetrigger;

	CBmeterModule() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CBmeterModule::PARAM_MODE, 0.0, 1.0, 0.0, "MODE");
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

void CBmeterModule::process(const ProcessArgs &args) {
	if (modetrigger.process(params[PARAM_MODE].getValue())){
		mode = (mode < 4) ? mode + 1 : 0;
	}

	// if (quantstep < (APP->engine->getSampleRate() * 0.01)){
	// 	quantstep++;
	// }else{
		if (inputs[INPUT_SIG].active){
			input_active = true;
            sig = fmaxf(8.0, inputs[INPUT_SIG].getVoltage());
            ysize = fminf(314.0, fmaxf(ysize - 0.01, fabsf(inputs[INPUT_SIG].getVoltage() * 0.1) * 314.0));
			ystart = 41.0 + (314.0 - ysize);
            clip = (inputs[INPUT_SIG].getVoltage() >= 10.0);
		}else{
			input_active = false;
		}
	// 	quantstep = 0;
	// }
}

struct CBmeterModuleWidget : ModuleWidget {

	CBmeterModuleWidget(CBmeterModule *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-meter.svg")));

		if (module)
		{
			//Meter
            CB_ColorCycler *CCycler = new CB_ColorCyclerMeter();
            CCycler->box.pos = Vec(1.4 , 355.0);
            CCycler->box.size = Vec(27.8 , 0.0);
            CCycler->sig = &module->sig;
            CCycler->ystart = &module->ystart;
            CCycler->ysize = &module->ysize;
			CCycler->active = &module->input_active;
			CCycler->mode = &module->mode;
            addChild(CCycler);

            //clipping
            CB_ColorCycler *CCycler2 = new CB_ColorCycler();
            CCycler2->box.pos = Vec(10.2, 19.7);
            CCycler2->box.size = Vec(9.6, 9.6);
            CCycler2->sig = &module->sig2;
			CCycler2->active = &module->clip;
			CCycler2->mode = &module->mode;
            addChild(CCycler2);

			//Front panel
			CBmeter_FrontPanel *frontpanel = new CBmeter_FrontPanel();
			addChild(frontpanel);
		}

		//mode button
		addParam(createParam<CB_Ledbutton_mini>(Vec(3 , 353), module, CBmeterModule::PARAM_MODE));

		//input
		addInput(createInput<CB_Input_small>(Vec(10, 360), module, CBmeterModule::INPUT_SIG));

		
	}
};

Model *modelCBmeterModule = createModel<CBmeterModule, CBmeterModuleWidget>("CatroBlanco_CB-meter");
