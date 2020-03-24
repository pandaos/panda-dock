# PandaOS Dock

Dock for PandaOS.

## Dependencies

```shell
sudo apt install cmake extra-cmake-modules g++ qt5-default qttools5-dev-tools libxcb-image0-dev libxcb-composite0-dev libxcb-ewmh-dev libqt5x11extras5-dev libxcb-damage0-dev libqt5svg5-dev libxtst-dev libkf5windowsystem-dev libkf5service-dev libpulse-dev
```

## Build

### Build with qmake

```shell
mkdir build
cd build
qmake ..
make
```

### Build with cmake

- install dependencies

```shell
sudo apt install extra-cmake-modules
```

- make

```shell
mkdir build
cd build
cmake ..
make
```

## Build a deb package

```shell
dpkg-buildpackage -b
```

## License

panda-dock is licensed under GPLv3.
