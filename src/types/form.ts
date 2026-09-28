export type EntityFieldType =
    | "text"
    | "textarea"
    | "date"
    | "number"
    | "select";

export interface EntityField<TValues extends object> {
    name: keyof TValues & string;
    label: string;
    type: EntityFieldType;
    required?: boolean;
    placeholder?: string;
    description?: string;
    rows?: number;
    options?: Array<{
        value: string;
        label: string;
    }>;

    parse?: (value: string) => unknown;
    format?: (value: unknown) => string;
    validate?: (value: unknown, values: TValues) => string | undefined;
}