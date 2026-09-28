import type { ReactNode } from "react";
import styles from "./Badge.module.css";
import type { StatusTone } from "../../../types/status";

export type BadgeVariant = StatusTone;

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