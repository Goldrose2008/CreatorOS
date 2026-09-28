import {
    useEffect,
    useState,
} from "react";
import {
    Archive,
    ArrowLeft,
    CalendarDays,
    FileText,
    ListTodo,
    Play,
    RotateCcw,
    UserRound,
    Video,
} from "lucide-react";
import { Link, useParams } from "react-router-dom";
import {
    Button,
    Card,
    Modal,
    EntityForm,
    EmptyState,
} from "../components/ui";
import { Workspace,} from "../components/layout";
import { EntityHeader } from "../components/entity";
import type { 
    Project,
    ProjectStatus
 } from "../models/Project";
import {
    PROJECT_FORM_FIELDS,
    PROJECT_STATUS_ACTIONS,
    getProjectStatusConfig,
    type ProjectFormValues
} from "../config/entities/projectConfig";
import {
    getProjectById,
    updateProject,
    updateProjectStatus,
} from "../services/projectService";
import styles from "./ProjectDetails.module.css";

function formatDate(value?: string | null): string {
    if (!value) { return "Не указана"; }

    const datePart = value.slice(0, 10);
    const [year, month, day] = datePart.split("-");

    if (!year || !month || !day) { return value; }
    return `${day}.${month}.${year}`;
}

function getResponsibleLabel(ownerId?: number | null): string {
    if (ownerId === undefined || ownerId === null) { return "Не назначен"; }
    return `Пользователь #${ownerId}`;
}

function getEditValues(project: Project): ProjectFormValues {
    return {
        name: project.name,
        description: project.description ?? "",
        planned_release_at: project.planned_release_at.slice(0, 10),
    };
}

