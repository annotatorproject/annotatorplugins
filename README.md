# AnnotatorPlugins [ ![Build Status] [travis-image] ] [travis] [ ![Documentation] [codedocs-image] ] [codedocs] [ ![Gitter] [gitter-image] ] [gitter]

[travis-image]: https://travis-ci.org/lasmue/annotator.png?branch=master
[travis]: http://travis-ci.org/lasmue/annotatorplugins

[codedocs-image]: https://codedocs.xyz/lasmue/annotator.svg
[codedocs]: https://codedocs.xyz/lasmue/annotatorplugins/

[gitter-image]: https://badges.gitter.im/Join%20Chat.svg
[gitter]: https://gitter.im/lasmue/annotatorplugins

### Quick start

First install CMake, Qt5, OpenCV and a c++11 compiler (gcc 5).

```
sudo apt-get install libboost-system-dev libboost-filesystem-dev
sudo apt-get install qtbase5-dev
sudo apt-get install libopencv-dev
```

Clone and install [dlib](http://dlib.net).

Then clone and build [annotatorlib](https://github.com/lasmue/annotatorlib).

```sh
git clone https://github.com/lasmue/annotatorlib
cd annotatorlib
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
```

Now you can clone and build annotator.

```sh
git clone https://github.com/lasmue/annotator
cd annotator
git submodule update --init --recursive
mkdir build
cd build
cmake -Dannotatorlib_DIR=../annotatorlib ..
make
```

Finally clone and build annotatorplugins
```sh
git clone https://github.com/lasmue/annotatorplugins
cd annotatorplugins
git submodule update --init --recursive
mkdir build
cd build
cmake -Dannotatorlib_DIR=../annotatorlib -Dannotator_DIR=../annotator ..
make
```

Now you can run annotator using the plugins by
```sh
make annotator
```
