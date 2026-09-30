import type { ReactNode } from "react";
import Badge from "../../ui/primitives/Badge";
import type { StatusTone } from "../../../types/status";

interface EntityHeaderStatus {
    label: string;
    variant?: StatusTone;
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
        "entity-header",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <header className={classes}>
            <div className={"entity-header__main"}>
                <div className={"entity-header__title-row"}>
                    <h1 className={"entity-header__title"}>
                        {title}
                    </h1>
                    {status && (
                        <Badge variant={status.variant ?? "neutral"}>
                            {status.label}
                        </Badge>
                    )}

                </div>

                {description && (
                    <p className={"entity-header__description"}>
                        {description}
                    </p>
                )}

                {meta.length > 0 && (
                    <div className={"entity-header__meta"}>
                        {meta.map((item, index) => (
                            <span key={index} className={"entity-header__meta-item"}>
                                {item}
                            </span>
                        ))}
                    </div>
                )}
            </div>

            {actions && (
                <div className={"entity-header__actions"}>
                    {actions}
                </div>
            )}
        </header>
    );
}

export default EntityHeader;