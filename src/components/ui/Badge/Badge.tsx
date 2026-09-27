import type { ReactNode } from "react";
import styles from "./Badge.module.css";

export type BadgeVariant =
    | "neutral"
    | "accent"
    | "success"
    | "warning"
    | "danger";

interface BadgeProps {
    children: ReactNode;
    variant?: BadgeVariant;
}

function Badge({
    children,
    variant = "neutral",
}: BadgeProps) {

    const classes = [
        styles.badge,
        styles[variant],
    ].join(" ");

    return (
        <span className={classes}>
            {children}
        </span>
    );
}

export default Badge;