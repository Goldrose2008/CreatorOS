# Структура программы CreatorOS

## 1. Назначение документа

Этот документ является картой текущего репозитория CreatorOS.

Для каждого файла указано его назначение. Основная часть документа описывает **актуальную C++/Qt 6 версию приложения**. Каталог `backup/legacy-electron-2026-10-01` описан отдельно: это архив предыдущей React/Electron реализации, который сохраняется только для справки и не является текущей основой программы.

Текущий стек основной версии:

```text
C++20
Qt 6 Widgets
CMake
SQLite
Qt SQL / QSQLITE
```

HTML/CSS/JavaScript, QWebEngineView и QWebChannel остаются только как переходный или специализированный web-слой до его окончательной очистки.

---

# 2. Корень репозитория

| Путь | Назначение |
|---|---|
| `.gitignore` | Список файлов и каталогов, которые Git не должен отслеживать. |
| `.vscode/c_cpp_properties.json` | Настройки IntelliSense и путей заголовков C++/Qt для VS Code. |
| `.vscode/settings.json` | Настройки CMake, генератора Visual Studio, архитектуры x64 и пути к Qt для VS Code. |
| `CMakeLists.txt` | Основное описание сборки CreatorOS: C++ standard, Qt-модули, исходные файлы, ресурсы, библиотеки и post-build deployment. |
| `README.md` | Основная документация репозитория, если файл присутствует в текущей версии. |
| `docs/DEVELOPMENT_LOG.md` | Журнал этапов разработки, контрольных точек, архитектурных изменений и результатов сборки. |
| `docs/TECHNICAL_SPEC.md` | Основная техническая и архитектурная спецификация CreatorOS. |
| `docs/USER_GUIDE.md` | Пользовательская документация приложения. |
| `docs/PROGRAM_STRUCTURE.md` | Карта структуры проекта с описанием назначения каждого файла. |
| `database/` | SQL-структура базы данных и миграции. |
| `resources/` | Qt-ресурсы приложения: локализация, database migrations и переходные web-ресурсы. |
| `src/` | Исходный код актуальной C++/Qt версии. |
| `backup/` | Архив предыдущей React/Electron реализации. |

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
| `src/app/MainWindow.cpp` | Создаёт главное окно, инициализирует БД и Project-слой, создаёт `AppShell` и устанавливает его как центральный виджет. |

## 3.2. Точка входа

| Файл | Назначение |
|---|---|
| `src/main.cpp` | Точка входа приложения: создаёт `QApplication`, подключает `CreatorStyle`, устанавливает имя/версию приложения, создаёт и показывает `MainWindow`. |

---

# 4. Domain — `src/domain`

Доменный слой содержит предметную модель и правила бизнеса. Он не должен зависеть от Qt Widgets, SQLite и конкретного UI.

```text
src/domain/
└── models/
    ├── Project.h
    └── Project.cpp
```

| Файл | Назначение |
|---|---|
| `src/domain/models/Project.h` | Доменная модель проекта: идентификатор, название, описание, владелец, дата выхода, статус, прогресс и даты создания/изменения. |
| `src/domain/models/Project.cpp` | Преобразование статуса Project между типом `ProjectStatus` и строковым представлением, используемым хранилищем. |

---

# 5. Application — `src/application`

Application-слой содержит пользовательские сценарии и связывает UI с доменной моделью и репозиториями.

```text
src/application/
└── projects/
    ├── IProjectRepository.h
    ├── ProjectService.cpp
    └── ProjectService.h
```

| Файл | Назначение |
|---|---|
| `src/application/projects/IProjectRepository.h` | Абстрактный контракт хранилища Project: получение списка, получение по ID, создание, изменение и удаление. |
| `src/application/projects/ProjectService.h` | Публичный контракт application service для сценариев работы с Project. |
| `src/application/projects/ProjectService.cpp` | Реализация сценариев Project, включая нормализацию и базовую проверку входных данных, а также обращение к репозиторию. |

---

# 6. Infrastructure — `src/infrastructure`

Infrastructure содержит конкретные технические реализации хранения и работы с внешней средой.

