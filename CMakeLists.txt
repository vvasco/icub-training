# requires minimum cmake version
cmake_minimum_required(VERSION 3.5)

# produce the cmake var PROJECT_NAME
project(icub-training)

# mandatory use of these packages
find_package(ICUB REQUIRED)
find_package(ICUBcontrib REQUIRED)
find_package(YARP 3.1.100 REQUIRED os dev sig cv math)
find_package(OpenCV REQUIRED)

# extend the current search path used by cmake to load helpers
list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})

# helpers defining certain macros
include(ICUBcontribHelpers)
include(ICUBcontribOptions)

# specify to install in $ICUBcontrib_DIR/bin
icubcontrib_set_default_prefix()

# generate ad-hoc project to perform "make uninstall"
icubcontrib_add_uninstall_target()

# add tutorial folders
add_subdirectory(tutorial_yarp-basics)
add_subdirectory(tutorial_yarp-opencv)
add_subdirectory(tutorial_gaze-interface)
add_subdirectory(tutorial_cartesian-interface)
