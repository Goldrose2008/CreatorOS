import type { ReactNode } from "react";
import Card from "../ui/Card/Card";
import Badge from "../ui/Badge/Badge";
import type { StatusTone } from "../../types/status";
import ProgressBar from "../ui/ProgressBar/ProgressBar";
import styles from "../../../styles/entities/EntityCard.module.css";

export interface EntityCardStatus {
    label: string;
    variant?: StatusTone;
}

export interface InitialCardProps {
    title: ReactNode;
    description?: ReactNode;
    status?: EntityCardStatus;
    progress?: number;
    meta?: ReactNode[];
    actions?: ReactNode;
}

export function InitialCard({
    title,
    description,
    status,
    progress,
    meta = [],
    actions,
}: InitialCardProps) {
    return (
        <Card className={styles.card}>
            <div className={styles.header}>
                <div className={styles.titleBlock}>
                    <h3 className={styles.title}>
                        {title}
                    </h3>

                    {description && (
                        <p className={styles.description}>
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
                <div className={styles.progress}>
                    <ProgressBar value={progress} />
                </div>
            )}

            {meta.length > 0 && (
                <div className={styles.meta}>
                    {meta.map((item, index) => (
                        <div key={index} className={styles.metaItem}>
                            {item}
                        </div>
                    ))}
                </div>
            )}

            {actions && (
                <div className={styles.actions}>
                    {actions}
                </div>
            )}
        </Card>
    );
}