cmake_minimum_required(VERSION 2.8.3)
project(roboy_simulation)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/devel/lib)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

SET(CMAKE_BUILD_TYPE Debug)

#==============================================================================
# Dependencies
#==============================================================================
find_package(Qt4)
find_package(Protobuf REQUIRED)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

find_package(gazebo REQUIRED)
find_package(Boost REQUIRED)

find_package(catkin REQUIRED 
  COMPONENTS 
    common_utilities
    controller_manager
    flexrayusbinterface
    gazebo_ros_control
    gazebo_ros_pkgs
    geometry_msgs     
    message_generation
    roscpp
    rviz
    std_msgs                            
)

link_directories(${gazebo_LIBRARY_DIRS})
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${gazebo_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}

)

add_message_files(
  DIRECTORY
    msg
  FILES
    Tendon.msg
    VisualizationControl.msg
    ForceTorque.msg
)

generate_messages(
  DEPENDENCIES
    geometry_msgs
    std_msgs
)

catkin_package(
  CATKIN_DEPENDS message_runtime message_generation
  DEPENDS Boost gazebo
  INCLDE_DIRS include
  LIBRARIES 
    contact_plugin
    force_torque_plugin
    muscle_plugin
    roboySim
    roboy_rviz
    tendon_visualizer
)


#==============================================================================
# Application
#==============================================================================
add_library(contact_plugin 
  SHARED 
    src/ContactPlugin.cpp
)
target_link_libraries(contact_plugin 
  ${catkin_LIBRARIES}
  ${GAZEBO_LIBRARIES}
)



add_library(force_torque_plugin 
  SHARED
    src/ForceTorquePlugin.cpp
)
add_dependencies(force_torque_plugin
  roboy_simulation_gencpp
)
target_link_libraries(force_torque_plugin
  ${catkin_LIBRARIES}
  ${GAZEBO_LIBRARIES}
)



add_library(muscle_plugin
  SHARED
    src/MusclePlugin.cpp
)
target_link_libraries(muscle_plugin 
  ${GAZEBO_LIBRARIES}
  ${Boost_LIBRARIES}
  ${ARMADILLO_LIBRARIES}
  ${catkin_LIBRARIES}
)



add_library(roboySim 
  SHARED
    src/roboySim.cpp
)
add_dependencies(roboySim
  common_utilities_gencpp
  roboy_simulation_gencpp
)
target_link_libraries(roboySim 
  ${catkin_LIBRARIES}
  ${GAZEBO_LIBRARIES}
  ${Boost_LIBRARIES}
)



qt4_wrap_cpp(
  MOC_FILES 
    include/walking_plugin.hpp
)
add_library(roboy_rviz
  src/walking_plugin.cpp 
  ${MOC_FILES}
)
add_dependencies(roboy_rviz 
  roboy_simulation_gencpp
)
target_link_libraries(roboy_rviz 
  ${QT_LIBRARIES}
  ${catkin_LIBRARIES}
  ${GAZEBO_LIBRARIES}
)



add_library(tendon_visualizer 
  SHARED
    src/TendonVisualizer.cpp
)
add_dependencies(tendon_visualizer 
  roboy_simulation_gencpp
)
target_link_libraries(tendon_visualizer 
  ${catkin_LIBRARIES}
  ${GAZEBO_LIBRARIES}
  ${Boost_LIBRARIES}
)



#==============================================================================
# Installation
#==============================================================================
install(TARGETS
  contact_plugin
  force_torque_plugin
  muscle_plugin
  roboySim
  roboy_rviz
  tendon_visualizer
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)

install(
  FILES
    package.xml
    muscle_plugin.xml
    walking_plugin.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

install(
  DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
)
