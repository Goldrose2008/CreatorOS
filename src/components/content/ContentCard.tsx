import {
    CalendarDays,
    Pencil,
    Trash2,
} from "lucide-react";
import { Link, useNavigate } from "react-router-dom";
import { EntityCard } from "../entity/EntityCard";
import Button from "../ui/primitives/Button";
import type { Content } from "../../models/Content";
import type { ContentType } from "../../models/ContentType";
import { formatDate } from "../../utils/date";

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
        <EntityCard
            onClick={openContent}

            title={
                <Link to={`/content/${content.id}`} className={"entity-card__title-link"} onClick={(event) => event.stopPropagation()}>
                    {content.name}
                </Link>
            }

            description={content.description}
            status={{ label: contentType.name }}
            progress={content.progress}

            meta={[
                <span key="release-date" className={"entity-card__meta-content"}>
                    <CalendarDays size={14} />

                    <span>
                        Планируемый выход:{" "}
                        {formatDate(content.planned_release_at)}
                    </span>
                </span>,

                <span key="progress">
                    Прогресс: {content.progress}%
                </span>,
            ]}

            actions={
                <>
                    <Button variant="secondary" onClick={(event) => { event.stopPropagation(); onEdit(content); }}>
                        <Pencil size={15} />
                        Редактировать
                    </Button>

                    {canDelete && (
                        <Button variant="danger" onClick={(event) => { event.stopPropagation(); onDelete(content); }}>
                            <Trash2 size={15} />
                            Удалить
                        </Button>
                    )}
                </>
            }
        />
    );
}

export default ContentCard;