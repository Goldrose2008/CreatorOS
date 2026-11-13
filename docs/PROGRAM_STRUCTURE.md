# Структура программы CreatorOS
Карта фактических каталогов и файлов репозитория CreatorOS. 

# 1. Корень репозитория
`CMakeLists.txt` - Основное описание сборки CreatorOS: C++ standard, Qt-модули, исходные файлы, ресурсы, библиотеки и post-build deployment. 

`database/` - SQL-структура базы данных и миграции. 
`resources/` - Qt-ресурсы приложения: локализация, database migrations. 
`src/` - Исходный код актуальной C++/Qt версии. 

# 2. `src/`
## 2.1. Точка входа
`main.cpp` - Точка входа приложения: создаёт `QApplication`, подключает `CreatorStyle`, устанавливает имя/версию приложения, создаёт и показывает главного окна CreatorOS - `MainWindow`. 

## 2.2. `src/app` - запуск приложения
`AppInfo.h` - Единственный источник основных данных приложения. На текущий момент содержит название и версию приложения. 
`MainWindow` - Создаёт главное окно, инициализирует БД, собирает Project/Content/Task/Platform/Account application-слои, создаёт **AppShell** и устанавливает его как центральный виджет. 

## 2.3 `src/application`
Что приложение умеет делать с сущностью

`I...Repository` — это интерфейсы, которые определяют, какие операции с данными доступны прикладной логике. 
`...Repository` — их конкретные реализации, которые выполняют эти операции с использованием SQLite. 
При этом `...Service` используют в своей логике `I...Repository`, не зная конкретной реализации в `...Repository`. 
Такое разделение позволяет Service работать независимо от конкретной технологии хранения данных.

---

`/accounts/AccountService` - Что можно делать с сущностью Account: создание, редактирование, удаление и так далее.
`/accounts/IAccountRepository` - перечень доступных операций в БД с сущностью Account.

`/common/StringUtils.h` - Общие типизированные механизмы для работы со строками, используемая несколькими application services.
`/common/ServiceUtils.h` - Общие типизированные механизмы для Services.

`/platforms/IPlatformRepository.h` - перечень доступных операций в БД с сущностью Platform.
`/platforms/PlatformService` - Что можно делать с сущностью Platform: создание, редактирование, удаление и так далее.

`/tasks/ITaskRepository.h` - перечень доступных операций в БД с сущностью Task.
`/tasks/TaskService` - Что можно делать с сущностью Task: создание, редактирование, удаление и так далее.





`/content/ContentService` - Что можно делать с сущностью Content: создание, редактирование, удаление и так далее.
`/content/IContentRepository.h` - перечень доступных операций в БД с сущностью Content.
`/content/IContentTypeRepository` - Application-контракт чтения справочника типов контента. 

`/projects/IProjectRepository.h` | Абстрактный контракт хранилища Project: получение списка, получение по ID, создание, изменение и удаление. |
`/projects/ProjectService.h + ProjectService.cpp` | Реализация сценариев Project, включая нормализацию и базовую проверку входных данных, проверку ContentType и обращение к репозиториям. |



### Правила составления файлов ...Service
Service реализует прикладные сценарии работы с сущностью. Он отвечает за правила и последовательность действий, а не за непосредственное выполнение SQL-запросов.

1. Один Service отвечает за одну сущность и её прикладные сценарии. Например, AccountService работает с Account, PlatformService — с Platform.
2. Зависимости передаются через конструктор. Service получает необходимые I...Repository по ссылке и хранит их как поля-ссылки. Он не создаёт конкретные Repository самостоятельно.
3. Публичные методы отражают прикладные операции. Например:
   - getAll<Entity>s() - например getAllAccounts();
   - get<Entity>ByID();
   - is<Entity>NameValid();
   - create<Entity>();
   - update<Entity>();
   - delete<Entity>().
