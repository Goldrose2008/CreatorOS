#pragma once

#include <cstdint>
#include <optional>

#include "../../components/forms/EditorDialog.h"
#include "../../../domain/models/Content.h"

class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QSpinBox;
class ContentService;
class LocalizationService;
class ProjectService;

class ContentEditorDialog final : public EditorDialog
{
    Q_OBJECT

public:
    explicit ContentEditorDialog(
        ContentService &contentService,
        ProjectService &projectService,
        LocalizationService &localization,
        QWidget *parent = nullptr,
        std::int64_t projectId = 0,
        std::optional<Content> content = std::nullopt
    );

protected:
    bool save() override;

private:
    ContentService &contentService_;
    ProjectService &projectService_;
    LocalizationService &localization_;

    std::int64_t projectId_;
    std::int64_t contentId_;

    QComboBox *contentTypeCombo_;
    QLineEdit *nameEdit_;
    QPlainTextEdit *descriptionEdit_;
    QSpinBox *prioritySpin_;
};