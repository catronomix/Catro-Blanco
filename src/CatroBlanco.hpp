#include "rack.hpp"

using namespace rack;


// Forward-declare the Plugin, defined in Template.cpp
extern Plugin *pluginInstance;

// Forward-declare each Model, defined in each module source file
extern Model *modelCB1Module;
extern Model *modelCB2Module;
extern Model *modelCB3Module;
extern Model *modelCB4Module;
extern Model *modelCB5Module;
extern Model *modelCB6Module;
extern Model *modelCB7Module;
extern Model *modelCBmeterModule;
//extern Model *modelCB9Module;
//extern Model *modelCB10Module;

//interface elements

struct CB_Input_small : SvgPort {
	CB_Input_small() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-input_small.svg")));
        shadow->opacity = 0;
	}
};

struct CB_Ledbutton_mini : SvgSwitch {
	CB_Ledbutton_mini() {
		momentary = true;
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-ledbutton_mini_0.svg")));
		addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-ledbutton_mini_1.svg")));
	}
};
//functions
struct CB_ZeroTrigger{
	bool high = false;
	bool last = false;
	bool trig = false;
	int clock = 0;
	const float interval = 44.0f;
	float freq = 0;
	float lastfreq = 0;

	CB_ZeroTrigger(){};

	bool process(float sig){
		//run counting clock
		if(clock < interval){
			clock++;
		}

		//zero trigger generation
		if (sig > 0.0001){
			high = true;
		}else{
			high = false;
		}
		if (last != high){
			trig = true;
			freq = powf(1.0f - (float)clock / interval, 2);
			lastfreq = lastfreq * 0.8 + freq * 0.2;
			clock = 0;
			
		}else{
			trig = false;
		}
		last = high;
		return trig;
	}

	float getFreq(){
		return lastfreq * 0.7 + freq * 0.3;
	}

};

//Panels

//Panel Fronts
struct CB_FrontPanel : SvgWidget {

	float *posx = nullptr;
	float *posy = nullptr;
	bool run = 0;

	CB_FrontPanel() {
	};

	void draw(const DrawArgs &args) override {
		if (run == 0){
			run = 1;
			box.pos.x = 0;
			box.pos.y = 0;
		}
		SVGWidget::draw(args);
	}
};

struct CB1_FrontPanel : CB_FrontPanel {
	CB1_FrontPanel() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-1-panel.svg")));
		wrap();
	};
};

struct CB2_FrontPanel : CB_FrontPanel {
	CB2_FrontPanel() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-2-panel.svg")));
		wrap();
	};
};

struct CB3_FrontPanel : CB_FrontPanel {
	CB3_FrontPanel() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-3-panel.svg")));
		wrap();
	};
};

struct CB4_FrontPanel : CB_FrontPanel {
	CB4_FrontPanel() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-4-panel.svg")));
		wrap();
	};
};

struct CB5_FrontPanel : CB_FrontPanel {
	CB5_FrontPanel() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-5-panel.svg")));
		wrap();
	};
};

struct CB6_FrontPanel : CB_FrontPanel {
	CB6_FrontPanel() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-6-panel.svg")));
		wrap();
	};
};

struct CB7_FrontPanel : CB_FrontPanel {
	CB7_FrontPanel() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-7-panel.svg")));
		wrap();
	};
};

struct CBmeter_FrontPanel : CB_FrontPanel {
	CBmeter_FrontPanel() {
		setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/CB-meter-panel.svg")));
		wrap();
	};
};

//Panel Color Cycler
struct CB_ColorCycler : TransparentWidget{
	float *sig;
	bool *active;
	int *mode;
	float *ystart;
	float *ysize;
	NVGcolor fillcolor;
	float asig = 0;
	CB_ZeroTrigger fliptrigger;
	unsigned char seetru;

	CB_ColorCycler(){
		fillcolor = nvgHSL(0.0 , 0.0 , 1.0);
		seetru = 0xff;
	};

	CB_ColorCycler(unsigned char alpha){
		fillcolor = nvgHSL(0.0 , 0.0 , 1.0);
		seetru = alpha;
	};

	void draw(const DrawArgs &args) override
	{
		if (*active == true)
		{
			fillcolor = procsig();
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0.0, 0.0, box.size.x, box.size.y);
			nvgFillColor(args.vg, fillcolor);
			nvgFill(args.vg);
			;
		}
	}

	NVGcolor procsig(){
		if (*mode == 0){
			fliptrigger.process(*sig);
			return nvgHSLA(0.0, 0.0, fliptrigger.getFreq(), seetru); //OK
		}else if (*mode == 1){
			return nvgHSLA(0.5 , 0.9, fminf(average(0.93, abs(*sig) * 0.2), 0.7f), seetru); //OK
		}else if (*mode == 2){
			return nvgHSLA(average(0.9 , (abs(*sig) * 0.1) + 0.2) , 1.0 , asig * 0.4 + 0.3, seetru); //OK
		}else if (*mode == 3){
			fliptrigger.process(*sig);
			float hue = fliptrigger.getFreq();
			hue = 0.7 + (hue > 0.3) ? hue - 1.0 : hue;
			return nvgHSLA(hue, 1.0, fminf(average(0.90, abs(*sig) * 0.2), 0.5f), seetru); //OK
		}else if (*mode == 4){
			return nvgHSLA(average(0.4 , *sig * 0.1), 1.0 , 0.45, seetru); //OK
		}
		//on invalid mode
		return nvgHSLA(0.7 , 0.8, 0.3, seetru);
	}

	float average(float decay, float sig){
		decay = clamp(decay, 0.0f, 1.0f);
		asig = clamp((asig * decay) + (sig * (1 - decay)), 0.0f, 1.0f);
		return asig;
	}

};

struct CB_ColorCyclerCircle : CB_ColorCycler{
	void draw(const DrawArgs &args) override
	{
		if (*active == true)
		{
			fillcolor = procsig();
			nvgBeginPath(args.vg);
			nvgCircle(args.vg, 0.0, 0.0, box.size.x);
			nvgFillColor(args.vg, fillcolor);
			nvgFill(args.vg);
			;
		}
	}
};

struct CB_ColorCyclerMeter : CB_ColorCycler{

	void draw(const DrawArgs &args) override
	{
		if (*active == true)
		{
			box.pos.y = *ystart;
			fillcolor = procsig();
			nvgBeginPath(args.vg);
			nvgRect(args.vg, 0.0, 0.0, box.size.x, *ysize);
			nvgFillColor(args.vg, fillcolor);
			nvgFill(args.vg);
			;
		}
	}
};
