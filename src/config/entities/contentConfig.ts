import type { EntityField } from "../../types/form";

export interface ContentTypeFormValues {
    name: string;
    description: string;
}

export const CONTENT_TYPE_FORM_FIELDS: EntityField<ContentTypeFormValues>[] = [
    {
        name: "name",
        label: "Название",
        type: "text",
        required: true,
        placeholder: "Например: Видео",
        description: "Название типа контента.",
    },

    {
        name: "description",
        label: "Особенности",
        type: "textarea",
        rows: 4,
        placeholder: "Кратко опишите особенности этого типа контента.",
        description: "Дополнительная информация, относящаяся именно к этому типу.",
    },
];