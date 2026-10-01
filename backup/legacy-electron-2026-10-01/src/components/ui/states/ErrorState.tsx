import type { ReactNode } from "react";

interface ErrorStateProps {
    title?: ReactNode;
    description?: ReactNode;
    action?: ReactNode;
}

function ErrorState({
    title = "Не удалось загрузить данные",
    description,
    action,
}: ErrorStateProps) {

    return (
        <div className="ui-error-state" role="alert">
            <h2 className="ui-error-state__title">
                {title}
            </h2>

            {description && (
                <p className="ui-error-state__description">
                    {description}
                </p>
            )}

            {action && (
                <div className="ui-error-state__action">
                    {action}
                </div>
            )}
        </div>
    );
}

export default ErrorState;