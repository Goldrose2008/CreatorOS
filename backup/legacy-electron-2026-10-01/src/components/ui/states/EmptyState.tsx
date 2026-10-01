import type { ReactNode } from "react";

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
        <div className="ui-empty-state">
            {title && (
                <h2 className="ui-empty-state__title">
                    {title}
                </h2>
            )}

            {description && (
                <p className="ui-empty-state__description">
                    {description}
                </p>
            )}

            {action && (
                <div className="ui-empty-state__action">
                    {action}
                </div>
            )}
        </div>
    );
}

export default EmptyState;