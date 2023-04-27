cmake_minimum_required(VERSION 3.0)
project(cczoecpp)

SET(CMAKE_CXX_COMPILER /home/cll/software/gcc-13.1.0/bin/g++)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -ggdb -Wall -Werror -Wno-deprecated -Wno-unused-function")
# -fsanitize=address -fno-omit-frame-pointer -fsanitize-address-use-after-scope

include_directories(/home/cll/zoe/yaml-cpp/include)
include_directories(/home/cll/zoe/boost_1_80_0)
include_directories(.)
include_directories(./tests)
include_directories(./cczoecpp)
include_directories(./cczoecpp/tests)
include_directories(./cczoecpp/Log)
include_directories(./cczoecpp/Config)
include_directories(./cczoecpp/Thread)
include_directories(./cczoecpp/Fiber)
include_directories(./cczoecpp/Scheduler)
include_directories(./cczoecpp/Net)
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
    cczoecpp/Scheduler/iomanager.cpp
    cczoecpp/Timer/timer.cpp
    cczoecpp/Timer/timerManager.cpp
    cczoecpp/Net/Hook/fdmanager.cpp
    cczoecpp/Net/Hook/hook.cpp
    cczoecpp/Net/ByteArray/bytearray.cpp
    cczoecpp/Net/Address/address.cpp
    cczoecpp/Net/Address/IPAddress/IPAddress.cpp
    cczoecpp/Net/Address/UnixAddress/unixAddress.cpp
    cczoecpp/Net/Socket/socket.cpp
    cczoecpp/Net/Socket/stream.cpp
    cczoecpp/Net/Server/TcpServer.cpp
    cczoecpp/utils/getInfo.cpp
    cczoecpp/utils/debug.cpp
)

add_library(cczoecpp SHARED ${LIB_SRC})

find_package(yaml-cpp PATHS /home/cll/zoe/yaml-cpp/)
# find_library(LIBASAN_PATH libasan.so.8 PATHS /home/cll/software/gcc-13.1.0/lib64/)

set(LIBS
    cczoecpp
    pthread
    dl
    yaml-cpp
    # ${LIBASAN_PATH}
)

# LD_PRELOAD=/home/cll/software/gcc-13.1.0/lib64/libasan.so.8:$LD_PRELOAD

add_executable(testLog tests/testLog.cpp)
add_dependencies(testLog cczoecpp)
target_link_libraries(testLog ${LIBS})

add_executable(testYaml tests/testYaml.cpp)
target_link_libraries(testYaml ${LIBS})

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

add_executable(testIOManager tests/testIOManager.cpp)
add_dependencies(testIOManager cczoecpp)
target_link_libraries(testIOManager ${LIBS})

add_executable(testTimer tests/testTimer.cpp)
add_dependencies(testTimer cczoecpp)
target_link_libraries(testTimer ${LIBS})

add_executable(testHook tests/testHook.cpp)
add_dependencies(testHook cczoecpp)
target_link_libraries(testHook ${LIBS})

add_executable(testByteArray tests/testByteArray.cpp)
add_dependencies(testByteArray cczoecpp)
target_link_libraries(testByteArray ${LIBS})

add_executable(testAddress tests/testAddress.cpp)
add_dependencies(testAddress cczoecpp)
target_link_libraries(testAddress ${LIBS})

add_executable(testSocket tests/testSocket.cpp)
add_dependencies(testSocket cczoecpp)
target_link_libraries(testSocket ${LIBS})

add_executable(testTcpServer tests/testTcpServer.cpp)
add_dependencies(testTcpServer cczoecpp)
target_link_libraries(testTcpServer ${LIBS})

add_executable(echoServer examples/echoServer.cpp)
add_dependencies(echoServer cczoecpp)
target_link_libraries(echoServer ${LIBS})

add_executable(tcpClient examples/tcpClient.cpp)
add_dependencies(tcpClient cczoecpp)
target_link_libraries(tcpClient ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


# configure -v \
#     --prefix=/home/cll/software/gcc-13.1.0 \
#     --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu \
#     --enable-checking=address \
#     --enable-languages=c,c++ \
#     --disable-werror --enable-default-pie \
#     --with-system-zlib --with-target-system-zlib=auto \
#     --enable-shared --disable-checking --with-abi=m64 \
#     --disable-multilib --disable-bootstrap

# make -j32 \
#     BOOT_CFLAGS='-O3' BOOT_CXXFLAGS='-O3' \
#     CFLAGS_FOR_TARGET='-O3' CXXFLAGS_FOR_TARGET='-O3' \
#     CFLAGS_FOR_BUILD='-O3' CXXFLAGS_FOR_BUILD='-O3' \
#     CFLAGS='-O3' CXXFLAGS='-O3' \
#     LIBCFLAGS='-O3'