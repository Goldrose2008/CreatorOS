# Технический документ проекта CreatorOS

**Рабочее название:** CreatorOS  
**Версия приложения:** 0.1  
**Версия документа:** 1.0 
**Статус:** Архитектура пересмотрена под C++ + Qt 6 + CMake
**Тип продукта:** Desktop-приложение  
**Целевые платформы:** Windows / macOS / Linux

## Архитектурные правила:
	1. CreatorOS — система управления полным циклом создания контента: от идеи до публикации и анализа результата.
	2. Центральная бизнес-сущность CreatorOS — Project (Проект).
	3. Один Project представляет одну единицу производства контента.
	4. Один Project имеет ровно один основной Content.
	5. Project может иметь 0..N дополнительных Content.
	6. Несколько связанных выпусков являются отдельными Projects; Series является отдельной будущей сущностью.
	7. Project объединяет основной и дополнительный Content, Tasks, Assets / Materials, Publications и Analytics.
	8. Связи между сущностями хранятся один раз и не дублируются без необходимости.
	9. Publication относится к Content; отдельный project_id в Publication не требуется.
	10. Account выбирается из подключённых аккаунтов; account_id не вводится пользователем вручную.
	11. Системные идентификаторы, даты создания и технические значения не редактируются пользователем вручную.
	12. Связь Content с Project и роль Content main/additional управляются системой.
	13. Статусы сущностей изменяются через предусмотренные действия и процессы, а не произвольным вводом.
	14. Прогресс Project и Content рассчитывается системой; пользователь не вводит процент вручную.
	15. Приоритет хранится на уровне Content; отдельный приоритет Project не используется.
	16. Приоритет Task в дальнейшем рассчитывается системой на основании контекста, сроков и влияния задачи.
	17. production_deadline_at рассчитывается системой; planned_release_at относится к планируемому выходу основного контента.
	18. User, Team, Platform, Account, Publication, Analytics, Automation, Series и другие сущности имеют отдельные области ответственности.
	19. Доступ к SQLite выполняется только через слой сервисов/репозиториев; UI-компоненты и страницы не работают с БД напрямую.
	20. Страницы являются точками сборки сценариев и связей между компонентами; логика конкретной сущности не должна без необходимости размазываться по страницам.
	21. Компонент CreatorOS является законченной функциональной единицей и может содержать собственный UI, состояние и связанную с ним логику.
	22. Универсальные компоненты являются переиспользуемыми строительными блоками и не содержат бизнес-логики конкретной сущности.
	23. Для повторяющихся UI-механизмов и функциональных механизмов применяется повторное использование через общие компоненты, сервисы, утилиты и хуки; одинаковая логика не копируется в разных местах.
	24. Общие сущностные UI-компоненты реализуются через композицию. EntityCard является переиспользуемым компонентом; ProjectCard, ContentCard, TaskCard, AssetCard, PublicationCard и другие специализированные карточки используют его общий шаблон через props и composition.
	25. EntityCard содержит общий визуальный шаблон, общие свойства, общие действия и правила поведения, не зависящие от конкретной сущности.
	26. Специализированная карточка передаёт в общий компонент данные и свои действия и добавляет только действительно уникальное представление или поведение.
	27. Общие функции UI, не являющиеся бизнес-логикой, выносятся в переиспользуемые утилиты или хуки. Например форматирование даты реализуется один раз и не дублируется в каждой странице.
	28. Конкретный UI-компонент содержит только относящиеся к нему данные представления и UI-логику; бизнес-логика по возможности находится в сервисном слое.
	29. Для подробных представлений используется общий компонент EntityDetails, переиспользуемый для ProjectDetails, ContentDetails, TaskDetails, AssetDetails, PublicationDetails и других представлений.
	30. EntityDetails содержит общий визуальный шаблон, общие свойства, общие действия и точки расширения через props/composition.
	31. Конкретное подробное представление передаёт в EntityDetails данные и уникальные элементы интерфейса и не дублирует общий шаблон.
	32. ООП применяется прежде всего в доменных моделях, сервисах, репозиториях и интеграционных адаптерах там, где классы, интерфейсы и инкапсуляция дают реальное преимущество. 
	33. ConfirmModal является единым самодостаточным объектом подтверждения. Приложение использует единый механизм/экземпляр, а публичная точка запуска ConfirmModal.StartEvent(...) принимает заголовок, описание, callback/event и дополнительные параметры при необходимости.
	34. Callback ConfirmModal.StartEvent(...) передаётся лениво, например () => DeleteProject(id), и выполняется только после подтверждения пользователя.
	35. Страницы и сущности не дублируют состояние открытия, закрытия и подтверждения ConfirmModal.
	36. Новая версия использует C++ и Qt 6 как основу приложения; HTML/CSS и минимальный JavaScript используются как web-слой пользовательского интерфейса.
	37. Основной desktop-shell реализуется на Qt Widgets. Корневое окно является объектом C++/Qt и не зависит от web-фреймворков.
	38. Для рендеринга HTML/CSS в desktop-приложении используется Qt WebEngine Widgets с QWebEngineView как текущий базовый механизм web-интерфейса. Qt WebView не фиксируется как основной renderer, поскольку его C++ API в актуальной ветке Qt 6 находится в Technology Preview.
	39. Связь C++ и web-интерфейса выполняется через ограниченный Qt WebChannel bridge. JavaScript вызывает только явно разрешённые прикладные команды и не получает прямой доступ к БД, файловой системе, секретам или HTTP-клиентам.
	40. HTML, CSS и JavaScript не содержат бизнес-правил. Бизнес-логика, доменные правила, состояния процессов и работа с внешними системами находятся в C++-слое.
	41. HTML/CSS является основным средством визуального оформления web-интерфейса. JavaScript используется минимально — для событий интерфейса, локального состояния представления, вызова bridge и обновления DOM.
	42. Web UI не использует React, Vue, Angular, Electron, Node.js runtime или другой обязательный UI-фреймворк. Предпочтение отдаётся обычным HTML, CSS и ES-модулям JavaScript без лишнего build-time стека.
	43. CSS-файлы web-интерфейса хранятся централизованно в каталоге UI web-слоя; дизайн-токены, тема, типографика и общие состояния определяются единым набором CSS-переменных.
	44. Общая визуальная структура EntityCard, EntityDetails, PageLayout и других повторяемых элементов реализуется один раз средствами HTML/CSS/JS и повторно используется через шаблоны/функции представления; общий markup не копируется без необходимости.
	45. Имена C++ классов и файлов должны сразу показывать их роль: <Entity>Service, <Entity>Repository, <Entity>Model, <Entity>Adapter, MainWindow, WebUiHost, WebBridge и аналогичные формы.
	46. После каждого завершённого этапа фактически выполненная работа заносится в docs/DEVELOPMENT_LOG.md; при необходимости обновляется docs/USER_GUIDE.md.
	47. docs/TECHNICAL_SPEC.md является источником постоянных архитектурных правил; docs/DEVELOPMENT_LOG.md — журналом выполненных этапов и перечнем временных решений.
	48. Перед каждым новым архитектурным или кодовым решением необходимо опираться на актуальные docs/TECHNICAL_SPEC.md, docs/DEVELOPMENT_LOG.md и фактическое состояние проекта.
	49. После сообщения пользователя о запушенном коммите и готовности продолжать необходимо проверить последний коммит и актуальное состояние всего проекта в GitHub: дерево файлов, наличие/отсутствие файлов, ключевые связи и соответствие текущей архитектуре.
	50. Основной технологический стек новой версии CreatorOS: C++ + Qt 6 + CMake + SQLite + HTML/CSS + минимальный JavaScript.
	51. CMake является единственной основной системой сборки проекта. qmake не используется как архитектурный инструмент проекта.
	52. Стандарт языка C++ фиксируется на современном стандарте, поддерживаемом выбранной версией Qt 6; целевой базовый стандарт проекта — C++20.
	53. Qt-модули выбираются по принципу минимально необходимого набора. Базовый набор: Qt Core, Gui, Widgets, Sql, Network, WebEngineWidgets и WebChannel; дополнительные модули добавляются только при наличии конкретной функции и после проверки лицензии.
	54. Для распространения CreatorOS под LGPL используются только совместимые open-source варианты Qt-модулей. Qt-модули, доступные open-source пользователям только под GPL, не включаются в proprietary-сборку CreatorOS без отдельного лицензионного решения.
	55. Qt распространяется динамически, отдельными библиотеками, чтобы приложение не было неразрывно связано со встроенной копией LGPL-библиотек и чтобы выполнить требования LGPL по возможности замены/relinking.
	56. При распространении Qt-компонентов CreatorOS предоставляет копию текста соответствующей лицензии, явное уведомление об использовании Qt и информацию об исходном коде используемых LGPL-компонентов и связанных с ними лицензионных обязательствах.
	57. Если исходный код Qt-компонента изменяется нами, такие изменения не считаются закрытой частью CreatorOS и распространяются в соответствии с лицензией соответствующего Qt-компонента.
	58. Qt WebEngine допускается в проекте, но рассматривается как компонент с повышенными лицензионными и дистрибуционными требованиями из-за Chromium и большого набора third-party компонентов. Для каждой реально поставляемой сборки ведётся список используемых уведомлений/лицензий.
	59. Qt WebView не используется как обязательная основа web UI до тех пор, пока требуемый C++ API не выйдет из Technology Preview и не будет отдельно проверен на стабильность и совместимость с целевыми платформами.
	60. Qt Network Authorization не используется в LGPL-only варианте проекта без отдельного лицензионного решения: OAuth 2.0/PKCE реализуется через разрешённые Qt Network API, системный браузер и собственный слой авторизации либо через отдельно проверенную permissive/LGPL-совместимую библиотеку.
	61. SQLite является локальной основной БД MVP. Доступ к SQLite выполняется из C++ infrastructure-слоя через Qt SQL (QSQLITE) и репозитории; web UI и presentation-слой SQL не выполняют.
	62. Доступ к данным строится через application services/use cases и repositories. UI не знает структуру таблиц и не содержит SQL-запросов.
	63. Внешние площадки подключаются отдельными C++ интеграционными адаптерами. Для HTTP/REST используется Qt Network; UI не вызывает API площадок напрямую.
	64. Секреты, OAuth-токены, refresh tokens и ключи не передаются в JavaScript без крайней необходимости. Для защищённого хранения используется отдельный security adapter с использованием системного защищённого хранилища ОС.
	65. Native C++ больше не является будущим дополнением: он является основой текущей архитектуры CreatorOS. Отдельный native bridge допускается только для внешних или платформозависимых компонентов, а не как обязательная прослойка между C++-ядром и UI.
	68. Архитектурная граница между UI и C++ выполняется через WebChannel bridge с минимальным публичным контрактом. Изменение конкретной web-реализации UI не должно требовать изменения доменной модели и репозиториев.
	69. Отсутствие файла в GitHub Code Search не является доказательством отсутствия файла в репозитории. При работе с проектом сначала проверяются дерево репозитория/каталоги, затем файл при необходимости запрашивается напрямую по известному пути.
	70. Все новые зависимости до добавления в проект проходят проверку лицензии, необходимости, кроссплатформенности и способа распространения. Для релиза поддерживается список third-party components и их лицензий.