```text
src/infrastructure/
└── database/
    ├── DatabaseManager.cpp
    ├── DatabaseManager.h
    ├── ProjectRepository.cpp
    └── ProjectRepository.h
```

| Файл | Назначение |
|---|---|
| `src/infrastructure/database/DatabaseManager.h` | Контракт объекта, который открывает SQLite-подключение, предоставляет БД и применяет схему/миграции. |
| `src/infrastructure/database/DatabaseManager.cpp` | Создаёт каталог данных приложения, открывает QSQLITE, включает foreign keys и выполняет database migration. |
| `src/infrastructure/database/ProjectRepository.h` | Объявление SQLite-реализации `IProjectRepository`. |
| `src/infrastructure/database/ProjectRepository.cpp` | SQL-операции с таблицей `projects`: чтение, создание, изменение и удаление проектов. |

---

# 7. Database — `database/`

```text
database/
└── migrations/
    └── 001_initial.sql
```

| Файл | Назначение |
|---|---|
| `database/migrations/001_initial.sql` | Первая версия структуры SQLite: создаёт таблицу `projects` с ограничениями статуса и диапазона прогресса. |

SQL является частью infrastructure/database-механизма, но сами SQL-файлы хранятся отдельно от C++ кода.

---

# 8. UI — `src/ui`

UI полностью реализуется средствами Qt Widgets. Страницы собираются из переиспользуемых компонентов.

```text
src/ui/
├── bridge/
├── components/
├── localization/
├── navigation/
├── pages/
├── shell/
├── style/
└── web/
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
| `src/ui/components/entity/EntityCard.h` | Объявление общего компонента карточки сущности, наследующегося от `Card`. |
| `src/ui/components/entity/EntityCard.cpp` | Реализация карточки сущности: заголовок, описание, статус, дополнительный контент, прогресс, метаданные и действия. |
| `src/ui/components/entity/EntityHeader.h` | Объявление универсального заголовка сущности. |
| `src/ui/components/entity/EntityHeader.cpp` | Реализация заголовка сущности с названием, описанием, status widget, metadata и action widgets. |
| `src/ui/components/entity/EntityDetails.h` | Объявление общего scroll-контейнера для подробных представлений сущностей. |
| `src/ui/components/entity/EntityDetails.cpp` | Реализация структуры подробного представления: navigation, header, error, summary и content. |

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
├── FormField.cpp
└── FormField.h
```

| Файл | Назначение |
|---|---|
| `src/ui/components/forms/FormField.h` | Универсальная оболочка поля формы: label, description, error и вложенный control. |
| `src/ui/components/forms/FormField.cpp` | Реализация визуальной структуры и состояний FormField. |

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
├── LoadingState.cpp
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
| `src/ui/components/states/LoadingState.h` | Объявление отдельного состояния загрузки. |
| `src/ui/components/states/LoadingState.cpp` | Реализация состояния загрузки с текстом. |

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
├── ProjectsPage.cpp
└── ProjectsPage.h
```

| Файл | Назначение |
|---|---|
| `src/ui/pages/projects/ProjectsPage.h` | Объявление основной страницы списка проектов. |
| `src/ui/pages/projects/ProjectsPage.cpp` | Загружает Project через `ProjectService`, отображает loading/empty/error состояния и создаёт `ProjectCard`. |
| `src/ui/pages/projects/ProjectCard.h` | Объявление специализированной карточки Project. |
| `src/ui/pages/projects/ProjectCard.cpp` | Собирает представление Project поверх общего `EntityCard`: название, описание, статус, дату, прогресс и открытие деталей. |
| `src/ui/pages/projects/ProjectDetailsPage.h` | Объявление подробной страницы одного Project. |
| `src/ui/pages/projects/ProjectDetailsPage.cpp` | Получает Project через `ProjectService` и отображает его header, summary и description через общие компоненты. |

На текущем этапе Projects UI поддерживает просмотр списка и подробностей. Создание/редактирование/удаление подключаются после появления необходимого Main Content / ContentType сценария.

---

## 8.5. Shell — `src/ui/shell`

```text
src/ui/shell/
├── AppShell.cpp
├── AppShell.h
├── MenuComposer.cpp
├── MenuComposer.h
├── Sidebar.cpp
├── Sidebar.h
├── WebUiHost.cpp
├── WebUiHost.h
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
| `src/ui/shell/WebUiHost.h` | Объявление переходного web-host компонента. |
| `src/ui/shell/WebUiHost.cpp` | Текущая реализация переходного host для старого web UI; предназначен для последующего удаления на Этапе 13. |

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

