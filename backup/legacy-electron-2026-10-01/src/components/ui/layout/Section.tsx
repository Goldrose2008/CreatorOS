import type {
    HTMLAttributes,
    ReactNode,
} from "react";

interface SectionProps extends Omit<HTMLAttributes<HTMLElement>, "title"> {
    title: ReactNode;
    description?: ReactNode;
    actions?: ReactNode;
    children: ReactNode;
}

function Section({
    title,
    description,
    actions,
    children,
    className = "",
    ...props
}: SectionProps) {

    const classes = [
        "ui-section",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <section className={classes} {...props}>
            <header className="ui-section__header">
                <div className="ui-section__main">
                    <h2 className="ui-section__title">
                        {title}
                    </h2>

                    {description && (
                        <p className="ui-section__description">
                            {description}
                        </p>
                    )}
                </div>

                {actions && (
                    <div className="ui-section__actions">
                        {actions}
                    </div>
                )}
            </header>

            <div className="ui-section__content">
                {children}
            </div>
        </section>
    );
}

export default Section;