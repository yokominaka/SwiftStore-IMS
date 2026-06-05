cmake_minimum_required(VERSION 3.16)

project(OOP_Project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure MSVC sets a correct __cplusplus value required by recent Qt builds
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# Handle Qt's internal tools (MOC, etc.)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_PREFIX_PATH "C:/Qt/6.11.0/mingw_64")

# 1. Find Qt 6.11.0 components
find_package(Qt6 6.11.0 REQUIRED COMPONENTS Core Gui Widgets Sql)

# 2. Define source files (Ensure these match your Explorer exactly)
add_executable(OOP_Project
    src/main.cpp
    src/database/DatabaseManager.cpp
    # src/database/DatabaseManager.h
    src/ui/MainWindow.cpp
    # src/ui/MainWindow.h
    # src/ui/LoginWidget.h
    src/ui/LoginWidget.cpp
    # src/ui/AdminWidget.h
    src/ui/AdminWidget.cpp
    # src/ui/AddProductDialog.h
    src/ui/AddProductDialog.cpp
    # src/ui/StockDelegate.h
    # src/ui/StockDelegate.cpp
    # src/models/CartItem.h
    # src/models/CartItem.cpp
    # src/ui/CustomerWidget.h
    src/ui/CustomerWidget.cpp
    # src/ui/VendorWidget.h
    src/ui/VendorWidget.cpp
)
target_link_libraries(OOP_Project PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)

target_include_directories(OOP_Project PUBLIC 
    src/database
    src
)