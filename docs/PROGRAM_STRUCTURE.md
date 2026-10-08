# Структура программы CreatorOS

## 1. Назначение документа

Этот документ является картой текущего репозитория CreatorOS.

Для каждого файла указано его назначение. Основная часть документа описывает **актуальную C++/Qt 6 версию приложения**. Архитектура ориентирована на целостные C++/Qt-классы: визуальная структура и UI-поведение одной ответственности находятся в одном классе. Разбиение файла на отдельные UI/logic части выполняется только при наличии самостоятельной ответственности, повторного использования или внешней технической границы.

Основной UI полностью реализуется средствами Qt Widgets.

---

# 2. Корень репозитория

| Путь | Назначение |
|---|---|
| `.vscode/c_cpp_properties.json` | Настройки IntelliSense и путей заголовков C++/Qt для VS Code. |
| `.vscode/settings.json` | Настройки CMake, генератора Visual Studio, архитектуры x64 и пути к Qt для VS Code. |
| `.gitignore` | Список файлов и каталогов, которые Git не должен отслеживать. |
| `CMakeLists.txt` | Основное описание сборки CreatorOS: C++ standard, Qt-модули, исходные файлы, ресурсы, библиотеки и post-build deployment. |
| `docs/DEVELOPMENT_LOG.md` | Журнал этапов разработки, контрольных точек, архитектурных изменений и результатов сборки. |
| `docs/TECHNICAL_SPEC.md` | Основная техническая и архитектурная спецификация CreatorOS. |
| `docs/USER_GUIDE.md` | Пользовательская документация приложения. |
| `docs/PROGRAM_STRUCTURE.md` | Карта структуры проекта с описанием назначения каждого файла. |
| `docs/THIRD_PARTY_LICENSES.md` | Реестр лицензий Qt и сторонних компонентов, лицензионных обязательств и release-проверок. |
| `docs/LOCALIZATION.md` | Спецификация системы локализации: правила text ID, устранение дублей и канонические общие ключи. |
| `database/` | SQL-структура базы данных и миграции. |
| `resources/` | Qt-ресурсы приложения: локализация, database migrations. |
| `src/` | Исходный код актуальной C++/Qt версии. |
| `backup/` | Архив предыдущей React/Electron реализации; не используется как рабочая основа актуальной версии. |

---

# 3. Актуальная программа — `src/`
## 3.1. Запуск приложения — `src/app`
```text
src/app/
├── AppInfo.h
├── MainWindow.cpp
└── MainWindow.h
```

| Файл | Назначение |
|---|---|
| `src/app/AppInfo.h` | Единственный источник имени и версии приложения. |
| `src/app/MainWindow.h` | Объявление главного окна CreatorOS на базе `QMainWindow`. |
| `src/app/MainWindow.cpp` | Создаёт главное окно, инициализирует БД, собирает Project/Content/Task application-слои, создаёт `AppShell` и устанавливает его как центральный виджет. |

## 3.2. Точка входа

| Файл | Назначение |
|---|---|
| `src/main.cpp` | Точка входа приложения: создаёт `QApplication`, подключает `CreatorStyle`, устанавливает имя/версию приложения, создаёт и показывает главного окна CreatorOS - `MainWindow`. |

---

# 4. Domain — `src/domain`
Доменный слой содержит предметную модель и правила бизнеса. Он не должен зависеть от Qt Widgets, SQLite и конкретного UI.

```text
src/domain/
└── models/
    ├── Project.h
    ├── Project.cpp
    ├── Content.h
    ├── Content.cpp
    └── ContentType.h
```

| Файл | Назначение |
|---|---|
| `src/domain/models/Project.h` | Доменная модель проекта: идентификатор, название, описание, владелец, дата выхода, статус, прогресс и даты создания/изменения. |
| `src/domain/models/Project.cpp` | Преобразование статуса Project между типом `ProjectStatus` и строковым представлением, используемым хранилищем. |
| `src/domain/models/Content.h` | Минимальная доменная модель Content: проект-владелец, тип контента, роль main/additional и название. |
| `src/domain/models/Content.cpp` | Преобразование роли Content между типом `ContentRole` и строковым представлением, используемым хранилищем. |
| `src/domain/models/ContentType.h` | Минимальная доменная модель справочника ContentType: идентификатор и название типа контента. |
| `src/domain/models/Task.h` | Доменная модель Task: контекст родителя, иерархия задач, название, описание, дедлайн, статус, приоритет и временные метки. |
| `src/domain/models/Task.cpp` | Преобразование типа родителя и статуса Task между enum и строковым представлением хранилища. |

