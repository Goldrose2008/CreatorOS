import {
    useCallback,
    useEffect,
    useState,
} from "react";
import Button from "../components/ui/Button/Button";
import Modal from "../components/ui/Modal/Modal";
import EntityForm from "../components/ui/EntityForm/EntityForm";
import EmptyState from "../components/ui/EmptyState/EmptyState";
import ConfirmModal from "../components/ui/ConfirmModal/ConfirmModal";
import Workspace from "../components/layout/Workspace/Workspace";
import ProjectCard from "../components/projects/ProjectCard/ProjectCard";
import type { Project } from "../models/Project";
import type { ContentType } from "../models/ContentType";
import { getContentTypes } from "../services/contentTypeService";
import { createContent } from "../services/contentService";
import {
    PROJECT_FORM_FIELDS,
    getProjectCreateFormFields,
    type ProjectCreateFormValues,
    type ProjectFormValues,
} from "../config/entities/projectConfig";
import {
    createProject,
    deleteProject,
    getProjects,
    updateProject,
} from "../services/projectService";
import styles from "./ProjectsPage.module.css";

function getCreateProjectValues(contentTypes: ContentType[]): ProjectCreateFormValues {
    return {
        name: "",
        description: "",
        planned_release_at: "",
        mainContentTypeId: contentTypes[0]?.id ?? 0,
        mainContentName: "",
    };
}

function getEditProjectValues(project: Project): ProjectFormValues {
    return {
        name: project.name,
        description: project.description ?? "",
        planned_release_at: project.planned_release_at.slice(0, 10),
    };
}

