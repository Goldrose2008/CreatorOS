# Технический документ проекта CreatorOS

**Рабочее название:** CreatorOS  
**Версия приложения:** 0.1  
**Версия документа:** 1.5  
**Статус:** Принято новое технологическое решение  
**Тип продукта:** Desktop-приложение  
**Целевые платформы:** Windows / macOS / Linux

## Правила разработки CreatorOS
Основные принципы работы над проектом:
* Все изменения выполняются постепенно и осознанно.
* Перед изменением существующего кода необходимо проверить текущую архитектуру, техническую спецификацию и журнал разработки.
* Полная перезапись файлов без необходимости запрещена. Предпочтительный формат изменений — точечные исправления с указанием места изменения.
* Архитектурные решения сначала обсуждаются, затем реализуются.
* После каждого крупного этапа выполняются проверка работоспособности, commit, push и запись в DEVELOPMENT_LOG.

Архитектурные принципы:
* CreatorOS развивается как desktop application на стеке TypeScript + React + Electron.
* Архитектура должна сохранять возможность дальнейшего расширения до варианта C с подключением native-слоя.
* UI, бизнес-логика и инфраструктура должны быть разделены.
* Компоненты интерфейса не должны напрямую работать с базой данных.
* Доступ к системным возможностям проходит через отдельные инфраструктурные слои.

Принцип работы со стилями:
* Общие стили, дизайн-токены, типографика, темы, состояния и стили UI-компонентов хранятся в `src/styles/` и его подкаталогах.
* CSS-файлы не размещаются рядом с `.tsx`-компонентами или страницами.
* Уникальный стиль конкретного компонента или страницы размещается в `src/styles/` в соответствующем разделе ответственности.
* Не создавать отдельные стили без необходимости.

