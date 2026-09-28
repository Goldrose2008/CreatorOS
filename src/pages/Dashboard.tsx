import {
    useEffect,
    useState,
} from "react";
import { Link } from "react-router-dom";
import Card from "../components/ui/Card/Card";
import EmptyState from "../components/ui/EmptyState/EmptyState";
import Workspace from "../components/layout/Workspace/Workspace";
import ProjectSummary from "../components/projects/ProjectSummary";
import type { Project } from "../models/Project";
import { getProjects } from "../services/projectService";
import styles from "./Dashboard.module.css";

function Dashboard() {
    const [projects, setProjects] = useState<Project[]>([]);
    const [loadingProjects, setLoadingProjects] = useState(true);
    const [projectsError, setProjectsError] = useState("");

    useEffect(() => {
        async function loadProjects() {
            try {
                const data = await getProjects();
                const currentProjects = data.filter((project) => project.status !== "archived");
                setProjects(currentProjects);
            }
            catch (error) {
                console.error("Ошибка загрузки проектов для Главной:", error);
                setProjectsError("Не удалось загрузить проекты.");
            }
            finally { setLoadingProjects(false); }
        }

        loadProjects();
    }, []);

    return (
        <Workspace>
            <div className={styles.page}>
                <header className={styles.header}>
                    <h1 className={styles.title}>
                        Главная
                    </h1>
                </header>

                <div className={styles.grid}>
                    <Card className={styles.section}>
                        <div className={styles.sectionHeader}>
                            <div>
                                <h2 className={styles.sectionTitle}>
                                    Сегодня
                                </h2>

                                <p className={styles.sectionDescription}>
                                    Задачи, которые требуют внимания сегодня.
                                </p>
                            </div>
                        </div>

                        <EmptyState
                            title="Здесь будут задачи"
                            description="Задачи на сегодня появятся после реализации раздела задач."
                        />
                    </Card>

                    <Card className={styles.section}>
                        <div className={styles.sectionHeader}>
                            <div>
                                <h2 className={styles.sectionTitle}>
                                    Требует внимания
                                </h2>

                                <p className={styles.sectionDescription}>
                                    События и проблемы, которые требуют реакции.
                                </p>
                            </div>
                        </div>

                        <EmptyState
                            title="Сейчас ничего нет"
                            description="Здесь система будет показывать то, что требует твоего внимания."
                        />
                    </Card>

                    <Card className={styles.section}>
                        <div className={styles.sectionHeader}>
                            <div>
                                <h2 className={styles.sectionTitle}>
                                    Активные проекты
                                </h2>

                                <p className={styles.sectionDescription}>
                                    Проекты, над которыми сейчас ведётся работа.
                                </p>
                            </div>
                            <Link to="/projects" className={styles.sectionLink}>
                                Все проекты
                            </Link>
                        </div>

                        {loadingProjects ? (
                            <EmptyState description="Загрузка проектов..." />
                        ) : projectsError ? (
                            <EmptyState title="Не удалось загрузить проекты" description={projectsError}/>
                        ) : projects.length === 0 ? (
                            <EmptyState title="Активных проектов пока нет" description="Создай проект, чтобы он появился здесь."/>
                        ) : (
                            <div className={styles.projectsList}>
                                {projects.slice(0, 5).map((project) => (
                                    <ProjectSummary key={project.id} project={project}/>
                                ))}
                            </div>
                        )}
                    </Card>

                    <Card className={styles.section}>
                        <div className={styles.sectionHeader}>
                            <div>
                                <h2 className={styles.sectionTitle}>
                                    Ближайшие публикации
                                </h2>

                                <p className={styles.sectionDescription}>
                                    Запланированные публикации на ближайшее время.
                                </p>
                            </div>
                        </div>

                        <EmptyState
                            title="Публикаций пока нет"
                            description="Этот блок будет подключён после реализации публикаций и интеграций."
                        />
                    </Card>
                </div>
            </div>
        </Workspace>
    );
}

export default Dashboard;