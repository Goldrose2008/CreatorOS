import Card from "../components/ui/Card/Card";
import EmptyState from "../components/ui/EmptyState/EmptyState";
import Workspace from "../components/layout/Workspace/Workspace";
import styles from "./Dashboard.module.css";

function Dashboard() {
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
                        </div>

                        <EmptyState
                            title="Здесь будут активные проекты"
                            description="Проекты в работе будут отображаться здесь."
                        />
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