Общие требования к разработке:
* Сохранять уже принятые удачные решения.
* Не усложнять архитектуру заранее.
* Каждое новое расширение должно учитывать будущие возможности:
  * API-интеграции;
  * публикации на внешние площадки;
  * автоматизацию;
  * командную работу;
  * возможное подключение native-модулей.

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
	32. ООП применяется прежде всего в доменных моделях, сервисах, репозиториях и интеграционных адаптерах там, где классы, интерфейсы и инкапсуляция дают реальное преимущество. React UI строится в основном на функциональных компонентах и композиции, а не на наследовании.
	33. ConfirmModal является единым самодостаточным объектом подтверждения. Приложение использует единый механизм/экземпляр, а публичная точка запуска ConfirmModal.StartEvent(...) принимает заголовок, описание, callback/event и дополнительные параметры при необходимости.
	34. Callback ConfirmModal.StartEvent(...) передаётся лениво, например () => DeleteProject(id), и выполняется только после подтверждения пользователя.
	35. Страницы и сущности не дублируют состояние открытия, закрытия и подтверждения ConfirmModal.
	36. Новая версия использует React + TypeScript как основную UI-модель; CSS Modules не обязательны и применяются только там, где изоляция стилей действительно упрощает поддержку.
	37. Визуальное оформление строится средствами CSS. Все CSS-файлы хранятся только в src/styles/ и его подкаталогах. Общие стили, дизайн-токены, типографика, темы и общие состояния хранятся централизованно; уникальные стили компонентов и страниц размещаются в src/styles/ в соответствующих разделах ответственности.
	38. Общий визуальный стиль EntityCard определяется один раз и не копируется в ProjectCard, ContentCard, TaskCard и другие наследники.
	39. Общий визуальный стиль EntityDetails определяется один раз и не копируется в конкретных Details.
	40. Отдельный стиль конкретной сущности создаётся только при доказанной уникальной визуальной ответственности.
	41. Не создаются отдельные файлы только для механического разделения UI и логики. Выделение выполняется только при наличии самостоятельной ответственности или реального повторного использования.
	42. Не используются чрезмерно абстрактные прослойки. Универсальность должна давать реальное повторное использование.
	43. Конфигурация сущностей не зависит от визуальных классов.
	44. Модели предметной области, сервисы, UI-компоненты и страницы сохраняют направленное разделение ответственности и не создают циклических зависимостей.
	45. Имена классов и файлов должны сразу показывать их роль: <Entity>Card, <Entity>Details, <Entities>Page и аналогичные формы.
	46. После каждого завершённого этапа фактически выполненная работа заносится в docs/DEVELOPMENT_LOG.md; при необходимости обновляется docs/USER_GUIDE.md.
	47. docs/TECHNICAL_SPEC.md является источником постоянных архитектурных правил; docs/DEVELOPMENT_LOG.md — журналом выполненных этапов и перечнем временных решений.
	48. Перед каждым новым архитектурным или кодовым решением необходимо опираться на актуальные docs/TECHNICAL_SPEC.md, docs/DEVELOPMENT_LOG.md и фактическое состояние проекта.
	49. После сообщения пользователя о запушенном коммите и готовности продолжать необходимо проверить последний коммит и актуальное состояние всего проекта в GitHub: дерево файлов, наличие/отсутствие файлов, ключевые связи и соответствие текущей архитектуре.
	50. Основной технологический стек новой версии CreatorOS: TypeScript + React + Electron.
	51. Electron используется как desktop-shell для Windows, macOS и Linux; Node.js-возможности Electron используются в безопасном main/preload-слое для системных функций.
	52. Vite может использоваться как инструмент сборки и разработки React renderer-части, поскольку он уже является частью legacy-структуры; заменять его без практической причины не требуется.
	53. CSS является основным средством визуального оформления интерфейса. Все CSS-файлы хранятся только в src/styles/ и его подкаталогах. Общие стили, дизайн-токены, типографика, цвета темы и общие состояния хранятся централизованно; уникальные стили компонентов и страниц хранятся в src/styles/ в соответствующих разделах ответственности.
	54. На первом этапе реализуется только тёмная тема. Архитектура темы использует семантические CSS-переменные/токены, чтобы светлую тему и переключение между темами можно было добавить позднее без переделки страниц.
	55. Работа с внешними API является фундаментальным архитектурным слоем CreatorOS. Предусматриваются HTTP/REST API, JSON, OAuth 2.0/PKCE, загрузка больших файлов, асинхронные операции, статусы публикаций и получение статистики в пределах возможностей конкретной площадки.
	56. Каждая внешняя площадка подключается через отдельный интеграционный адаптер с единым внутренним контрактом. UI не должен содержать прямые вызовы API YouTube, VK, RuTube, Дзена и других площадок.
	57. Токены OAuth, refresh tokens, ключи и другие секреты не хранятся в renderer и не передаются в него без необходимости. Для защищённого хранения используется системный механизм secure storage выбранного Electron/OS-слоя.
	58. Renderer не получает произвольный доступ к Node.js или файловой системе. Между React UI и системной/API-частью используется ограниченный preload/IPC boundary с явными командами и проверкой входных данных.
	59. SQLite остаётся локальной основной БД MVP. На текущем этапе выбран встроенный модуль node:sqlite в Node.js 24.x, доступ к которому выполняется только через инфраструктурный/репозиторный слой Electron/Node.js; React-компоненты и страницы не работают с БД напрямую.
	60. В первой реализации native-код не является обязательной частью продукта. Архитектурные границы должны позволять в дальнейшем добавить C++ или Rust для тяжёлой обработки видео, изображений, аудио и других ресурсоёмких задач.
	61. Вариант B должен оставаться совместимым с последующим переходом к гибридной архитектуре варианта C без переписывания UI и доменной модели. Native-слой добавляется только при наличии обоснованной задачи и подключается через чёткий bridge/API-контракт.
	62. Ветка legacy-react-tauri сохраняет старую реализацию как исходный материал для миграции и сравнения и не изменяется в рамках перехода на Electron.
	63. Новая main не создаётся заново без необходимости. Основой миграции служит полезная структура legacy-react-tauri: React/TypeScript-код, существующая UI-структура, стили и ассеты проходят аудит; Tauri и всё лишнее удаляются или заменяются на Electron.
	64. AppShell является владельцем глобального layout. Sidebar, Workspace и глобальный header/область окна не должны реализовываться отдельно внутри страниц.
	65. Workspace является промежуточным layout-слоем между AppShell и текущим route. Страницы не дублируют глобальный workspace-layout.
	66. PageLayout является единым шаблоном содержательной страницы и собирается из context navigation?, PageHeader, Toolbar? и Content.
	67. PageHeader является общим компонентом заголовка страницы с необязательными description и actions.
	68. EntityCard и EntityDetails являются композиционными шаблонами, а специализированные представления передают им данные и уникальные части через props/composition.
	69. Все CSS-файлы CreatorOS находятся только в src/styles/ и его подкаталогах. Любые старые CSS-файлы за пределами src/styles считаются миграционным хвостом и должны переноситься при затрагивании соответствующего участка, после чего старое расположение удаляется.

## Технологический стек

- TypeScript + React + Electron
- Vite как текущий инструмент разработки renderer
- SQLite через Electron/Node.js infrastructure
- React Router
- Lucide icons

## 1. Назначение приложения

CreatorOS управляет полным циклом производства контента: планированием, задачами, материалами, публикациями и аналитикой.

## 2. Основная модель предметной области

Центральная сущность — Project.

~~~text
Project
├── main Content
├── additional Content[]
├── Tasks[]
├── Assets / Materials[]
├── Publications[]
└── Analytics
~~~

Один Project = одна производственная единица контента.

## 3. Project

Модель:

~~~text
Project
├── id
├── name
├── description?
├── owner_id?
├── planned_release_at
├── status
├── progress
├── created_at
└── updated_at
~~~

## 4. Content

Модель:

~~~text
Content
├── id
├── project_id
├── content_type_id
├── content_role
├── name
├── description?
├── planned_release_at
├── priority
├── production_deadline_at?
├── status
├── progress
├── created_at
└── updated_at
~~~

