# requires minimum cmake version
cmake_minimum_required(VERSION 3.5)

# produce the cmake var PROJECT_NAME
project(tutorial_gaze-interface)

# mover
add_executable(${PROJECT_NAME}-mover src/mover.cpp)
target_compile_definitions(${PROJECT_NAME}-mover PRIVATE _USE_MATH_DEFINES)
target_link_libraries(${PROJECT_NAME}-mover ${YARP_LIBRARIES} ctrlLib)
install(TARGETS ${PROJECT_NAME}-mover DESTINATION bin)

# controller
add_executable(${PROJECT_NAME}-controller src/control-gaze.cpp)
target_compile_definitions(${PROJECT_NAME}-controller PRIVATE _USE_MATH_DEFINES)
target_link_libraries(${PROJECT_NAME}-controller ${YARP_LIBRARIES})
install(TARGETS ${PROJECT_NAME}-controller DESTINATION bin)

add_subdirectory(app)
