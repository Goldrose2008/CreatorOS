import type {
    HTMLAttributes,
    ReactNode,
} from "react";

interface CardProps extends HTMLAttributes<HTMLDivElement> {
    header?: ReactNode;
    footer?: ReactNode;
}

function Card({
    className = "",
    header,
    footer,
    children,
    ...props
}: CardProps) {

    const classes = [
        "ui-card",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <div className={classes} {...props}>
            {header && (
                <div className="ui-card__header">
                    {header}
                </div>
            )}

            {children}

            {footer && (
                <div className="ui-card__footer">
                    {footer}
                </div>
            )}
        </div>
    );
}

export default Card;