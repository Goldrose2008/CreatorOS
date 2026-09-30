import type {
    HTMLAttributes,
    ReactNode,
} from "react";

interface PageHeaderProps extends Omit<HTMLAttributes<HTMLElement>, "title"> {
    title: ReactNode;
    description?: ReactNode;
    actions?: ReactNode;
}

function PageHeader({
    title,
    description,
    actions,
    className = "",
    ...props
}: PageHeaderProps) {

    const classes = [
        "ui-page-header",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <header className={classes} {...props}>
            <div className="ui-page-header__main">
                <h1 className="ui-page-header__title">
                    {title}
                </h1>

                {description && (
                    <p className="ui-page-header__description">
                        {description}
                    </p>
                )}
            </div>

            {actions && (
                <div className="ui-page-header__actions">
                    {actions}
                </div>
            )}
        </header>
    );
}

export default PageHeader;