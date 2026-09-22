find_package(Qt6 6.8 REQUIRED COMPONENTS Test)

qt_add_executable(frameworks_model_tests
    test_documentcontroller.cpp
    ../src/documentcontroller.cpp
    ../src/documentcontroller.h
)

target_link_libraries(frameworks_model_tests PRIVATE Qt6::Core Qt6::Gui Qt6::Test)
target_include_directories(frameworks_model_tests PRIVATE ../src)

add_test(NAME frameworks_model_tests COMMAND frameworks_model_tests)
