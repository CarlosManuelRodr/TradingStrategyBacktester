cmake_minimum_required(VERSION 3.16)
project(Backtester)
find_package (Threads)

set(CMAKE_CXX_STANDARD 17)

option(MATHLINK_BUILD "Build for Mathlink" OFF)

include_directories(.)
include_directories(Add_on)
include_directories(../Libraries/cereal/include)
include_directories(../Libraries/digestpp)
include_directories(../Libraries/AngelScript/angelscript/include)
include_directories(../Libraries/AngelScript/add_on/scriptbuilder)
include_directories(../Libraries/AngelScript/add_on/scriptarray)
include_directories(../Libraries/AngelScript/add_on/scriptstdstring)

link_directories(../Libraries/AngelScript/angelscript/lib)
link_directories(../Libraries/AngelScript/add_on/scriptbuilder)
link_directories(../Libraries/AngelScript/add_on/scriptarray)
link_directories(../Libraries/AngelScript/add_on/scriptstdstring)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE EQUAL Release)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if (MATHLINK_BUILD)
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Mathematica" ${CMAKE_MODULE_PATH})
    find_package(Mathematica COMPONENTS MathLink)
    if (Mathematica_MathLink_DEFINITIONS)
        add_definitions(${Mathematica_MathLink_DEFINITIONS})
    endif()
    include_directories(${Mathematica_INCLUDE_DIRS})
    Mathematica_MathLink_MPREP_TARGET(mathlink.tm OUTPUT mathlinktm.cpp)

    add_executable(Backtester
            mathlink_interface.cpp
            mathlinktm.cpp
            dataset_loader.cpp
            dataset_loader.h
            dataset_types.h
            csv_parser.h
            filesystem.cpp
            filesystem.h
            functional_map.h
            global_definitions.h
            progress_bar.cpp
            progress_bar.h
            strategy_evaluator.cpp
            strategy_evaluator.h
            technical_indicators.h
            technical_indicators.cpp
            backtester_models.h
            backtester_models.cpp
            utilities.h)

    target_link_libraries(Backtester ${Mathematica_MathLink_LIBRARIES})

else()
    add_executable(Backtester
            cli_interface.cpp
            csv_parser.h
            dataset_loader.cpp
            dataset_loader.h
            dataset_types.h
            filesystem.cpp
            filesystem.h
            functional_map.h
            global_definitions.h
            progress_bar.cpp
            progress_bar.h
            strategy_evaluator.cpp
            strategy_evaluator.h
            technical_indicators.h
            technical_indicators.cpp
            backtester_models.h
            backtester_models.cpp
            utilities.h)
endif()

target_link_libraries(Backtester angelscript scriptbuilder scriptarray scriptstdstring ${CMAKE_THREAD_LIBS_INIT})
