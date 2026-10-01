import type {
    MouseEventHandler,
    ReactNode,
} from "react";
import Card from "../ui/layout/Card";
import Badge from "../ui/primitives/Badge";
import type { StatusTone } from "../../types/status";
import ProgressBar from "../ui/primitives/ProgressBar";

export interface EntityCardStatus {
    label: string;
    variant?: StatusTone;
}

export interface EntityCardProps {
    title: ReactNode;
    description?: ReactNode;
    content?: ReactNode;
    status?: EntityCardStatus;
    progress?: number;
    meta?: ReactNode[];
    actions?: ReactNode;
    onClick?: MouseEventHandler<HTMLDivElement>;
    className?: string;
}

export function EntityCard({
    title,
    description,
    content,
    status,
    progress,
    meta = [],
    actions,
    onClick,
    className = "",
}: EntityCardProps) {
    return (
    <Card
        className={[
            "entity-card",
            onClick ? "entity-card--clickable" : "",
            className,
        ]
            .filter(Boolean)
            .join(" ")}
        onClick={onClick}
    >
            <div className={"entity-card__header"}>
                <div className={"entity-card__title-block"}>
                    <h3 className={"entity-card__title"}>
                        {title}
                    </h3>

                    {description && (
                        <p className={"entity-card__description"}>
                            {description}
                        </p>
                    )}
                    {content && (
                        <div className={"entity-card__content"}>
                            {content}
                        </div>
                    )}
                </div>

                {status && (
                    <Badge variant={status.variant ?? "neutral"}>
                        {status.label}
                    </Badge>
                )}
            </div>

            {progress !== undefined && (
                <div className={"entity-card__progress"}>
                    <ProgressBar value={progress} />
                </div>
            )}

            {meta.length > 0 && (
                <div className={"entity-card__meta"}>
                    {meta.map((item, index) => (
                        <div key={index} className={"entity-card__meta-item"}>
                            {item}
                        </div>
                    ))}
                </div>
            )}

            {actions && (
                <div className={"entity-card__actions"}>
                    {actions}
                </div>
            )}
        </Card>
    );
}