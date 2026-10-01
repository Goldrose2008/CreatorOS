import type {
    HTMLAttributes,
    ReactNode,
} from "react";

interface PageLayoutProps extends HTMLAttributes<HTMLDivElement> {
    navigation?: ReactNode;
    header?: ReactNode;
    toolbar?: ReactNode;
    children: ReactNode;
}

function PageLayout({
    navigation,
    header,
    toolbar,
    children,
    className = "",
    ...props
}: PageLayoutProps) {

    const classes = [
        "ui-page-layout",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <div className={classes} {...props}>
            {navigation && (
                <div className="ui-page-layout__navigation">
                    {navigation}
                </div>
            )}

            {header && (
                <div className="ui-page-layout__header">
                    {header}
                </div>
            )}

            {toolbar && (
                <div className="ui-page-layout__toolbar">
                    {toolbar}
                </div>
            )}

            <div className="ui-page-layout__content">
                {children}
            </div>
        </div>
    );
}

export default PageLayout;