Роли:
- main
- additional

## 5. ContentType

Модель:

~~~text
ContentType
├── id
├── name
├── description?
├── created_at
└── updated_at
~~~

## 6. Task

Модель:

~~~text
Task
├── id
├── parent_id
├── parent_type
├── parent_task_id?
├── title
├── description?
├── status
├── assigned_user_id?
├── start_date?
├── due_date?
├── completed_at?
├── created_at
└── updated_at
~~~

## 7. Publication

Publication относится к Content и содержит платформенные данные. Точные UI-сценарии реализуются после проектирования Platform/Account.

## 8. Platform и Account

Platform и Account являются интеграционными сущностями. UI не знает конкретный API площадки.

## 9. EntityCard

EntityCard — общий визуальный каркас карточки сущности. Специализированные карточки используют его через composition.

## 10. EntityDetails

EntityDetails — общий каркас подробного представления.

## 11. ConfirmModal

ConfirmModal — единый механизм подтверждения действий.

## 12. Pages

Основные страницы:
- Dashboard
- Projects
- ProjectDetails
- ContentDetails
- Tasks
- Planning
- Library
- Analytics
- Settings

## 13. Навигация

Основная навигация:
- Главная
- Проекты
- Задачи
- Планирование
- Библиотека
- Аналитика
- Настройки

## 14. Settings

Категории:
- Общие
- Внешний вид
- Горячие клавиши
- Проекты
- Типы контента
- Автоматизация
- Площадки
- Команда
- AI

## 15. Архитектура приложения

~~~text
React UI
    ↓
Application Service / State
    ↓
Repository / Adapter
    ↓
SQLite / External API
~~~

Electron boundary:

~~~text
React renderer
    ↓
preload / IPC
    ↓
Electron main
    ↓
system / database / integrations
~~~

## 16. ООП-архитектура CreatorOS

ООП применяется там, где есть самостоятельные доменные сущности, сервисы, репозитории или интеграционные адаптеры. React UI преимущественно функциональный и композиционный.

## 17. Стили

Все CSS-файлы находятся только в `src/styles/` и его подкаталогах. Дизайн-токены и темы централизованы.

На первом этапе используется только тёмная тема; переход к светлой теме не должен требовать переписывания страниц.

## 18. База данных

SQLite — основная локальная БД MVP. Renderer не работает с ней напрямую.

## 19. Минимальная версия приложения

MVP включает:
- Project
- Content
- ContentType
- Task
- базовую Library
- базовую Planning
- Dashboard
- Settings
- SQLite
- Electron shell

## 20. Кроссплатформенность

Целевые платформы: Windows, macOS, Linux.

## 21. Лицензирование

Используются только компоненты и ассеты с допустимыми лицензиями. Лицензионные ограничения внешних библиотек и API учитываются при подключении.

## 22. Миграция с legacy-react-tauri

Legacy-ветка сохраняется как контрольная точка. Main развивается на TypeScript + React + Electron.

## 23. Этапы развития

Разработка идёт по слоям:
1. UI primitives
2. Common UI
3. Layout
4. Entity framework
5. Project
6. Content
7. Task
8. Planning / Library / Dashboard
9. Publication / Platform / Account / Analytics

## 24. Главный принцип проекта

CreatorOS строится как конструктор независимых блоков.

## 25. Миграционная стратегия и сохранение legacy

Полезная React/TypeScript/Vite-часть legacy сохраняется; Tauri-specific инфраструктура заменена Electron-слоем.

## 26. Правило имени приложения в USER_GUIDE

В пользовательской документации используется CreatorOS.

## 27. Структура корня исходного проекта

Корень содержит renderer, electron, docs, scripts, package.json и конфигурационные файлы сборки.

## 28. Архитектура UI-конструктора CreatorOS

### 28.1 Цель

Интерфейс CreatorOS строится как конструктор из независимых переиспользуемых компонентов. Компонент более высокого уровня собирается из компонентов более низкого уровня через композицию.

Основная цель:

~~~text
один общий визуальный/функциональный механизм
                ↓
        одна реализация
                ↓
    много специализированных использований
~~~

Изменение общего компонента должно автоматически отражаться во всех его использованиях. Специализированные компоненты не копируют общий JSX и CSS.

React UI не использует классическое наследование как основной механизм повторного использования. Наследование и ООП применяются преимущественно в domain/service/repository/integration слоях.

### 28.2 Уровни конструктора

~~~text
Уровень 0 — UI primitives
        ↓
Уровень 1 — Common UI components
        ↓
Уровень 2 — Layout / page patterns
        ↓
Уровень 3 — Entity components
        ↓
Pages — сборка пользовательских сценариев
~~~

Параллельно существует отдельный поток данных и бизнес-логики:

~~~text
Page / Component
        ↓
Application Service
        ↓
Repository / Adapter
        ↓
SQLite / External API
~~~

UI-компонент не должен напрямую зависеть от SQLite, файловой системы, OAuth или внешнего API.

