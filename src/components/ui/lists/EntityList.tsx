import type {
    HTMLAttributes,
    ReactNode,
} from "react";

import EmptyState from "../states/EmptyState";
import LoadingState from "../states/LoadingState";

interface EntityListProps<T> extends HTMLAttributes<HTMLDivElement> {
    items: T[];
    renderItem: (item: T, index: number) => ReactNode;

    loading?: boolean;
    loadingState?: ReactNode;
    errorState?: ReactNode;
    emptyState?: ReactNode;
}

function EntityList<T>({
    items,
    renderItem,
    loading = false,
    loadingState,
    errorState,
    emptyState,
    className = "",
    ...props
}: EntityListProps<T>) {

    if (loading) {
        return (
            <>
                {loadingState ?? <LoadingState />}
            </>
        );
    }

    if (errorState) {
        return (
            <>
                {errorState}
            </>
        );
    }

    if (items.length === 0) {
        return (
            <>
                {emptyState ?? (<EmptyState title="Нет данных"/>)}
            </>
        );
    }

    const classes = [
        "ui-entity-list",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <div className={classes} {...props}>
            {items.map(renderItem)}
        </div>
    );
}

export default EntityList;