import type { InputHTMLAttributes } from "react";

export interface InputProps extends InputHTMLAttributes<HTMLInputElement> {
    invalid?: boolean;
}

function Input({
    invalid = false,
    className = "",
    ...props
}: InputProps) {

    const classes = [
        "ui-input",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    const ariaInvalid =
        invalid ||
        props["aria-invalid"] === true ||
        props["aria-invalid"] === "true";

    return (
        <input
            {...props}
            className={classes}
            aria-invalid={ariaInvalid || undefined}
        />
    );
}

export default Input;
