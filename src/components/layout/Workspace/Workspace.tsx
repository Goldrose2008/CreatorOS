import type { ReactNode } from "react";
import styles from "./Workspace.module.css";

interface WorkspaceProps {
    children: ReactNode;
    navigation?: ReactNode;
    className?: string;
}

function Workspace({
    children,
    navigation,
    className = "",
}: WorkspaceProps) {

    const classes = [
        styles.workspace,
        className,
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <div className={classes}>
            <div className={styles.navigation}>
                {navigation}
            </div>
            
            <main className={styles.content}>
                {children}
            </main>

        </div>
    );
}

export default Workspace;