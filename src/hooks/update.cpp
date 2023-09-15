#include <Sapphire.hpp>
#include <InternalLoader.hpp>

USE_SAPPHIRE_NAMESPACE();

class $modify(CCScheduler) {
    void update(float dt) {
        InternalLoader::get()->executeGDThreadQueue();
        return CCScheduler::update(dt);
    }
};
