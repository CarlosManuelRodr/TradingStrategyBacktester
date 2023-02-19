cmake_minimum_required(VERSION 3.23)
project(TradingStrategyBacktester)

set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED BUILD_TYPE)
    set(BUILD_TYPE "Tests")
endif()

add_subdirectory(external/angelscript/angelscript/projects/cmake angelscript)

include_directories(external/cereal/include)
include_directories(external/digestpp)
include_directories(external/angelscript/angelscript/include)
include_directories(external/angelscript/add_on/scriptarray)
include_directories(external/angelscript/add_on/scriptbuilder)
include_directories(external/angelscript/add_on/scriptstdstring)

if (BUILD_TYPE MATCHES Tests)

    message("Compiling unit tests" )
    include_directories(external/doctest/doctest)

    add_executable(TradingStrategyBacktester

            include/thread_pool.h
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
            tests/test_backtester.cpp
            tests/test_filesystem.cpp)

    target_link_libraries(TradingStrategyBacktester PRIVATE ${ANGELSCRIPT_LIBRARY_NAME})

elseif(BUILD_TYPE MATCHES Python)

    message("Compiling Python bindings" )
    add_subdirectory(external/pybind11)

    find_package(PythonInterp 3)
    find_package(PythonLibs   3)

    pybind11_add_module(TradingStrategyBacktester

            include/thread_pool.h
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

            python/bindings.cpp
            )

    target_link_libraries(TradingStrategyBacktester PRIVATE ${ANGELSCRIPT_LIBRARY_NAME})

    add_custom_command(TARGET TradingStrategyBacktester POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy
            "$<TARGET_FILE:TradingStrategyBacktester>"
            "${CMAKE_CURRENT_SOURCE_DIR}/python/TradingStrategyBacktester/$<TARGET_FILE_NAME:TradingStrategyBacktester>"
            COMMENT "Copying to output directory")

endif()

# Use the headers in the build-tree or the installed ones
target_include_directories(TradingStrategyBacktester PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        )