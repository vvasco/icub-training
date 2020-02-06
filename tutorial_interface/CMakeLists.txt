# requires minimum cmake version
cmake_minimum_required(VERSION 3.5)

# produce the cmake var PROJECT_NAME
project(tutorial_interface)

# mover
add_executable(${PROJECT_NAME}-mover src/mover.cpp)
target_compile_definitions(${PROJECT_NAME}-mover PRIVATE _USE_MATH_DEFINES)
target_link_libraries(${PROJECT_NAME}-mover ${YARP_LIBRARIES} ctrlLib)
install(TARGETS ${PROJECT_NAME}-mover DESTINATION bin)

# controller gaze
add_executable(${PROJECT_NAME}-control-gaze src/control-gaze.cpp)
target_compile_definitions(${PROJECT_NAME}-control-gaze PRIVATE _USE_MATH_DEFINES)
target_link_libraries(${PROJECT_NAME}-control-gaze ${YARP_LIBRARIES})
install(TARGETS ${PROJECT_NAME}-control-gaze DESTINATION bin)

# controller cartesian
add_executable(${PROJECT_NAME}-control-cartesian src/control-cartesian.cpp)
target_compile_definitions(${PROJECT_NAME}-control-cartesian PRIVATE _USE_MATH_DEFINES)
target_link_libraries(${PROJECT_NAME}-control-cartesian ${YARP_LIBRARIES})
install(TARGETS ${PROJECT_NAME}-control-cartesian DESTINATION bin)

add_subdirectory(app)
