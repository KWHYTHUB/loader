import sys, os
from shutil import copyfile, rmtree
from distutils.dir_util import copy_tree
print(sys.argv)

out_dir = os.getcwd()

os.makedirs(out_dir, exist_ok=True)
os.chdir(out_dir)
os.makedirs("tmp/Library/MobileSubstrate/DynamicLibraries/", exist_ok=True)
os.makedirs("tmp/Applications/", exist_ok=True)
os.makedirs("tmp/DEBIAN/", exist_ok=True)

open("tmp/DEBIAN/control", "w").write("""Name: Sapphire Launcher
Architecture: iphoneos-arm
Depends: com.cokepokes.libnotifications (>= 0.2-2)
Description: Modding suite for Geometry Dash (test package!!)
Maintainer: camila314
Package: com.camila314.sapphire-test
Priority: optional
Section: Tweaks
Version: 0.1.0
""")

copy_tree("Sapphire Helper.app/", "tmp/Applications/Sapphire Helper.app/")
copyfile("/Users/jakrillis/tmp/sapphire/loader/loader_ios/build/Sapphire.dylib", "bin/ios/Sapphire.dylib")
os.system("ldid -S SapphireLauncher.dylib")
os.system("ldid -S bin/ios/Sapphire.dylib")
copyfile("SapphireLauncher.dylib", "tmp/Library/MobileSubstrate/DynamicLibraries/SapphireLauncher.dylib")
copyfile("bin/ios/Sapphire.dylib", "tmp/Library/MobileSubstrate/DynamicLibraries/Sapphire.dylib")

open("tmp/Library/MobileSubstrate/DynamicLibraries/SapphireLauncher.plist", "w").write("""{ Filter = { Bundles = ( "com.robtop.geometryjump" ); }; }""")
os.system("dpkg-deb --build tmp sapphireloader-test-arm64.deb")
rmtree("tmp")
