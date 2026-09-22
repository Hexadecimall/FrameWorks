cmake_minimum_required(VERSION 3.25)

project(FrameWorks VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.8 REQUIRED COMPONENTS Quick Qml QuickControls2)
qt_standard_project_setup(REQUIRES 6.8)

set_source_files_properties(ui/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qt_add_executable(frameworks
    src/main.cpp
    src/documentcontroller.cpp
    src/documentcontroller.h
    src/artworkcanvas.cpp
    src/artworkcanvas.h
)

qt_add_qml_module(frameworks
    URI FrameWorks
    VERSION 1.0
    QML_FILES
        ui/Main.qml
        ui/Theme.qml
        ui/components/StudioButton.qml
)

target_link_libraries(frameworks PRIVATE Qt6::Quick Qt6::Qml Qt6::QuickControls2)

set_target_properties(frameworks PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER app.frameworks.designer
    MACOSX_BUNDLE_BUNDLE_NAME FrameWorks
    WIN32_EXECUTABLE TRUE
)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
