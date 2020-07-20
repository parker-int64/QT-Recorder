cmake_minimum_required(VERSION 3.5)

project(recorder-opencv LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ../bin)


find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)

 add_executable(recorder-opencv
    main.cpp
    widget.cpp
    uiComponents.qrc
    widget.h
    form.h
    form.cpp
    widget.ui
    form.ui
    app.rc
  )
target_link_libraries(recorder-opencv
    PRIVATE
        Qt5::Widgets
        ${OpenCV_LIBS})

