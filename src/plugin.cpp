#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;
	// Add modules here
	// p->addModel(modelMyModule);
	p->addModel(modelMirror);
	p->addModel(modelPlayGround);
	p->addModel(modelSynth);

}
