#### About
```radiotray-lite``` is a lightweight clone of the original [Radio Tray](http://radiotray.sourceforge.net/) online radio streaming player rewritten in C++.

![Screenshot](images/radiotray-lite.png)

#### Build
For a while this project is only tested on Ubuntu (14.04 and 16.04 versions). In order to build it you need to have
installed ```-dev``` versions of the following packages:
* ```libgtkmm-3.0```
* ```libgstreamermm-0.10-2``` or ```libgstreamermm-1.0```
* ```libcurl3```
* ```libnotify4```
* ```libappindicator3```

To build Ubuntu package issue following (with obvious amendments) commands from build directory:
* ```$ cmake /path/to/radiotray-lite/ -DCMAKE_INSTALL_PREFIX=/usr/```
* ```$ make```
* ```$ make package```

```.deb``` archive will be located in the ```packages``` folder of the build directory.

#### Licensing
See [LICENSE.md](LICENSE.md) file for license information.