## Технологический стек

### Основной стек

| Слой | Технология | Назначение |
|---|---|---|
| Основной язык | C++20 | Доменная модель, application services, инфраструктура и интеграции |
| Desktop framework | Qt 6 | Окна, GUI, события, сеть, SQL, web-интеграция |
| Desktop UI host | Qt Widgets | Главное окно и размещение web-интерфейса |
| Web UI renderer | Qt WebEngine Widgets | Отображение локального HTML/CSS/JS внутри приложения |
| C++ ↔ Web bridge | Qt WebChannel | Ограниченный вызов C++ API из JavaScript |
| Визуальный слой | HTML + CSS | Структура и оформление интерфейса |
| Скрипты | Минимальный JavaScript | UI-события, DOM, WebChannel и локальное состояние представления |
| Сборка | CMake | Конфигурация и сборка всех target'ов |
| База данных | SQLite через Qt SQL / QSQLITE | Локальное хранилище MVP |
| Сеть | Qt Network | HTTP/REST, загрузки, сетевые операции |
| Контроль версий | Git + GitHub | Исходный код и история изменений |
| Среда разработки | Visual Studio Code или другой редактор | Разработка и отладка |

Конкретная patch-версия Qt 6 фиксируется при настройке среды разработки. Версия выбирается по стабильности, доступности open-source пакетов, совместимости с CMake/MSVC/Clang и потребностям web-рендера.