# 9. Transition Web Layer — `src/ui/web`

Этот каталог содержит переходную web-реализацию. Он **не является основой текущего UI** и предназначен для удаления после завершения native migration, если специализированный web-сценарий не потребует его сохранить.

```text
src/ui/web/
├── index.html
├── i18n/
│   ├── i18n.js
│   └── localization.tsv
├── js/
│   ├── app.js
│   ├── bridge.js
│   ├── components/
│   │   ├── brand.js
│   │   ├── menu-item.js
│   │   └── menu-section.js
│   ├── icons.js
│   └── menu/
│       └── main-menu.js
└── styles/
    ├── base.css
    ├── shell.css
    └── theme.css
```

| Файл | Назначение |
|---|---|
| `src/ui/web/index.html` | Корневой HTML старого web UI. |
| `src/ui/web/i18n/i18n.js` | JavaScript-механизм локализации старого web UI. |
| `src/ui/web/i18n/localization.tsv` | Старая таблица локализации web UI. Не является источником локализации актуальной версии. |
| `src/ui/web/js/app.js` | Основной JavaScript bootstrap старого web интерфейса. |
| `src/ui/web/js/bridge.js` | JavaScript-часть старого bridge между web UI и C++. |
| `src/ui/web/js/components/brand.js` | Старый web-компонент брендинга приложения. |
| `src/ui/web/js/components/menu-item.js` | Старый web-компонент пункта меню. |
| `src/ui/web/js/components/menu-section.js` | Старый web-компонент секции меню. |
| `src/ui/web/js/icons.js` | Старый web-механизм работы с иконками. |
| `src/ui/web/js/menu/main-menu.js` | Сборка основного меню старого web UI. |
| `src/ui/web/styles/base.css` | Базовые CSS-правила старого web UI. |
| `src/ui/web/styles/shell.css` | CSS оформления shell/side navigation старого web UI. |
| `src/ui/web/styles/theme.css` | Цветовая тема и CSS design tokens старого web UI. |

---

# 10. C++ Web Bridge — `src/ui/bridge`

```text
src/ui/bridge/
├── WebBridge.cpp
└── WebBridge.h
```

| Файл | Назначение |
|---|---|
| `src/ui/bridge/WebBridge.h` | Объявление переходного QObject bridge для связи web UI с C++. |
| `src/ui/bridge/WebBridge.cpp` | Реализация переходного web bridge. Предназначен для удаления вместе с web UI на финальном migration этапе, если отдельная web-функция не требует его сохранить. |

---

# 11. Qt Resources — `resources/`

```text
resources/
├── database.qrc
├── localization.qrc
├── localization/
│   └── localization.tsv
└── web.qrc
```

| Файл | Назначение |
|---|---|
| `resources/database.qrc` | Подключает SQL-миграции к Qt Resource System. |
| `resources/localization.qrc` | Подключает основную таблицу локализации к Qt Resource System. |
| `resources/localization/localization.tsv` | Единый исходный файл переводов актуальной C++/Qt версии. |
| `resources/web.qrc` | Подключает ресурсы переходного web UI. Предназначен для удаления на Этапе 13. |

---

# 12. CMake и зависимости

| Файл | Назначение |
|---|---|
| `CMakeLists.txt` | Управляет компиляцией, Qt modules, ресурсами, линковкой и `windeployqt` для Windows. |

Текущие Qt-модули, используемые проектом:

```text
Qt6::Core
Qt6::Gui
Qt6::Widgets
Qt6::Sql
Qt6::Network
Qt6::WebEngineWidgets
Qt6::WebChannel
```

WebEngine/WebChannel остаются в CMake до финальной проверки миграции.

