# Структура программы CreatorOS

## 1. Назначение документа

Этот документ является картой текущего репозитория CreatorOS.

Для каждого файла указано его назначение. Основная часть документа описывает **актуальную C++/Qt 6 версию приложения**. Архитектура ориентирована на целостные C++/Qt-классы: визуальная структура и UI-поведение одной ответственности находятся в одном классе. Разбиение файла на отдельные UI/logic части выполняется только при наличии самостоятельной ответственности, повторного использования или внешней технической границы.

HTML/CSS/JavaScript, QWebEngineView и QWebChannel остаются только как переходный или специализированный web-слой до его окончательной очистки.

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
| `src/app/MainWindow.cpp` | Создаёт главное окно, инициализирует БД и Project-слой, создаёт `AppShell` и устанавливает его как центральный виджет. |

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

---

# 8. UI — `src/ui`
UI полностью реализуется средствами Qt Widgets. Страница является целостным UI-классом и может использовать отдельные переиспользуемые классы только там, где они действительно имеют самостоятельную ответственность, контракт или сложное повторное поведение.

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
| `src/ui/pages/projects/ProjectCard.h` | Объявление специализированного UI-класса `ProjectCard`, который после архитектурного рефакторинга наследует `EntityCard` и знает конкретную модель Project. |
| `src/ui/pages/projects/ProjectCard.cpp` | Реализация `ProjectCard`: настройка общих UI-механизмов `EntityCard`, отображение данных Project и обработка специфических пользовательских действий. Общий каркас карточки не дублируется. |
| `src/ui/pages/projects/ProjectDetailsPage.h` | Объявление специализированного details-класса Project; целевая форма — наследник `EntityDetails`, если текущий navigation/page-контракт сохраняется при рефакторинге. |
| `src/ui/pages/projects/ProjectDetailsPage.cpp` | Реализация `ProjectDetailsPage`: загрузка Project через `ProjectService`, настройка общих механизмов `EntityDetails`, отображение подходящих Project-регионов и локальное UI-поведение. |

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
