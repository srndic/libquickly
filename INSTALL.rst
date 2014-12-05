===============================
LIBQUICKLY INSTALLATION NOTES
===============================

Copyright 2014 Nedim Srndic, University of Tuebingen

Libquickly is C++ software and uses CMake for building. It was 
developed and tested on Ubuntu 14.04. 

Dependencies
====================

Please install the following dependencies before building
libquickly:

 - CMake, version 2.8
 - a C++ compiler, though only g++ was tested
 - Boost Thread and Boost System, Ubuntu 14.04 version. 

Building libquickly
====================

Libquickly uses the usual CMake procedure for building. Here are the 
individual steps (assuming you are in the root of libquickly, i.e., 
where the README file is)::

  mkdir build
  cd build
  cmake -DMAKE_TESTS=1 ..
  make

Installation
===================

To install libquickly, cd to the build directory as created in 
the build procedure description above and type::

  make install

On Ubuntu, you should use sudo::

  sudo make install
