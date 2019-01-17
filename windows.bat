npm install node-serialport
npm install --save serialport
npm install --save-dev electron-rebuild
.\node_modules\.bin\electron-rebuild.cmd

npm install electron-packager
electron-packager . SP35_callib --platform=win32 --arch=x64 --version 1.7.9

