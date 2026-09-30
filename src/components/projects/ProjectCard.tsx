import {
    CalendarDays,
    Pencil,
    Trash2,
} from "lucide-react";
import { Link } from "react-router-dom";
import { EntityCard } from "../entity/EntityCard";
import Button from "../ui/Button/Button";
import type { Project } from "../../models/Project";
import { getProjectStatusConfig } from "../../config/entities/projectConfig";

interface ProjectCardProps {
    project: Project;
    onEdit: (project: Project) => void;
    onDelete: (id: number) => void;
}

function formatProjectDate(value: string): string {
    const datePart = value.slice(0, 10);
    const [year, month, day] = datePart.split("-");

    if (!year || !month || !day) { return value;}

    return `${day}.${month}.${year}`;
}

function ProjectCard({
    project,
    onEdit,
    onDelete,
}: ProjectCardProps) {
    const status = getProjectStatusConfig(project.status);

    return EntityCard({
        title: (
            <Link
                to={`/projects/${project.id}`}
                className={"entity-card__title-link"}
            >
                {project.name}
            </Link>
        ),

        description: project.description,

        status: {
            label: status.label,
            variant: status.variant,
        },

        progress: project.progress,

        meta: [
            <span
                key="release-date"
                className={"entity-card__meta-content"}
            >
                <CalendarDays size={14} />

                <span>
                    Выход: {formatProjectDate(project.planned_release_at)}
                </span>
            </span>,
        ],

        actions: (
            <>
                <Link
                    to={`/projects/${project.id}`}
                    className={"entity-card__link-button"}
                >
                    Открыть
                </Link>

                <Button
                    variant="secondary"
                    onClick={() => onEdit(project)}
                >
                    <Pencil size={15} />
                    Редактировать
                </Button>

                <Button
                    variant="danger"
                    className={"entity-card__delete-button"}
                    onClick={() => onDelete(project.id)}
                >
                    <Trash2 size={15} />
                    Удалить
                </Button>
            </>
        ),
    });
}

export default InitialProjectCard;