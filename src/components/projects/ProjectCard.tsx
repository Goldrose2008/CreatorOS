import {
    CalendarDays,
    Pencil,
    Trash2,
} from "lucide-react";
import { Link } from "react-router-dom";
import { InitialCard } from "../entity/EntityCard";
import Button from "../ui/Button/Button";
import type { Project } from "../../models/Project";
import { getProjectStatusConfig } from "../../config/entities/projectConfig";
import styles from "../../styles/entities/EntityCard.module.css";

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

function InitialProjectCard({
    project,
    onEdit,
    onDelete,
}: ProjectCardProps) {
    const status = getProjectStatusConfig(project.status);

    return InitialCard({
        title: (
            <Link
                to={`/projects/${project.id}`}
                className={styles.titleLink}
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
                className={styles.metaContent}
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
                    className={styles.linkButton}
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
                    className={styles.deleteButton}
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