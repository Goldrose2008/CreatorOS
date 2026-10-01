import type { ReactNode } from "react";

interface LoadingStateProps {
    children?: ReactNode;
}

function LoadingState({ children = "Загрузка..." }: LoadingStateProps) {

    return (
        <div className="ui-loading-state" role="status" aria-live="polite">
            <div className="ui-loading-state__indicator">
                <span className="ui-loading-state__dot" />
                <span>{children}</span>
            </div>
        </div>
    );
}

export default LoadingState;