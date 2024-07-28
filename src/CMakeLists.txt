# RxQLIE
cmake_minimum_required(VERSION 3.25)

# Project Name
project(Rvn_RxQLIE)

# Export Symbols
if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Set Source File Path
list(APPEND Rvn_RxQLIE_Core_Src_Files
    "Core/RxQLIE/HashFilePack.cpp"
    "Core/RxQLIE/Cryptor.cpp"
    "Core/RxQLIE/ABMPData.cpp"
    "Core/RxQLIE/ABMPImage.cpp"
    "Core/RxQLIE/ABMPSound.cpp"
    "Core/RxQLIE/ABMPUtility.cpp"
    "Core/RxQLIE/AnimationBMP.cpp")

# Set Library
add_library(Rvn_RxQLIE_Core ${Rvn_RxQLIE_Core_Src_Files})
add_library(Rvn::RxQLIE::Core ALIAS Rvn_RxQLIE_Core)
target_include_directories(Rvn_RxQLIE_Core PUBLIC "Core")
target_compile_features(Rvn_RxQLIE_Core PRIVATE cxx_std_23)
target_compile_options(Rvn_RxQLIE_Core PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
target_link_libraries(Rvn_RxQLIE_Core PUBLIC Zut::ZxMem Zut::ZxCvt Zut::ZxJson Zut::ZxFile)

# Warning
if(MSVC)
    target_compile_options(Rvn_RxQLIE_Core PRIVATE /W4)
else()
    target_compile_options(Rvn_RxQLIE_Core PRIVATE -Wall -Wextra)
endif()

