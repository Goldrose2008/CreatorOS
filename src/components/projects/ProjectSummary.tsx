import { CalendarDays } from "lucide-react";
import { Link } from "react-router-dom";
import Badge from "../ui/Badge/Badge";
import ProgressBar from "../ui/ProgressBar/ProgressBar";
import type { Project } from "../../models/Project";
import { getProjectStatusConfig } from "../../config/entities/projectConfig";
import styles from "./ProjectSummary.module.css";

interface ProjectSummaryProps {
    project: Project;
}

function formatProjectDate(value: string): string {
    const datePart = value.slice(0, 10);

    const [
        year,
        month,
        day,
    ] = datePart.split("-");

    if (!year || !month || !day) { return value; }

    return `${day}.${month}.${year}`;
}

function ProjectSummary({ project }: ProjectSummaryProps) {
    const status = getProjectStatusConfig(project.status);

    return (
        <div className={styles.item}>
            <div className={styles.main}>
                <div className={styles.titleRow}>
                    <Link to={`/projects/${project.id}`} className={styles.title}>
                        {project.name}
                    </Link>

                    <Badge variant={status.variant}>
                        {status.label}
                    </Badge>
                </div>

                <div className={styles.meta}>
                    <span className={styles.date}>
                        <CalendarDays size={14} />
                        <span>
                            Выход: {formatProjectDate(project.planned_release_at)}
                        </span>
                    </span>

                    <span>
                        Готовность: {project.progress}%
                    </span>
                </div>

                <ProgressBar value={project.progress} />
            </div>
        </div>
    );
}

export default ProjectSummary;