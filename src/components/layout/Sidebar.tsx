import {
    BarChart3,
    CalendarDays,
    FolderKanban,
    LayoutDashboard,
    ListTodo,
    Library,
    Settings
} from "lucide-react";
import {NavLink} from "react-router-dom";
import styles from "../../styles/layout/Sidebar.module.css";
import { APP_NAME } from "../../config/appConfig";

const navigation = [
    {
        label: "Главная",
        path: "/",
        icon: LayoutDashboard
    },
    {
        label: "Проекты",
        path: "/projects",
        icon: FolderKanban
    },
    {
        label: "Задачи",
        path: "/tasks",
        icon: ListTodo
    },
    {
        label: "Планирование",
        path: "/planning",
        icon: CalendarDays
    },
    {
        label: "Библиотека",
        path: "/library",
        icon: Library
    },
    {
        label: "Аналитика",
        path: "/analytics",
        icon: BarChart3
    }
];

function Sidebar() {
    return (
        <aside className={styles.sidebar}>
            <div className={styles.brand}>
                <span className={styles.brandMark} />
                <span>{APP_NAME}</span>
            </div>
            <nav className={styles.nav}>
                <div className={styles.sectionTitle}>Рабочее пространство</div>
                {navigation.map((item) => {
                    const Icon = item.icon;
                    return (
                        <NavLink
                            key={item.path}
                            to={item.path}
                            className={({ isActive }) =>
                                [
                                    styles.navLink,
                                    isActive
                                        ? styles.navLinkActive
                                        : ""
                                ]
                                    .filter(Boolean)
                                    .join(" ")
                            }>
                            <Icon size={18} strokeWidth={1.9}/>
                            <span>{item.label}</span>
                        </NavLink>
                    );
                })}
                <div className={styles.sectionTitleSettings}>
                    Система
                </div>
                <NavLink to="/settings"
                    className={({ isActive }) =>
                        [
                            styles.navLink,
                            isActive
                                ? styles.navLinkActive
                                : ""
                        ]
                            .filter(Boolean)
                            .join(" ")
                    }>
                    <Settings size={18} strokeWidth={1.9}/>
                    <span>Настройки</span>
                </NavLink>
            </nav>
        </aside>
    );
}
export default Sidebar;