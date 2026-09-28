import type { EntityField } from "../../types/form";
import type { StatusTone } from "../../types/status";
import type { ProjectStatus } from "../../models/Project";
import type { ContentType } from "../../models/ContentType";

export interface ProjectBaseFormValues  {
    name: string;
    description: string;
    planned_release_at: string;
}

export interface ProjectFormValues extends ProjectBaseFormValues {}

export interface ProjectCreateFormValues
    extends ProjectBaseFormValues {
    mainContentTypeId: number;
    mainContentName: string;
}

function getProjectBaseFields<TValues extends ProjectBaseFormValues>(): EntityField<TValues>[] {
    return [
        {
            name: "name",
            label: "Название",
            type: "text",
            required: true,
            placeholder: "Название проекта",
            description: "Короткое название, по которому проект будет легко найти.",
        },

        {
            name: "description",
            label: "Описание",
            type: "textarea",
            rows: 5,
            placeholder: "Кратко опишите проект.",
        },

        {
            name: "planned_release_at",
            label: "Планируемая дата выхода",
            type: "date",
            required: true,
            description: "Дата планируемого выхода основного контента проекта.",
        },
    ];
}

export const PROJECT_FORM_FIELDS = getProjectBaseFields<ProjectFormValues>();

export function getProjectCreateFormFields(contentTypes: ContentType[]): EntityField<ProjectCreateFormValues>[] {
    return [
        ...getProjectBaseFields<ProjectCreateFormValues>(),

        {
            name: "mainContentTypeId",
            label: "Основной тип контента",
            type: "select",
            required: true,
            options: contentTypes.map((contentType) => ({
                value: String(contentType.id),
                label: contentType.name,
            })),
            parse: (value) => Number(value),
            description: "Тип основного контента, который будет создан вместе с проектом.",
        },
        {
            name: "mainContentName",
            label: "Название основного контента",
            type: "text",
            required: true,
            placeholder: "Название основного контента",
            description: "Название главного материала, который создаётся в рамках проекта.",
        },
    ];
}

export const PROJECT_STATUSES: Record <ProjectStatus,{
        label: string;
        variant: StatusTone;
    }> = 
{
    draft: {
        label: "Черновик",
        variant: "neutral",
    },

    active: {
        label: "В работе",
        variant: "accent",
    },

    archived: {
        label: "Архив",
        variant: "neutral",
    },
}

export interface ProjectStatusAction {
    action: string;
    label: string;
    nextStatus: ProjectStatus;
}

export const PROJECT_STATUS_ACTIONS: Record<ProjectStatus, ProjectStatusAction[]> = {

    draft: [
        {
            action: "activate",
            label: "Взять в работу",
            nextStatus: "active",
        },
    ],

    active: [
        {
            action: "archive",
            label: "Архивировать",
            nextStatus: "archived",
        },
    ],

    archived: [
        {
            action: "restore",
            label: "Восстановить",
            nextStatus: "draft",
        },
    ],
};

export function getProjectStatusConfig(status: ProjectStatus) {
    return PROJECT_STATUSES[status];
}