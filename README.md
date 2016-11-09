#### About
```radiotray-lite``` is a lightweight clone of the original [Radio Tray](http://radiotray.sourceforge.net/) online radio streaming player rewritten in C++.

#### Build
This project supports building with CMake, to build Ubuntu package issue following (with obvious amendments) commands from build directory:
* ```$ cmake /path/to/radiotray-lite/ -DCMAKE_INSTALL_PREFIX=/usr/```
* ```$ make```
* ```$ make package```

```.deb``` archive will be located in the ```packages``` folder of the build directory.

#### Licensing
See [LICENSE.md](LICENSE.md) file for license information.