### Принцип технологического стека

Целевая архитектура:

~~~text
                    CreatorOS
                       │
              ┌────────┴────────┐
              │                 │
         Qt Desktop        C++ Application Core
         Shell/Host          / Domain Layer
              │                 │
       QWebEngineView            ├── Services / Use Cases
              │                  ├── Repositories
       HTML + CSS + JS           └── Integrations
              │                         │
         QWebChannel            ┌────────┴─────────┐
              │                 │                  │
              └──────────────► SQLite           APIs
                               (Qt SQL)        (Qt Network)
~~~

Главное правило:

> **Web-интерфейс отвечает за представление. C++ отвечает за данные, правила и действия приложения.**

JavaScript не является вторым backend. Он только связывает DOM с ограниченным WebChannel API.

### Принцип UI и стилей

Весь прикладной интерфейс CreatorOS строится из обычных HTML-элементов и переиспользуемых шаблонов представления:

~~~text
Application Shell
├── Sidebar
├── Workspace
│   └── Current Page
│       ├── PageHeader
│       ├── Toolbar
│       ├── Content
│       └── Overlays
└── Native Qt dialogs / window UI
~~~

Целевая организация web-слоя:

~~~text
src/ui/web/
├── index.html
├── pages/
├── components/
├── styles/
│   ├── theme.css
│   ├── base.css
│   ├── entities.css
│   ├── ui/
│   ├── layout/
│   └── pages/
└── js/
    ├── app.js
    ├── bridge.js
    └── views/
~~~

Web UI не использует React или другой обязательный JavaScript-фреймворк. JavaScript остаётся небольшим, а основная архитектура держится в C++.

### Принцип C++ ↔ Web UI

~~~text
HTML/CSS/JS
     │
     │ QWebChannel
     ▼
WebBridge : QObject
     │
     ▼
Application Services
     │
     ├── Repositories → SQLite
     ├── File Service → File System
     ├── Security Service → Secure Storage
     └── Integration Adapters → External APIs
~~~

В WebBridge публикуются только методы, необходимые интерфейсу: получение данных, запуск разрешённых действий, получение статусов и подписка на события/изменения. Универсальный объект доступа ко всему приложению не создаётся.

### Принцип API и интеграций

Интеграции организуются в C++:

~~~text
src/integrations/
├── youtube/
├── rutube/
├── vk_video/
├── dzen/
└── common/
~~~

Каждый адаптер получает отдельный внутренний контракт и скрывает детали конкретной площадки.

~~~text
authorize()
upload()
create/update metadata
publish/schedule
getStatus()
getPublication()
getStatistics()
disconnect()
~~~

