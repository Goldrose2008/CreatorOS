import type { HTMLAttributes, ReactNode } from "react";
import PageLayout from "../ui/layout/PageLayout";

export interface EntityDetailsProps
    extends HTMLAttributes<HTMLDivElement> {
    navigation?: ReactNode;
    header?: ReactNode;
    error?: ReactNode;
    summary?: ReactNode;
    children: ReactNode;
    overlays?: ReactNode;
}

function EntityDetails({
    navigation,
    header,
    error,
    summary,
    children,
    overlays,
    className = "",
    ...props
}: EntityDetailsProps) {
    const classes = [
        "entity-details",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <PageLayout
            navigation={navigation}
            header={header}
            className={classes}
            {...props}
        >
            {error && (
                <div className="entity-details__error">
                    {error}
                </div>
            )}

            {summary && (
                <div className="entity-details__summary">
                    {summary}
                </div>
            )}

            <div className="entity-details__content">
                {children}
            </div>

            {overlays}
        </PageLayout>
    );
}

export default EntityDetails;