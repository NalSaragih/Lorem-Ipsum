cmake_minimum_required(VERSION 3.16)
project(SocialMediaQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCE_FILES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    SocialMedia.cpp
    SocialMedia.h
    DataStructures.h
    feed.cpp
    feed.hpp
    Notifikasi.cpp
    Notifikasi.hpp
    chat.cpp
    chat.hpp
    follow.cpp
    follow.hpp
    like.cpp
    like.hpp
    trending.cpp
    trending.hpp
    PostWidget.cpp
    PostWidget.h
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE .)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
