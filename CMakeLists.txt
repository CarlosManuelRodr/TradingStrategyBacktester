cmake_minimum_required(VERSION 3.23)
project(TradingStrategyBacktester)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/angelscript/angelscript/projects/cmake angelscript)

include_directories(external/doctest/doctest)
include_directories(external/cereal/include)
include_directories(external/digestpp)
include_directories(external/angelscript/angelscript/include)
include_directories(external/angelscript/add_on/scriptarray)
include_directories(external/angelscript/add_on/scriptbuilder)
include_directories(external/angelscript/add_on/scriptstdstring)

add_executable(TradingStrategyBacktester

        include/vector_ops.h
        include/utilities.h
        include/dataset.h
        include/loader.h
        include/indicators.h
        include/evaluator.h
        include/backtester.h
        include/returns.h

        include/filesystem.h
        source/filesystem.cpp
        source/loader.cpp
        source/indicators.cpp
        source/evaluator.cpp
        source/backtester.cpp
        source/returns.cpp

        external/angelscript/add_on/scriptarray/scriptarray.cpp
        external/angelscript/add_on/scriptbuilder/scriptbuilder.cpp
        external/angelscript/add_on/scriptstdstring/scriptstdstring.cpp
        external/angelscript/add_on/scriptstdstring/scriptstdstring_utils.cpp

        tests/testing.cpp
        tests/test_loader.cpp
        tests/test_backtester.cpp)

target_link_libraries(TradingStrategyBacktester ${ANGELSCRIPT_LIBRARY_NAME})

# Use the headers in the build-tree or the installed ones
target_include_directories(TradingStrategyBacktester PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )