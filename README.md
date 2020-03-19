# FlymaOS Dock

Dock for FlymaOS.

## Dependencies

* sudo apt install g++ qt5-default qttools5-dev-tools libxcb-image0-dev libxcb-composite0-dev libxcb-ewmh-dev libqt5x11extras5-dev libxcb-damage0-dev libqt5svg5-dev libxtst-dev libkf5windowsystem-dev libpulse-dev

## Build

* mkdir build
* cd build
* qmake ..
* make

## Build a deb package

* dpkg-buildpackage -b

## License

flyma-dock is licensed under GPLv3.
