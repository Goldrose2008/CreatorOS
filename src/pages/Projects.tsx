import {
    useCallback,
    useEffect,
    useState,
} from "react";
import {
    Button,
    Modal,
    EntityForm,
    EmptyState,
} from "../components/ui";
import { Workspace } from "../components/layout";
import ProjectCard from "../components/projects/ProjectCard";
import type { Project } from "../models/Project";
import {
    PROJECT_FORM_FIELDS,
    type ProjectFormValues,
} from "../config/entities/projectConfig";
import {
    createProject,
    deleteProject,
    getProjects,
    updateProject,
} from "../services/projectService";
import styles from "./Projects.module.css";

function getCreateProjectValues(): ProjectFormValues {
    return {
        name: "",
        description: "",
        planned_release_at: "",
    };
}

function getEditProjectValues(project: Project): ProjectFormValues {
    return {
        name: project.name,
        description: project.description ?? "",
        planned_release_at: project.planned_release_at.slice(0, 10),
    };
}

function Projects() {
    const [projects, setProjects] = useState<Project[]>([]);
    const [loading, setLoading] = useState(true);
    const [createOpen, setCreateOpen] = useState(false);
    const [editingProject, setEditingProject] = useState<Project | null>(null);
    const [saving, setSaving] = useState(false);
    const [formError, setFormError] = useState("");
    const loadProjects = useCallback(async () => {
            const data = await getProjects();
            setProjects(data);
        },
        []
    );

    useEffect(() => {
        async function loadInitialProjects() {

            try { await loadProjects();}
            catch (error) { console.error("Ошибка загрузки проектов:", error); }
            finally { setLoading(false); }
        }

        loadInitialProjects();

    }, [loadProjects]);

    function openCreateModal() {
        setFormError("");
        setCreateOpen(true);
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

    async function handleCreate(values: ProjectFormValues) {
        try {
            setSaving(true);
            setFormError("");

            await createProject(
                values.name.trim(),
                values.description.trim(),
                values.planned_release_at
            );

            await loadProjects();
            setCreateOpen(false);
        }
        catch (error) {
            console.error("Ошибка создания проекта:", error);
            setFormError("Не удалось создать проект.");

        }
        finally { setSaving(false); }
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

    async function handleDelete(id: number) {
        const project = projects.find((item) => item.id === id);
        if (!project) { return; }

        const confirmed = window.confirm(`Удалить проект "${project.name}"?`);
        if (!confirmed) { return; }

        try {
            await deleteProject(id);
            await loadProjects();
        }
        catch (error) { console.error("Ошибка удаления проекта:", error); }
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
                    <Button onClick={openCreateModal}>
                        Создать новый проект
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
                <Modal
                    open={createOpen}
                    title="Новый проект"
                    onClose={closeCreateModal}
                >
                    <EntityForm<ProjectFormValues>
                        key="create-project"
                        fields={PROJECT_FORM_FIELDS}
                        initialValues={getCreateProjectValues()}
                        submitLabel="Создать проект"
                        saving={saving}
                        error={formError}
                        onSubmit={handleCreate}
                        onCancel={closeCreateModal}
                    />
                </Modal>

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
            </div>
        </Workspace>
    );
}

export default Projects;