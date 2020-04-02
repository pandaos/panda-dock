# PandaOS Dock

Dock for PandaOS.

## Dependencies

```shell
sudo apt install cmake extra-cmake-modules g++ qt5-default qttools5-dev-tools libqt5x11extras5-dev libqt5svg5-dev libkf5windowsystem-dev
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
