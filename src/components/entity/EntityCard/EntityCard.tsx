import type { ReactNode } from "react";
import Card from "../../ui/layout/Card";
import Badge from "../../ui/primitives/Badge";
import type { StatusTone } from "../../../types/status";
import ProgressBar from "../../ui/primitives/ProgressBar";

export interface EntityCardStatus {
    label: string;
    variant?: StatusTone;
}

export interface EntityCardProps {
    title: ReactNode;
    description?: ReactNode;
    status?: EntityCardStatus;
    progress?: number;
    meta?: ReactNode[];
    actions?: ReactNode;
}

export function EntityCard({
    title,
    description,
    status,
    progress,
    meta = [],
    actions,
}: EntityCardProps) {
    return (
        <Card className={"entity-card"}>
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