import {
    useState,
    type SubmitEvent,
} from "react";

import Button from "../Button/Button";
import FormField from "../FormField/FormField";
import Input from "../Input/Input";
import Textarea from "../Textarea/Textarea";
import Select from "../Select/Select";
import styles from "./EntityForm.module.css";
import type { EntityField } from "../../../types/form";

interface EntityFormProps<TValues extends object> {
    fields: EntityField<TValues>[];

    initialValues: TValues;

    submitLabel?: string;
    cancelLabel?: string;

    saving?: boolean;
    error?: string;

    onSubmit: (values: TValues) => Promise<void> | void;
    onFieldChange?: (
        fieldName: keyof TValues & string,
        value: unknown,
        values: TValues
    ) => Partial<TValues> | void;
    onCancel: () => void;
}

function EntityForm<TValues extends object>({
    fields,
    initialValues,
    submitLabel = "Сохранить",
    cancelLabel = "Отмена",
    saving = false,
    error,
    onSubmit,
    onFieldChange,
    onCancel,
}: EntityFormProps<TValues>) {

    const [values, setValues] = useState<TValues>(() => initialValues);
    const [fieldErrors, setFieldErrors] = useState<Record<string, string>>({});

    function getValue(field: EntityField<TValues>): string {
        const value = values[field.name];

        if (field.format) { return field.format(value); }

        return value == null ? "" : String(value);
    }

    function handleChange(field: EntityField<TValues>, rawValue: string) {
        const parsedValue = field.parse ? field.parse(rawValue) : rawValue;
        const nextValues = { ...values, [field.name]: parsedValue } as TValues;
        const fieldChanges = onFieldChange?.(field.name, parsedValue, nextValues);

        setValues({
            ...nextValues,
            ...(fieldChanges ?? {}),
        });

        setFieldErrors((current) => {
            const next = { ...current };
            delete next[field.name];
            return next;
        });
    }

    function validate(): boolean {
        const errors: Record<string, string> = {};

        for (const field of fields) {
            const value = values[field.name];

            if (field.required && (value == null || String(value).trim() === "")) {
                errors[field.name] = "Поле обязательно для заполнения.";
                continue;
            }

            if (field.validate) {
                const validationError = field.validate(value, values);
                if (validationError) { errors[field.name] = validationError; }
            }
        }
        setFieldErrors(errors);
        return Object.keys(errors).length === 0;
    }

    async function handleSubmit(event: SubmitEvent<HTMLFormElement>) {
        event.preventDefault();
        if (!validate()) { return; }
        await onSubmit(values);
    }

    function renderInput(field: EntityField<TValues>) {
        const id = `entity-field-${field.name}`;
        const value = getValue(field);
        const commonProps = {
            id,
            value,
            placeholder: field.placeholder,
            disabled: saving,
        };

        switch (field.type) {
            case "textarea":
                return (
                    <Textarea
                        {...commonProps}
                        rows={field.rows ?? 4}
                        invalid={Boolean(fieldErrors[field.name])}
                        onChange={(event) => handleChange(field, event.target.value)}
                    />
                );
            case "date":
                return (
                    <Input
                        {...commonProps}
                        type="date"
                        invalid={Boolean(fieldErrors[field.name])}
                        onChange={(event) => handleChange(field, event.target.value)}
                    />
                );
            case "number":
                return (
                    <Input
                        {...commonProps}
                        type="number"
                        invalid={Boolean(fieldErrors[field.name])}
                        onChange={(event) => handleChange(field, event.target.value)}
                    />
                );
            case "select":
                return (
                    <Select
                        {...commonProps}
                        invalid={Boolean(fieldErrors[field.name])}
                        onChange={(event) => handleChange(field, event.target.value)}
                    >
                        {field.options?.map(
                            (option) => (
                                <option key={option.value} value={option.value}>
                                    {option.label}
                                </option>
                            )
                        )}
                    </Select>
                );
            case "text":
            default:
                return (
                    <Input
                        {...commonProps}
                        type="text"
                        invalid={Boolean(fieldErrors[field.name])}
                        onChange={(event) => handleChange(field, event.target.value)}
                    />
                );
        }
    }

    return (
        <form className={styles.form} onSubmit={handleSubmit}>
            <div className={styles.fields}>

                {fields.map((field) => (
                    <FormField
                        key={field.name}
                        label={field.label}
                        htmlFor={
                            `entity-field-${field.name}`
                        }
                        required={
                            field.required
                        }
                        description={
                            field.description
                        }
                        error={
                            fieldErrors[field.name]
                        }
                    >
                        {renderInput(field)}
                    </FormField>
                ))}

            </div>

            {error && (
                <div className={styles.formError}>
                    {error}
                </div>
            )}

            <div className={styles.actions}>
                <Button type="submit" disabled={saving}>
                    {saving ? "Сохранение..." : submitLabel}
                </Button>

                <Button type="button" variant="secondary" onClick={onCancel} disabled={saving}>
                    {cancelLabel}
                </Button>
            </div>
        </form>
    );
}

export default EntityForm;