Не все операции обязаны существовать у каждой платформы. Адаптер отражает реальные возможности API.

### Принцип лицензирования Qt

Основной open-source вариант Qt для CreatorOS — LGPL v3 при соблюдении её условий. Это не означает, что весь Qt одинаково лицензирован: актуальная документация Qt указывает отдельные GPL-only модули.

Практические правила CreatorOS:

1. Не использовать GPL-only Qt-модули в базовой proprietary-сборке без отдельного лицензионного решения.
2. Использовать динамическую поставку LGPL-библиотек Qt.
3. Поставлять необходимые license notices и информацию об исходниках LGPL-компонентов.
4. Не изменять Qt без отдельной фиксации исходников и лицензирования изменений.
5. Вести список реально поставляемых third-party компонентов.
6. Отдельно проверять Qt WebEngine из-за лицензий Chromium и других компонентов.

Коммерческий релиз CreatorOS должен иметь экран О программе → Лицензии, где перечислены Qt, SQLite и остальные реально поставляемые сторонние компоненты.

## 1. Назначение приложения

CreatorOS предназначен для управления полным циклом создания контента:

```text
Идея
↓
Проект
↓
Создание контента
↓
Производство
↓
Подготовка публикации
↓
Публикация
↓
Аналитика
```

Главная цель:

> CreatorOS должен брать на себя рутинную организацию процесса и позволять пользователю сосредоточиться на содержательных решениях.

## 2. Основная модель предметной области

```text
Project
├── 1 Main Content
├── 0..N Additional Content
├── Tasks
├── Assets / Materials
├── Publications
└── Analytics
```

## 3. Project

Project является центральной бизнес-сущностью.

Основные данные:

```text
id
name
description
owner_id
planned_release_at
status
progress
created_at
updated_at
```

Project не хранит отдельный приоритет.

## 4. Content

Content является самостоятельной сущностью, находящейся внутри Project.

Основные данные:

```text
id
project_id
content_type_id
content_role
name
description
priority
production_deadline_at
status
progress
created_at
updated_at
```

Роли:

```text
main
additional
```

Для одного Project допускается только один Content с ролью main.

## 5. ContentType

ContentType является справочником типов контента.

Примеры:

```text
Видео
Short
Статья
```

Пользователь может добавлять и изменять типы контента через Settings.

## 6. Task

Task имеет контекст родительской сущности.

Основная модель:

```text
parent_id
parent_type
parent_task_id
```

Это позволяет связывать задачу с Project, Content или другой поддерживаемой сущностью без создания отдельной таблицы связей для каждого типа объекта.

## 7. Publication

Publication относится к конкретному Content и конкретному Account.

Основные направления данных:

```text
content_id
platform_id
account_id
status
scheduled_at
published_at
external_id
external_url
title
description
publication_data
error_message
```

## 8. Platform и Account

Platform описывает площадку публикации.

Account представляет конкретный аккаунт/канал пользователя на этой площадке.

Связь:

```text
Platform
    ↓
Account
    ↓
Publication
    ↓
Content
```

## 9. EntityCard

EntityCard — базовый переиспользуемый HTML/CSS-шаблон карточки сущности.

Он содержит:
- общий визуальный каркас;
- общие области заголовка, описания, статуса, прогресса, метаданных и действий;
- точки расширения через данные и небольшие функции представления.

Принцип использования:

~~~text
EntityCard
   ↓ data/view configuration
ProjectCard
ContentCard
TaskCard
...
~~~

EntityCard не содержит бизнес-логику и не знает о конкретной сущности. Общий HTML/CSS не копируется в специализированных карточках.

## 10. EntityDetails

EntityDetails — базовый переиспользуемый HTML/CSS-шаблон подробного представления сущности.

Он содержит:
- общую структуру Details;
- общие свойства представления;
- стандартные области действий;
- точки расширения через HTML-фрагменты и конфигурацию представления.

Конкретное представление сущности добавляет только уникальные секции и действия. Бизнес-логика находится в C++ application/domain слоях.

## 11. ConfirmModal

ConfirmModal является единым механизмом подтверждения действий в web UI.

Концептуальный поток:

~~~text
C++ Application Service
↓
WebBridge
↓
ConfirmModal
↓
решение пользователя
↓
WebBridge
↓
C++ действие
~~~

JavaScript отвечает только за отображение и передачу результата. Само удаление или изменение сущности выполняется C++ service после проверки правил.

## 12. Pages

Основные страницы:
~~~text
DashboardPage
ProjectsPage
ProjectDetails
ContentDetails
TasksPage
PlanningPage
LibraryPage
AnalyticsPage
Settings
~~~

Page является точкой сборки web-представления пользовательского сценария.

Page не содержит SQL, сетевых вызовов, работы с файлами или бизнес-правил.

## 13. Навигация

Основная навигация:

~~~text
Главная
Проекты
Планирование
Задачи
Библиотека
Аналитика
Настройки
~~~

Навигация выполняется внутри локального web UI. Право на выполнение действия и доступность данных определяются C++ application layer.

## 14. Settings

