# requires minimum cmake version
cmake_minimum_required(VERSION 3.5)

# produce the cmake var PROJECT_NAME
project(yarp-basics)

# add idl
yarp_add_idl(IDL_GEN_FILES src/${PROJECT_NAME}.thrift)
set(idl src/${PROJECT_NAME}.thrift)

# building module
include_directories(${CMAKE_SOURCE_DIR}/src)

# buulding server
add_executable(${PROJECT_NAME} src/main.cpp ${idl} ${IDL_GEN_FILES})
target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES} ${OpenCV_LIBRARIES})
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

file(GLOB resources ${CMAKE_SOURCE_DIR}/resources/*)
yarp_install(FILES ${resources} DESTINATION ${ICUBCONTRIB_CONTEXTS_INSTALL_DIR}/${PROJECT_NAME})

add_subdirectory(app)
