import type { ReactNode } from "react";
import Sidebar from "../Sidebar/Sidebar";
import styles from "./AppShell.module.css";

interface AppShellProps {
    children: ReactNode;
}

function AppShell({
    children,
}: AppShellProps) {

    return (
        <div className={styles.shell}>
            <Sidebar />
            <main className={styles.content}>
                {children}
            </main>
        </div>
    );
}

export default AppShell;