cmake_minimum_required(VERSION 3.0)
project(cczoecpp)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)
include_directories(./tests)
include_directories(./cczoecpp)
include_directories(./cczoecpp/tests)
include_directories(./cczoecpp/Log)
include_directories(./cczoecpp/Config)
include_directories(./cczoecpp/Thread)
include_directories(./cczoecpp/Fiber)
include_directories(./cczoecpp/Scheduler)
include_directories(./cczoecpp/utils)

set(LIB_SRC
    cczoecpp/Log/LogAppender/logAppender.cpp
    cczoecpp/Log/LogEvent/logEvent.cpp
    cczoecpp/Log/Logger/logger.cpp
    cczoecpp/Log/LogFormatter/logFormatter.cpp
    cczoecpp/Log/LoggerManager/loggerManager.cpp
    cczoecpp/Config/config.cpp
    cczoecpp/Thread/Semaphore/sem.cpp
    cczoecpp/Thread/Mutex/mutex.cpp
    cczoecpp/Thread/thread.cpp
    cczoecpp/Fiber/fiber.cpp
    cczoecpp/Scheduler/scheduler.cpp
    cczoecpp/utils/getInfo.cpp
    cczoecpp/utils/debug.cpp
)

set(LIBS
    cczoecpp
    pthread
    ${YAMLCPP}
)

find_library(YAMLCPP yaml-cpp)

add_library(cczoecpp ${LIB_SRC})

add_executable(testLog tests/testLog.cpp)
add_dependencies(testLog cczoecpp)
target_link_libraries(testLog ${LIBS})

add_executable(testYaml tests/testYaml.cpp)
target_link_libraries(testYaml ${YAMLCPP})

add_executable(testConfig tests/testConfig.cpp)
add_dependencies(testConfig cczoecpp)
target_link_libraries(testConfig ${LIBS})

add_executable(testThread tests/testThread.cpp)
add_dependencies(testThread cczoecpp)
target_link_libraries(testThread ${LIBS})

add_executable(testMacro tests/testMacro.cpp)
add_dependencies(testMacro cczoecpp)
target_link_libraries(testMacro ${LIBS})

add_executable(testFiber tests/testFiber.cpp)
add_dependencies(testFiber cczoecpp)
target_link_libraries(testFiber ${LIBS})

add_executable(testScheduler tests/testScheduler.cpp)
add_dependencies(testScheduler cczoecpp)
target_link_libraries(testScheduler ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