function ProjectsPage() {
    const [projects, setProjects] = useState<Project[]>([]);
    const [loading, setLoading] = useState(true);
    const [createOpen, setCreateOpen] = useState(false);
    const [editingProject, setEditingProject] = useState<Project | null>(null);
    const [saving, setSaving] = useState(false);
    const [formError, setFormError] = useState("");
    const [contentTypes, setContentTypes] = useState<ContentType[]>([]);
    const [loadingContentTypes, setLoadingContentTypes] = useState(false);
    const [syncMainContentName, setSyncMainContentName] = useState(true);
    const [deletingProject, setDeletingProject] = useState<Project | null>(null);
    const [deleteSaving, setDeleteSaving] = useState(false);
    const loadProjects = useCallback(async () => {
        const data = await getProjects(); setProjects(data); }, []);

    useEffect(() => {
        async function loadInitialProjects() {

            try { await loadProjects();}
            catch (error) { console.error("Ошибка загрузки проектов:", error); }
            finally { setLoading(false); }
        }

        loadInitialProjects();

    }, [loadProjects]);

    async function openCreateModal() {
        setSyncMainContentName(true);
        setFormError("");

        try {
            setLoadingContentTypes(true);
            const types = await getContentTypes();
            setContentTypes(types);
            
            if (types.length === 0) {
                setFormError("Невозможно создать проект: нет типов контента.");
                return;
            }

            setCreateOpen(true);
        }
        catch (error) {
            console.error("Ошибка загрузки типов контента:", error);
            setFormError("Не удалось загрузить типы контента.");
        }
        finally { setLoadingContentTypes(false); }
    }

    function closeCreateModal() {
        if (saving) { return; }
        setFormError("");
        setCreateOpen(false);
    }

    function openEditModal(project: Project) {
        setFormError("");
        setEditingProject(project);
    }

    function closeEditModal() {
        if (saving) { return; }
        setFormError("");
        setEditingProject(null);
    }

    function handleCreateProjectFieldChange(
    fieldName: keyof ProjectCreateFormValues & string,
    value: unknown
): Partial<ProjectCreateFormValues> | undefined {
    if (fieldName === "name" && syncMainContentName) {
        return { mainContentName: String(value ?? ""), };
    }

    if (fieldName === "mainContentName") { setSyncMainContentName(false); }

    return undefined;
}

    async function handleCreate(values: ProjectCreateFormValues) {
    try {
        setSaving(true);
        setFormError("");

        const projectId = await createProject(
            values.name.trim(),
            values.description.trim(),
            values.planned_release_at
        );

        try {
            await createContent(
                projectId,
                values.mainContentTypeId,
                "main",
                values.mainContentName.trim(),
                ""
            );
        }
        catch (contentError) {
            console.error("Ошибка создания основного контента:", contentError);
            await deleteProject(projectId);
            throw contentError;
        }

        await loadProjects();
        setCreateOpen(false);
    }
    catch (error) {
        console.error("Ошибка создания проекта:", error);
        setFormError("Не удалось создать проект.");
    }
    finally {setSaving(false);}
}

    async function handleEdit(values: ProjectFormValues) {
        if (!editingProject) { return; }
        try {
            setSaving(true);
            setFormError("");

            await updateProject(
                editingProject.id,
                values.name.trim(),
                values.description.trim(),
                values.planned_release_at
            );

            await loadProjects();
            setEditingProject(null);
        }
        catch (error) {
            console.error("Ошибка обновления проекта:", error);
            setFormError("Не удалось сохранить изменения.");
        }
        finally { setSaving(false); }
    }

    function handleDelete(id: number) {
        const project = projects.find((item) => item.id === id);

        if (!project) { return; }

        setDeletingProject(project);
    }

    function closeDeleteModal() {
        if (deleteSaving) { return; }
        setDeletingProject(null);
    }

    async function handleDeleteConfirm() {
        if (!deletingProject) { return; }

        try {
            setDeleteSaving(true);
            await deleteProject(deletingProject.id);
            await loadProjects();
            setDeletingProject(null);
        }
        catch (error) { console.error("Ошибка удаления проекта:", error); }
        finally { setDeleteSaving(false); }
    }

    return (
        <Workspace>
            <div className={styles.page}>
                <header className={styles.header}>
                    <div>
                        <h1 className={styles.title}>
                            Проекты
                        </h1>
                    </div>
                    <Button onClick={openCreateModal} disabled={loadingContentTypes}>
                        {loadingContentTypes ? "Загрузка..." : "Создать новый проект"}
                    </Button>
                </header>

                <section className={styles.list}>
                    {loading ? (
                        <EmptyState 
                            description="Загрузка проектов..."
                        />
                    ) : projects.length === 0 ? (
                        <EmptyState
                            title="Проектов пока нет"
                            description="Создай первый проект, чтобы начать работу."
                        />
                    ) : (
                        projects.map((project) => (
                            <ProjectCard
                                key={project.id}
                                project={project}
                                onEdit={openEditModal}
                                onDelete={handleDelete}
                            />
                        ))
                    )}
                </section>
        {/* Открытие проекта */}
                <Modal
                    open={createOpen}
                    title="Новый проект"
                    onClose={closeCreateModal}
                >
                    <EntityForm<ProjectCreateFormValues>
                        key="create-project"
                        fields={getProjectCreateFormFields(contentTypes)}
                        initialValues={getCreateProjectValues(contentTypes)}
                        submitLabel="Создать проект"
                        saving={saving}
                        error={formError}
                        onSubmit={handleCreate}
                        onFieldChange={handleCreateProjectFieldChange}
                        onCancel={closeCreateModal}
                    />
                </Modal>
        {/* Редактирование проекта */}
                <Modal
                    open={editingProject !== null}
                    title="Редактирование проекта"
                    onClose={closeEditModal}
                >
                    {editingProject && (
                        <EntityForm<ProjectFormValues>
                            key={`edit-project-${editingProject.id}`}
                            fields={PROJECT_FORM_FIELDS}
                            initialValues={getEditProjectValues(editingProject)}
                            submitLabel="Сохранить"
                            saving={saving}
                            error={formError}
                            onSubmit={handleEdit}
                            onCancel={closeEditModal}
                        />
                    )}
                </Modal>
        {/* Окно-запрос подтверждения удаления */}
                <ConfirmModal
                    open={deletingProject !== null}
                    title="Удаление проекта"
                    message={
                        <>
                            Удалить проект{" "}
                            <strong>«{deletingProject?.name}»</strong>?
                            <br />
                            Это действие нельзя отменить.
                        </>
                    }
                    saving={deleteSaving}
                    onConfirm={handleDeleteConfirm}
                    onCancel={closeDeleteModal}
                />
            </div>
        </Workspace>
    );
}

export default ProjectsPage;