Предполагаемые категории:

~~~text
Общие
Внешний вид
Проекты
Типы контента
Автоматизация
Интеграции
Команда
AI
~~~

Нереализованные разделы не должны создавать ложного ощущения законченной функциональности. Настройки читаются и изменяются через C++ application services.

## 15. Архитектура приложения

CreatorOS строится как C++ desktop-приложение с разделёнными слоями:

~~~text
Application
│
├── Presentation
│   ├── Qt MainWindow / Widgets shell
│   ├── QWebEngineView
│   ├── WebBridge / QWebChannel
│   └── HTML/CSS/Minimal JS
│
├── Application
│   ├── Services / Use Cases
│   ├── DTO / View data
│   └── orchestration
│
├── Domain
│   ├── Models
│   ├── rules
│   └── domain services
│
├── Infrastructure
│   ├── SQLite / Qt SQL
│   ├── File System
│   ├── Security / Secure Storage
│   └── configuration
│
└── Integrations
    ├── YouTube
    ├── RuTube
    ├── VK Video
    ├── Dzen
    └── ...
~~~

Поток пользовательского сценария:

~~~text
HTML/CSS/JS
   ↓
QWebChannel / WebBridge
   ↓
Application Service / Use Case
   ↓
Domain + Repository / Adapter
   ↓
SQLite / File System / External API
   ↓
result / event
   ↓
WebBridge
   ↓
HTML/CSS/JS
~~~

Qt Widgets используются для desktop-оболочки, системных окон, меню, диалогов и технического размещения web-интерфейса. Основное прикладное содержимое интерфейса рисуется HTML/CSS.

## 16. ООП-архитектура CreatorOS

C++ является естественной ООП-основой CreatorOS.

Используются:
~~~text
class
struct
абстрактные интерфейсы
composition
инкапсуляция
RAII
Qt signals/slots
явные зависимости / dependency injection
~~~

Основные domain-классы не зависят от web UI. Где возможно, domain layer использует стандартную библиотеку C++ и не знает о WebEngine.

Пример:

~~~text
Project Web UI
        ↓
ProjectService
        ↓
IProjectRepository
        ↓
ProjectRepository
        ↓
Qt SQL
~~~

Интерфейсы репозиториев и интеграций вводятся там, где они дают реальную заменяемость и тестируемость. Искусственные абстракции для каждого класса не создаются.

QObject/Q_OBJECT применяется там, где нужны signals/slots, свойства, события или WebChannel. Доменный объект не становится QObject только ради ООП.

## 17. Стили

Визуальное оформление прикладного интерфейса выполняется HTML/CSS.

Целевая организация:

~~~text
src/ui/web/styles/
├── theme.css
├── base.css
├── entities.css
├── ui/
├── layout/
└── pages/
~~~

theme.css содержит семантические CSS-переменные, цвета, типографику, размеры, радиусы, состояния и другие дизайн-токены.

На первом этапе реализуется тёмная тема. Архитектура допускает светлую тему позднее без переписывания HTML.

Qt Style Sheets используются только для ограниченной части native Qt Widgets. Основной прикладной визуальный слой не переводится в QSS.

JavaScript не содержит бизнес-правил и не хранит дублирующую систему визуальных токенов.

## 18. База данных

SQLite является локальной основной БД MVP.

Доступ к БД строится только через C++ infrastructure/repository layer:

~~~text
Web UI
↓
WebBridge
↓
Application Service
↓
Repository
↓
Qt SQL / QSQLITE
↓
SQLite
~~~

UI-компоненты и страницы не работают с БД напрямую. JavaScript не выполняет SQL.

### DatabaseManager

Отдельный инфраструктурный объект отвечает за:
~~~text
открытие подключения
↓
проверку/создание схемы
↓
применение миграций
↓
управление транзакциями
↓
закрытие подключения
~~~

Репозитории отвечают за операции конкретных сущностей:

~~~text
IProjectRepository
IContentRepository
ITaskRepository
...
~~~

SQLite-файл хранится в пользовательском каталоге данных приложения, а не рядом с исполняемым файлом.

### Миграции

Структура БД изменяется последовательными SQL-миграциями. Версия схемы хранится в самой БД.

~~~text
schema v1
↓
migration 2
↓
schema v2
↓
migration 3
↓
...
~~~

Миграции контролируются механизмом версии схемы и выполняются транзакционно, когда это возможно.

## 19. Минимальная версия приложения

Первая рабочая версия должна постепенно включать:

```text
Projects
Main Content
Additional Content
Content Types
Tasks
Library
Planning
Dashboard
Local SQLite
```

Интеграции публикаций, команды, AI, сложная автоматизация и расширенная аналитика добавляются позднее.

## 20. Кроссплатформенность

Целевые desktop-платформы:

~~~text
Windows
macOS
Linux
~~~

Qt используется как основной кроссплатформенный framework.

Platform-specific код допускается только через явно выделенные adapters.

Для каждой ОС отдельно проверяются:

~~~text
window management
file dialogs
secure storage
browser/OAuth callback
font rendering
WebEngine deployment
application data directory
packaging
~~~