4. Методы чтения возвращают доменные модели или их стандартные контейнеры. Для получения одной сущности используется std::optional, для списка — std::vector.
5. Правила и инварианты проверяются в Service. UI может предварительно проверять данные для удобства пользователя, но Service самостоятельно гарантирует корректность операции.
6. Нормализация входных данных выполняется в Service. UI передаёт исходное содержимое полей, Service приводит данные к нужному виду перед сохранением.
7. Создание и обновление используют вспомогательный метод set<Entity>Fields(), если это оправдано общей логикой заполнения. Метод принимает изменяемую сущность и исходный объект, а не набор отдельных полей. Он отвечает за заполнение редактируемых данных и относящиеся к ним проверки.
8. Общие прикладные механизмы не копируются между Service. Если алгоритм действительно повторяется, используется подходящий общий типизированный механизм из application/common/.
9. Service не содержит техническую реализацию хранения. Он работает с интерфейсами Repository и не должен зависеть от SQLite, QSqlQuery и других деталей Infrastructure.
10. Вспомогательные методы располагаются в private. Публичными остаются только операции, которые действительно нужны вызывающим компонентам.
11. Каждый ...Service, отвечающий за создание и редактирование сущности, должен содержать приватную функцию set<Entity>Fields(). Она принимает изменяемый объект сущности и исходный объект с переданными данными, заполняет поля, доступные для создания или редактирования, и выполняет необходимые проверки и нормализацию. Функции create<Entity>() и update<Entity>() передают в неё объекты целиком, не перечисляя поля по отдельности. Поля, которые не должны изменяться при редактировании, сохраняются из существующей сущности или устанавливаются отдельно в соответствии с правилами её жизненного цикла.

### Правила составления файлов I...Repository
Интерфейс определяет контракт операций с данными конкретной сущности, которыми может пользоваться Application-слой.
1. Интерфейс располагается рядом с соответствующим Service в src/application/<entity>/.
2. Название — I<Entity>Repository, например IAccountRepository.
3. Интерфейс содержит виртуальный деструктор:    virtual ~I<Entity>Repository() = default;
4. Все операции объявляются чисто виртуальными (= 0).
5. Методы описывают операции с данными, а не прикладные сценарии. Например:
   - findAll();
   - findById();
   - create();
   - update();
   - remove().
6. Методы возвращают доменные модели, стандартные контейнеры и простые типы. Интерфейс не должен раскрывать детали SQLite или Qt SQL.
7. Дополнительные методы допускаются, если они представляют необходимую операцию с данными. Например, hasAccounts() у IPlatformRepository или findByParent() у ITaskRepository. Не нужно искусственно ограничивать интерфейс только CRUD-операциями.
8. Интерфейс не реализует бизнес-логику. Проверки прикладных инвариантов и сценарии остаются в Service.
9. Конкретная реализация (AccountRepository, PlatformRepository и т. д.) находится в Infrastructure, наследуется от соответствующего интерфейса и реализует методы с override.

## 2.4 `src/domain`
Модели предметных сущностей и их значения.

`src/domain/models/Account.h` - Доменная модель Account.

`src/domain/models/Platform.h` - Доменная модель Platform.






`src/domain/models/Project.h + Project.cpp` | Доменная модель проекта: идентификатор, название, описание, владелец, дата выхода, статус, прогресс и даты создания/изменения. |
`src/domain/models/Content.h + Content.cpp` | Минимальная доменная модель Content: проект-владелец, тип контента, роль main/additional и название. |
`src/domain/models/ContentType.h` | Минимальная доменная модель справочника ContentType: идентификатор и название типа контента. |
`src/domain/models/Task.h + Task.cpp` | Доменная модель Task: контекст родителя, иерархия задач, название, описание, дедлайн, статус, приоритет и временные метки. |

## 2.5 `src/infrastructure`
Реализации хранилища и технических интеграций.

```text
src/infrastructure/
└── database/
    ├── AccountRepository.cpp
    ├── AccountRepository.h
    ├── ContentRepository.cpp
    ├── ContentRepository.h
    ├── ContentTypeRepository.cpp
    ├── ContentTypeRepository.h
    ├── DatabaseManager.cpp
    ├── DatabaseManager.h
    ├── PlatformRepository.cpp
    ├── PlatformRepository.h
    ├── ProjectRepository.cpp
    ├── ProjectRepository.h
    ├── TaskRepository.cpp
    └── TaskRepository.h
```

| Файл | Назначение |
|---|---|
| `src/infrastructure/database/AccountRepository.h + AccountRepository.cpp` | SQL-операции с таблицей `accounts`: список, получение по ID, создание, изменение и удаление. |
| `src/infrastructure/database/PlatformRepository.h + PlatformRepository.cpp` | SQL-операции с таблицей `platforms`: список, получение по ID, CRUD и проверка связанных Accounts. |
| `src/infrastructure/database/ContentTypeRepository.h + ContentTypeRepository.cpp` | SQL-чтение справочника `content_types`: получение всех типов и типа по ID. |
| `src/infrastructure/database/ContentRepository.h + ContentRepository.cpp` | SQL-операции с таблицей `contents`: чтение всех записей и выборка по Project/ID, создание, изменение и удаление Content. |
| `src/infrastructure/database/DatabaseManager.h + DatabaseManager.cpp` | Создаёт каталог данных приложения, открывает QSQLITE, включает foreign keys и выполняет database migration. |
| `src/infrastructure/database/ProjectRepository.h + ProjectRepository.cpp` | SQL-операции с таблицей `projects`: чтение, создание, изменение и удаление проектов. |
| `src/infrastructure/database/TaskRepository.h + TaskRepository.cpp` | SQL-операции с таблицей `tasks`: чтение списка, выборка по контексту родителя/иерархии, создание, изменение и удаление Task. |