### 28.3 Уровень 0 — UI primitives

Минимальные универсальные элементы:

| Компонент | Ответственность | Данные/props | Состояния |
|---|---|---|---|
| Button | действие пользователя | label/children, variant, disabled, type, icon | normal, hover, disabled, focus |
| Badge | короткая метка/статус | label/children, tone | neutral, accent, success, warning, danger |
| ProgressBar | визуализация прогресса | value, showValue | 0–100 |
| Input | ввод короткого значения | value, placeholder, disabled, onChange | normal, focus, error, disabled |
| Textarea | многострочный ввод | value, rows, placeholder, disabled, onChange | normal, focus, error, disabled |
| Select | выбор из вариантов | value, children/options, disabled, onChange | normal, focus, error, disabled |
| Icon | отдельного универсального wrapper сейчас не создавать | — | Lucide используется напрямую |

Input, Textarea и Select выделяются в отдельные примитивы только при наличии реального повторного использования. После начала шага 1 повторное использование подтверждено EntityForm, поэтому эти три примитива теперь реализуются и используются в форме.

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

#### Card

~~~text
Card
├── Header?
├── Content
└── Footer / Actions?
~~~

Все области, кроме основного содержимого, опциональны. Card не знает о Project, Content или Task.

#### Section

~~~text
Section
├── Header
│   ├── title
│   ├── description?
│   └── actions?
└── Content
~~~

#### PageHeader

~~~text
PageHeader
├── title
├── description?
└── actions?
~~~

#### Toolbar

~~~text
Toolbar
├── search?
├── filters?
├── sorting?
└── actions?
~~~

#### EntityList

~~~text
EntityList<T>
├── loading?
├── error?
├── empty?
└── items
      └── renderItem(item)
~~~

EntityList не знает, является ли item Project, Content или Task.

#### Modal

~~~text
Modal
├── Header
├── Close
└── Content
~~~

#### ConfirmModal

~~~text
ConfirmModal
└── Modal
    ├── message
    └── actions
~~~

Внутри не хранится логика удаления конкретной сущности.

#### EmptyState, LoadingState, ErrorState

Это независимые состояния, которые могут использоваться в любом списке, Section или Page. Удаление одного из них не должно ломать сам список.

### 28.5 Уровень 2 — layout и шаблоны

#### AppShell

~~~text
AppShell
├── Sidebar
├── Header?
└── Workspace
      └── Current Route
~~~

Страницы не знают, где физически расположен Sidebar.

#### Workspace

Рабочая область между глобальным layout и конкретной страницей. Она отвечает за техническое размещение страницы и контекстную навигацию, но не за данные Project/Content.

#### PageLayout

~~~text
PageLayout
├── context navigation?
├── PageHeader
├── Toolbar?
└── Content
~~~

### 28.6 Уровень 3 — общие сущностные компоненты

#### EntityHeader

~~~text
EntityHeader
├── title
├── description?
├── status?
├── meta?
└── actions?
~~~

#### EntityCard

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

Все секции, кроме основной идентификации, по возможности опциональны.

#### EntityDetails

~~~text
EntityDetails
├── context navigation?
├── EntityHeader
├── error?
├── summary / info?
├── sections[]
└── modals / overlays?
~~~

#### EntityActions

Набор действий конкретной сущности. Отдельный компонент создаётся только при наличии самостоятельной повторяемой ответственности.

### 28.7 Project — целевая структура

~~~text
Project
├── id
├── name
├── description?
├── owner_id?
├── planned_release_at
├── status
├── progress
├── created_at
└── updated_at
~~~

Статусы:
~~~text
draft → Черновик
active → В работе
archived → Архив
~~~

Переходы:
~~~text
draft
└── Взять в работу → active

active
└── Архивировать → archived

archived
└── Восстановить → draft
~~~

#### ProjectsPage

~~~text
ProjectsPage
├── PageLayout
│   ├── PageHeader
│   │   ├── title: "Проекты"
│   │   └── actions
│   │       └── Button: "Создать новый проект"
│   │
│   └── EntityList<Project>
│       ├── LoadingState?
│       ├── EmptyState?
│       └── ProjectCard[]
│
└── Modal / ConfirmModal
    ├── создать проект
    ├── редактировать проект
    └── подтвердить удаление
~~~

#### ProjectCard

~~~text
ProjectCard
└── EntityCard
    ├── title
    │   └── ссылка на ProjectDetails
    ├── description
    ├── status → Badge
    ├── progress → ProgressBar
    ├── meta
    │   └── planned_release_at + Date
    └── actions
        ├── Открыть
        ├── Редактировать
        └── Удалить
~~~

#### ProjectDetails

