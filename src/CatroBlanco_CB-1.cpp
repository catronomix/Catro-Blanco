#include "CatroBlanco.hpp"


//Catro-Blanco CB-1: 2hp blank

struct CB1Module : Module {

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
	bool input_active = false;
	int quantstep = 0;
	int mode = 0;
	dsp::SchmittTrigger modetrigger;

	CB1Module() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CB1Module::PARAM_MODE, 0.0, 1.0, 0.0, "MODE");
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

void CB1Module::process(const ProcessArgs &args) {
	if (modetrigger.process(params[PARAM_MODE].getValue())){
		mode = (mode < 4) ? mode + 1 : 0;
	}

	// if (quantstep < (APP->engine->getSampleRate() * 0.01)){
	// 	quantstep++;
	// }else{
		if (inputs[INPUT_SIG].active){
			input_active = true;
			sig = inputs[INPUT_SIG].getVoltageSum();
		}else{
			input_active = false;
		}
	// 	quantstep = 0;
	// }
}

struct CB1ModuleWidget : ModuleWidget {

	CB1ModuleWidget(CB1Module *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-1.svg")));

		if (module)
		{
			//Visualisation
            CB_ColorCycler *CCycler = new CB_ColorCycler();
            CCycler->box.pos = Vec(0.0, 0.0);
            CCycler->box.size = Vec(29.0, 379.0);
            CCycler->sig = &module->sig;
			CCycler->active = &module->input_active;
			CCycler->mode = &module->mode;
            addChild(CCycler);

			//Front panel
			CB1_FrontPanel *frontpanel = new CB1_FrontPanel();
			// ledindicator->posx = &module->ledx;
			// ledindicator->posy = &module->ledy;
			addChild(frontpanel);
		}

		//mode button
		addParam(createParam<CB_Ledbutton_mini>(Vec(3 , 353), module, CB1Module::PARAM_MODE));

		//input
		addInput(createInput<CB_Input_small>(Vec(10, 360), module, CB1Module::INPUT_SIG));

		
	}
};

Model *modelCB1Module = createModel<CB1Module, CB1ModuleWidget>("CatroBlanco_CB-1");