---


## 2.6 `src/ui`
UI-компоненты, страницы, навигация, shell и визуальные стили на Qt Widgets.

```text
src/ui/
├── components/
├── localization/
├── navigation/
├── pages/
├── shell/
└── style/
```

---

### 8.1. UI Components — `src/ui/components`
### Entity

```text
src/ui/components/entity/
├── EntityCard.cpp
├── EntityCard.h
├── EntityDetails.cpp
├── EntityDetails.h
├── EntityHeader.cpp
├── EntityHeader.h
├── EntityList.cpp
└── EntityList.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/entity/EntityCard.h + EntityCard.cpp` | Реализация общего каркаса и UI-поведения `EntityCard`: заголовок, описание, статус, прогресс, метаданные, дополнительные области и действия. |
| `src/ui/components/entity/EntityHeader.h + EntityHeader.cpp` | Реализация заголовка сущности с названием, описанием, status widget, metadata и action widgets. |
| `src/ui/components/entity/EntityDetails.h + EntityDetails.cpp` | Реализация общей структуры `EntityDetails`: navigation, header, состояния, summary/info, actions и content-регионы. |
| `src/ui/components/entity/EntityList.h + EntityList.cpp` | Реализация списка сущностей: добавление и очистка элементов, подсчёт текущего количества; размещён рядом с общими entity-компонентами. |

### Foundation

```text
src/ui/components/foundation/
├── SurfaceWidget.cpp
└── SurfaceWidget.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/foundation/SurfaceWidget.h + SurfaceWidget.cpp` | Общая отрисовка поверхности: фон, рамка, скругление, внутренние отступы и интервалы. |

### Forms

```text
src/ui/components/forms/
├── ConfirmModal.cpp
├── ConfirmModal.h
├── EditorDialog.cpp
├── EditorDialog.h
├── FormField.cpp
└── FormField.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/forms/FormField.h + FormField.cpp` | Реализация визуальной структуры и состояний FormField. |
| `src/ui/components/forms/EditorDialog.h + EditorDialog.cpp` | Реализация общего layout, кнопок Save/Cancel, хранения режима и вызова виртуального `applyChanges()` перед закрытием диалога. |
| `src/ui/components/forms/ConfirmModal.h + ConfirmModal.cpp` | Реализация общего layout, текста подтверждения, кнопок подтверждения/отмены и выполнения переданного callback только после подтверждения. |

### Layout

```text
src/ui/components/layout/
├── Card.cpp
├── Card.h
├── PageHeader.cpp
├── PageHeader.h
├── Panel.cpp
├── Panel.h
├── Section.cpp
├── Section.h
├── Toolbar.cpp
└── Toolbar.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/layout/Panel.h + Panel.cpp` | Panel через `SurfaceWidget` с цветом обычной поверхности. |
| `src/ui/components/layout/Card.h + Card.cpp` | Card с elevated-вариантом поверхности. |
| `src/ui/components/layout/Section.h + Section.cpp` | Section. |
| `src/ui/components/layout/PageHeader.h + PageHeader.cpp` | PageHeader. |
| `src/ui/components/layout/Toolbar.h + Toolbar.cpp` | Toolbar и его методов добавления widgets/stretch. |

### Navigation

```text
src/ui/components/navigation/
├── MenuItem.cpp
├── MenuItem.h
├── MenuSection.cpp
└── MenuSection.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/navigation/MenuItem.h + MenuItem.cpp` | Реализация визуального элемента меню и сигнала маршрута/действия. |
| `src/ui/components/navigation/MenuSection.h + MenuSection.cpp` | Реализация оформления и размещения элементов секции. |

### States

