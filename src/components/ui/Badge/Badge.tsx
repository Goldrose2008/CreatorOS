import type { ReactNode } from "react";
import styles from "./Badge.module.css";
import type { StatusTone } from "../../../types/status";


interface BadgeProps {
    children: ReactNode;
    variant?: StatusTone;
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