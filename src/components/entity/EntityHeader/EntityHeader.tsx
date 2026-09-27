import type { ReactNode } from "react";
import Badge, { type BadgeVariant } from "../../ui/Badge/Badge";
import styles from "./EntityHeader.module.css";

interface EntityHeaderStatus {
    label: string;
    variant?: BadgeVariant;
}

interface EntityHeaderProps {
    title: ReactNode;
    description?: ReactNode;
    status?: EntityHeaderStatus;
    meta?: ReactNode[];
    actions?: ReactNode;
    className?: string;
}

function EntityHeader({
    title,
    description,
    status,
    meta = [],
    actions,
    className = "",
}: EntityHeaderProps) {

    const classes = [
        styles.header,
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <header className={classes}>
            <div className={styles.main}>
                <div className={styles.titleRow}>
                    <h1 className={styles.title}>
                        {title}
                    </h1>
                    {status && (
                        <Badge variant={status.variant ?? "neutral"}>
                            {status.label}
                        </Badge>
                    )}

                </div>

                {description && (
                    <p className={styles.description}>
                        {description}
                    </p>
                )}

                {meta.length > 0 && (
                    <div className={styles.meta}>
                        {meta.map((item, index) => (
                            <span key={index} className={styles.metaItem}>
                                {item}
                            </span>
                        ))}
                    </div>
                )}
            </div>

            {actions && (
                <div className={styles.actions}>
                    {actions}
                </div>
            )}
        </header>
    );
}

export default EntityHeader;