function ProjectDetails() {
    const { id } = useParams<{ id: string }>();
    const [project, setProject] = useState<Project | null>(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState("");
    const [editOpen, setEditOpen] = useState(false);
    const [saving, setSaving] = useState(false);
    const [formError, setFormError] = useState("");

    useEffect(() => {
        async function loadProject() {
            if (!id) {
                setError("Идентификатор проекта не указан.");
                setLoading(false);
                return;
            }

            const projectId = Number(id);

            if (!Number.isInteger(projectId)) {
                setError("Некорректный идентификатор проекта.");
                setLoading(false);
                return;
            }

            try {
                const data = await getProjectById(projectId);

                setProject(data);
                if (!data) { setError("Проект не найден."); }
            }
            catch (loadError) {
                console.error("Ошибка загрузки проекта:", loadError);
                setError("Не удалось загрузить проект.");
            }
            finally { setLoading(false); }
        }
        loadProject();
    }, [id]);

    function openEditModal() {
        if (!project) { return; }
        setFormError("");
        setEditOpen(true);
    }

    function closeEditModal() {
        if (saving) { return; }
        setFormError("");
        setEditOpen(false);
    }

    async function handleSave(values: ProjectFormValues) {
        if (!project) { return;}

        try {
            setSaving(true);
            setFormError("");

            await updateProject(
                project.id,
                values.name.trim(),
                values.description.trim(),
                values.planned_release_at
            );

            const updatedProject = await getProjectById(project.id);

            setProject(updatedProject);
            setEditOpen(false);
        }
        catch (saveError) {
            console.error("Ошибка сохранения проекта:", saveError);
            setFormError("Не удалось сохранить изменения.");
        }
        finally { setSaving(false); }
    }

    async function handleStatusChange(newStatus: ProjectStatus) {
        if (!project) { return; }

        try {
            setSaving(true);
            setFormError("");

            await updateProjectStatus(
                project.id,
                newStatus
            );

            const updatedProject = await getProjectById(project.id);
            setProject(updatedProject);
        }
        catch (statusError) {
            console.error("Ошибка изменения статуса проекта:", statusError);
            setFormError("Не удалось изменить статус проекта.");
        }
        finally { setSaving(false); }
    }

    if (loading) {
        return (
            <Workspace>
                <div className={styles.page}>
                    <EmptyState description="Загрузка проекта..."/>
                </div>
            </Workspace>
        );
    }

    if (error || !project) {
        return (
            <Workspace navigation={
              <Link className={styles.backLink} to="/projects">
                <ArrowLeft size={16} /> 
                Вернуться к проектам
              </Link>}>
                <div className={styles.page}>                    
                    <EmptyState title="Проект не найден" description={error || "Проект не найден."}/>
                </div>
            </Workspace>
        );
    }

    const status = getProjectStatusConfig(project.status);
    const statusActions = PROJECT_STATUS_ACTIONS[project.status];

    return (
        <Workspace navigation={
            <Link className={styles.backLink} to="/projects">
                <ArrowLeft size={16} />
                Вернуться к проектам
            </Link>}>           
            <div className={styles.page}>
                <EntityHeader
                    title={project.name}
                    description={project.description}
                    status={{
                        label: status.label,
                        variant: status.variant,
                    }}
                    meta={[
                        <span key="progress">
                            Прогресс:{" "}
                            {project.progress}%
                        </span>,

                        <span key="release">
                            <CalendarDays size={15} />
                            Планируемая дата выхода:{" "}
                            {formatDate(project.planned_release_at)}
                        </span>,

                        <span key="owner">
                            <UserRound size={15} />
                            Ответственный:{" "}
                            {getResponsibleLabel(project.owner_id)}
                        </span>,

                        <span key="created">
                            <CalendarDays size={15} />
                            Создано:{" "}
                            {formatDate(project.created_at)}
                        </span>,

                        <span key="updated">
                            <CalendarDays size={15} />
                            Изменено:{" "}
                            {formatDate(project.updated_at)}
                        </span>,
                    ]}
                    actions={
                        <>
                            <Button onClick={openEditModal} disabled={saving}>
                                Редактировать
                            </Button>

                            {statusActions.map(
                                (action) => {
                                    let icon;

                                    switch (action.action) {
                                        case "activate":
                                            icon = (<Play size={16}/>);
                                            break;
                                        case "archive":
                                            icon = (<Archive size={16}/>);
                                            break;
                                        case "restore":
                                            icon = (<RotateCcw size={16}/>);
                                            break;
                                    }

                                    return (
                                        <Button key={action.action} variant="secondary" onClick={() => handleStatusChange(action.nextStatus)} disabled={saving}>
                                            {icon}
                                            {action.label}
                                        </Button>
                                    );
                                }
                            )}
                        </>
                    }
                />
                {formError && (
                    <div className={styles.error}>
                        {formError}
                    </div>
                )}

                <div className={styles.grid}>
                    <Card className={styles.section}>
                        <div className={styles.sectionHeader}>
                            <div className={styles.sectionIcon}>
                                <Video size={18} />
                            </div>
                            <div>
                                <h2>
                                    Основной контент
                                </h2>
                                <p>
                                    Главный результат проекта.
                                </p>
                            </div>
                        </div>
                        <div className={styles.sectionEmpty}>
                            <p>
                                Основной контент проекта пока не создан.
                            </p>
                            <Button>
                                Добавить контент
                            </Button>
                        </div>
                    </Card>
                    <Card className={styles.section}>
                        <div className={styles.sectionHeader}>
                            <div className={styles.sectionIcon}>
                                <FileText size={18} />
                            </div>
                            <div>
                                <h2>
                                    Дополнительный контент
                                </h2>
                                <p>
                                    Материалы, связанные с основным контентом.
                                </p>
                            </div>
                        </div>
                        <div className={styles.sectionEmpty}>
                            <p>
                                Дополнительного контента пока нет.
                            </p>
                            <Button variant="secondary">
                                Добавить
                            </Button>
                        </div>
                    </Card>
                    <Card className={styles.section}>
                        <div className={styles.sectionHeader}>
                            <div className={styles.sectionIcon}>
                                <ListTodo size={18} />
                            </div>
                            <div>
                                <h2>
                                    Задачи
                                </h2>
                                <p>
                                    Здесь появятся задачи проекта.
                                </p>
                            </div>
                        </div>
                        <div className={styles.sectionEmpty}>
                            <p>
                                Задач пока нет.
                            </p>
                            <Button variant="secondary">
                                Добавить задачу
                            </Button>
                        </div>
                    </Card>
                </div>
                <Modal
                    open={editOpen}
                    title="Редактирование проекта"
                    onClose={closeEditModal}
                >
                    <EntityForm<ProjectFormValues>
                        key={`edit-project-${project.id}`}
                        fields={PROJECT_FORM_FIELDS}
                        initialValues={getEditValues(project)}
                        submitLabel="Сохранить"
                        saving={saving}
                        error={formError}
                        onSubmit={handleSave}
                        onCancel={closeEditModal}
                    />
                </Modal>
            </div>
        </Workspace>
    );
}

export default ProjectDetails;