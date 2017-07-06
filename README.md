ShadowsocksQt
=============

Introduction
------------

`ShadowsocksQt` is a lightweight [shadowsocks][ss] Client.

Written in C++ using Qt 5 framework and Botan library.

This is a standalone program in both local-side and server-side.

[ss]: http://shadowsocks.org


Compile
-------

MacOS:

    brew install qt5

    brew install https://raw.githubusercontent.com/Homebrew/homebrew-core/9289b652846abb0d889ab9545ce2e0e74a658e11/Formula/botan.rb


Feature
-------

Based on `libQtShadowsocks` 1.10.0.

1.Remove OTA

2.Remove RC4-MD5 & RC4

3.Remove encryption speedtest

4.Update command options


License
-------

![](http://www.gnu.org/graphics/lgplv3-147x51.png)

Copyright (C) 2014-2016 Symeon Huang

Copyright (C) 2017 Kevin Li

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library. If not, see <http://www.gnu.org/licenses/>.
