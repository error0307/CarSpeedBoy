cmake_minimum_required(VERSION 3.16)

project(CarSpeedBoy
    VERSION 1.0.0
    DESCRIPTION "Speed monitoring IVI app with animated character for Automotive Grade Linux"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Qt specific settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Options
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    WebSockets
)

# Find nlohmann_json
find_package(nlohmann_json REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}
)

# Sources
set(SOURCES
    src/main.cpp
    src/application_controller.cpp
    src/data_acquisition/vehicle_data_manager.cpp
    src/data_acquisition/configuration_manager.cpp
    src/business_logic/speed_monitor.cpp
    src/business_logic/expression_state_machine.cpp
    src/business_logic/data_logger.cpp
    src/business_logic/alert_manager.cpp
    src/presentation/character_animation_engine.cpp
)

# Headers
set(HEADERS
    include/application_controller.h
    include/data_acquisition/vehicle_data_manager.h
    include/data_acquisition/configuration_manager.h
    include/business_logic/speed_monitor.h
    include/business_logic/expression_state_machine.h
    include/business_logic/data_logger.h
    include/business_logic/alert_manager.h
    include/presentation/character_animation_engine.h
)

# QML files
set(QML_FILES
    qml/main.qml
    qml/SpeedDisplay.qml
    qml/CharacterView.qml
    qml/SettingsDialog.qml
)

# Resources
qt5_add_resources(RESOURCES resources/resources.qrc)

# Executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::WebSockets
    nlohmann_json::nlohmann_json
)

# Install
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(FILES ${QML_FILES}
    DESTINATION share/${PROJECT_NAME}/qml
)

install(DIRECTORY resources/characters
    DESTINATION share/${PROJECT_NAME}
)

install(FILES config/config.json
    DESTINATION /etc/${PROJECT_NAME}
)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Documentation
if(BUILD_DOCS)
    add_subdirectory(docs)
endif()

# Code coverage
if(ENABLE_COVERAGE)
    target_compile_options(${PROJECT_NAME} PRIVATE --coverage)
    target_link_options(${PROJECT_NAME} PRIVATE --coverage)
endif()
