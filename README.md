NES Emulator
=============
 By,
 Sanjay S Bhat
 Shanjiv A
 Calisto Mathias
 Nishant A S

 This is a slightly simple emulator version of the OG NES that we have implemented for our IEEE yearlong executive project.
 Runs Tetris,for now :) 

Compiling
-----------

You need:
* [SFML 2.*](#installing-sfml) development headers and library
* C++11 compliant compiler
* [CMake](https://cgold.readthedocs.io/en/latest/first-step/installation.html) build system

 Linux:
```
$ git clone https://github.com/Calisto-Mathias/nes-emulator
$ cd nes-emulator
$ mkdir build/ && cd build/
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j4    # Replace 4 with however many cores you have to spare
```
Running
-----------------

Just pass the path to a .nes image like

```
$ ./SimpleNES Tetris.nes
```
To set size of the window,
```
$ ./SimpleNES -w 600 Tetris.nes
```


