import { NavLink, Outlet } from "react-router-dom";
import {
    Keyboard,
    Palette,
    Settings2,
    FolderKanban,
    FileType,
    Workflow,
    Plug,
    Users,
    Sparkles
} from "lucide-react";
import { APP_NAME } from "../../config/appConfig";
import styles from "./SettingsLayout.module.css";

interface SettingsNavigationItem {
    label: string;
    path: string;
    icon: React.ComponentType<{
        size?: number;
        strokeWidth?: number;
    }>;
}

interface SettingsNavigationGroup {
    title: string;
    items: SettingsNavigationItem[];
}

const navigationGroups: SettingsNavigationGroup[] = [
    {
        title: "Система",
        items: [
            {
                label: "Общие",
                path: "/settings/general",
                icon: Settings2
            },
            {
                label: "Внешний вид",
                path: "/settings/appearance",
                icon: Palette
            },
            {
                label: "Горячие клавиши",
                path: "/settings/shortcuts",
                icon: Keyboard
            }
        ]
    },
    {
        title: "Контент",
        items: [
            {
                label: "Проекты",
                path: "/settings/projects",
                icon: FolderKanban
            },
            {
                label: "Типы контента",
                path: "/settings/content",
                icon: FileType
            },
            {
                label: "Автоматизация",
                path: "/settings/automation",
                icon: Workflow
            }
        ]
    },
    {
        title: "Интеграции",
        items: [
            {
                label: "Площадки",
                path: "/settings/integrations",
                icon: Plug
            },
            {
                label: "Команда",
                path: "/settings/team",
                icon: Users
            }
        ]
    },
    {
        title: "Дополнительно",
        items: [
            {
                label: "AI",
                path: "/settings/ai",
                icon: Sparkles
            }
        ]
    }
];

function SettingsLayout() {
    return (
        <div className={styles.layout}>
            <aside className={styles.sidebar}>
                <div className={styles.sidebarHeader}>
                    <h1 className={styles.sidebarTitle}>Настройки</h1>
                    <p className={styles.sidebarDescription}>Настрой {APP_NAME} под свой рабочий процесс.</p>
                </div>
                <nav className={styles.nav}>
                    {navigationGroups.map((group) => (
                        <div key={group.title} className={styles.navGroup}>
                            <div className={styles.navGroupTitle}>
                                {group.title}
                            </div>
                            <div className={styles.navItems}>
                                {group.items.map((item) => {
                                    const Icon = item.icon;
                                    return (
                                        <NavLink key={item.path} to={item.path} className={({ isActive }) => [styles.navLink, isActive ? styles.navLinkActive : ""].filter(Boolean).join(" ")}>
                                            <Icon size={17} strokeWidth={1.9}/>
                                            <span>{item.label}</span>
                                        </NavLink>
                                    );
                                })}
                            </div>
                        </div>
                    ))}
                </nav>
            </aside>
            <main className={styles.content}>
                <Outlet />
            </main>
        </div>
    );
}
export default SettingsLayout;