---

# 5. Application — `src/application`
Application-слой содержит пользовательские сценарии и связывает UI с доменной моделью и репозиториями.

```text
src/application/
├── common/
│   └── StringUtils.h
├── content/
│   ├── ContentService.cpp
│   ├── ContentService.h
│   ├── IContentRepository.h
│   └── IContentTypeRepository.h
├── projects/
│   ├── IProjectRepository.h
│   ├── ProjectService.cpp
│   └── ProjectService.h
└── tasks/
    ├── ITaskRepository.h
    ├── TaskService.cpp
    └── TaskService.h
```

| Файл | Назначение |
|---|---|
| `src/application/content/IContentTypeRepository.h` | Application-контракт чтения справочника типов контента. |
| `src/application/content/IContentRepository.h` | Application-контракт хранилища Content: получение Content по Project/ID, создание, изменение и удаление. |
| `src/application/content/ContentService.h` | Публичный контракт application service для сценариев полноценного Content. |
| `src/application/content/ContentService.cpp` | Реализация сценариев Content, включая проверки Project/ContentType и запрет удаления Main Content. |
| `src/application/projects/IProjectRepository.h` | Абстрактный контракт хранилища Project: получение списка, получение по ID, создание, изменение и удаление. |
| `src/application/projects/ProjectService.h` | Публичный контракт application service для сценариев работы с Project. |
| `src/application/projects/ProjectService.cpp` | Реализация сценариев Project, включая нормализацию и базовую проверку входных данных, проверку ContentType и обращение к репозиториям. |
| `src/application/common/StringUtils.h` | Общая прикладная утилита нормализации строк, используемая несколькими application services вместо дублирования одинаковой реализации. |
| `src/application/tasks/ITaskRepository.h` | Application-контракт хранилища Task: список, выборка по родителю/ID, создание, изменение и удаление. |
| `src/application/tasks/TaskService.h` | Публичный контракт application service для получения и базовых операций Task. |
| `src/application/tasks/TaskService.cpp` | Реализация сценариев Task с проверкой контекста Project/Content и иерархической связи с родительской Task. |

---

# 6. Infrastructure — `src/infrastructure`
Infrastructure содержит конкретные технические реализации хранения и работы с внешней средой.

```text
src/infrastructure/
└── database/
    ├── ContentRepository.cpp
    ├── ContentRepository.h
    ├── ContentTypeRepository.cpp
    ├── ContentTypeRepository.h
    ├── DatabaseManager.cpp
    ├── DatabaseManager.h
    ├── ProjectRepository.cpp
    ├── ProjectRepository.h
    ├── TaskRepository.cpp
    └── TaskRepository.h
```

| Файл | Назначение |
|---|---|
| `src/infrastructure/database/ContentTypeRepository.h` | Объявление SQLite-реализации `IContentTypeRepository`. |
| `src/infrastructure/database/ContentTypeRepository.cpp` | SQL-чтение справочника `content_types`: получение всех типов и типа по ID. |
| `src/infrastructure/database/ContentRepository.h` | Объявление SQLite-реализации `IContentRepository`. |
| `src/infrastructure/database/ContentRepository.cpp` | SQL-операции с таблицей `contents`: чтение по Project/ID, создание, изменение и удаление Content. |
| `src/infrastructure/database/DatabaseManager.h` | Контракт объекта, который открывает SQLite-подключение, предоставляет БД и применяет схему/миграции. |
| `src/infrastructure/database/DatabaseManager.cpp` | Создаёт каталог данных приложения, открывает QSQLITE, включает foreign keys и выполняет database migration. |
| `src/infrastructure/database/ProjectRepository.h` | Объявление SQLite-реализации `IProjectRepository`. |
| `src/infrastructure/database/ProjectRepository.cpp` | SQL-операции с таблицей `projects`: чтение, создание, изменение и удаление проектов. |
| `src/infrastructure/database/TaskRepository.h` | Объявление SQLite-реализации `ITaskRepository`. |
| `src/infrastructure/database/TaskRepository.cpp` | SQL-операции с таблицей `tasks`: чтение списка, выборка по контексту родителя/иерархии, создание, изменение и удаление Task. |

