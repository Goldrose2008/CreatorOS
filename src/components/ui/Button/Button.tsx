import type {
    ButtonHTMLAttributes,
    ReactNode,
} from "react";

export type ButtonVariant =
    | "primary"
    | "secondary"
    | "ghost"
    | "danger";

interface ButtonProps extends ButtonHTMLAttributes<HTMLButtonElement> {
    variant?: ButtonVariant;
    children: ReactNode;
}

function Button({
    variant = "primary",
    className = "",
    children,
    type = "button",
    ...props
}: ButtonProps) {

    const classes = [
        "ui-button",
        `ui-button--${variant}`,
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <button type={type} className={classes} {...props}>
            {children}
        </button>
    );
}

export default Button;