Общая предметная модель, application services, repositories и большая часть web UI должны оставаться платформонезависимыми.

## 21. Лицензирование

CreatorOS планируется как proprietary/closed-source приложение, поэтому лицензии сторонних компонентов должны позволять такое распространение.

### Qt

Основной open-source вариант — использование LGPL v3 компонентов Qt при полном соблюдении LGPL.

Qt не является однородно лицензированным пакетом: в актуальном Qt 6.12 часть модулей доступна open-source пользователям только под GPL v3. Эти GPL-only модули не включаются в базовую proprietary-сборку без отдельного лицензионного решения.

Для CreatorOS принимаются следующие практические меры:

~~~text
Qt DLLs
↓
динамическая поставка
↓
license notices
↓
информация об исходниках LGPL-компонентов
↓
возможность замены/relinking согласно LGPL
~~~

Особое внимание:
~~~text
модульный состав Qt
Qt WebEngine и third-party/Chromium лицензии
Qt Network Authorization
изменения исходного кода Qt
~~~

### SQLite

SQLite используется через Qt SQL/QSQLITE. В документации Qt 6.12 SQLite указан как third-party компонент под SQLite Blessing.

### Прочие зависимости

Каждая дополнительная библиотека проверяется до включения в основной target:

~~~text
license
commercial redistribution
source/notice obligations
static/dynamic linking requirements
platform compatibility
~~~

В релизе CreatorOS должен существовать раздел О программе → Лицензии.

Этот раздел является архитектурным правилом проекта, а не индивидуальным юридическим заключением. Перед первым коммерческим релизом итоговый набор распространяемых компонентов и их лицензий проверяется по актуальным лицензионным текстам.


## 23. Этапы развития

### Новый Этап 0 — архитектурная основа C++/Qt

~~~text
legacy audit
↓
подготовка Qt 6 + CMake
↓
структура C++ слоёв
↓
SQLite / DatabaseManager
↓
QWebEngineView
↓
HTML/CSS
↓
QWebChannel bridge
↓
первый рабочий shell
~~~

### Этап 1 — Project
~~~text
Project
↓
ProjectRepository
↓
ProjectService
↓
Project UI
↓
CRUD
~~~

### Этап 2 — Content
~~~text
Content
↓
ContentRepository
↓
ContentService
↓
Content UI
↓
ContentType
~~~

### Этап 3 — Tasks
~~~text
Task
↓
TaskRepository
↓
TaskService
↓
Task UI
↓
Task workflow
~~~

### Этап 4 — Планирование и библиотека

### Этап 5 — Публикации и интеграции

~~~text
Publication
↓
Platform / Account
↓
Integration Adapter
↓
OAuth
↓
Upload / Publish
↓
Status / Analytics
~~~

### Этап 6 — Автоматизация

### Этап 7 — Команды и роли

### Этап 8 — AI и расширенная аналитика

## 24. Главный принцип проекта

CreatorOS строится вокруг процесса производства контента, а не вокруг отдельных экранов.

Главная цепочка:

~~~text
Project
↓
Content
↓
Production
↓
Publication
↓
Analytics
~~~

Программная архитектура:

~~~text
HTML/CSS/JS presentation
↓
WebChannel Bridge
↓
Application Services / Use Cases
↓
Domain
↓
Repository / Integration Adapter
↓
SQLite / File System / Platform API
~~~

Общая UI-функциональность переиспользуется через web-шаблоны, CSS и небольшие функции представления. ООП применяется прежде всего в C++ domain/application/infrastructure/integration слоях.

## 26. Правило имени приложения в USER_GUIDE

В `docs/USER_GUIDE.md` нельзя жёстко записывать отображаемое название приложения.

Для обозначения самого приложения используются:

```text
Приложение
```

или:

```text
APP_NAME
```

Название доменной сущности **Проект** не изменяется и продолжает использоваться как термин предметной модели.


## 27. Структура корня исходного проекта

Целевая структура новой C++/Qt версии:

~~~text
CreatorOS/
├── docs/
│   ├── TECHNICAL_SPEC.md
│   ├── DEVELOPMENT_LOG.md
│   └── USER_GUIDE.md
├── cmake/
│   ├── modules/
│   └── helpers/
├── src/
│   ├── app/
│   ├── domain/
│   │   ├── models/
│   │   ├── rules/
│   │   └── services/
│   ├── application/
│   │   ├── projects/
│   │   ├── content/
│   │   ├── tasks/
│   │   ├── publications/
│   │   └── common/
│   ├── infrastructure/
│   │   ├── database/
│   │   ├── filesystem/
│   │   ├── security/
│   │   └── configuration/
│   ├── integrations/
│   │   ├── youtube/
│   │   ├── rutube/
│   │   ├── vk_video/
│   │   ├── dzen/
│   │   └── common/
│   ├── ui/
│   │   ├── shell/
│   │   ├── bridge/
│   │   └── web/
│   │       ├── pages/
│   │       ├── components/
│   │       ├── styles/
│   │       └── js/
│   └── common/
├── resources/
│   ├── web.qrc
│   ├── icons/
│   └── licenses/
├── database/
│   ├── schema/
│   └── migrations/
├── tests/
│   ├── domain/
│   ├── application/
│   ├── infrastructure/
│   └── integrations/
├── CMakeLists.txt
└── README.md
~~~

