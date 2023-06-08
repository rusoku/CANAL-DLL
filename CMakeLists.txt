cmake_minimum_required(VERSION 3.24)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(canal)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_COMPILER)
set(CMAKE_CXX_COMPILER)

#add_definitions(-DUNICODE)
#add_definitions(-D_UNICODE)
add_definitions(-D_MBCS)
add_definitions(-D_WIN32_WINNT_WIN10)

#add_compile_options(-Wall -Wextra -Wpedantic)
add_compile_options(-Wno-pragma-once-outside-header)
add_compile_options(-Wno-unused-parameter)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    #add_compile_definitions(canal DEBUG_CANAL)
endif()

add_library(canal SHARED
        CAN_interface_list.c
        debug.c
        CDllList.cpp
        CDllDrvObj.cpp
        CTouCANobj.cpp
        CTouCANobjCmdMsg.cpp
        CTouCANobjWinUSB.cpp
        canal.cpp
        utils.c
        Threads.cpp
        include/CDllDrvObj.h
        include/canal.h
        include/CTouCANobj.h
        include/CDllList.h
        include/canal_macro.h)

set_property(TARGET canal PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

target_link_libraries(canal cfgmgr32 winusb)
set_target_properties(canal PROPERTIES OUTPUT_NAME "canal$<$<CONFIG:Debug>:d>")



