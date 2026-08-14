cmake_minimum_required(VERSION 3.16)

project(CakeManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp

    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

    qt_add_executable(CakeManager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        src/models/ingredient.h
        src/models/ingredient.cpp

        src/database/DatabaseManager.h
        src/database/DatabaseManager.cpp
        src/models/cake.h src/models/cake.cpp
    )

else()

    if(ANDROID)

        add_library(CakeManager SHARED
            ${PROJECT_SOURCES}
        )

    else()

        add_executable(CakeManager
            ${PROJECT_SOURCES}
        )

    endif()

endif()

target_link_libraries(CakeManager PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)


# =========================
# Ingredient Test
# =========================

add_executable(testIngredient

    tests/testIngredient.cpp

    src/models/ingredient.h
    src/models/ingredient.cpp

    src/database/DatabaseManager.h
    src/database/DatabaseManager.cpp
)

target_link_libraries(testIngredient PRIVATE
    Qt${QT_VERSION_MAJOR}::Sql
)


# =========================
# Application Properties
# =========================

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CakeManager
    )
endif()

set_target_properties(CakeManager PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING
        ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)


include(GNUInstallDirs)

install(TARGETS CakeManager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CakeManager)
endif()