~~~text
ProjectDetails
└── EntityDetails
    ├── context navigation
    │   └── "Вернуться к проектам"
    ├── EntityHeader
    │   ├── title
    │   ├── description
    │   ├── status → Badge
    │   ├── meta
    │   │   ├── progress
    │   │   ├── planned_release_at
    │   │   ├── owner_id
    │   │   ├── created_at
    │   │   └── updated_at
    │   └── actions
    │       ├── Редактировать
    │       └── Взять в работу / Архивировать / Восстановить
    │
    ├── Section: Основной контент
    │   └── ContentCard
    │
    ├── Section: Дополнительный контент
    │   └── ContentCard[]
    │
    └── Section: Задачи
        └── TaskList / TaskCard[]
~~~

### 28.8 Content — целевая структура

~~~text
Content
├── id
├── project_id
├── content_type_id
├── content_role
├── name
├── description?
├── planned_release_at
├── priority
├── production_deadline_at?
├── status
├── progress
├── created_at
└── updated_at
~~~

Роли:
~~~text
main
additional
~~~

Правило: один Project имеет ровно один main Content.

#### ContentCard

~~~text
ContentCard
└── EntityCard
    ├── title
    │   └── ссылка на ContentDetails
    ├── description
    ├── status/type → Badge
    ├── progress → ProgressBar
    ├── meta
    │   ├── content type
    │   └── planned_release_at
    └── actions
        ├── Редактировать
        └── Удалить? (только для additional)
~~~

#### ContentDetails

~~~text
ContentDetails
└── EntityDetails
    ├── context navigation
    │   └── "Вернуться к проекту"
    ├── EntityHeader
    │   ├── title
    │   ├── description
    │   ├── meta
    │   │   ├── content type
    │   │   ├── role
    │   │   ├── planned_release_at
    │   │   └── progress
    │   └── actions
    │       └── Редактировать
    └── Section: Информация о контенте
        ├── Тип контента
        ├── Роль
        ├── Планируемый выход
        └── Прогресс
~~~

### 28.9 ContentType

~~~text
ContentType
├── id
├── name
├── description?
├── created_at
└── updated_at
~~~

Представление:

~~~text
ContentTypesPage
├── PageLayout
│   ├── PageHeader
│   └── EntityList<ContentType>
└── Modal
    └── EntityForm<ContentType>
~~~

### 28.10 Task — целевая структура MVP

~~~text
Task
├── id
├── parent_id
├── parent_type
├── parent_task_id?
├── title
├── description?
├── status
├── assigned_user_id?
├── start_date?
├── due_date?
├── completed_at?
├── created_at
└── updated_at
~~~

#### TasksPage

~~~text
TasksPage
├── PageLayout
│   ├── PageHeader
│   │   ├── title: "Задачи"
│   │   └── actions
│   │       └── "Создать задачу"
│   ├── Toolbar
│   │   ├── поиск
│   │   ├── фильтры
│   │   └── сортировка
│   └── EntityList<Task>
│       └── TaskCard[]
~~~

#### TaskCard

~~~text
TaskCard
└── EntityCard
    ├── title
    ├── status → Badge
    ├── description?
    ├── meta
    │   ├── parent entity
    │   ├── assigned user?
    │   ├── due date?
    │   └── priority/context (расчётная)
    └── actions
        ├── Открыть
        ├── Редактировать
        └── Удалить
~~~

### 28.11 Asset / Library — целевая структура MVP

~~~text
LibraryPage
├── PageLayout
│   ├── PageHeader
│   │   ├── title: "Библиотека"
│   │   └── actions
│   │       └── "Добавить материал"
│   ├── Toolbar
│   │   ├── поиск
│   │   ├── фильтры
│   │   └── сортировка
│   └── EntityList<Asset>
│       └── AssetCard[]
~~~

Точная модель полей Asset ещё не утверждена.

### 28.12 Planning — целевая структура

~~~text
PlanningPage
├── PageLayout
│   ├── PageHeader
│   ├── Toolbar
│   │   ├── период
│   │   ├── фильтры
│   │   └── переключатель представления
│   └── PlanningView
│       ├── Calendar / Timeline
│       └── PlannedItem[]
~~~

### 28.13 Dashboard — целевая структура

~~~text
DashboardPage
└── PageLayout
    ├── PageHeader
    │   └── title: "Главная"
    └── DashboardGrid
        ├── Section: Сегодня
        │   └── TaskList
        ├── Section: Требует внимания
        │   └── AttentionList
        ├── Section: Активные проекты
        │   └── ProjectSummary[]
        └── Section: Ближайшие публикации
            └── PublicationSummary[]
~~~

### 28.14 Analytics — текущая граница

~~~text
AnalyticsPage
└── PageLayout
    ├── PageHeader
    ├── Toolbar
    └── AnalyticsView
        ├── Summary
        ├── Charts
        └── Details
~~~

### 28.15 Settings — общая структура

~~~text
Settings
└── SettingsLayout
    ├── SettingsSidebar
    └── SettingsWorkspace
         └── SettingsPage
~~~

Категории:
~~~text
Общие
Внешний вид
Горячие клавиши
Проекты
Типы контента
Автоматизация
Площадки
Команда
AI
~~~

### 28.16 Global layout и навигация