---

# 7. Database — `database/`

```text
database/
└── migrations/
    ├── 001_initial.sql
    ├── 002_content_and_content_types.sql
    ├── 003_full_content.sql
    └── 004_tasks.sql
```

| Файл | Назначение |
|---|---|
| `database/migrations/001_initial.sql` | Первая версия структуры SQLite: создаёт таблицу `projects` с ограничениями статуса и диапазона прогресса. |
| `database/migrations/002_content_and_content_types.sql` | Вторая версия структуры SQLite: создаёт `content_types` и `contents`, связывает Content с Project/ContentType, ограничивает один `main` Content на Project и добавляет базовые типы контента. |
| `database/migrations/003_full_content.sql` | Третья версия структуры SQLite: расширяет `contents` полями description, priority, production deadline, status, progress и временными метками. |
| `database/migrations/004_tasks.sql` | Четвёртая версия структуры SQLite: создаёт `tasks` с контекстом Project/Content, иерархией через `parent_task_id`, статусами, приоритетом и дедлайном. |

---

# 8. UI — `src/ui`
UI полностью реализуется средствами Qt Widgets. Страница является целостным UI-классом и может использовать отдельные переиспользуемые классы только там, где они действительно имеют самостоятельную ответственность, контракт или сложное повторное поведение.

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

## 8.1. UI Components — `src/ui/components`
### Entity

```text
src/ui/components/entity/
├── EntityCard.cpp
├── EntityCard.h
├── EntityDetails.cpp
├── EntityDetails.h
├── EntityHeader.cpp
└── EntityHeader.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/entity/EntityCard.h` | Объявление базового целостного UI-класса карточки сущности, наследующегося от `Card`; предоставляет общие UI-механизмы специализированным карточкам. |
| `src/ui/components/entity/EntityCard.cpp` | Реализация общего каркаса и UI-поведения `EntityCard`: заголовок, описание, статус, прогресс, метаданные, дополнительные области и действия. База не знает конкретный тип сущности. |
| `src/ui/components/entity/EntityHeader.h` | Объявление самостоятельного повторяемого UI-класса заголовка сущности. |
| `src/ui/components/entity/EntityHeader.cpp` | Реализация заголовка сущности с названием, описанием, status widget, metadata и action widgets. |
| `src/ui/components/entity/EntityDetails.h` | Объявление базового целостного UI-класса подробного представления сущности; предоставляет общие UI-механизмы и опциональные регионы наследникам. |
| `src/ui/components/entity/EntityDetails.cpp` | Реализация общей структуры `EntityDetails`: navigation, header, состояния, summary/info, actions и content-регионы. Конкретные сущности внутри базы не проверяются. |

### Foundation

```text
src/ui/components/foundation/
└── SurfaceWidget.cpp
└── SurfaceWidget.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/foundation/SurfaceWidget.h` | Общий базовый QWidget для поверхностей с единым layout и контрактом выбора цвета поверхности. |
| `src/ui/components/foundation/SurfaceWidget.cpp` | Общая отрисовка поверхности: фон, рамка, скругление, внутренние отступы и интервалы. |

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
| `src/ui/components/forms/FormField.h` | Универсальная оболочка поля формы: label, description, error и вложенный control. |
| `src/ui/components/forms/FormField.cpp` | Реализация визуальной структуры и состояний FormField. |
| `src/ui/components/forms/EditorDialog.h` | Базовый модальный editor-dialog для общего Save/Cancel-lifecycle и режима `Create/Edit`; предоставляет `isEditMode()` специализированным editor'ам. |
| `src/ui/components/forms/EditorDialog.cpp` | Реализация общего layout, кнопок Save/Cancel, хранения режима и вызова виртуального `save()` перед закрытием диалога. |
| `src/ui/components/forms/ConfirmModal.h` | Самодостаточный модальный компонент подтверждения опасного действия; предоставляет единый публичный механизм `confirm()` с callback после подтверждения. |
| `src/ui/components/forms/ConfirmModal.cpp` | Реализация общего layout, текста подтверждения, кнопок подтверждения/отмены и выполнения переданного callback только после подтверждения. |

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
| `src/ui/components/layout/Panel.h` | Объявление поверхности типа Panel. |
| `src/ui/components/layout/Panel.cpp` | Реализация Panel через `SurfaceWidget` с цветом обычной поверхности. |
| `src/ui/components/layout/Card.h` | Объявление базовой карточки, наследующейся от `SurfaceWidget`. |
| `src/ui/components/layout/Card.cpp` | Реализация Card с elevated-вариантом поверхности. |
| `src/ui/components/layout/Section.h` | Объявление секции с заголовком и областью содержимого. |
| `src/ui/components/layout/Section.cpp` | Реализация Section. |
| `src/ui/components/layout/PageHeader.h` | Объявление заголовка страницы с описанием и action widgets. |
| `src/ui/components/layout/PageHeader.cpp` | Реализация PageHeader. |
| `src/ui/components/layout/Toolbar.h` | Объявление горизонтальной панели инструментов. |
| `src/ui/components/layout/Toolbar.cpp` | Реализация Toolbar и его методов добавления widgets/stretch. |

