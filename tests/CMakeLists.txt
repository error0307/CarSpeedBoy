cmake_minimum_required(VERSION 3.16)

# Enable testing
enable_testing()

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)

# Find Qt5 Test module
find_package(Qt5 REQUIRED COMPONENTS Test Core WebSockets)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/business_logic
    ${CMAKE_SOURCE_DIR}/include/data_acquisition
)

# Helper function to create tests
function(add_carspeedboy_test test_name)
    add_executable(${test_name} ${ARGN})
    target_link_libraries(${test_name}
        Qt5::Test
        Qt5::Core
        Qt5::WebSockets
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
    set_tests_properties(${test_name} PROPERTIES
        TIMEOUT 30
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )
endfunction()

# Test: ExpressionStateMachine
add_carspeedboy_test(test_expression_state_machine
    test_expression_state_machine.cpp
    ${CMAKE_SOURCE_DIR}/src/business_logic/expression_state_machine.cpp
    ${CMAKE_SOURCE_DIR}/include/business_logic/expression_state_machine.h
)

# Test: ConfigurationManager
add_carspeedboy_test(test_configuration_manager
    test_configuration_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/data_acquisition/configuration_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/data_acquisition/configuration_manager.h
)

# Test: VehicleDataManager (with mocks)
add_carspeedboy_test(test_vehicle_data_manager
    test_vehicle_data_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/data_acquisition/vehicle_data_manager.cpp
    ${CMAKE_SOURCE_DIR}/include/data_acquisition/vehicle_data_manager.h
)

# Coverage report (optional)
if(ENABLE_COVERAGE)
    find_program(GCOV gcov)
    find_program(LCOV lcov)
    find_program(GENHTML genhtml)
    
    if(GCOV AND LCOV AND GENHTML)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
        
        add_custom_target(coverage
            COMMAND ${LCOV} --directory . --zerocounters
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${LCOV} --directory . --capture --output-file coverage.info
            COMMAND ${LCOV} --remove coverage.info '/usr/*' --output-file coverage.info
            COMMAND ${GENHTML} coverage.info --output-directory coverage_html
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report"
        )
    endif()
endif()