---

# 13. Архив старой реализации

## `backup/legacy-electron-2026-10-01`

Этот каталог **не используется как рабочая основа текущей программы**.

Он хранит предыдущую React/Vite/Electron реализацию для справки при переносе UX, предметных моделей, CSS и сценариев.

## 13.1. Файлы верхнего уровня архива

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/.gitignore` | Git-исключения старого Electron проекта. |
| `backup/legacy-electron-2026-10-01/README.md` | Документация старой реализации. |
| `backup/legacy-electron-2026-10-01/index.html` | HTML entry point старого Vite UI. |
| `backup/legacy-electron-2026-10-01/package.json` | NPM зависимости и scripts старого проекта. |
| `backup/legacy-electron-2026-10-01/package-lock.json` | Зафиксированные версии NPM зависимостей старого проекта. |
| `backup/legacy-electron-2026-10-01/tsconfig.json` | Общая TypeScript конфигурация старого проекта. |
| `backup/legacy-electron-2026-10-01/tsconfig.app.json` | TypeScript-конфигурация приложения старого UI. |
| `backup/legacy-electron-2026-10-01/tsconfig.node.json` | TypeScript-конфигурация Node/Vite части. |
| `backup/legacy-electron-2026-10-01/vite.config.ts` | Конфигурация Vite старого UI. |
| `backup/legacy-electron-2026-10-01/eslint.config.js` | Правила ESLint старого проекта. |

## 13.2. Electron

### `backup/legacy-electron-2026-10-01/electron`

| Файл | Назначение |
|---|---|
| `electron/main.ts` | Точка входа Electron-процесса старой версии. |
| `electron/preload.cts` | Preload-слой Electron для безопасной передачи разрешённых API в renderer. |
| `electron/tsconfig.json` | TypeScript-конфигурация Electron-части. |
| `electron/database/database.ts` | Старый database client для SQLite. |
| `electron/database/schema.sql` | Старая SQL-схема базы данных, использовавшаяся до перехода на C++. |
| `electron/ipc/databaseHandlers.ts` | Старые IPC handlers для database operations. |

### Scripts

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/scripts/dev-electron.mjs` | Скрипт запуска/разработки старого Electron-приложения. |

## 13.3. Старый frontend source

### Общие файлы

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/src/App.tsx` | Корневой React-компонент старого приложения. |
| `backup/legacy-electron-2026-10-01/src/main.tsx` | Точка входа React renderer. |
| `backup/legacy-electron-2026-10-01/src/assets/hero.png` | Растровый визуальный ресурс старого интерфейса. |
| `backup/legacy-electron-2026-10-01/src/assets/react.svg` | Стандартный React asset старого проекта. |
| `backup/legacy-electron-2026-10-01/src/assets/vite.svg` | Стандартный Vite asset старого проекта. |

### Models

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/src/models/Entity.ts` | Базовая TypeScript-модель сущностей старой версии. |
| `backup/legacy-electron-2026-10-01/src/models/Project.ts` | Старая модель Project. |
| `backup/legacy-electron-2026-10-01/src/models/Content.ts` | Старая модель Content. |
| `backup/legacy-electron-2026-10-01/src/models/ContentType.ts` | Старая модель ContentType. |
| `backup/legacy-electron-2026-10-01/src/models/Task.ts` | Старая модель Task. |

### Types

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/src/types/electron.d.ts` | TypeScript declarations для Electron bridge API. |
| `backup/legacy-electron-2026-10-01/src/types/form.ts` | Типы конфигурации форм старой версии. |
| `backup/legacy-electron-2026-10-01/src/types/status.ts` | Типы визуальных status tones старого UI. |

### Config

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/src/config/appConfig.ts` | Конфигурация старого приложения. |
| `backup/legacy-electron-2026-10-01/src/config/databaseSchema.ts` | Описание/версия database schema старого приложения. |
| `backup/legacy-electron-2026-10-01/src/config/entities/contentConfig.ts` | Конфигурация UI и полей Content. |
| `backup/legacy-electron-2026-10-01/src/config/entities/projectConfig.ts` | Конфигурация полей, статусов и действий Project. |

