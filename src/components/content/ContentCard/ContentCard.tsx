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
import { formatDate } from "../../../utils/date";

interface ContentCardProps {
    content: Content;
    contentType: ContentType;
    canDelete?: boolean;
    onEdit: (content: Content) => void;
    onDelete: (content: Content) => void;
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
        <div className={"content-card"} onClick={openContent}>
            <div className={"content-card__main"}>
                <div className={"content-card__title-row"}>
                    <Link to={`/content/${content.id}`} className={"content-card__title"} onClick={(event) => event.stopPropagation()}>
                        {content.name}
                    </Link>

                    <Badge>
                        {contentType.name}
                    </Badge>
                </div>

                {content.description && (
                    <p className={"content-card__description"}>
                        {content.description}
                    </p>
                )}

                <div className={"content-card__meta"}>
                        <span className={"content-card__meta-item"}>
                            <CalendarDays size={14} />
                            Планируемый выход:{" "}
                            {formatDate(content.planned_release_at)}
                        </span>

                    <span className={"content-card__meta-item"}>
                        Прогресс: {content.progress}%
                    </span>
                </div>

                <div className={"content-card__progress"}>
                    <ProgressBar value={content.progress} />
                </div>
            </div>
    {/* Блок действий */}
            <div className={"content-card__actions"}>
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