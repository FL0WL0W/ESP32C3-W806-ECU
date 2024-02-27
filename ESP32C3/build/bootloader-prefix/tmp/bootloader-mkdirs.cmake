# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/daniel/esp/master/esp-idf/components/bootloader/subproject"
  "/home/daniel/git/ESP32C3-W806-ECU/ESP32C3/build/bootloader"
  "/home/daniel/git/ESP32C3-W806-ECU/ESP32C3/build/bootloader-prefix"
  "/home/daniel/git/ESP32C3-W806-ECU/ESP32C3/build/bootloader-prefix/tmp"
  "/home/daniel/git/ESP32C3-W806-ECU/ESP32C3/build/bootloader-prefix/src/bootloader-stamp"
  "/home/daniel/git/ESP32C3-W806-ECU/ESP32C3/build/bootloader-prefix/src"
  "/home/daniel/git/ESP32C3-W806-ECU/ESP32C3/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/daniel/git/ESP32C3-W806-ECU/ESP32C3/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/daniel/git/ESP32C3-W806-ECU/ESP32C3/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
