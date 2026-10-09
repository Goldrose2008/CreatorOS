#pragma once

#include <QString>

enum class LocalizationValidationSeverity
{
    Error,
    Warning,
    Info
};

enum class LocalizationValidationType
{
    MissingId,
    UnusedId,
    EmptyTranslation,
    DuplicateTranslation,
    PlaceholderMismatch,
    DynamicReference
};

struct LocalizationValidationIssue
{
    LocalizationValidationSeverity severity = LocalizationValidationSeverity::Warning;
    LocalizationValidationType type = LocalizationValidationType::DynamicReference;

    QString id;
    QString locale;
    QString filePath;

    int line = 0;
    int column = 0;

    QString message;
};