Назначение областей:

~~~text
src/app/              запуск приложения и root window
src/domain/           предметная модель и правила
src/application/      пользовательские сценарии / use cases
src/infrastructure/   SQLite, файлы, настройки, security
src/integrations/     внешние площадки
src/ui/shell/         Qt Widgets shell
src/ui/bridge/        C++ WebChannel API
src/ui/web/           HTML/CSS/minimal JS
database/             schema и SQL migrations
resources/licenses/   license notices / third-party information
tests/                автоматические тесты
~~~

Каталоги создаются по мере появления соответствующей ответственности.

## 28. Архитектура UI-конструктора CreatorOS

### 28.1 Цель

Интерфейс CreatorOS строится как конструктор из независимых переиспользуемых web-компонентов представления. Компонент более высокого уровня собирается из HTML-фрагментов, CSS-классов и небольших JS-функций.

### 28.2 Уровни конструктора

~~~text
Уровень 0 — HTML primitives
        ↓
Уровень 1 — Common UI components
        ↓
Уровень 2 — Layout / page patterns
        ↓
Уровень 3 — Entity components
        ↓
Pages — сборка пользовательских сценариев
~~~

Поток данных:

~~~text
Page / Component
        ↓
WebBridge
        ↓
Application Service
        ↓
Repository / Adapter
        ↓
SQLite / External API
~~~

### 28.3 Уровень 0 — UI primitives

~~~text
button
badge
progress
input
textarea
select
icon
link
~~~

Они не содержат бизнес-логики. Семантический HTML и доступность используются там, где это возможно.

### 28.4 Уровень 1 — общие составные UI-компоненты

~~~text
Card
Panel
Section
PageHeader
Toolbar
EntityList
Modal
ConfirmModal
EmptyState
LoadingState
ErrorState
FormField
EntityForm
~~~

Эти элементы являются повторно используемыми HTML-шаблонами/функциями представления. Они не знают о конкретном Project, Content или Task.

### 28.5 Уровень 2 — layout и шаблоны

~~~text
AppShell
├── Sidebar
├── Header?
└── Workspace
      └── Current Page
~~~

AppShell отвечает за глобальный layout, Workspace — за размещение текущей страницы.

### 28.6 Уровень 3 — общие сущностные компоненты

EntityHeader, EntityCard и EntityDetails являются общими механизмами представления.

~~~text
EntityCard
├── Header
│   ├── title
│   └── status?
├── description?
├── content?
├── progress?
├── meta?
└── actions?
~~~

~~~text
EntityDetails
├── context navigation?
├── EntityHeader
├── error?
├── summary / info?
├── sections[]
└── overlays?
~~~

ProjectCard, ContentCard, TaskCard и специализированные Details используют общий шаблон и передают только уникальные данные/секции.

### 28.7 Project — UI

~~~text
ProjectsPage
├── PageHeader
├── Toolbar
├── EntityList<Project>
│   └── ProjectCard[]
└── Modal / ConfirmModal
~~~

ProjectCard показывает название, описание, статус, прогресс, плановую дату и действия.

ProjectDetails использует EntityDetails и показывает основной контент, дополнительный контент и задачи.

### 28.8 Content — UI

~~~text
ContentDetails
├── EntityHeader
├── Content information
├── Production data
├── Publications
└── Tasks
~~~

### 28.9 ContentType

ContentType является справочником и редактируется через Settings. UI вызывает application service через WebBridge.

### 28.10 Task — UI

~~~text
TasksPage
├── PageHeader
├── Toolbar
├── EntityList<Task>
│   └── TaskCard[]
└── TaskDetails / Form
~~~

Приоритет, сроки и статусы отображаются по данным C++-слоя.

### 28.11 Asset / Library — UI

~~~text
LibraryPage
├── Toolbar
├── filters / search
└── AssetCard[]
~~~

Работа с файлами выполняется C++ File Service.

### 28.12 Planning — UI

Planning объединяет Project, Content и Task во временной последовательности. Расчёты сроков выполняются application/domain layer.

### 28.13 Dashboard — UI

~~~text
Dashboard
├── Today
├── Attention
├── Active Projects
└── Upcoming Publications
~~~

Источником данных являются application services.

### 28.14 Analytics

Analytics получает данные от C++ integration/application слоя и не содержит прямых вызовов API площадок.

### 28.15 Settings

Предполагаемые категории:

~~~text
Общие
Внешний вид
Проекты
Типы контента
Автоматизация
Интеграции
Команда
AI
~~~

### 28.16 Global layout и навигация

~~~text
Главная
Проекты
Планирование
Задачи
Библиотека
Аналитика
Настройки
~~~

Текущий route/page выбирается JS-слоем представления, но данные и допустимость операций определяются C++ application layer.

### 28.17 Правило слабой связанности

UI знает только WebBridge contract.

~~~text
HTML/JS
   ↓
WebBridge
   ↓
Application services
~~~

