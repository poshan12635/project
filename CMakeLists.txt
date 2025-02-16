cmake_minimum_required(VERSION 3.16)

project(untitled VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# Handle Qt version-specific logic
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(untitled
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        regi.h regi.cpp regi.ui
        img.qrc
        attendance.h attendance.cpp attendance.ui
        IMAGE.qrc


    )
else()
    add_executable(untitled
        ${PROJECT_SOURCES}
    )
endif()

# Set the database source and destination paths
set(DATABASE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/database") # Source directory for database files
set(DATABASE_DEST_DIR "${CMAKE_BINARY_DIR}/data")       # Destination directory in the build folder

# Create the destination directory if it doesn't exist
file(MAKE_DIRECTORY ${DATABASE_DEST_DIR})

# Copy database files to the build directory
file(COPY "${DATABASE_SOURCE_DIR}/admin.db" DESTINATION ${DATABASE_DEST_DIR})
file(COPY "${DATABASE_SOURCE_DIR}/admin.sqbpro" DESTINATION ${DATABASE_DEST_DIR})


# Add the build directory to the include path
include_directories(${CMAKE_BINARY_DIR})

# Link Qt libraries
target_link_libraries(untitled PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

# Finalize the executable for Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(untitled)
endif()