```text
src/ui/components/states/
├── EmptyState.h
├── ErrorState.cpp
├── ErrorState.h
├── LoadingState.h
├── StateWidget.cpp
└── StateWidget.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/states/StateWidget.h + StateWidget.cpp` | Общая визуальная реализация title/description для state-компонентов. |
| `src/ui/components/states/EmptyState.h` | Пустое состояние, наследующее общую структуру `StateWidget`. |
| `src/ui/components/states/ErrorState.h + ErrorState.cpp` | Реализация retry-кнопки и сигнала `retryRequested`. |
| `src/ui/components/states/LoadingState.h` | Специализированное состояние загрузки, наследующее общую UI-механику `StateWidget`; отдельной реализации `.cpp` не требует. |

---

## 8.2. Localization — `src/ui/localization`

```text
src/ui/localization/
├── LocalizationService.cpp
└── LocalizationService.h
```

| Файл | Назначение |
|---|---|
| `src/ui/localization/LocalizationService.h + LocalizationService.cpp` | Загружает TSV из Qt Resource System и строит in-memory каталог переводов. |

---

## 8.3. Navigation — `src/ui/navigation`

```text
src/ui/navigation/
├── CurrentPage.h
├── NavigationController.cpp
└── NavigationController.h
```

| Файл | Назначение |
|---|---|
| `src/ui/navigation/CurrentPage.h` | Контракт записи навигации: route + соответствующий QWidget. |
| `src/ui/navigation/NavigationController.h + NavigationController.cpp` | Хранит зарегистрированные страницы и переключает `QStackedWidget` по route. |

---

## 8.4. Pages — `src/ui/pages`

```text
src/ui/pages/accounts/
├── AccountEditorDialog.h
└── AccountsPage.h
```

| Файл | Назначение |
|---|---|
| `src/ui/pages/accounts/AccountEditorDialog.h` | Форма создания и редактирования Account с выбором существующей Platform и полями профиля; сохранение выполняется через `AccountService`. |
| `src/ui/pages/accounts/AccountsPage.h` | Страница списка Account: отображение платформы и сведений аккаунта, создание, редактирование, удаление с подтверждением и состояния загрузки/пустого списка/ошибки. |



### Projects

```text
src/ui/pages/projects/
├── ProjectCard.cpp
├── ProjectCard.h
├── ProjectDetailsPage.cpp
├── ProjectDetailsPage.h
├── ProjectEditorDialog.cpp
├── ProjectEditorDialog.h
├── ProjectsPage.cpp
└── ProjectsPage.h
```

| Файл | Назначение |
|---|---|
| `src/ui/pages/projects/ProjectsPage.h + ProjectsPage.cpp` | Загружает Project через `ProjectService`, отображает loading/empty/error состояния и создаёт `ProjectCard`. |
| `src/ui/pages/projects/ProjectCard.h + ProjectCard.cpp` | `ProjectCard`: настройка общих UI-механизмов `EntityCard`, отображение данных Project и обработка специфических пользовательских действий. |
| `src/ui/pages/projects/ProjectDetailsPage.h + ProjectDetailsPage.cpp` | Подробности Project: загрузка данных и действия пользователя через `ProjectService`. |
| `src/ui/pages/projects/ProjectEditorDialog.h + ProjectEditorDialog.cpp` | Форма создания и редактирования Project; сохраняет данные через `ProjectService`. |

---


## 8.4.1. Content

```text
src/ui/pages/content/
├── ContentCard.cpp
├── ContentCard.h
├── ContentDetails.cpp
├── ContentDetails.h
├── ContentEditorDialog.cpp
└── ContentEditorDialog.h
```

| Файл | Назначение |
|---|---|
| `src/ui/pages/content/ContentCard.h + ContentCard.cpp` | Карточка Content с основными полями и событием открытия подробностей. |
| `src/ui/pages/content/ContentDetails.h + ContentDetails.cpp` | Подробности Content: просмотр, редактирование и удаление Additional Content. |
| `src/ui/pages/content/ContentEditorDialog.h + ContentEditorDialog.cpp` | Реализация формы Content и сохранения через `ContentService`; содержит только редактируемые пользователем поля. |



## 8.4.2. Tasks

```text
src/ui/pages/tasks/
├── TaskCard.cpp
├── TaskCard.h
├── TaskDetailsPage.cpp
├── TaskDetailsPage.h
├── TaskEditorDialog.cpp
├── TaskEditorDialog.h
├── TasksPage.cpp
└── TasksPage.h
```

