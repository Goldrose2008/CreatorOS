#include "MainWindow.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <exception>

#include "AppInfo.h"
#include "../ui/shell/AppShell.h"
#include "../application/accounts/AccountService.h"
#include "../application/content/ContentService.h"
#include "../application/object_relations/ObjectRelationService.h"
#include "../application/objects/ObjectService.h"
#include "../application/object_types/ObjectTypeService.h"
#include "../application/platforms/PlatformService.h"
#include "../application/projects/ProjectService.h"
#include "../application/properties/PropertyDefinitionService.h"
#include "../application/properties/PropertyValueService.h"
#include "../application/tasks/TaskService.h"
#include "../infrastructure/database/AccountRepository.h"
#include "../infrastructure/database/ContentRepository.h"
#include "../infrastructure/database/ContentTypeRepository.h"
#include "../infrastructure/database/ObjectRelationRepository.h"
#include "../infrastructure/database/ObjectRepository.h"
#include "../infrastructure/database/ObjectTypeRepository.h"
#include "../infrastructure/database/PlatformRepository.h"
#include "../infrastructure/database/ProjectRepository.h"
#include "../infrastructure/database/PropertyDefinitionRepository.h"
#include "../infrastructure/database/PropertyValueRepository.h"
#include "../infrastructure/database/TaskRepository.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(QString::fromLatin1(AppInfo::Name));
    resize(1280, 800);
}

bool MainWindow::initialize(QString *errorMessage)
{
    if (!databaseManager_.initialize(errorMessage))
    {
        return false;
    }

    QSqlDatabase database = databaseManager_.database();

    if (!database.transaction())
    {
        if (errorMessage != nullptr)
            *errorMessage = database.lastError().text();

        return false;
    }

    accountRepository_ = std::make_unique<AccountRepository>(databaseManager_);
    contentRepository_ = std::make_unique<ContentRepository>(databaseManager_);
    contentTypeRepository_ = std::make_unique<ContentTypeRepository>(databaseManager_);
    objectTypeRepository_ = std::make_unique<ObjectTypeRepository>(databaseManager_);
    objectRepository_ = std::make_unique<ObjectRepository>(databaseManager_);
    objectRelationRepository_ = std::make_unique<ObjectRelationRepository>(databaseManager_);
    platformRepository_ = std::make_unique<PlatformRepository>(databaseManager_);
    projectRepository_ = std::make_unique<ProjectRepository>(databaseManager_);
    propertyDefinitionRepository_ = std::make_unique<PropertyDefinitionRepository>(databaseManager_);
    propertyValueRepository_ = std::make_unique<PropertyValueRepository>(databaseManager_);
    taskRepository_ = std::make_unique<TaskRepository>(databaseManager_);
    
    accountService_ = std::make_unique<AccountService>(*accountRepository_, *platformRepository_);
    contentService_ = std::make_unique<ContentService>(*contentRepository_, *projectRepository_, *contentTypeRepository_);
    objectTypeService_ = std::make_unique<ObjectTypeService>(*objectTypeRepository_, *objectRepository_);
    objectService_ = std::make_unique<ObjectService>(*objectRepository_, *objectTypeRepository_);
    objectRelationService_ = std::make_unique<ObjectRelationService>(*objectRelationRepository_, *objectRepository_);
    platformService_ = std::make_unique<PlatformService>(*platformRepository_);
    projectService_ = std::make_unique<ProjectService>(*projectRepository_, *contentTypeRepository_);
    propertyDefinitionService_ = std::make_unique<PropertyDefinitionService>(*propertyDefinitionRepository_, *objectTypeRepository_);
    propertyValueService_ = std::make_unique<PropertyValueService>(*propertyValueRepository_, *propertyDefinitionRepository_, *objectRepository_);
    taskService_ = std::make_unique<TaskService>(*taskRepository_, *projectRepository_, *contentRepository_);
    
    try
    {
        if (objectTypeService_->getAllObjectTypes().empty())
        {
            objectTypeService_->createDefaultObjectTypes();

            // Здесь в дальнейшем будут добавляться остальные начальные данные.
        }

        if (!database.commit())
        {
            const QString commitError = database.lastError().text();
            database.rollback();

            if (errorMessage != nullptr)
                *errorMessage = commitError;

            return false;
        }
    }
    catch (const std::exception &error)
    {
        database.rollback();

        if (errorMessage != nullptr)
            *errorMessage = QString::fromUtf8(error.what());

        return false;
    }

    auto *shell = new AppShell(
        *projectService_,
        *contentService_,
        *taskService_,
        *accountService_,
        *platformService_,
        this
    );
    
    setCentralWidget(shell);

    return true;
}

MainWindow::~MainWindow() = default;