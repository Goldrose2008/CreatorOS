import type { TextareaHTMLAttributes } from "react";

export interface TextareaProps extends TextareaHTMLAttributes<HTMLTextAreaElement> {
    invalid?: boolean;
}

function Textarea({
    invalid = false,
    className = "",
    ...props
}: TextareaProps) {

    const classes = [
        "ui-textarea",
        className,
    ]
        .filter(Boolean)
        .join(" ");

    const ariaInvalid =
        invalid ||
        props["aria-invalid"] === true ||
        props["aria-invalid"] === "true";

    return (
        <textarea
            {...props}
            className={classes}
            aria-invalid={ariaInvalid || undefined}
        />
    );
}

export default Textarea;
