import type { ReactNode } from "react";
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
        "ui-badge",
        `ui-badge--${variant}`,
    ].join(" ");

    return (
        <span className={classes}>
            {children}
        </span>
    );
}

export default Badge;
