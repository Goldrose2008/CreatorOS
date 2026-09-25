import { useEffect, useState } from "react";
import {getProjects, createProject, deleteProject, updateProject} from "../services/projectService";
import type { Project } from "../models/Project";
import ProjectCard from "../components/ProjectCard";
import Button from "../components/Button";

function Projects() {
    const [projects, setProjects] = useState<Project[]>([]);
    const [name, setName] = useState("");
    const [description, setDescription] = useState("");

    async function loadProjects() {
        const data = await getProjects();
        setProjects(data);
    }

    async function addProject() {
      console.log("Создание проекта:", name, description);

      if (!name.trim()) {
         console.log("Название пустое");
         return;
      }
      try {
          await createProject(name, description);

         console.log("Проект создан");

          setName("");
          setDescription("");

          await loadProjects();

          console.log("Список обновлён");

      } 
      catch (error) {

        console.error("Ошибка создания проекта:", error);

      }
    }

    async function removeProject(id: number) {
        const project = projects.find(project => project.id === id);

        if (!project) {
            return;
        }

        const confirmed = window.confirm(`Удалить проект "${project.name}"?`);

        if (!confirmed) {
            return;
        }

         try {
             await deleteProject(id);
            await loadProjects();
        } 
        catch (error) {
            console.error("Ошибка удаления проекта:", error);
        }
    }

    async function editProject(
        id: number, 
        name: string, 
        description: string
    ) {
        const project = projects.find(project => project.id === id);

        if (!project) {
            return;
        }

        try {
            await updateProject(
                id, 
                name, 
                description,
                project.status,
                project.planned_release_at || null
            );
            await loadProjects();
        } 
        catch (error) {
            console.error("Ошибка обновления проекта:", error);
            throw error;
        }
    }

    useEffect(() => {async function loadInitialProjects() {
        try {
            await loadProjects();
        } 
        catch (error) {
            console.error("Ошибка загрузки проектов:", error);
        }
    }

    loadInitialProjects();
}, []);

    return (
        <div className="page">
            <header className="page-header">
                <div className="page-header__main">
                    <h1 className="page-title">Проекты</h1>
                    <p className="page-description">Контентные проекты и их производство</p>
                </div>
            </header>
            <section className="ui-card project-create">
                <div className="project-create__body">
                    <div className="project-create__fields">
                        <input className="ui-input" placeholder="Название проекта" value={name} onChange={(e) => setName(e.target.value)}/>
                        <input className="ui-input" placeholder="Описание проекта" value={description} onChange={(e) => setDescription(e.target.value)}/>
                    </div>
                    <div className="project-create__actions">
                        <Button onClick={addProject} disabled={!name.trim()}>Создать проект</Button>
                    </div>  
                </div>
            </section>
            <section className="projects-list">
                {projects.length === 0 ? (
                    <div className="empty-state">
                        <h2>Проектов пока нет</h2>
                        <p>Создай первый проект, чтобы начать работу.</p>
                    </div>
                    ) : (projects.map(project => (
                        <ProjectCard 
                            key={project.id} 
                            project={project} 
                            onDelete={removeProject}
                            onUpdate={editProject}
                        />
                    ))
                )}       
            </section>
        </div>
    );
}
export default Projects;