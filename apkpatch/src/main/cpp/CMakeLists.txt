# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("apkpatch")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
# 在 add_library 前添加 bzip2 编译


# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.


#option(BUILD_SHARED_LIBS "Set to ON to build shared libraries" OFF)
#option(BUILD_STANDALONES "Set to OFF to not build standalones" ON)

# bzip2
add_library(bzip2 STATIC
        bzip2/bzlib.c
        bzip2/compress.c
        bzip2/decompress.c
        bzip2/blocksort.c
        bzip2/crctable.c
        bzip2/huffman.c
        bzip2/randtable.c)

# libdivsufsort
function(add_libdivsufsort)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    set(BUILD_EXAMPLES OFF)
    set(BUILD_DIVSUFSORT64 ON)
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(libdivsufsort)
endfunction()

add_libdivsufsort()

# bsdiff
add_library(${CMAKE_PROJECT_NAME} SHARED
        native-lib.cpp
        source/bsdiff_private.h
        source/misc.c
        source/stream_file.c
        source/stream_memory.c
        source/stream_sub.c
        source/compressor_bz2.c
        source/decompressor_bz2.c
        source/patch_packer_bz2.c
        source/bsdiff.c
        source/bspatch.c
)
target_include_directories(${CMAKE_PROJECT_NAME}
        PRIVATE "bzip2"
        PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/libdivsufsort/include"
        PRIVATE "include")
if (BUILD_SHARED_LIBS)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE "BSDIFF_DLL" "BSDIFF_EXPORTS")
endif ()

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE bzip2 PRIVATE divsufsort PRIVATE divsufsort64 android
        log)
