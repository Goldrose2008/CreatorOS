import type { BadgeVariant } from "../../components/ui/Badge/Badge";
import type { EntityField } from "../../components/ui/EntityForm/EntityForm";

export interface ProjectFormValues {
    name: string;
    description: string;
    planned_release_at: string;
}

export const PROJECT_FORM_FIELDS: EntityField<ProjectFormValues>[] = [

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

export const PROJECT_STATUSES = {
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
} as const satisfies Record<
    string,
    {
        label: string;
        variant: BadgeVariant;
    }
>;

export type ProjectStatus = keyof typeof PROJECT_STATUSES;

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

export function getProjectStatusConfig(status: string) {
    if (status in PROJECT_STATUSES) { return PROJECT_STATUSES[status as ProjectStatus]; }

    return {
        label: status,
        variant: "neutral" as BadgeVariant,
    };
}