### Lists

```text
src/ui/components/lists/
├── EntityList.cpp
└── EntityList.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/lists/EntityList.h` | Объявление прокручиваемого списка визуальных элементов. |
| `src/ui/components/lists/EntityList.cpp` | Реализация списка: добавление, очистка элементов и подсчёт текущего количества. |

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
| `src/ui/components/navigation/MenuItem.h` | Объявление отдельного элемента бокового меню. |
| `src/ui/components/navigation/MenuItem.cpp` | Реализация визуального элемента меню и сигнала маршрута/действия. |
| `src/ui/components/navigation/MenuSection.h` | Объявление секции бокового меню. |
| `src/ui/components/navigation/MenuSection.cpp` | Реализация оформления и размещения элементов секции. |

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
| `src/ui/components/states/StateWidget.h` | Общая база текстовых состояний UI. |
| `src/ui/components/states/StateWidget.cpp` | Общая визуальная реализация title/description для state-компонентов. |
| `src/ui/components/states/EmptyState.h` | Пустое состояние, наследующее общую структуру `StateWidget`. |
| `src/ui/components/states/ErrorState.h` | Состояние ошибки с возможностью повторного действия. |
| `src/ui/components/states/ErrorState.cpp` | Реализация retry-кнопки и сигнала `retryRequested`. |
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
| `src/ui/localization/LocalizationService.h` | Интерфейс загрузки локализации, получения текста и смены языка. |
| `src/ui/localization/LocalizationService.cpp` | Загружает TSV из Qt Resource System и строит in-memory каталог переводов. |

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
| `src/ui/navigation/NavigationController.h` | Объявление контроллера навигации между страницами. |
| `src/ui/navigation/NavigationController.cpp` | Хранит зарегистрированные страницы и переключает `QStackedWidget` по route. |

---

## 8.4. Pages — `src/ui/pages`

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
| `src/ui/pages/projects/ProjectsPage.h` | Объявление основной страницы списка проектов. |
| `src/ui/pages/projects/ProjectsPage.cpp` | Загружает Project через `ProjectService`, отображает loading/empty/error состояния и создаёт `ProjectCard`. |
| `src/ui/pages/projects/ProjectCard.h` | Объявление специализированного UI-класса `ProjectCard`, который после архитектурного рефакторинга наследует `EntityCard` и знает конкретную модель Project. |
| `src/ui/pages/projects/ProjectCard.cpp` | Реализация `ProjectCard`: настройка общих UI-механизмов `EntityCard`, отображение данных Project и обработка специфических пользовательских действий. Общий каркас карточки не дублируется. |
| `src/ui/pages/projects/ProjectDetailsPage.h` | Объявление специализированного details-класса Project, наследующего `EntityDetails`; содержит локальное состояние Project и действия просмотра/редактирования/удаления. |
| `src/ui/pages/projects/ProjectDetailsPage.cpp` | Реализация `ProjectDetailsPage`: загрузка Project через `ProjectService`, настройка общих механизмов `EntityDetails`, отображение подходящих Project-регионов и локальное UI-поведение. |
| `src/ui/pages/projects/ProjectEditorDialog.h` | Специализированный editor Project, наследующий `EditorDialog`; содержит поля Project и Main Content для сценария создания и поля Project для редактирования. |
| `src/ui/pages/projects/ProjectEditorDialog.cpp` | Реализация UI и сохранения Project через `ProjectService`; использует общий lifecycle `EditorDialog`. |

