import type { ReactNode } from "react";

interface FormFieldProps {
    label: string;
    htmlFor: string;
    required?: boolean;
    error?: string;
    description?: string;
    children: ReactNode;
}

function FormField({
    label,
    htmlFor,
    required = false,
    error,
    description,
    children,
}: FormFieldProps) {

    return (
        <div className="ui-form-field">
            <label className="ui-form-field__label" htmlFor={htmlFor}>
                {label}
                {required && (
                    <span className="ui-form-field__required" aria-hidden="true">
                        *
                    </span>
                )}
            </label>
            {children}
            {description && (
                <div className="ui-form-field__description">
                    {description}
                </div>
            )}

            {error && (
                <div className="ui-form-field__error">
                    {error}
                </div>
            )}
        </div>
    );
}

export default FormField;