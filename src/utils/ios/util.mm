#include <utils/platform.hpp>

#ifdef SAPPHIRE_IS_IOS

USE_SAPPHIRE_NAMESPACE();

#include <iostream>
#include <sstream>
#include <UIKit/UIKit.h>

bool utils::clipboard::write(std::string const& data) {
	[UIPasteboard generalPasteboard].string = [NSString stringWithUTF8String: data.c_str()];
    return true;
}

std::string utils::clipboard::read() {
	return std::string([[UIPasteboard generalPasteboard].string UTF8String]);
}

ghc::filesystem::path utils::dirs::sapphireRoot() {
	return ghc::filesystem::path([[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject].path.UTF8String);
}

void utils::web::openLinkInBrowser(std::string const& url) {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithUTF8String: url.c_str()]]];
}

void sapphire_nslog(uintptr_t x) {
	NSLog(@"sapphire %lx", x);
}

#endif
