cmake_minimum_required(VERSION 3.16)

project(VigilantHub VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)
find_package(OpenCV REQUIRED)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appVigilantHub
    main.cpp
)

qt_add_qml_module(appVigilantHub
    URI VigilantHub
    VERSION 1.0
    QML_FILES
        Main.qml
        SOURCES CameraThread.h
        SOURCES CameraThread.cpp
SOURCES CameraProvider.h
SOURCES CameraProvider.h
SOURCES
SOURCES
SOURCES MLProcessor.h
SOURCES MLProcessor.cpp

)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appVigilantHub PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appVigilantHub
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appVigilantHub
    PRIVATE Qt6::Quick
)
target_include_directories(appVigilantHub PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(appVigilantHub PRIVATE Qt6::Quick Qt6::Qml Qt6::Gui ${OpenCV_LIBS})


include(GNUInstallDirs)
install(TARGETS appVigilantHub
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Add Info.plist for macOS
if(APPLE)
    set(MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/Info.plist)
    set_target_properties(appVigilantHub PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${MACOSX_BUNDLE_INFO_PLIST}
    )
endif()
if(APPLE)
    set_target_properties(appVigilantHub PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS ${CMAKE_SOURCE_DIR}/VigilantHub.entitlements
    )
endif()
