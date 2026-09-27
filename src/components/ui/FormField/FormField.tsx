import type { ReactNode } from "react";
import styles from "./FormField.module.css";

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
        <div className={styles.field}>
            <label className={styles.label} htmlFor={htmlFor}>
                {label}
                {required && (
                    <span className={styles.required} aria-hidden="true">
                        *
                    </span>
                )}
            </label>
            {children}
            {description && (
                <div className={styles.description}>
                    {description}
                </div>
            )}

            {error && (
                <div className={styles.error}>
                    {error}
                </div>
            )}
        </div>
    );
}

export default FormField;