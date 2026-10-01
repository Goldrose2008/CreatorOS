import type { ReactNode } from "react";
import styles from "../../styles/layout/Workspace.module.css";

interface WorkspaceProps {
    children: ReactNode;
    className?: string;
}

function Workspace({
    children,
    className = "",
}: WorkspaceProps) {

    const classes = [
        styles.workspace,
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <main className={classes}>
            {children}
        </main>
    );
}

export default Workspace;