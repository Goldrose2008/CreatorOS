# Система локализации CreatorOS

**Статус:** рабочая спецификация системы локализации  
**Источник переводов:** `resources/localization/localization.tsv`  
**Сервис:** `src/ui/localization/LocalizationService`

## 1. Основной принцип

Одна и та же локализуемая фраза не должна храниться под несколькими text ID только из-за того, что она используется в разных сущностях или экранах.

Перед добавлением нового ID необходимо:

1. найти в `localization.tsv` уже существующий перевод;
2. проверить, совпадает ли смысл и контекст;
3. при совпадении переиспользовать существующий ID;
4. при необходимости общего текста вынести его в пространство `common.*`;
5. создавать отдельный entity-specific ID только тогда, когда различается смысл, контекст или требуемая формулировка.

Точное совпадение перевода является сильным сигналом к переиспользованию, но не единственным критерием. Нельзя объединять два текста только потому, что английский перевод совпадает, если русская формулировка или UI-контекст различаются.

## 2. Неймспейсы ключей

### `common.*`

Используется для действительно общих текстов:

- действия: `common.action.*`;
- общие статусы: `common.status.*`;
- общие подписи: `common.label.*`;
- названия сущностей: `common.entity.*`;
- общие метаданные: `common.meta.*`.

### `project.*`, `content.*`, `task.*`

Используются для entity-specific текстов, когда формулировка зависит от сущности или её сценария.

### `projects.*`, `tasks.*`

Используются для текстов конкретной страницы/раздела, а не для общих действий и подписей.

## 3. Канонические общие ключи

В результате текущего рефакторинга следующие дубли должны перейти на единые ID:

| Новый ID | Текущие дублирующиеся ID |
|---|---|
| `common.label.priority` | `content.priority`, `content.create.priority` |
| `common.status.draft` | `content.status.draft`, `project.status.draft` |
| `common.status.in_progress` | `content.status.in_progress`, `project.status.active`, `task.status.in_progress` |
| `common.status.archived` | `content.status.archived`, `project.status.archived` |
| `common.action.open` | `content.open`, `project.open` |
| `common.label.summary` | `content.summary`, `project.summary` |
| `common.entity.project` | `content.project`, `project.not_selected` |
| `common.label.description` | `content.description`, `content.create.description`, `project.description`, `project.create.description` |
| `common.entity.content` | `content.not_selected`, `project.content` |
| `common.action.edit` | `content.edit`, `project.edit` |
| `common.action.delete` | `content.delete`, `content.delete.confirm`, `project.delete`, `project.delete.confirm` |
| `common.action.save` | `content.edit.save`, `project.edit.save` |
| `common.action.cancel` | `content.create.cancel`, `content.delete.cancel`, `project.create.cancel`, `project.delete.cancel` |
| `common.action.create` | `content.create.save`, `project.create.save` |
| `common.action.retry` | `projects.retry`, `tasks.retry` |
| `common.meta.created` | `content.created`, `project.created` |
| `common.meta.updated` | `content.updated`, `project.updated` |

For `common.status.in_progress` use the canonical translation:

- RU: `В работе`
- EN: `In progress`

The Project domain may still contain the enum value `Active`; this does not require the localization ID to be named `active`. The UI translation expresses the common user-facing status.

## 4. Project release label

The two current IDs:

- `project.release`
- `project.create.release`

represent the same user-facing label and should use one ID:

`project.release`

Canonical translation:

- RU: `Планируемая дата выхода`
- EN: `Planned release date`

The same key is valid both for Project display and for the Project editor.

## 5. Message about unavailable content types

The following IDs contain the same phrase:

- `content.create.no_types`
- `project.create.no_content_types`

Use one canonical ID:

`content.type.none_available`

Canonical translation:

- RU: `Нет доступных типов контента.`
- EN: `No content types are available.`

The key belongs to the ContentType concept, even when it is displayed from Project creation.

## 6. Что не объединяем

Похожие тексты не должны объединяться автоматически.

### Not found

`content.not_found.*` и `project.not_found.*` отличаются сущностью. Их можно заменить на полностью общий `Not found` только если UI действительно устраивает потеря названия сущности в заголовке.

Для текущего UI предпочтительно сохранить entity-specific title/description.

### Back

`content.back` и `project.back` содержат разную информацию о destination:

- Back to project
- Back to projects

Они не являются дублями. Сохраняем оба.

### Error

`content.error.*` и `project.error.*` описывают разные сущности. Общий заголовок `Error` сейчас не даёт преимущества.

### Edit title

`content.edit.title` и `project.edit.title` различаются сущностью.

### Form names и required messages

`content.create.name`, `project.create.name`, `project.create.content_name` и соответствующие `*_required` имеют разный смысл.

### Delete dialog titles/descriptions

`content.delete.title`, `project.delete.title` и их description различаются не только сущностью, но и описанием удаляемых данных.

### Loading / empty / error состояния страниц

`projects.*` и `tasks.*` остаются page-specific.

### Workspace

`workspace`, `workspace.lowercase` и `your_workspace` имеют разные регистр/контекст и не объединяются.

### Progress

На текущий момент `content.progress` и `project.progress` нельзя считать полным дублем: в TSV русские формулировки различаются (`Готовность` и `Прогресс`), хотя английский текст одинаков. Пока сохраняем два ID.

## 7. Правила для исходного кода

При использовании общего текста source code обращается к его каноническому ID.

Пример:

```cpp
localization.text(QStringLiteral("common.action.delete"))
```

вместо entity-specific копий `content.delete` / `project.delete`.

Для динамических status ID нельзя механически строить строку из entity namespace, если часть статусов вынесена в `common.status.*`. При таком сценарии используется явное отображение enum → localization ID.

## 8. Проверка перед добавлением нового ключа

Перед добавлением строки в `localization.tsv` разработчик проверяет:

1. существует ли уже такой пользовательский текст;
2. совпадает ли смысл и контекст;
3. можно ли использовать `common.*`;
4. не создаётся ли новый entity-specific дубль;
5. не отличается ли только регистр или место использования.

Цель — иметь один text ID для одного общего пользовательского текста, а не отдельный ID для каждой сущности.

## 9. Техническое ограничение текущего LocalizationService

Текущий `LocalizationService` не проверяет:

- повторное использование одинаковых переводов под разными ID;
- дублирование смысловых строк;
- наличие нескольких потенциально канонических ID.

Поэтому контроль дублирования является правилом структуры `localization.tsv` и code review.

Автоматическая проверка дубликатов может быть добавлена позднее как отдельный инструмент качества, но не должна вводить искусственное объединение текстов с разным контекстом.
