#define EXPORT_NAME TestDependency
#include <Sapphire.hpp>

API_INIT("com.sapphire.testdep")

class TestDependency {
public:
	static void depTest(GJGarageLayer* gl);
		// API_DECL(&TestDependency::depTest, gl);
};
