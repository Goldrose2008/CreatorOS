import { useEffect, useState } from "react";
import {
    ArrowLeft, 
    CalendarDays, 
    FileText, 
    ListTodo, 
    UserRound, 
    Video,
} from "lucide-react";
import { Link, useParams } from "react-router-dom";
import Button from "../components/Button";
import Card from "../components/Card";
import type { Project } from "../models/Project";
import { getProjectById, updateProject } from "../services/projectService";

function formatDate(value?: string | null): string {
    if (!value) {
        return "Не указана";
    }

    const datePart = value.slice(0, 10);
    const [year, month, day] = datePart.split("-");

    if (!year || !month || !day) {
        return value;
    }

    return `${day}.${month}.${year}`;
}

function getProjectStatusLabel(status: string): string {
    switch (status) {
        case "active":
            return "В работе";
        case "draft":
            return "Черновик";
        case "archived":
            return "Архив";
        default:
            return status;
    }
}

function getResponsibleLabel(ownerId?: number | null): string {
    if (ownerId === undefined || ownerId === null) {
        return "Не назначен";
    }

    return `Пользователь #${ownerId}`;
}

function getDateInputValue(value?: string | null): string {
    if (!value) {
        return "";
    }

    return value.slice(0, 10);
}

function ProjectDetails() {
    const { id } = useParams<{ id: string }>();
    const [project, setProject] = useState<Project | null>(null);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState("");
    const [isEditing, setIsEditing] = useState(false);
    const [saving, setSaving] = useState(false);
    const [formError, setFormError] = useState("");
    const [name, setName] = useState("");
    const [description, setDescription] = useState("");
    const [status, setStatus] = useState("active");
    const [plannedReleaseAt, setPlannedReleaseAt] = useState("");

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

                if (!data) {
                    setError("Проект не найден.");
                }
            } 
            catch (error) {
                console.error("Ошибка загрузки проекта:", error);
                setError("Не удалось загрузить проект.");
            } 
            finally {
                setLoading(false);
            }
        }

        loadProject();
    }, [id]);

    function startEditing() {
        if (!project) {
            return;
        }

        setName(project.name);
        setDescription(project.description || "");
        setStatus(project.status);
        setPlannedReleaseAt(getDateInputValue(project.planned_release_at));
        setFormError("");
        setIsEditing(true);
    }

    function cancelEditing() {
        setFormError("");
        setIsEditing(false);
    }

    async function handleSave() {
        if (!project) {
            return;
        }

        const trimmedName = name.trim();

        if (!trimmedName) {
            setFormError("Название проекта не может быть пустым.");
            return;
        }

        try {
            setSaving(true);
            setFormError("");

            await updateProject(
                project.id,
                trimmedName,
                description.trim(),
                status,
                plannedReleaseAt || null
            );

            const updatedProject = await getProjectById(project.id);

            setProject(updatedProject);
            setIsEditing(false);
        } 
        catch (error) {
            console.error("Ошибка сохранения проекта:", error);
            setFormError("Не удалось сохранить изменения.");
        } 
        finally {
            setSaving(false);
        }
    }

    if (loading) {
        return (
            <div className="page">
                <div className="empty-state">Загрузка проекта...</div>
            </div>
        );
    }

    if (error || !project) {
        return (
            <div className="page">
                <Link className="page-back-link" to="/projects"><ArrowLeft size={16} /> Вернуться к проектам</Link>
                <div className="empty-state">
                    <h2>Проект не найден</h2>
                    <p>{error || "Проект не найден."}</p>
                </div>
            </div>
        );
    }

    return (
        <div className="page">
            <Link className="page-back-link" to="/projects"><ArrowLeft size={16} /> Вернуться к проектам</Link>
{/* Форма редактирования */}
            {isEditing ? (
                <Card className="project-edit-card">
                    <div className="project-edit-card__header">
                        <div>
                            <h1 className="page-title">Редактирование проекта</h1>
                            <p className="page-description">Измените основные параметры проекта.</p>
                        </div>
                    </div>
                    <form className="project-edit-form" onSubmit={(event) => {event.preventDefault(); handleSave();}}>
                        <div className="project-edit-form__grid">
    {/* Редактирование названия */}
                            <div className="project-form-field project-form-field--full">
                                <label className="project-form-label" htmlFor="project-name">Название</label>
                                <input id="project-name" className="ui-input" type="text" value={name} onChange={(event) => setName(event.target.value)} disabled={saving}/>
                            </div>
    {/* Редактирование описания */}
                            <div className="project-form-field project-form-field--full">
                                <label className="project-form-label" htmlFor="project-description">Описание</label>
                                <textarea id="project-description" className="ui-textarea" value={description} onChange={(event) => setDescription(event.target.value)} rows={4} disabled={saving}/>
                            </div>
    {/* Редактирование статуса проекта */}            
                            <div className="project-form-field">
                                <label className="project-form-label" htmlFor="project-status">Статус</label>
                                <select id="project-status" className="ui-select" value={status} onChange={(event) => setStatus(event.target.value)} disabled={saving}>
                                    <option value="active">Активен</option>
                                    <option value="draft">Черновик</option>
                                    <option value="archived">Архив</option>
                                </select>
                            </div>
    {/* Редактирование даты публикации */}            
                            <div className="project-form-field">
                                <label className="project-form-label" htmlFor="project-release-date">Планируемая дата выхода</label>
                                <input id="project-release-date" className="ui-input" type="date" value={plannedReleaseAt} onChange={(event) => setPlannedReleaseAt(event.target.value)} disabled={saving}/>
                            </div>
                        </div>
                {/* Форма ошибки */}
                        {formError && (
                            <div className="project-form-error">{formError}</div>
                        )}
    {/* Кнопки управления*/}    
                        <div className="project-edit-form__actions">
                            <Button type="submit" disabled={saving}>{saving ? "Сохранение..." : "Сохранить"}</Button>
                            <Button type="button" variant="secondary" onClick={cancelEditing} disabled={saving}>Отмена</Button>
                        </div>
                    </form>
                </Card>
            ) : (
                <header className="project-details-header">
                    <div className="project-details-header__main">
                        <h1 className="page-title">{project.name}</h1>
                        <p className="page-description">{project.description || "Описание проекта отсутствует."}</p>
                        <div className="project-details-meta">
                {/* Статус */}
                        <span className="project-meta">
                            <span className="project-status-dot"/>
                            {getProjectStatusLabel(project.status)}
                        </span>
                {/* Планируемая дата выхода */}
                        <span className="project-meta">
                            <CalendarDays size={15} />
                            Планируемая дата выхода:{" "}{formatDate(project.planned_release_at)}
                        </span>
                {/* Ответственный */}
                        <span className="project-meta">
                            <UserRound size={15} />
                            Ответственный:{" "}{getResponsibleLabel(project.owner_id)}
                        </span>
                {/* Дата создания */}
                        <span className="project-meta">
                            <CalendarDays size={15} />
                            Создано:{" "}{formatDate(project.created_at)}
                        </span>
                    </div>
                </div>
        {/* Кнопки управления */}    
                <div className="project-details-header__actions">
                    <Button onClick={startEditing}> Редактировать </Button>
                </div>
            </header>
        )}
            <div className="project-details-grid">
                <Card className="project-section">
                    <div className="project-section__header">
                        <div className="project-section__icon">
                            <Video size={18} />
                        </div>
                        <div>
                            <h2>Основной контент</h2>
                            <p>Главный результат проекта.</p>
                        </div>
                    </div>
                    <div className="project-section__empty">
                        <p>Основной контент проекта пока не создан.</p>
                        <Button>Добавить контент</Button>
                    </div>
                </Card>
                <Card className="project-section">
                    <div className="project-section__header">
                        <div className="project-section__icon">
                            <FileText size={18} />
                        </div>
                        <div>
                            <h2>Дополнительный контент</h2>
                            <p>Материалы, связанные с основным контентом.</p>
                        </div>
                    </div>
                    <div className="project-section__empty">
                        <p>Дополнительного контента пока нет.</p>
                        <Button variant="secondary">Добавить</Button>
                    </div>
                </Card>
                 <Card className="project-section">
                    <div className="project-section__header">
                        <div className="project-section__icon">
                            <ListTodo size={18} />
                        </div>
                        <div>
                            <h2>Задачи</h2>
                            <p>Здесь появятся задачи проекта.</p>
                        </div>
                    </div>
                    <div className="project-section__empty">
                        <p>Задач пока нет.</p>
                        <Button variant="secondary">Добавить задачу</Button>
                    </div>
                 </Card>
            </div>  
        </div>
    );
}
export default ProjectDetails;