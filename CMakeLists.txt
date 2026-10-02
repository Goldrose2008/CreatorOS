cmake_minimum_required(VERSION 3.21)

project(CreatorOS
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 6.11 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
    Network
    WebEngineWidgets
    WebChannel
)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(CreatorOS
    WIN32

    src/main.cpp
    src/app/MainWindow.cpp
    src/app/MainWindow.h

    src/ui/localization/LocalizationService.cpp
    src/ui/localization/LocalizationService.h

    src/ui/components/MenuItem.cpp
    src/ui/components/MenuItem.h
    src/ui/components/MenuSection.cpp
    src/ui/components/MenuSection.h
    src/ui/components/Panel.cpp
    src/ui/components/Panel.h
    src/ui/components/Card.cpp
    src/ui/components/Card.h
    src/ui/components/Section.cpp
    src/ui/components/Section.h
    src/ui/components/PageHeader.cpp
    src/ui/components/PageHeader.h
    src/ui/components/Toolbar.cpp
    src/ui/components/Toolbar.h
    src/ui/components/EntityList.cpp
    src/ui/components/EntityList.h
    src/ui/components/EmptyState.cpp
    src/ui/components/EmptyState.h
    src/ui/components/LoadingState.cpp
    src/ui/components/LoadingState.h
    src/ui/components/ErrorState.cpp
    src/ui/components/ErrorState.h
    src/ui/components/FormField.cpp
    src/ui/components/FormField.h

    src/ui/style/CreatorStyle.cpp
    src/ui/style/CreatorStyle.h
    src/ui/style/Icons.cpp
    src/ui/style/Icons.h

    src/ui/navigation/CurrentPage.h
    src/ui/navigation/NavigationController.cpp
    src/ui/navigation/NavigationController.h

    src/ui/shell/AppShell.cpp
    src/ui/shell/AppShell.h
    src/ui/shell/Sidebar.cpp
    src/ui/shell/Sidebar.h
    src/ui/shell/Workspace.cpp
    src/ui/shell/Workspace.h
    src/ui/shell/MenuComposer.cpp
    src/ui/shell/MenuComposer.h

    src/ui/shell/WebUiHost.cpp
    src/ui/shell/WebUiHost.h

    src/ui/bridge/WebBridge.cpp
    src/ui/bridge/WebBridge.h

    resources/web.qrc
    resources/localization.qrc
)

target_link_libraries(CreatorOS
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Network
        Qt6::WebEngineWidgets
        Qt6::WebChannel
)

if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE
        NAMES windeployqt.exe
        HINTS "${Qt6_DIR}/../../../bin"
        REQUIRED
    )

    add_custom_command(TARGET CreatorOS POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
            "$<IF:$<CONFIG:Debug>,--debug,--release>"
            "--skip-plugin-types"
            "position"
            "$<TARGET_FILE:CreatorOS>"

        COMMENT "Deploying Qt runtime for CreatorOS..."
        VERBATIM
    )
endif()