### Services

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/src/services/projectService.ts` | Старый frontend service для операций Project. |
| `backup/legacy-electron-2026-10-01/src/services/contentService.ts` | Старый frontend service для операций Content. |
| `backup/legacy-electron-2026-10-01/src/services/contentTypeService.ts` | Старый frontend service для ContentType. |
| `backup/legacy-electron-2026-10-01/src/services/themeService.ts` | Управление темой старого UI. |

### Utils

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/src/utils/date.ts` | Старые функции форматирования и работы с датами. |

---

# 14. Архивные UI components

## `backup/legacy-electron-2026-10-01/src/components/entity`

| Файл | Назначение |
|---|---|
| `EntityCard.tsx` | Старый React-шаблон карточки сущности. |
| `EntityHeader.tsx` | Старый React-шаблон заголовка сущности. |
| `EntityDetails.tsx` | Старый React-шаблон подробного представления сущности. |

## `backup/legacy-electron-2026-10-01/src/components/projects`

| Файл | Назначение |
|---|---|
| `ProjectCard.tsx` | Старое специализированное представление Project в виде карточки. |
| `ProjectSummary.tsx` | Старый компактный summary-блок Project. |

## `backup/legacy-electron-2026-10-01/src/components/content`

| Файл | Назначение |
|---|---|
| `ContentCard.tsx` | Старое специализированное представление Content. |

## `backup/legacy-electron-2026-10-01/src/components/layout`

| Файл | Назначение |
|---|---|
| `AppShell.tsx` | Старый React shell приложения. |
| `Sidebar.tsx` | Старый React sidebar. |
| `Workspace.tsx` | Старый React workspace. |

## `backup/legacy-electron-2026-10-01/src/components/ui/forms`

| Файл | Назначение |
|---|---|
| `EntityForm.tsx` | Универсальная форма сущности старой версии. |
| `FormField.tsx` | Старый универсальный form field. |

## `backup/legacy-electron-2026-10-01/src/components/ui/layout`

| Файл | Назначение |
|---|---|
| `Card.tsx` | Старый базовый Card. |
| `PageHeader.tsx` | Старый заголовок страницы. |
| `PageLayout.tsx` | Старый общий layout страницы. |
| `Section.tsx` | Старый layout section. |
| `Toolbar.tsx` | Старый toolbar. |

## `backup/legacy-electron-2026-10-01/src/components/ui/lists`

| Файл | Назначение |
|---|---|
| `EntityList.tsx` | Старый универсальный список сущностей. |

## `backup/legacy-electron-2026-10-01/src/components/ui/overlays`

| Файл | Назначение |
|---|---|
| `Modal.tsx` | Старый универсальный modal. |
| `ConfirmModal.tsx` | Старый modal подтверждения действия. |

## `backup/legacy-electron-2026-10-01/src/components/ui/primitives`

| Файл | Назначение |
|---|---|
| `Badge.tsx` | Старый badge/status indicator. |
| `Button.tsx` | Старый общий компонент кнопки. |
| `Input.tsx` | Старое текстовое поле. |
| `ProgressBar.tsx` | Старый progress bar. |
| `Select.tsx` | Старый select. |
| `Textarea.tsx` | Старое многострочное текстовое поле. |

## `backup/legacy-electron-2026-10-01/src/components/ui/states`

| Файл | Назначение |
|---|---|
| `EmptyState.tsx` | Старое состояние пустого содержимого. |
| `ErrorState.tsx` | Старое состояние ошибки. |
| `LoadingState.tsx` | Старое состояние загрузки. |

---

# 15. Архивные pages

```text
backup/legacy-electron-2026-10-01/src/pages/
```

| Файл | Назначение |
|---|---|
| `DashboardPage.tsx` | Старый dashboard. |
| `ProjectsPage.tsx` | Старый список проектов и CRUD UI Project. |
| `ProjectDetails.tsx` | Старое подробное представление Project. |
| `ContentDetails.tsx` | Старое подробное представление Content. |
| `TasksPage.tsx` | Старый экран задач. |
| `PlanningPage.tsx` | Старый экран планирования. |
| `LibraryPage.tsx` | Старый экран библиотеки. |
| `AnalyticsPage.tsx` | Старый экран аналитики. |

