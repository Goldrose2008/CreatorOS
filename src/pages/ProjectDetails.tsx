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
import ContentCard from "../components/content/ContentCard/ContentCard";
import Button from "../components/ui/primitives/Button";
import Card from "../components/ui/layout/Card";
import Modal from "../components/ui/overlays/Modal";
import EntityForm from "../components/ui/forms/EntityForm";
import EmptyState from "../components/ui/states/EmptyState";
import ConfirmModal from "../components/ui/overlays/ConfirmModal";
import Workspace from "../components/layout/Workspace/Workspace";
import EntityHeader from "../components/entity/EntityHeader/EntityHeader";
import type { Project, ProjectStatus } from "../models/Project";
import type { Content, ContentRole } from "../models/Content";
import type { ContentType } from "../models/ContentType";
import {
    PROJECT_FORM_FIELDS,
    PROJECT_STATUS_ACTIONS,
    getProjectStatusConfig,
    type ProjectFormValues
} from "../config/entities/projectConfig";
import {
    getContentFormFields,
    type ContentFormValues,
} from "../config/entities/contentConfig";
import {
    getProjectById,
    updateProject,
    updateProjectStatus,
} from "../services/projectService";
import { getContentTypes } from "../services/contentTypeService";
import {
    createContent,
    deleteContent,
    getProjectContent,
    updateContent,
} from "../services/contentService";
import styles from "./ProjectDetails.module.css";
import { formatDate } from "../utils/date";

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
    const [content, setContent] = useState<Content[]>([]);
    const [contentTypes, setContentTypes] = useState<ContentType[]>([]);
    const [contentLoading, setContentLoading] = useState(true);
    const [contentError, setContentError] = useState("");
    const [contentSaving, setContentSaving] = useState(false);
    const [createContentRole, setCreateContentRole] = useState<ContentRole | null>(null);
    const [editingContent, setEditingContent] = useState<Content | null>(null);
    const [deletingContent, setDeletingContent] = useState<Content | null>(null);
    const [contentDeleteSaving, setContentDeleteSaving] = useState(false);
    const [contentFormError, setContentFormError] = useState("");

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

        useEffect(() => {
        async function loadContentData() {
            if (!id) { return; }

            const projectId = Number(id);
            if (!Number.isInteger(projectId)) { return; }

            try {
                setContentLoading(true);
                setContentError("");

                const [
                    projectContent,
                    types,
                ] = await Promise.all([
                    getProjectContent(projectId),
                    getContentTypes(),
                ]);

                setContent(projectContent);
                setContentTypes(types);
            }
            catch (loadError) {
                console.error("Ошибка загрузки контента проекта:", loadError);
                setContentError("Не удалось загрузить контент проекта.");
            }
            finally { setContentLoading(false); }
        }

        loadContentData();
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
    const mainContent = content.find((item) => item.content_role === "main");
    const additionalContent = content.filter((item) => item.content_role === "additional");
    const contentFormFields = getContentFormFields(contentTypes);

    function getContentType(contentItem: Content): ContentType | undefined {
        return contentTypes.find((type) => type.id === contentItem.content_type_id);
    }

    function openCreateContent(role: ContentRole) {
        setContentFormError("");
        setCreateContentRole(role);
    }

    function closeCreateContent() {
        if (contentSaving) { return; }

        setContentFormError("");
        setCreateContentRole(null);
    }

    function openEditContent(contentItem: Content) {
        setContentFormError("");
        setEditingContent(contentItem);
    }

    function closeEditContent() {
        if (contentSaving) { return; }

        setContentFormError("");
        setEditingContent(null);
    }

    function getCreateContentValues(): ContentFormValues {
        return {
            contentTypeId: contentTypes[0]?.id ?? 0,
            name: "",
            description: "",
        };
    }

    function getEditContentValues(contentItem: Content): ContentFormValues {
        return {
            contentTypeId: contentItem.content_type_id,
            name: contentItem.name,
            description: contentItem.description ?? "",
        };
    }

    async function handleCreateContent(values: ContentFormValues) {
        if (!project || !createContentRole) { return; }

        try {
            setContentSaving(true);
            setContentFormError("");

            await createContent(
                project.id,
                values.contentTypeId,
                createContentRole,
                values.name.trim(),
                values.description.trim()
            );

            const updatedContent = await getProjectContent(project.id);

            setContent(updatedContent);
            setCreateContentRole(null);
        }
        catch (createError) {
            console.error("Ошибка создания контента:", createError);
            setContentFormError("Не удалось создать контент.");
        }
        finally { setContentSaving(false); }
    }

    async function handleEditContent(values: ContentFormValues) {
        if (!project || !editingContent) { return; }

        try {
            setContentSaving(true);
            setContentFormError("");

            await updateContent(
                editingContent.id,
                values.contentTypeId,
                values.name.trim(),
                values.description.trim()
            );

            const updatedContent = await getProjectContent(project.id);

            setContent(updatedContent);
            setEditingContent(null);
        }
        catch (updateError) {
            console.error("Ошибка обновления контента:", updateError);
            setContentFormError("Не удалось сохранить изменения.");
        }
        finally { setContentSaving(false); }
    }

    function handleDeleteContent(contentItem: Content) {
        if (contentItem.content_role === "main") { return; }
        setDeletingContent(contentItem);
    }

    function closeDeleteContentModal() {
        if (contentDeleteSaving) { return; }
        setDeletingContent(null);
    }

    async function handleDeleteContentConfirm() {
        if (!deletingContent) { return; }

        const projectId = deletingContent.project_id;

        try {
            setContentDeleteSaving(true);
            await deleteContent(deletingContent.id);
            const updatedContent = await getProjectContent(projectId);
            setContent(updatedContent);
            setDeletingContent(null);
        }
        catch (deleteError) { console.error("Ошибка удаления контента:", deleteError); }
        finally { setContentDeleteSaving(false); }
    }

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
                            {!mainContent && (
                                <div className={styles.sectionHeaderActions}>
                                    <Button onClick={() => openCreateContent("main")} disabled={contentTypes.length === 0}>
                                        Добавить
                                    </Button>
                                </div>
                            )}
                        </div>

                        {contentLoading ? (
                            <EmptyState description="Загрузка контента..."/>
                        ) : contentError ? (
                            <EmptyState title="Не удалось загрузить контент" description={contentError}/>
                        ) : mainContent ? (
                            <div className={styles.contentList}>
                                {getContentType(mainContent) ? (
                                    <ContentCard
                                        content={mainContent}
                                        contentType={getContentType(mainContent)!}
                                        canDelete={false}
                                        onEdit={openEditContent}
                                        onDelete={handleDeleteContent}
                                    />
                                ) : (
                                    <EmptyState
                                        title="Неизвестный тип контента"
                                        description="Тип контента отсутствует в справочнике."
                                    />
                                )}
                            </div>
                        ) : (
                            <div className={styles.sectionEmpty}>
                                <p>
                                    Основной контент проекта пока не создан.
                                </p>
                            </div>
                        )}  
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
                            <div className={styles.sectionHeaderActions}>
                                <Button variant="secondary" onClick={() => openCreateContent("additional")} disabled={contentTypes.length === 0}>
                                    Добавить
                                </Button>
                            </div>
                        </div>

                        {contentLoading ? (
                            <EmptyState description="Загрузка контента..."/>
                        ) : contentError ? (
                            <EmptyState title="Не удалось загрузить контент" description={contentError}/>
                        ) : additionalContent.length > 0 ? (
                            <div className={styles.contentList}>
                                {additionalContent.map((contentItem) => {
                                    const contentType = getContentType(contentItem);

                                    if (!contentType) {
                                        return (
                                            <EmptyState
                                                key={contentItem.id}
                                                title="Неизвестный тип контента"
                                                description="Тип контента отсутствует в справочнике."
                                            />
                                        );
                                    }

                                    return (
                                        <ContentCard
                                            key={contentItem.id}
                                            content={contentItem}
                                            contentType={contentType}
                                            onEdit={openEditContent}
                                            onDelete={handleDeleteContent}
                                        />
                                    );
                                })}
                            </div>
                        ) : (
                            <div className={styles.sectionEmpty}>
                                <p>
                                    Дополнительного контента пока нет.
                                </p>
                            </div>
                        )}
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
                    open={createContentRole !== null}
                    title={
                        createContentRole === "main"
                            ? "Новый основной контент"
                            : "Новый дополнительный контент"
                    }
                    onClose={closeCreateContent}
                >
                    <EntityForm<ContentFormValues>
                        key={`create-content-${createContentRole}`}
                        fields={contentFormFields}
                        initialValues={getCreateContentValues()}
                        submitLabel="Добавить"
                        saving={contentSaving}
                        error={contentFormError}
                        onSubmit={handleCreateContent}
                        onCancel={closeCreateContent}
                    />
                </Modal>
                <Modal
                    open={editingContent !== null}
                    title="Редактирование контента"
                    onClose={closeEditContent}
                >
                    {editingContent && (
                        <EntityForm<ContentFormValues>
                            key={`edit-content-${editingContent.id}`}
                            fields={contentFormFields}
                            initialValues={getEditContentValues(editingContent)}
                            submitLabel="Сохранить"
                            saving={contentSaving}
                            error={contentFormError}
                            onSubmit={handleEditContent}
                            onCancel={closeEditContent}
                        />
                    )}
                </Modal>
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
                
                <ConfirmModal
                    open={deletingContent !== null}
                    title="Удаление контента"
                    message={
                        <>
                            Удалить контент{" "}
                            <strong>«{deletingContent?.name}»</strong>?
                            <br />
                            Это действие нельзя отменить.
                        </>
                    }
                    saving={contentDeleteSaving}
                    onConfirm={handleDeleteContentConfirm}
                    onCancel={closeDeleteContentModal}
                />

            </div>
        </Workspace>
    );
}

export default ProjectDetails;