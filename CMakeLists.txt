cmake_minimum_required(VERSION 2.8.0)

project(BtOgre)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-extras/)

find_package(Bullet REQUIRED)
find_package(OGRE REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include/
    ${BULLET_INCLUDE_DIRS}
    ${OGRE_INCLUDE_DIRS}
)

add_library(BtOgre STATIC BtOgre.cpp)
target_link_libraries(BtOgre ${BULLET_LIBRARIES} ${OGRE_LIBRARIES})
