# Структура программы CreatorOS

## 1. Назначение документа

Карта фактических каталогов и файлов репозитория CreatorOS. Для каждой программной единицы указано краткое назначение. Пары заголовочного и исходного файлов C++ относятся к одной программной единице.


---

# 2. Корень репозитория

| Путь | Назначение |
|---|---|
| `.vscode/c_cpp_properties.json` | Настройки IntelliSense и путей заголовков C++/Qt для VS Code. |
| `.vscode/settings.json` | Настройки CMake, генератора Visual Studio, архитектуры x64 и пути к Qt для VS Code. |
| `.gitignore` | Список файлов и каталогов, которые Git не должен отслеживать. |
| `CMakeLists.txt` | Основное описание сборки CreatorOS: C++ standard, Qt-модули, исходные файлы, ресурсы, библиотеки и post-build deployment. |
| `docs/DEVELOPMENT_LOG.md` | Журнал этапов разработки, контрольных точек, архитектурных изменений и результатов сборки. |
| `docs/TECHNICAL_SPEC.md` | Требования к функциям, модели данных, пользовательскому интерфейсу и технологиям CreatorOS. |
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
| `src/app/MainWindow.h + MainWindow.cpp` | Создаёт главное окно, инициализирует БД, собирает Project/Content/Task application-слои, создаёт `AppShell` и устанавливает его как центральный виджет. |

## 3.2. Точка входа

| Файл | Назначение |
|---|---|
| `src/main.cpp` | Точка входа приложения: создаёт `QApplication`, подключает `CreatorStyle`, устанавливает имя/версию приложения, создаёт и показывает главного окна CreatorOS - `MainWindow`. |

---

# 4. Domain — `src/domain`
Модели предметных сущностей и их значения.

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
| `src/domain/models/Project.h + Project.cpp` | Доменная модель проекта: идентификатор, название, описание, владелец, дата выхода, статус, прогресс и даты создания/изменения. |
| `src/domain/models/Content.h + Content.cpp` | Минимальная доменная модель Content: проект-владелец, тип контента, роль main/additional и название. |
| `src/domain/models/ContentType.h` | Минимальная доменная модель справочника ContentType: идентификатор и название типа контента. |
| `src/domain/models/Task.h + Task.cpp` | Доменная модель Task: контекст родителя, иерархия задач, название, описание, дедлайн, статус, приоритет и временные метки. |

---

# 5. Application — `src/application`
Прикладные сервисы и контракты доступа к данным.

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
| `src/application/content/ContentService.h + ContentService.cpp` | Реализация сценариев Content, включая проверки Project/ContentType и запрет удаления Main Content. |
| `src/application/projects/IProjectRepository.h` | Абстрактный контракт хранилища Project: получение списка, получение по ID, создание, изменение и удаление. |
| `src/application/projects/ProjectService.h + ProjectService.cpp` | Реализация сценариев Project, включая нормализацию и базовую проверку входных данных, проверку ContentType и обращение к репозиториям. |
| `src/application/common/StringUtils.h` | Общая прикладная утилита нормализации строк, используемая несколькими application services вместо дублирования одинаковой реализации. |
| `src/application/tasks/ITaskRepository.h` | Application-контракт хранилища Task: список, выборка по родителю/ID, создание, изменение и удаление. |
| `src/application/tasks/TaskService.h + TaskService.cpp` | Реализация сценариев Task с проверкой контекста Project/Content и иерархической связи с родительской Task. |

---

# 6. Infrastructure — `src/infrastructure`
Реализации хранилища и технических интеграций.

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
| `src/infrastructure/database/ContentTypeRepository.h + ContentTypeRepository.cpp` | SQL-чтение справочника `content_types`: получение всех типов и типа по ID. |
| `src/infrastructure/database/ContentRepository.h + ContentRepository.cpp` | SQL-операции с таблицей `contents`: чтение по Project/ID, создание, изменение и удаление Content. |
| `src/infrastructure/database/DatabaseManager.h + DatabaseManager.cpp` | Создаёт каталог данных приложения, открывает QSQLITE, включает foreign keys и выполняет database migration. |
| `src/infrastructure/database/ProjectRepository.h + ProjectRepository.cpp` | SQL-операции с таблицей `projects`: чтение, создание, изменение и удаление проектов. |
| `src/infrastructure/database/TaskRepository.h + TaskRepository.cpp` | SQL-операции с таблицей `tasks`: чтение списка, выборка по контексту родителя/иерархии, создание, изменение и удаление Task. |

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
| `src/ui/components/entity/EntityCard.h + EntityCard.cpp` | Реализация общего каркаса и UI-поведения `EntityCard`: заголовок, описание, статус, прогресс, метаданные, дополнительные области и действия. |
| `src/ui/components/entity/EntityHeader.h + EntityHeader.cpp` | Реализация заголовка сущности с названием, описанием, status widget, metadata и action widgets. |
| `src/ui/components/entity/EntityDetails.h + EntityDetails.cpp` | Реализация общей структуры `EntityDetails`: navigation, header, состояния, summary/info, actions и content-регионы. |

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
| `src/ui/components/forms/EditorDialog.h + EditorDialog.cpp` | Реализация общего layout, кнопок Save/Cancel, хранения режима и вызова виртуального `save()` перед закрытием диалога. |
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

