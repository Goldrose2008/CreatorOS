import type { ReactNode } from "react";
import styles from "./EmptyState.module.css";

interface EmptyStateProps {
    title?: ReactNode;
    description?: ReactNode;
    action?: ReactNode;
}

function EmptyState({
    title,
    description,
    action,
}: EmptyStateProps) {

    return (
        <div className={styles.container}>
            {title && (
                <h2 className={styles.title}>
                    {title}
                </h2>
            )}

            {description && (
                <p className={styles.description}>
                    {description}
                </p>
            )}

            {action && (
                <div className={styles.action}>
                    {action}
                </div>
            )}
        </div>
    );
}

export default EmptyState;