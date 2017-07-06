cmake_minimum_required(VERSION 3.7)
project(ShadowsocksQt)

#Qt
set(CMAKE_PREFIX_PATH "/usr/local/opt/qt5/")
set(CMAKE_INCLUDE_CURRENT_DIR on)

#Botan
include_directories(/usr/local/include/botan-1.10/)
link_libraries(/usr/local/lib/libbotan-1.10.dylib)


#Qt MOC&UIC
set(CMAKE_AUTOMOC on)
set(CMAKE_AUTOUIC on)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

#Qt Module
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)

set(SOURCE_FILES main.cpp client.cpp lib/address.cpp lib/addresstester.cpp lib/chacha.cpp lib/cipher.cpp lib/common.cpp lib/controller.cpp lib/encryptor.cpp lib/encryptorprivate.cpp lib/tcprelay.cpp lib/udprelay.cpp lib/tcpserver.cpp lib/profile.cpp)

add_executable(ShadowsocksQt ${SOURCE_FILES})
qt5_use_modules(ShadowsocksQt Core Widgets Network)