~~~text
App
└── AppShell
    ├── Sidebar
    └── Workspace
        └── PageLayout
            └── Current Page
~~~

### 28.17 Правило слабой связанности

~~~text
Page
 ↓
Domain Component
 ↓
Common Component
 ↓
Primitive
~~~

Примеры запрещённых зависимостей:

~~~text
Button ❌→ ProjectCard
Card ❌→ Project
EntityList ❌→ Content
EntityCard ❌→ ProjectService
~~~

Разрешённые связи:

~~~text
ProjectCard → EntityCard
ProjectCard → Project model
ProjectsPage → ProjectService
~~~

### 28.18 Правило опциональных частей

~~~text
EntityCard
├── title                 обязательный
├── description?          optional
├── status?               optional
├── progress?             optional
├── meta?                 optional
├── content?              optional
└── actions?              optional
~~~

Отсутствие progress, status или actions не должно разрушать базовый компонент.

### 28.19 Что считается самостоятельным компонентом

Файл/компонент создаётся только если:
1. он используется в нескольких местах;
2. имеет самостоятельную визуальную ответственность;
3. имеет самостоятельное состояние/поведение;
4. представляет устойчивый архитектурный шаблон.

### 28.20 Что НЕ создаём заранее

~~~text
BaseComponent
BasePage
BaseEntity
UniversalWidget
UniversalIcon
UniversalText
UniversalContainer
~~~

### 28.21 Текущая карта реализации

~~~text
                          App
                           │
                       AppShell
                    ┌──────┴──────┐
                 Sidebar       Workspace
                                 │
                            PageLayout
                                 │
                ┌────────────────┼────────────────┐
                │                │                │
            PageHeader        Toolbar          Content
                                                   │
                     ┌─────────────────────────────┼─────────────┐
                     │                             │             │
                EntityList                     Section       DashboardGrid
                     │                             │
          ┌──────────┼───────────┐                 │
          │          │           │                 │
      ProjectCard ContentCard TaskCard        EntityDetails
          │          │           │                 │
          └──────────┼───────────┘                 │
                     │                        ┌────┴────┐
                 EntityCard              ProjectDetails  ContentDetails
                     │
        ┌────────────┼─────────────┐
        │            │             │
      Badge      ProgressBar    Actions
~~~

Эта схема является целевой, а не требованием немедленно создавать каждый прямоугольник отдельным .tsx-файлом.

### 28.22 Приоритет ближайшей реализации

~~~text
1. UI primitives
   Button / Badge / ProgressBar / Input / Textarea / Select

2. Common UI
   Card / Section / PageHeader / EntityList /
   Modal / EmptyState / FormField

3. Layout
   AppShell / Workspace / PageLayout

4. Entity framework
   EntityHeader / EntityCard / EntityDetails

5. Project
   ProjectCard / ProjectDetails / ProjectsPage

6. Content
   ContentCard / ContentDetails / Content integration

7. Task
   TaskCard / TaskDetails / TasksPage

8. Planning / Library / Dashboard

9. Publication / Platform / Account / Analytics
~~~

### 28.23 Статус реализации UI-конструктора

| Объект | Сейчас | Целевое состояние |
|---|---|---|
| Button | реализован | UI primitive |
| Badge | реализован | UI primitive |
| ProgressBar | реализован | UI primitive |
| Input | новый primitive, реализуется на шаге 1 | UI primitive |
| Textarea | новый primitive, реализуется на шаге 1 | UI primitive |
| Select | новый primitive, реализуется на шаге 1 | UI primitive |
| Card | реализован как простой контейнер | общий Common UI |
| Modal | реализован | общий Common UI |
| ConfirmModal | реализован | общий Common UI |
| EmptyState | реализован | общий Common UI |
| FormField | реализован | общий Common UI |
| EntityForm | реализован | общий Common UI; использует form primitives |
| AppShell | реализован частично | единый глобальный layout |
| Sidebar | реализован | часть AppShell |
| Workspace | реализован частично | часть глобального layout |
| PageLayout | отсутствует | создать |
| PageHeader | отсутствует | создать |
| Toolbar | отсутствует | создать по мере появления сценария |
| EntityList | отсутствует | создать |
| EntityHeader | реализован | общий Entity UI |
| EntityCard | реализован частично | полноценный общий шаблон |
| EntityDetails | отсутствует | создать |
| ProjectCard | реализован | собрать поверх EntityCard |
| ProjectDetails | реализован | собрать поверх EntityDetails |
| ContentCard | реализован | собрать поверх EntityCard |
| ContentDetails | реализован | собрать поверх EntityDetails |
| TaskCard | отсутствует | создать на этапе Tasks |
| TaskDetails | отсутствует | создать на этапе Tasks |
| AssetCard | отсутствует | создать после утверждения модели Asset |
| ProjectSummary | реализован | сохранить до доказанной необходимости EntitySummary |
| DashboardGrid / аналогичный контейнер | отсутствует | создать только после определения реального повторного использования |
| PlanningView | отсутствует | определить на этапе Planning |
| AnalyticsView | отсутствует | определить после проектирования Analytics |
| SettingsLayout | реализован | привести к общему page/layout принципу при необходимости |