UI не зависит от конкретного SQLite-класса, SQL-запроса или implementation класса интеграции.

### 28.18 Правило опциональных частей

Если блок не имеет данных, он не должен создавать визуальную пустоту без необходимости. Опциональность определяется данными.

### 28.19 Что считается самостоятельным компонентом

Компонент выделяется отдельно только при наличии самостоятельной ответственности, реального повторного использования, собственного поведения представления, независимого тестирования или заметной сложности разметки.

### 28.20 Что не создаём заранее

~~~text
универсальный UI framework
глубокий JS state manager
сложный template engine
отдельные классы C++ для каждого DOM-элемента
универсальный bridge на весь application API
лишние abstraction layers
~~~

### 28.21 C++ API bridge

WebBridge : QObject является контролируемой точкой входа web UI.

~~~text
WebBridge
├── projects()
├── createProject(...)
├── updateProject(...)
├── deleteProject(...)
├── content()
├── tasks()
├── settings()
└── notifications / events
~~~

По мере роста приложения bridge разделяется на небольшие специализированные объекты.

### 28.22 События

Для push-обновлений UI C++ использует Qt signals/slots, которые публикуются в web-слой через WebChannel при необходимости.

~~~text
Service
↓ signal
Bridge
↓ WebChannel
JavaScript event handler
↓
DOM update
~~~

### 28.23 Безопасность web-слоя

Web UI загружает локальные доверенные ресурсы CreatorOS.

Запрещено:

~~~text
произвольно загружать удалённые страницы в основном UI
передавать секреты в JS
исполнять SQL из JS
разрешать произвольный filesystem API
открывать универсальный eval-like command bridge
~~~

Внешние ссылки, OAuth и удалённый контент проходят через явно разрешённые сценарии/отдельные окна или системный браузер.

### 28.24 Работа с HTML/CSS

Рекомендуемая схема:

~~~text
CMake
↓
qt_add_resources
↓
application resource
↓
QWebEngineView
↓
local HTML/CSS/JS
~~~

Web assets не должны зависеть от локального dev server после production-сборки.

### 28.25 Принцип изменения визуала

Изменения выполняются преимущественно в HTML templates, CSS и небольших JS view helpers. Изменение цвета, размера, отступов или сетки не требует изменения C++.

### 28.26 Рабочая визуальная схема CreatorOS

~~~text
MainWindow (Qt)
└── QWebEngineView
    └── AppShell
        ├── Sidebar
        └── Workspace
            └── PageLayout
                ├── Context Navigation?
                ├── PageHeader
                ├── Toolbar?
                └── Content
~~~

Native Qt controls добавляются только там, где они дают явное преимущество: системные file dialogs, native menus, window-level interactions и аналогичные ОС-зависимые сценарии.

### 28.27 Визуальная модель зависимостей

~~~text
Qt MainWindow
      ↓
QWebEngineView
      ↓
HTML/CSS/JS
      ↓
QWebChannel
      ↓
WebBridge
      ↓
Application Services
      ↓
Repository / Adapter
      ↓
SQLite / File System / External API
~~~

### 28.28 Визуальный порядок реализации

~~~text
ШАГ 1
HTML primitives
CSS tokens
Button
Badge
Progress
Input
Textarea
Select
        ↓
ШАГ 2
Card
Section
PageHeader
Toolbar
EntityList
Modal
ConfirmModal
EmptyState
LoadingState
ErrorState
FormField
EntityForm
        ↓
ШАГ 3
AppShell
Sidebar
Workspace
PageLayout
        ↓
ШАГ 4
EntityHeader
EntityCard
EntityDetails
        ↓
ШАГ 5
Project UI + Project C++ layers
        ↓
ШАГ 6
Content UI + Content C++ layers
        ↓
ШАГ 7
Task UI + Task C++ layers
        ↓
ШАГ 8
Planning
Library
Dashboard
        ↓
ШАГ 9
Publication
Platform
Account
Analytics
~~~

Каждый шаг считается завершённым только после проверки сборки/работоспособности, commit, push и записи фактического результата в DEVELOPMENT_LOG.

### 28.29 Организация файлов UI

~~~text
src/ui/
├── shell/
│   ├── MainWindow.*
│   └── WebUiHost.*
├── bridge/
│   ├── WebBridge.*
│   └── events/
└── web/
    ├── index.html
    ├── pages/
    ├── components/
    ├── styles/
    │   ├── theme.css
    │   ├── base.css
    │   ├── entities.css
    │   ├── ui/
    │   ├── layout/
    │   └── pages/
    └── js/
        ├── app.js
        ├── bridge.js
        └── views/
~~~

C++ файлы shell/bridge не содержат CSS. Web UI не содержит SQL.

### 28.30 Глобальный layout

~~~text
MainWindow
└── WebUiHost
    └── QWebEngineView
        └── AppShell
            ├── Sidebar
            └── Workspace
                └── Current Page
                    └── PageLayout
~~~

MainWindow является единственным владельцем root web host. Страницы не создают отдельные desktop windows для обычной навигации.

~~~text
MainWindow
   ↓
WebUiHost
   ↓
QWebEngineView
   ↓
HTML AppShell
~~~
