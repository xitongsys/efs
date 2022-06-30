PROJECT(efs)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++2a")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++2a")

INCLUDE_DIRECTORIES("efs-lib/src")
INCLUDE_DIRECTORIES("efs-server")
INCLUDE_DIRECTORIES("efs-client")

FILE(GLOB_RECURSE EFS_SERVER_SRCS "efs-server/*.cc" "efs-lib/src/*.cc")
FILE(GLOB_RECURSE EFS_CLIENT_SRCS "efs-client/*.cc" "efs-lib/src/*.cc")

ADD_EXECUTABLE(efs-server.out ${EFS_SERVER_SRCS})
# ADD_EXECUTABLE(efs-client ${EFS_CLIENT_SRCS})

TARGET_LINK_LIBRARIES(efs-server.out pthread rocksdb yaml-cpp)
# TARGET_LINK_LIBRARIES(efs-client pthread rocksdb yaml-cpp readline)

