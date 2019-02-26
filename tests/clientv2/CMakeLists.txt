cmake_policy(VERSION 3.9)
cmake_minimum_required(VERSION 3.9)
set(TEST_EXE_NAME clientv2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Core Gui Qml Quick REQUIRED)

file(GLOB SOURCES ./*.cpp ./*.h ./*.c ./*.ui)

add_executable(${TEST_EXE_NAME} ${SOURCES})


find_package(SWSCALE QUIET)
if(SWSCALE_FOUND)
    include_directories(${SWSCALE_INCLUDE_DIR})
    list(APPEND  LINK_LIB_LIST ${SWSCALE_LIBRARIES})
    message(STATUS "found  libswscale: ${SWSCALE_LIBRARIES}")
else(SWSCALE_FOUND)
    set (SWSCALE_LIBRARIES /usr/lib/x86_64-linux-gnu/libswscale.so)
    list(APPEND  LINK_LIB_LIST ${SWSCALE_LIBRARIES})
    message(STATUS "found  libswscale: ${SWSCALE_LIBRARIES}")
endif(SWSCALE_FOUND)

set(AVUTIL_LIBRARIES /usr/lib/x86_64-linux-gnu/libavutil.so)
list(APPEND  LINK_LIB_LIST ${AVUTIL_LIBRARIES})
message(STATUS "found  AVUTIL ${AVUTIL_LIBRARIES}")


set(AVFORMAT_LIBRARIES /usr/lib/x86_64-linux-gnu/libavformat.so)
list(APPEND  LINK_LIB_LIST ${AVFORMAT_LIBRARIES})
message(STATUS "found  AVFORMAT ${AVFORMAT_LIBRARIES}")

target_link_libraries(${TEST_EXE_NAME} Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick ${LINK_LIB_LIST})

message(STATUS "client:${LINK_LIB_LIST}")
