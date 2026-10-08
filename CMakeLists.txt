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
)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)

qt_add_executable(CreatorOS
    WIN32

    src/main.cpp
    
    src/app/MainWindow.cpp
    src/app/MainWindow.h

    src/application/content/IContentTypeRepository.h
    src/application/content/IContentRepository.h
    src/application/content/ContentService.cpp
    src/application/content/ContentService.h

    src/application/projects/IProjectRepository.h
    src/application/projects/ProjectService.cpp
    src/application/projects/ProjectService.h
    
    src/domain/models/Project.cpp
    src/domain/models/Project.h
    src/domain/models/Content.cpp
    src/domain/models/Content.h
    src/domain/models/ContentType.h
    src/domain/models/Task.cpp
    src/domain/models/Task.h

    src/infrastructure/database/DatabaseManager.cpp
    src/infrastructure/database/DatabaseManager.h
    src/infrastructure/database/ContentRepository.cpp
    src/infrastructure/database/ContentRepository.h
    src/infrastructure/database/ContentTypeRepository.cpp
    src/infrastructure/database/ContentTypeRepository.h
    src/infrastructure/database/ProjectRepository.cpp
    src/infrastructure/database/ProjectRepository.h
    src/infrastructure/database/TaskRepository.cpp
    src/infrastructure/database/TaskRepository.h

    src/ui/components/entity/EntityHeader.cpp
    src/ui/components/entity/EntityHeader.h
    src/ui/components/entity/EntityCard.cpp
    src/ui/components/entity/EntityCard.h
    src/ui/components/entity/EntityDetails.cpp
    src/ui/components/entity/EntityDetails.h

    src/ui/components/forms/ConfirmModal.cpp
    src/ui/components/forms/ConfirmModal.h
    src/ui/components/forms/EditorDialog.cpp
    src/ui/components/forms/EditorDialog.h
    src/ui/components/forms/FormField.cpp
    src/ui/components/forms/FormField.h

    src/ui/components/foundation/SurfaceWidget.cpp
    src/ui/components/foundation/SurfaceWidget.h

    src/ui/components/layout/Panel.cpp
    src/ui/components/layout/Panel.h
    src/ui/components/layout/Card.cpp
    src/ui/components/layout/Card.h
    src/ui/components/layout/Section.cpp
    src/ui/components/layout/Section.h
    src/ui/components/layout/PageHeader.cpp
    src/ui/components/layout/PageHeader.h
    src/ui/components/layout/Toolbar.cpp
    src/ui/components/layout/Toolbar.h
    
    src/ui/components/lists/EntityList.cpp
    src/ui/components/lists/EntityList.h

    src/ui/components/navigation/MenuItem.cpp
    src/ui/components/navigation/MenuItem.h
    src/ui/components/navigation/MenuSection.cpp
    src/ui/components/navigation/MenuSection.h

    src/ui/components/states/StateWidget.cpp
    src/ui/components/states/StateWidget.h
    src/ui/components/states/EmptyState.h
    src/ui/components/states/LoadingState.h
    src/ui/components/states/ErrorState.cpp
    src/ui/components/states/ErrorState.h

    src/ui/localization/LocalizationService.cpp
    src/ui/localization/LocalizationService.h

    src/ui/navigation/CurrentPage.h
    src/ui/navigation/NavigationController.cpp
    src/ui/navigation/NavigationController.h

    src/ui/pages/content/ContentCard.cpp
    src/ui/pages/content/ContentCard.h
    src/ui/pages/content/ContentDetails.cpp
    src/ui/pages/content/ContentDetails.h
    src/ui/pages/content/ContentEditorDialog.cpp
    src/ui/pages/content/ContentEditorDialog.h

    src/ui/pages/projects/ProjectCard.cpp
    src/ui/pages/projects/ProjectCard.h
    src/ui/pages/projects/ProjectsPage.cpp
    src/ui/pages/projects/ProjectsPage.h
    src/ui/pages/projects/ProjectDetailsPage.cpp
    src/ui/pages/projects/ProjectDetailsPage.h
    src/ui/pages/projects/ProjectEditorDialog.cpp
    src/ui/pages/projects/ProjectEditorDialog.h

    src/ui/pages/tasks/TaskCard.cpp
    src/ui/pages/tasks/TaskCard.h
    src/ui/pages/tasks/TasksPage.cpp
    src/ui/pages/tasks/TasksPage.h

    src/application/common/StringUtils.h

    src/application/tasks/ITaskRepository.h
    src/application/tasks/TaskService.cpp
    src/application/tasks/TaskService.h

    src/ui/shell/AppShell.cpp
    src/ui/shell/AppShell.h
    src/ui/shell/Sidebar.cpp
    src/ui/shell/Sidebar.h
    src/ui/shell/Workspace.cpp
    src/ui/shell/Workspace.h
    src/ui/shell/MenuComposer.cpp
    src/ui/shell/MenuComposer.h

    src/ui/style/CreatorStyle.cpp
    src/ui/style/CreatorStyle.h
    src/ui/style/Icons.cpp
    src/ui/style/Icons.h

    resources/localization.qrc
    resources/database.qrc

)

target_link_libraries(CreatorOS
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Network
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