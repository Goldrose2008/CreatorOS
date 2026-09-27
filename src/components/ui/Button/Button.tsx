import type {
    ButtonHTMLAttributes,
    ReactNode,
} from "react";

import styles from "./Button.module.css";

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
        styles.button,
        styles[variant],
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