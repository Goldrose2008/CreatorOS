import type {
    HTMLAttributes,
    ReactNode,
} from "react";

interface ToolbarProps extends HTMLAttributes<HTMLDivElement> {
    search?: ReactNode;
    filters?: ReactNode;
    sorting?: ReactNode;
    actions?: ReactNode;
}

function Toolbar({
    search,
    filters,
    sorting,
    actions,
    className = "",
    ...props
}: ToolbarProps) {

    const classes = [
        "ui-toolbar",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    if (!search && !filters && !sorting && !actions) { return null; }

    return (
        <div className={classes} {...props}>
            {(filters || sorting) && (
                <div className="ui-toolbar__start">
                    {filters}
                    {sorting}
                </div>
            )}

            {search && (
                <div className="ui-toolbar__search">
                    {search}
                </div>
            )}

            {actions && (
                <div className="ui-toolbar__actions">
                    {actions}
                </div>
            )}
        </div>
    );
}

export default Toolbar;