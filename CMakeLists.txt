################################################################################
#
# qrtape build
#
################################################################################

# project settings #############################################################

cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

project(qrtape)

include_directories(${PROJECT_SOURCE_DIR})

# dependencies #################################################################

find_package(PkgConfig REQUIRED)

pkg_check_modules(tclap REQUIRED tclap)

# subdirectories ###############################################################

add_subdirectory(qrtape)
