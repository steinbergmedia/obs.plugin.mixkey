# OBS MixKey Plug-in

## Introduction

The plug-in is designed for use with the Steinberg MixKey desktop app (https://www.steinberg.net/mixkey).

## Prerequistes

Install OBS!

## Getting Started

To clone and create the project, open a command prompt and proceed as follows:

### Windows

```sh
git clone https://github.com/steinbergmedia/obs.plugin.mixkey.git
mkdir obs.plugin.mixkey
cmake --preset="windows-x64"
cmake --build ./build_x64
```

### macOS

```sh
git clone https://github.com/steinbergmedia/obs.plugin.mixkey.git
mkdir obs.plugin.mixkey
cmake --preset="macos"
cmake --build ./build_macos
```

### Linux

```sh
git clone https://github.com/steinbergmedia/obs.plugin.mixkey.git
mkdir obs.plugin.mixkey
cmake --preset="ubuntu-x86_64"
cmake --build ./build_x86_64
```

## Getting Help

* https://docs.obsproject.com
* https://obsproject.com/kb/plugins-guide
* https://forums.steinberg.net/c/mixkey/170