Projects UI поддерживает просмотр списка и подробностей, создание Project вместе с обязательным Main Content, редактирование Project и удаление Project.

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
| `src/ui/pages/content/ContentCard.h` | Объявление специализированного UI-класса `ContentCard`, наследующего `EntityCard` и работающего с моделью Content. |
| `src/ui/pages/content/ContentCard.cpp` | Реализация карточки Content: отображает название, описание, роль, тип, приоритет, производственный дедлайн, статус и прогресс через общие механизмы `EntityCard`; передаёт событие открытия Content. |
| `src/ui/pages/content/ContentDetails.h` | Объявление специализированного details-класса Content, наследующего `EntityDetails`; содержит локальное состояние Content и действия просмотра, редактирования и удаления. |
| `src/ui/pages/content/ContentDetails.cpp` | Реализация просмотра Content через `ContentService`, отображение полной модели, редактирование через `ContentEditorDialog` и удаление только Additional Content через `ConfirmModal`. |
| `src/ui/pages/content/ContentEditorDialog.h` | Объявление специализированного editor Content, наследующего `EditorDialog`; используется для создания Additional Content и редактирования существующего Content. |
| `src/ui/pages/content/ContentEditorDialog.cpp` | Реализация формы Content и сохранения через `ContentService`; содержит только редактируемые пользователем поля. |

Content UI поддерживает просмотр Main/Additional Content внутри Project, создание Additional Content, редактирование Content и удаление только Additional Content.
 
## 8.4.2. Tasks

```text
src/ui/pages/tasks/
├── TaskCard.cpp
├── TaskCard.h
├── TasksPage.cpp
└── TasksPage.h
```

| Файл | Назначение |
|---|---|
| `src/ui/pages/tasks/TaskCard.h` | Объявление специализированного UI-класса `TaskCard`, наследующего `EntityCard` и работающего с моделью Task. |
| `src/ui/pages/tasks/TaskCard.cpp` | Реализация карточки Task: название, описание, статус, дедлайн и информация о родительской Task через общие механизмы `EntityCard`. |
| `src/ui/pages/tasks/TasksPage.h` | Объявление целостной страницы списка задач, использующей `TaskService`, `EntityList` и общие loading/empty/error states. |
| `src/ui/pages/tasks/TasksPage.cpp` | Реализация загрузки списка Task через `TaskService` и отображение `TaskCard` в единой странице Tasks. |

Tasks UI на текущей контрольной точке поддерживает просмотр общего списка задач. Создание, подробности и workflow Task относятся к последующим подэтапам.

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
| `src/ui/shell/AppShell.h` | Объявление основного layout-контейнера приложения. |
| `src/ui/shell/AppShell.cpp` | Собирает Sidebar + Workspace, подключает LocalizationService, NavigationController и прикладные страницы. |
| `src/ui/shell/Sidebar.h` | Объявление боковой панели навигации. |
| `src/ui/shell/Sidebar.cpp` | Реализация Sidebar, добавление секций/элементов и управление активным route. |
| `src/ui/shell/Workspace.h` | Объявление контейнера страниц на базе `QStackedWidget`. |
| `src/ui/shell/Workspace.cpp` | Реализация Workspace. |
| `src/ui/shell/MenuComposer.h` | Объявление компоновщика основного меню. |
| `src/ui/shell/MenuComposer.cpp` | Собирает Sidebar из MenuSection/MenuItem и передаёт локализованные подписи, иконки и routes. |

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
| `src/ui/style/CreatorStyle.h` | Объявление глобального Qt style на базе `QProxyStyle`. |
| `src/ui/style/CreatorStyle.cpp` | Настройка базовой Qt палитры под визуальную систему CreatorOS. |
| `src/ui/style/Icons.h` | Единый интерфейс получения иконок. |
| `src/ui/style/Icons.cpp` | Текущая реализация доступа к иконкам; временно использует стандартные Qt-иконки там, где собственные ресурсы ещё не подключены. |

