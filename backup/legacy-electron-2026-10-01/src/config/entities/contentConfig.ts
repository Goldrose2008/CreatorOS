import type { EntityField } from "../../types/form";
import type { ContentType } from "../../models/ContentType";

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

export interface ContentFormValues {
    contentTypeId: number;
    name: string;
    description: string;
}

export function getContentFormFields(
    contentTypes: ContentType[]
): EntityField<ContentFormValues>[] {
    return [
        {
            name: "contentTypeId",
            label: "Тип контента",
            type: "select",
            required: true,
            options: contentTypes.map((contentType) => ({
                value: String(contentType.id),
                label: contentType.name,
            })),
            parse: (value) => Number(value),
            description: "Выбери тип создаваемого контента.",
        },

        {
            name: "name",
            label: "Название",
            type: "text",
            required: true,
            placeholder: "Название контента",
        },

        {
            name: "description",
            label: "Описание",
            type: "textarea",
            rows: 4,
            placeholder: "Краткое описание контента.",
        },
    ];
}