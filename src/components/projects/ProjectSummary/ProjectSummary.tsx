import { CalendarDays } from "lucide-react";
import { Link } from "react-router-dom";
import Badge from "../ui/Badge/Badge";
import ProgressBar from "../ui/ProgressBar/ProgressBar";
import type { Project } from "../../models/Project";
import { getProjectStatusConfig } from "../../config/entities/projectConfig";
import { formatDate } from "../../utils/date";

interface ProjectSummaryProps {
    project: Project;
}

function ProjectSummary({ project }: ProjectSummaryProps) {
    const status = getProjectStatusConfig(project.status);

    return (
        <div className={"project-summary"}>
            <div className={"project-summary__main"}>
                <div className={"project-summary__title-row"}>
                    <Link to={`/projects/${project.id}`} className={"project-summary__title"}>
                        {project.name}
                    </Link>

                    <Badge variant={status.variant}>
                        {status.label}
                    </Badge>
                </div>

                <div className={"project-summary__meta"}>
                    <span className={"project-summary__date"}>
                        <CalendarDays size={14} />
                        <span>
                            Выход: {formatDate(project.planned_release_at)}
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