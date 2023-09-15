// #include <Sapphire.hpp>
// // this is the fix for the dynamic_cast problems

// USE_SAPPHIRE_NAMESPACE();

// #if defined(SAPPHIRE_IS_IOS) || defined(SAPPHIRE_IS_MACOS)
// namespace sapphire::fixes {
// 	using namespace sapphire::cast;


// class $modify(CCDictionaryTypeinfoFix, CCDictionary) {
// 	const CCString* valueForKey(const gd::string& key) {
// 		CCString* pStr = dynamic_cast<CCString*>(objectForKey(key));
// 	    if (pStr == nullptr) {
// 	        pStr = const_cast<CCString*>(CCDictionary::valueForKey(key));
// 	    }
// 	    return pStr;
// 	}

// 	const CCString* valueForKey(intptr_t key) {
// 		CCString* pStr = dynamic_cast<CCString*>(objectForKey(key));
// 	    if (pStr == nullptr) {
// 	        pStr = const_cast<CCString*>(CCDictionary::valueForKey(key));
// 	    }
// 	    return pStr;
// 	}
// };

// } // sapphire::fixes

// #endif