### 28.24 Границы текущей реализации

В текущем конструкторе последовательно реализуются:

~~~text
AppShell
Workspace
PageLayout
PageHeader

Button
Badge
ProgressBar
Input
Textarea
Select
Card
Section
Toolbar
EntityList
Modal
ConfirmModal
EmptyState
LoadingState
ErrorState
FormField
EntityForm

EntityHeader
EntityCard
EntityDetails

Project
Content
ContentType
Task

ProjectsPage
ProjectCard
ProjectDetails
ContentCard
ContentDetails
TasksPage

DashboardPage
PlanningPage
LibraryPage
AnalyticsPage
Settings
~~~

Publication, Platform, Account, Team, Series, AI, расширенная Analytics и сложная Automation остаются отдельными последующими этапами.

### 28.25 Принцип изменения визуала

Любое изменение общего визуального шаблона выполняется в базовом компоненте или общем стиле:

~~~text
изменили Button
   ↓
все использующие Button контролы

изменили EntityCard
   ↓
ProjectCard
ContentCard
TaskCard
AssetCard
PublicationCard

изменили PageHeader
   ↓
ProjectsPage
TasksPage
PlanningPage
LibraryPage

изменили AppShell
   ↓
все страницы получают новый глобальный layout
~~~

Специализированная сущность не реализует отдельную копию базовой разметки ради изменения общего визуального шаблона.

### 28.26 Рабочая визуальная схема CreatorOS

Этот раздел является визуальной картой, на которую опирается реализация UI-конструктора. Схемы описывают размещение, состав и основные действия, а не пиксельные размеры.

#### Глобальный экран

~~~text
CreatorOS
└── App
    │
    └── AppShell
        │
        ├── Sidebar
        │   ├── Brand
        │   ├── Главная
        │   ├── Проекты
        │   ├── Задачи
        │   ├── Планирование
        │   ├── Библиотека
        │   ├── Аналитика
        │   └── Настройки
        │
        └── Workspace
            │
            └── PageLayout
                ├── Context Navigation?
                ├── PageHeader
                ├── Toolbar?
                └── Current Page Content
~~~

#### AppShell

~~~text
AppShell
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│ ┌──────────────────┐ ┌─────────────────────────────────────┐ │
│ │     Sidebar      │ │             Workspace               │ │
│ │  • Главная       │ │                                     │ │
│ │  • Проекты       │ │       текущая страница              │ │
│ │  • Задачи        │ │                                     │ │
│ │  • Планирование  │ │                                     │ │
│ │  • Библиотека    │ │                                     │ │
│ │  • Аналитика     │ │                                     │ │
│ │  Система         │ │                                     │ │
│ │  • Настройки     │ │                                     │ │
│ └──────────────────┘ └─────────────────────────────────────┘ │
└──────────────────────────────────────────────────────────────┘
~~~

#### PageLayout

~~~text
PageLayout
┌──────────────────────────────────────────────────────────────┐
│ Context Navigation?                                          │
├──────────────────────────────────────────────────────────────┤
│ PageHeader                                                    │
│ Заголовок страницы                         [ Действия ]       │
│ Описание страницы?                                           │
├──────────────────────────────────────────────────────────────┤
│ Toolbar?                                                      │
├──────────────────────────────────────────────────────────────┤
│ Content                                                       │
└──────────────────────────────────────────────────────────────┘
~~~

#### EntityCard

~~~text
EntityCard
┌──────────────────────────────────────────────────────┐
│  TITLE                                  [STATUS]     │
│  Description                                         │
│  ███████████████████░░░░░░ 72%                     │
│  META1                  META2                        │
├──────────────────────────────────────────────────────┤
│ [Action] [Action]                         [Delete]  │
└──────────────────────────────────────────────────────┘
~~~

#### ProjectsPage

~~~text
ProjectsPage
┌────────────────────────────────────────────────────────────────┐
│  место под контекстную навигацию                                │
├────────────────────────────────────────────────────────────────┤
│  ПРОЕКТЫ                                      [Создать проект]  │
├────────────────────────────────────────────────────────────────┤
│ [Фильтр ▼] [Сортировка ▼] [Поиск.........................]     │
├────────────────────────────────────────────────────────────────┤
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ PROJECT CARD №1                                          │  │
│  │ Название проекта                         [В работе]      │  │
│  │ Описание проекта                                        │  │
│  │ █████████████████░░░░░░░ 68%                             │  │
│  │ 📅 Выход: 12.10.2026                                    │  │
│  ├──────────────────────────────────────────────────────────┤  │
│  │ [Открыть] [Редактировать]                    [Удалить]  │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                │
│  ...                                                           │
└────────────────────────────────────────────────────────────────┘
~~~

#### ProjectDetails

