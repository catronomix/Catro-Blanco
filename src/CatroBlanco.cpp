#include "CatroBlanco.hpp"

Plugin *pluginInstance;


void init(Plugin *p) {
	pluginInstance = p;

	// Add all Models defined throughout the pluginInstance
	p->addModel(modelCB1Module);
	p->addModel(modelCB2Module);
	p->addModel(modelCB3Module);
	p->addModel(modelCB4Module);
	p->addModel(modelCB5Module);
	p->addModel(modelCB6Module);
	p->addModel(modelCB7Module);
	//p->addModel(modelCB8Module);
	//p->addModel(modelCB9Module);
	//p->addModel(modelCB10Module);

	// Any other pluginInstance initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