| Файл | Назначение |
|---|---|
| `src/ui/pages/tasks/TaskCard.h + TaskCard.cpp` | Карточка Task с основными полями, метаданными и действием открытия подробностей через сигнал `openRequested`. |
| `src/ui/pages/tasks/TasksPage.h + TasksPage.cpp` | Загружает и отображает список Task, открывает форму создания и передаёт события открытия Task в страницу подробностей. |
| `src/ui/pages/tasks/TaskEditorDialog.h + TaskEditorDialog.cpp` | Форма создания Task с выбором Project/Content-родителя и редактирования названия, описания и дедлайна. |
| `src/ui/pages/tasks/TaskDetailsPage.h + TaskDetailsPage.cpp` | Подробности Task, отображение родителя и метаданных, редактирование и удаление с подтверждением. |

## 8.5. Shell — `src/ui/shell`

```text
src/ui/shell/
├── AppShell.cpp
├── AppShell.h
├── MenuComposer.cpp
├── MenuComposer.h
├── Sidebar.cpp
├── Sidebar.h
├── Workspace.cpp
└── Workspace.h
```

| Файл | Назначение |
|---|---|
| `src/ui/shell/AppShell.h + AppShell.cpp` | Собирает Sidebar + Workspace, подключает LocalizationService, NavigationController и страницы Project, Content, Task, Platform и Account; связывает списки с соответствующими страницами подробностей. |
| `src/ui/shell/Sidebar.h + Sidebar.cpp` | Sidebar, добавление секций/элементов и управление активным route. |
| `src/ui/shell/Workspace.h + Workspace.cpp` | Контейнер отображения текущей страницы приложения. |
| `src/ui/shell/MenuComposer.h + MenuComposer.cpp` | Собирает Sidebar из MenuSection/MenuItem и передаёт локализованные подписи, иконки и routes. |

---

## 8.6. Style — `src/ui/style`

```text
src/ui/style/
├── Colors.h
├── CreatorStyle.cpp
├── CreatorStyle.h
├── Icons.cpp
├── Icons.h
└── Metrics.h
```

| Файл | Назначение |
|---|---|
| `src/ui/style/Colors.h` | Центральные цветовые токены интерфейса. |
| `src/ui/style/Metrics.h` | Центральные размеры, spacing, высоты контролов и радиусы. |
| `src/ui/style/CreatorStyle.h + CreatorStyle.cpp` | Настройка базовой Qt палитры под визуальную систему CreatorOS. |
| `src/ui/style/Icons.h + Icons.cpp` | Единый доступ к иконкам интерфейса. |

---

## 8.7. Localization Dashboard developer tool — `tools/localization`

Developer tool для управления каталогом локализации. Он отделён от runtime-приложения CreatorOS.

| Файл | Назначение |
|---|---|
| `tools/localization/LocalizationEntry.h` | Модель одной записи локализации: ID и набор переводов по locale. |
| `tools/localization/LocalizationCatalog.h + LocalizationCatalog.cpp` | Каталог записей и переводов, операции с ID и добавление языковой колонки. |
| `tools/localization/LocalizationTsvStore.h + LocalizationTsvStore.cpp` | Загрузка TSV с проверкой структуры и безопасное сохранение через `QSaveFile`. |
| `tools/localization/LocalizationUsage.h` | Модели статического использования localization ID и динамической ссылки на localization-вызов. |
| `tools/localization/LocalizationUsageIndex.h + LocalizationUsageIndex.cpp` | Хранит статические использования по ID, места использования и динамические ссылки. |
| `tools/localization/LocalizationSourceScanner.h + LocalizationSourceScanner.cpp` | Сканирование исходников и поиск статических/динамических обращений к локализации. |
| `tools/localization/LocalizationRefactorService.h + LocalizationRefactorService.cpp` | Предварительный просмотр и безопасное переименование распознанных ссылок на localization ID. |
| `tools/localization/LocalizationValidationIssue.h` | Модель структурированной проблемы Validator: тип, severity, ID, locale и расположение. |
| `tools/localization/LocalizationValidator.h + LocalizationValidator.cpp` | Проверка ID, переводов, placeholders и ссылок на ключи локализации. |
| `tools/localization/tests/LocalizationValidatorTests.cpp` | Регрессионные тесты Validator, TSV persistence и добавления locale; используют временные файлы и не изменяют рабочий каталог. |
| `tools/localization/LocalizationDashboardWindow.h + LocalizationDashboardWindow.cpp` | Окно каталога: поиск, редактирование переводов, Problems, использование ID, Rename, экспорт и добавление языка. |
| `tools/localization/main.cpp` | Точка входа developer tool: загружает TSV, запускает source scanner и открывает Localization Dashboard. |