---

# 9. Архитектурная карта UI-классов

Физические каталоги помогают находить код, но не задают правило дробления UI. Один целостный UI-сценарий может находиться в одном классе `.h + .cpp`.

Целевая модель:

~~~text
целостная UI-сущность
        ↓
наследование
        ↓
специализированная UI-сущность
        +
composition внутри неё для сложных независимых объектов
~~~

Для entity UI:

~~~text
SurfaceWidget
└── Card
    └── EntityCard
        ├── ProjectCard
        ├── ContentCard
        └── TaskCard

EntityDetails
├── ProjectDetailsPage
├── ContentDetails
└── TaskDetails
~~~

Базовый класс знает общие UI-механизмы. Наследник знает конкретную сущность и решает, какие из доступных механизмов использовать, заполнить или скрыть.

База не выполняет проверки вида `if (Project) ... else if (Task) ...`.

### Правило самостоятельного компонента

Компонент появляется только тогда, когда часть интерфейса действительно стала отдельным объектом: имеет собственную ответственность, контракт, сложное состояние/жизненный цикл или реальное повторное использование.

`QLabel`, `QPushButton`, layout и простой участок разметки отдельным архитектурным классом не становятся только потому, что их «можно вынести».

### Правило наследования

Наследование — основной механизм переиспользования общей целостной сущности при реальном `is-a` отношении и общем поведении.

Composition используется внутри сущности для сложных независимых частей и не заменяет наследование как универсальное правило.

### Правило бизнес-логики

Application/domain содержат бизнес-правила, инварианты, вычисления, прикладные операции и жизненный цикл сущностей.

UI содержит отображение, локальное состояние, пользовательское взаимодействие и вызовы application services/use cases.

### Правило пары .h/.cpp

Файлы `Class.h` и `Class.cpp` рассматриваются как одна программная единица. Отдельные `ClassView.*`, `ClassLogic.*`, `ClassLayout.*` без самостоятельной ответственности не создаются.

### Правило актуальности карты

После добавления каждого нового рабочего файла его путь и назначение добавляются в PROGRAM_STRUCTURE. После удаления файла соответствующая запись удаляется. Документ не должен содержать записи о несуществующих рабочих файлах.


## 8.7. Localization Dashboard developer tool — `tools/localization`

Текущий developer-tool слой локализации отделён от runtime-приложения `CreatorOS`.

| Файл | Назначение |
|---|---|
| `tools/localization/LocalizationEntry.h` | Модель одной записи локализации: ID и набор переводов по locale. |
| `tools/localization/LocalizationCatalog.h` | Публичный контракт каталога локализации в памяти. |
| `tools/localization/LocalizationCatalog.cpp` | Добавление, изменение, удаление и переименование записей каталога. |
| `tools/localization/LocalizationTsvStore.h` | Контракт чтения и сохранения каталога в TSV. |
| `tools/localization/LocalizationTsvStore.cpp` | Загрузка TSV с проверкой структуры и безопасное сохранение через `QSaveFile`. |
| `tools/localization/LocalizationUsage.h` | Модели статического использования localization ID и динамической ссылки на localization-вызов. |
| `tools/localization/LocalizationUsageIndex.h` | Контракт индекса использований localization ID и динамических ссылок. |
| `tools/localization/LocalizationUsageIndex.cpp` | Хранит статические использования по ID, места использования и динамические ссылки. |
| `tools/localization/LocalizationSourceScanner.h` | Контракт рекурсивного сканирования исходного дерева. |
| `tools/localization/LocalizationSourceScanner.cpp` | Сканирует `src/`, распознаёт безопасный статический формат localization references и отдельно фиксирует динамические вызовы. |
| `tools/localization/LocalizationDashboardWindow.h` | Контракт главного Qt Widgets-окна Localization Dashboard и его таблицы/поиска. |
| `tools/localization/LocalizationDashboardWindow.cpp` | Реализация Dashboard UI: таблица каталога, поиск и фильтры, usage/problems, редактирование, добавление/удаление записей, сохранение и защита от потери несохранённых изменений. |
| `tools/localization/main.cpp` | Точка входа developer tool: загружает TSV, запускает source scanner и открывает Localization Dashboard. |

