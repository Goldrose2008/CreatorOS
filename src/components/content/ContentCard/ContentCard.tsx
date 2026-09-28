import { CalendarDays, Pencil, Trash2 } from "lucide-react";
import { Link } from "react-router-dom";
import Button from "../../ui/Button/Button";
import Badge from "../../ui/Badge/Badge";
import ProgressBar from "../../ui/ProgressBar/ProgressBar";
import type { Content } from "../../../models/Content";
import type { ContentType } from "../../../models/ContentType";
import styles from "./ContentCard.module.css";

interface ContentCardProps {
    content: Content;
    contentType: ContentType;
    canDelete?: boolean;
    onEdit: (content: Content) => void;
    onDelete: (content: Content) => void;
}

function formatDate(value?: string | null): string {
    if (!value) { return "Не указана"; }

    const datePart = value.slice(0, 10);
    const [year, month, day] = datePart.split("-");

    if (!year || !month || !day) { return value; }

    return `${day}.${month}.${year}`;
}

function ContentCard({
    content,
    contentType,
    canDelete = true,
    onEdit,
    onDelete,
}: ContentCardProps) {
    return (
        <div className={styles.card}>
            <div className={styles.main}>
                <div className={styles.titleRow}>
                    <Link to={`/projects/${content.project_id}`} className={styles.title}>
                        {content.name}
                    </Link>

                    <Badge>
                        {contentType.name}
                    </Badge>
                </div>

                {content.description && (
                    <p className={styles.description}>
                        {content.description}
                    </p>
                )}

                <div className={styles.meta}>
                        <span className={styles.metaItem}>
                            <CalendarDays size={14} />
                            Планируемый выход:{" "}
                            {formatDate(content.planned_release_at)}
                        </span>

                    <span className={styles.metaItem}>
                        Прогресс: {content.progress}%
                    </span>
                </div>

                <div className={styles.progress}>
                    <ProgressBar value={content.progress} />
                </div>
            </div>

            <div className={styles.actions}>
                <Button
                    variant="secondary"
                    onClick={() => onEdit(content)}
                >
                    <Pencil size={15} />
                    Редактировать
                </Button>

                {canDelete && (
                    <Button
                        variant="danger"
                        onClick={() => onDelete(content)}
                    >
                        <Trash2 size={15} />
                        Удалить
                    </Button>
                )}
            </div>
        </div>
    );
}

export default ContentCard;