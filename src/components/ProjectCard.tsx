import { useState } from "react";
import { Link } from "react-router-dom";
import type { Project } from "../models/Project";
import Button from "./Button";
import Card from "./Card";

interface Props {
    project: Project;
    onDelete: (id: number) => void;
    onUpdate: (id: number, name: string, description: string) => Promise<void>;
}

function ProjectCard({ project, onDelete, onUpdate }: Props) {
    const [isEditing, setIsEditing] = useState(false);
    const [name, setName] = useState(project.name);
    const [description, setDescription] = useState(project.description ?? "");
    const [isSaving, setIsSaving] = useState(false);

    function cancelEditing() {
        setName(project.name);
        setDescription(project.description ?? "");
        setIsEditing(false);
    }

    async function saveChanges() {
        if (!name.trim()) {
            return;
        }

        try {
            setIsSaving(true);
            await onUpdate(project.id, name.trim(), description);
            setIsEditing(false);
        } 
        finally {
            setIsSaving(false);
        }
    }

    if (isEditing) {
        return (
            <Card className="project-card">
                <div className="project-card__body">
                    <h3 className="project-card__title">Редактирование проекта</h3>
                    <div className="project-card__form">
                        <input className="ui-input" placeholder="Название проекта" value={name} onChange={(e) => setName(e.target.value)}/>
                        <input className="ui-input" placeholder="Описание" value={description} onChange={(e) => setDescription(e.target.value)}/>
                    </div>
                    <div className="project-card__actions">
                        <Button onClick={saveChanges} disabled={isSaving || !name.trim()}>{isSaving ? "Сохранение..." : "Сохранить"}</Button>
                        <Button variant="secondary" onClick={cancelEditing} disabled={isSaving}>Отмена</Button>
                    </div>
                </div>
            </Card>
        );
    }

    return (
        <Card className="project-card">
            <div className="project-card__body">
                <h3 className="project-card__title">
                    <Link className="project-card__link" to={`/projects/${project.id}`}>{project.name}</Link>
                </h3>
                <p className="project-card__description">{project.description || "Описание отсутствует."}</p>
                <div className="project-card__meta">
                    <span>Статус: {project.status}</span>
                </div>
                <div className="project-card__actions">
                    <Link className="ui-button ui-button--secondary" to={`/projects/${project.id}`}>Открыть</Link>
                    <Button variant="secondary" onClick={() => setIsEditing(true)}>Редактировать</Button>
                    <Button variant="danger" onClick={() => onDelete(project.id)}>Удалить</Button>
                </div>
            </div>
        </Card>
    );
}
export default ProjectCard;