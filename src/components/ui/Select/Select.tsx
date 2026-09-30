import type { SelectHTMLAttributes } from "react";

export interface SelectProps extends SelectHTMLAttributes<HTMLSelectElement> {
    invalid?: boolean;
}

function Select({
    invalid = false,
    className = "",
    ...props
}: SelectProps) {

    const classes = [
        "ui-select",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    const ariaInvalid =
        invalid ||
        props["aria-invalid"] === true ||
        props["aria-invalid"] === "true";

    return (
        <select
            {...props}
            className={classes}
            aria-invalid={ariaInvalid || undefined}
        />
    );
}

export default Select;
