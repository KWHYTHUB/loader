#include <Sapphire.hpp>

USE_SAPPHIRE_NAMESPACE();

SAPPHIRE_API bool SAPPHIRE_CALL sapphire_load(Mod*) {
	// Dispatcher::get()->addFunction<void(GJGarageLayer*)>("test-garage-open", [](GJGarageLayer* gl) {
	// 	auto label = CCLabelBMFont::create("Dispatcher works!", "bigFont.fnt");
	// 	label->setPosition(100, 80);
	// 	label->setScale(.4f);
	// 	label->setZOrder(99999);
	// 	gl->addChild(label);
	// });
	return true;
}
