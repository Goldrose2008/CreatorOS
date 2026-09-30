import type { ReactNode } from "react";
import Sidebar from "./Sidebar";
import Workspace from "./Workspace";
import styles from "../../styles/layout/AppShell.module.css";

interface AppShellProps {
    children: ReactNode;
}

function AppShell({
    children,
}: AppShellProps) {

    return (
        <div className={styles.shell}>
            <Sidebar />
            <Workspace>
                {children}
            </Workspace>
        </div>
    );
}

export default AppShell;