### Lists

```text
src/ui/components/lists/
├── EntityList.cpp
└── EntityList.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/lists/EntityList.h + EntityList.cpp` | Реализация списка: добавление, очистка элементов и подсчёт текущего количества. |

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
| `src/ui/pages/projects/ProjectDetailsPage.h + ProjectDetailsPage.cpp` | `ProjectDetailsPage`: загрузка Project через `ProjectService`, настройка общих механизмов `EntityDetails`, отображение подходящих Project-регионов и локальное UI-поведение. |
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
| `src/ui/pages/content/ContentCard.h + ContentCard.cpp` | Реализация карточки Content: отображает название, описание, роль, тип, приоритет, производственный дедлайн, статус и прогресс через общие механизмы `EntityCard`; передаёт событие открытия Content. |
| `src/ui/pages/content/ContentDetails.h + ContentDetails.cpp` | Реализация просмотра Content через `ContentService`, отображение полной модели, редактирование через `ContentEditorDialog` и удаление только Additional Content через `ConfirmModal`. |
| `src/ui/pages/content/ContentEditorDialog.h + ContentEditorDialog.cpp` | Реализация формы Content и сохранения через `ContentService`; содержит только редактируемые пользователем поля. |



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
| `src/ui/pages/tasks/TaskCard.h + TaskCard.cpp` | Реализация карточки Task: название, описание, статус, дедлайн и информация о родительской Task через общие механизмы `EntityCard`. |
| `src/ui/pages/tasks/TasksPage.h + TasksPage.cpp` | Реализация загрузки списка Task через `TaskService` и отображение `TaskCard` в единой странице Tasks. |

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
| `src/ui/shell/AppShell.h + AppShell.cpp` | Собирает Sidebar + Workspace, подключает LocalizationService, NavigationController и прикладные страницы. |
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
| `tools/localization/LocalizationCatalog.h + LocalizationCatalog.cpp` | Операции с записями каталога и locale: добавление/изменение/удаление/переименование ID, добавление языковой колонки с копированием из исходной locale. |
| `tools/localization/LocalizationTsvStore.h + LocalizationTsvStore.cpp` | Загрузка TSV с проверкой структуры и безопасное сохранение через `QSaveFile`. |
| `tools/localization/LocalizationUsage.h` | Модели статического использования localization ID и динамической ссылки на localization-вызов. |
| `tools/localization/LocalizationUsageIndex.h + LocalizationUsageIndex.cpp` | Хранит статические использования по ID, места использования и динамические ссылки. |
| `tools/localization/LocalizationSourceScanner.h + LocalizationSourceScanner.cpp` | Сканирует `src/`, распознаёт безопасный статический формат localization references и отдельно фиксирует динамические вызовы. |
| `tools/localization/LocalizationRefactorService.h + LocalizationRefactorService.cpp` | Строит preview и безопасно применяет Rename к распознанным static references с повторной проверкой, откатом при ошибке и сохранением каталога. |
| `tools/localization/LocalizationValidationIssue.h` | Модель структурированной проблемы Validator: тип, severity, ID, locale и расположение. |
| `tools/localization/LocalizationValidator.h + LocalizationValidator.cpp` | Проверяет формат ID, пропущенные/неиспользуемые ID, пустые переводы, дубли переводов, согласованность placeholders и динамические ссылки. |
| `tools/localization/tests/LocalizationValidatorTests.cpp` | Автоматические регрессионные тесты Validator, TSV persistence и добавления locale: проверка структуры загрузки, Unicode round-trip, стабильной сортировки, копирования значений из исходного языка и отказов без частичных изменений; использует временные файлы, не изменяет рабочий localization.tsv. |
| `tools/localization/LocalizationDashboardWindow.h + LocalizationDashboardWindow.cpp` | Окно каталога локализации: редактирование переводов, проблемы проверки, места использования ID, Rename и экспорт TSV. |
| `tools/localization/main.cpp` | Точка входа developer tool: загружает TSV, запускает source scanner и открывает Localization Dashboard. |
