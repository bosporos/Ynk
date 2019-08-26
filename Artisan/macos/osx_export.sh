#!/bin/sh

echo "=== [\x1b[33mBUILDING\x1b[0m] ==="

ninja -j1

echo "=== [\x1b[33mCONSTRUCTING APPLICATION BUNDLE\x1b[0m] ==="

rm -rf build/YnkExported.app

mkdir -p build/YnkExported.app/Contents
cp Artisan/macos/Info.tpl.plist build/YnkExported.app/Contents/Info.plist

mkdir -p build/YnkExported.app/Contents/Library
cp build/ynked build/YnkExported.app/Contents/Library
cp build/libynk.dylib build/YnkExported.app/Contents/Library
cp Artisan/macos/libglfw.3.3.dylib build/YnkExported.app/Contents/Library
install_name_tool -change build/libynk.dylib @executable_path/../Library/libynk.dylib build/YnkExported.app/Contents/Library/ynked
install_name_tool -change lib/libglfw.3.dylib @executable_path/../Library/libglfw.3.3.dylib build/YnkExported.app/Contents/Library/ynked
install_name_tool -change lib/libglfw.3.dylib @executable_path/../Library/libglfw.3.3.dylib build/YnkExported.app/Contents/Library/libynk.dylib
cp -R Art/shaders build/YnkExported.app/Contents/Library

mkdir -p build/YnkExported.app/Contents/MacOS
echo "#!/bin/bash" > build/YnkExported.app/Contents/MacOS/YnkExported
echo "cd \"\${0%/*}/../\"" >> build/YnkExported.app/Contents/MacOS/YnkExported
echo "./Library/ynked" >> build/YnkExported.app/Contents/MacOS/YnkExported
chmod +x build/YnkExported.app/Contents/MacOS/YnkExported

mkdir build/YnkExported.app/Contents/Resources
cp Artisan/macos/Ikonik.icns build/YnkExported.app/Contents/Resources/YnkExported.icns

echo "=== [\x1b[33mPACKAGING APPLICATION BUNDLE\x1b[0m] ==="

zip -r build/YnkExported.app.zip build/YnkExported.app
