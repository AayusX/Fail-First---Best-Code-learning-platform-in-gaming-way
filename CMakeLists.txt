cmake_minimum_required(VERSION 3.16)

project(FailFirst VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Prefer Qt6, fallback to Qt5 if needed
find_package(Qt6 COMPONENTS Core Widgets Sql Network QUIET)
if(Qt6_FOUND)
    set(QT_VERSION 6)
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets Sql Network)
    set(QT_VERSION 5)
endif()

set(PROJECT_SOURCES
    src/main.cpp
    src/core/EconomyManager.cpp
    src/core/GamificationEngine.cpp
    src/core/Validator.cpp
    src/core/CompilerManager.cpp
    src/database/DatabaseManager.cpp
    src/ui/Dashboard.cpp
    src/ui/StyleHelper.cpp
    src/ui/SimpleSections.cpp
    src/ui/LearnSection.cpp
    src/ui/ChallengeSection.cpp
    src/ui/NotificationManager.cpp
    src/ui/LoginWindow.cpp
    src/ui/LanguageSelection.cpp
    src/ui/AvatarSelector.cpp
    src/ui/SplashScreen.cpp
    resources.qrc
)

# Static linking option for portable builds
option(STATIC_BUILD "Build with static Qt for portable deployment" OFF)
if(STATIC_BUILD)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
endif()

# Add Windows icon
if(WIN32)
    set(APP_ICON_RC app_icon.rc)
    enable_language(RC)
endif()

add_executable(FailFirst WIN32
    ${PROJECT_SOURCES}
    ${APP_ICON_RC}
)

target_include_directories(FailFirst PRIVATE src)

target_link_libraries(FailFirst PRIVATE
    Qt${QT_VERSION}::Core
    Qt${QT_VERSION}::Widgets
    Qt${QT_VERSION}::Sql
    Qt${QT_VERSION}::Network
)

# Copy resources to build directory if needed, but we use QRC.
