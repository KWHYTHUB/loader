#!/bin/sh
cd `dirname $0`
cp /Users/jakrillis/tmp/sapphire/loader/loader_ios/build/Sapphire.dylib ./bin/ios/Sapphire.dylib
rm -rf Sapphire\ Helper.app
cp -r /Users/jakrillis/Library/Developer/Xcode/DerivedData/Sapphire_Helper-eojylhlhpfgucjaupttaqyhqcbmp/Build/Products/Debug-iphoneos/Sapphire\ Helper.app .
g++ main.cpp -arch arm64 -std=c++17 -isysroot `xcrun --show-sdk-path --sdk iphoneos` -dynamiclib -o SapphireLauncher.dylib

python3 pkg.py

scp "./sapphireloader-test-arm64.deb" root@three.local:/var/mobile/Documents
ssh root@three.local dpkg -i "/var/mobile/Documents/sapphireloader-test-arm64.deb" "&&" killall GeometryJump #";" uicache