### Settings

| Файл | Назначение |
|---|---|
| `pages/Settings/SettingsLayout.tsx` | Старый общий layout настроек. |
| `pages/Settings/ContentTypes.tsx` | Старый экран управления ContentType. |
| `pages/Settings/SettingsAppearance.tsx` | Старые настройки внешнего вида. |
| `pages/Settings/SettingsPlaceholder.tsx` | Placeholder для ещё не реализованных настроек. |

---

# 16. Архивные styles

## Общие styles

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/src/styles/common-ui.css` | Общие CSS-компоненты старого интерфейса. |
| `backup/legacy-electron-2026-10-01/src/styles/entities.css` | CSS сущностных компонентов старого UI. |
| `backup/legacy-electron-2026-10-01/src/styles/index.css` | Главный CSS entry point старого UI. |
| `backup/legacy-electron-2026-10-01/src/styles/theme.css` | Общая тема старого интерфейса. |
| `backup/legacy-electron-2026-10-01/src/styles/ui-primitives.css` | CSS примитивов старого интерфейса. |

## Layout styles

| Файл | Назначение |
|---|---|
| `styles/layout/AppShell.module.css` | Стили старого AppShell. |
| `styles/layout/Sidebar.module.css` | Стили старого Sidebar. |
| `styles/layout/Workspace.module.css` | Стили старого Workspace. |
| `styles/layout/page-layout.css` | Общий layout страниц старого UI. |

## Page styles

| Файл | Назначение |
|---|---|
| `styles/pages/ContentDetails.module.css` | Стили старого Content details. |
| `styles/pages/DashboardPage.module.css` | Стили старого dashboard. |
| `styles/pages/ProjectDetails.module.css` | Стили старого Project details. |
| `styles/pages/ProjectsPage.module.css` | Стили старого Projects page. |

## Settings styles

| Файл | Назначение |
|---|---|
| `styles/settings/ContentTypes.module.css` | Стили старого ContentType settings. |
| `styles/settings/Settings.module.css` | Стили старого Settings UI. |
| `styles/settings/SettingsLayout.module.css` | Стили layout старого Settings. |

---

# 17. Архивные public/assets

| Файл | Назначение |
|---|---|
| `backup/legacy-electron-2026-10-01/public/favicon.svg` | Favicon старого web-приложения. |
| `backup/legacy-electron-2026-10-01/public/icons.svg` | Набор SVG-иконок старого UI. |

---

# 18. Архитектурный поток актуальной версии

Основной поток данных:

```text
User
  ↓
Qt Widgets
  ↓
UI Page / UI Component
  ↓
Application Service
  ↓
Repository Interface
  ↓
Infrastructure Repository
  ↓
Qt SQL / SQLite
```

Для Project:

```text
ProjectsPage
    ↓
ProjectService
    ↓
IProjectRepository
    ↓
ProjectRepository
    ↓
DatabaseManager
    ↓
SQLite
```

Навигация:

```text
MenuItem
    ↓ signal(route)
Sidebar
    ↓
NavigationController
    ↓
Workspace / QStackedWidget
    ↓
Current Page
```

---

# 19. Правила чтения этой структуры

### Актуальный код

Рабочая C++/Qt реализация находится в:

```text
src/app
src/domain
src/application
src/infrastructure
src/ui
database
resources
```

### Архив

```text
backup/legacy-electron-2026-10-01
```

не является источником текущей архитектуры. Архивные файлы можно использовать для переноса идей, пользовательских сценариев и ранее проработанной модели данных, но новые зависимости на них не добавляются.

### Документация

Архитектурные решения находятся в:

```text
docs/TECHNICAL_SPEC.md
```

Ход разработки и контрольные точки:

```text
docs/DEVELOPMENT_LOG.md
```

Карта файлов:

```text
docs/PROGRAM_STRUCTURE.md
```

Пользовательская документация:

```text
docs/USER_GUIDE.md
```
