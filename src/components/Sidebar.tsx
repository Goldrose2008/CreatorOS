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
import { APP_NAME } from "../config/appConfig";

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
        <aside className="sidebar">
            <div className="sidebar__brand">
                <span className="sidebar__brand-mark" />
                <span>{APP_NAME}</span>
            </div>
            <nav className="sidebar__nav">
                <div className="sidebar__section-title">Рабочее пространство</div>
                {navigation.map((item) => {
                    const Icon = item.icon;
                    return (
                        <NavLink
                            key={item.path}
                            to={item.path}
                            className={({ isActive }) =>
                                [
                                    "nav-link",
                                    isActive
                                        ? "nav-link--active"
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
                <div className="sidebar__section-title sidebar__section-title--settings">
                    Система
                </div>
                <NavLink to="/settings"
                    className={({ isActive }) =>
                        [
                            "nav-link",
                            isActive
                                ? "nav-link--active"
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