~~~text
ProjectDetails
┌───────────────────────────────────────────────────────────────┐
│ ← Вернуться к проектам                                       │
├───────────────────────────────────────────────────────────────┤
│ Мой проект                                      [Редактировать]│
│ Описание проекта                                               │
│ [В работе]                                                     │
│ Прогресс: 72%                                                  │
│ Планируемая дата выхода: 12.10.2026                            │
│ Ответственный: Пользователь #1                                │
│ Создано: 01.09.2026                                            │
│ Изменено: 28.09.2026                                           │
├───────────────────────────────────────────────────────────────┤
│ Основной контент                                    [Добавить] │
│   ContentCard                                                  │
├───────────────────────────────────────────────────────────────┤
│ Дополнительный контент                             [Добавить] │
│   ContentCard                                                  │
│   ContentCard                                                  │
├───────────────────────────────────────────────────────────────┤
│ Задачи                                                         │
│   [Добавить задачу]                                           │
└───────────────────────────────────────────────────────────────┘
~~~

#### ContentCard

~~~text
ContentCard
┌──────────────────────────────────────────────────────────────┐
│ Название контента                              [Видео]        │
│ Описание                                                      │
│ 📅 Планируемый выход: 12.10.2026                             │
│ Прогресс: 55%                                                 │
│ ███████████████░░░░░░                                        │
├──────────────────────────────────────────────────────────────┤
│                              [Редактировать] [Удалить]         │
└──────────────────────────────────────────────────────────────┘
~~~

Для main Content действие удаления отсутствует; для additional Content оно доступно согласно бизнес-правилам.

#### ContentDetails

~~~text
ContentDetails
┌──────────────────────────────────────────────────────────────┐
│ ← Вернуться к проекту                                        │
├──────────────────────────────────────────────────────────────┤
│ Видео: Обзор нового проекта                    [Редактировать]│
│ Описание                                                      │
│ Тип: Видео                                                    │
│ Роль: Основной контент                                       │
│ Планируемый выход: 12.10.2026                               │
│ Прогресс: 55%                                                 │
├──────────────────────────────────────────────────────────────┤
│ КОНТЕНТ                                                       │
│ Тип контента          Видео                                   │
│ Роль                  Основной контент                        │
│ Планируемый выход     12.10.2026                              │
│ Прогресс              55%                                     │
└──────────────────────────────────────────────────────────────┘
~~~

#### TasksPage

~~~text
TasksPage
┌──────────────────────────────────────────────────────────────┐
│ ЗАДАЧИ                                         [Создать задачу]│
├──────────────────────────────────────────────────────────────┤
│ [Фильтр] [Сортировка] [Поиск.................................]│
├──────────────────────────────────────────────────────────────┤
│ TaskCard...                                                   │
│ TaskCard...                                                   │
└──────────────────────────────────────────────────────────────┘
~~~

#### DashboardPage

~~~text
DashboardPage
┌──────────────────────────────────────────────────────────────┐
│ Главная                                                       │
├──────────────────────────────────────────────────────────────┤
│ ┌──────────────────────┐ ┌───────────────────────────────────┐│
│ │ СЕГОДНЯ              │ │ ТРЕБУЕТ ВНИМАНИЯ                 ││
│ │ TaskList             │ │ AttentionList                    ││
│ └──────────────────────┘ └───────────────────────────────────┘│
│ ┌────────────────────────────────────────────────────────────┐│
│ │ АКТИВНЫЕ ПРОЕКТЫ                            [Все проекты]  ││
│ │ ProjectSummary                                             ││
│ │ ProjectSummary                                             ││
│ └────────────────────────────────────────────────────────────┘│
│ ┌────────────────────────────────────────────────────────────┐│
│ │ БЛИЖАЙШИЕ ПУБЛИКАЦИИ                                       ││
│ │ PublicationSummary — после появления Publication          ││
│ └────────────────────────────────────────────────────────────┘│
└──────────────────────────────────────────────────────────────┘
~~~

### 28.27 Визуальная модель зависимостей

~~~text
App
└── AppShell
    ├── Sidebar
    └── Workspace
        └── PageLayout
            ├── Context Navigation?
            ├── PageHeader
            ├── Toolbar?
            └── Content

Pages
  ↓
Domain Components
  ↓
Common UI
  ↓
UI Primitives
  ↓
CSS / design tokens
~~~

Поток данных остаётся отдельным:

~~~text
UI
 ↓
Application State / Service
 ↓
Repository / Adapter
 ↓
SQLite / External API
~~~

### 28.28 Порядок реализации конструктора

Работа выполняется снизу вверх:

~~~text
ШАГ 1
Button
Badge
ProgressBar
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
Workspace
PageLayout
        ↓
ШАГ 4
EntityHeader
EntityCard
EntityDetails
        ↓
ШАГ 5
ProjectCard
ProjectDetails
ProjectsPage
        ↓
ШАГ 6
ContentCard
ContentDetails
        ↓
ШАГ 7
TaskCard
TaskDetails
TasksPage
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
