import type {ButtonHTMLAttributes, ReactNode} from "react";

type ButtonVariant =
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
    ...props
}: ButtonProps) {

    const classes = [
        "ui-button",
        `ui-button--${variant}`,
        className
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <button className={classes} {...props}>{children}</button>
    );
}

export default Button;