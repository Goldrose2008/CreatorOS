import {
    CalendarDays,
    Pencil,
    Trash2,
} from "lucide-react";
import { Link, useNavigate } from "react-router-dom";
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
    const navigate = useNavigate();

    function openContent() { navigate(`/content/${content.id}`); }

    return (
        <div className={styles.card} onClick={openContent}>
            <div className={styles.main}>
                <div className={styles.titleRow}>
                    <Link to={`/content/${content.id}`} className={styles.title} onClick={(event) => event.stopPropagation()}>
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
    {/* Блок действий */}
            <div className={styles.actions}>
        {/* Кнопка редактировать контент */}
                <Button variant="secondary" onClick={(event) => { event.stopPropagation(); onEdit(content); }}>
                    <Pencil size={15} />
                    Редактировать
                </Button>
        {/* Кнопка удалить контент (только для доп.контента)*/}
                {canDelete && (
                    <Button variant="danger" onClick={(event) => { event.stopPropagation(); onDelete(content); }}>
                        <Trash2 size={15} />
                        Удалить
                    </Button>
                )}
            </div>
        </div>
    );
}

export default ContentCard;