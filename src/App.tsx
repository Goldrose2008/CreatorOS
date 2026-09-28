import { useEffect } from "react";
import { BrowserRouter, Routes, Route, Navigate } from "react-router-dom";
import AppShell from "./components/layout/AppShell/AppShell";
import Analytics from "./pages/Analytics";
import Dashboard from "./pages/Dashboard";
import Library from "./pages/Library";
import Planning from "./pages/Planning";
import ProjectDetails from "./pages/ProjectDetails";
import Projects from "./pages/Projects";
import SettingsAppearance from "./pages/Settings/SettingsAppearance";
import SettingsLayout from "./pages/Settings/SettingsLayout";
import SettingsPlaceholder from "./pages/Settings/SettingsPlaceholder";
import Tasks from "./pages/Tasks";
import { applyAccentColor, getAccentColor } from "./services/themeService";

function App() {
  useEffect(() => { applyAccentColor(getAccentColor()); }, []);
  
  return (
    <BrowserRouter>
      <AppShell>
        <Routes>
          <Route path="/" element={<Dashboard />} />
          <Route path="/projects" element={<Projects />} />
          <Route path="/projects/:id" element={<ProjectDetails />}/>
          <Route path="/tasks" element={<Tasks />} />
          <Route path="/planning" element={<Planning />} />
          <Route path="/library" element={<Library />} />
          <Route path="/analytics" element={<Analytics />} />
          <Route path="/settings" element={<SettingsLayout />} >
            <Route index element={<Navigate to="/settings/appearance" replace/>} />
            <Route path="general" element={<SettingsPlaceholder title="Общие" description="Основные настройки приложения и рабочего пространства."/>} />
            <Route path="appearance" element={<SettingsAppearance />} />
            <Route path="shortcuts" element={<SettingsPlaceholder title="Горячие клавиши" description="Настройки сочетания клавиш для быстрых действий."/>} />
            <Route path="projects" element={<SettingsPlaceholder title="Проекты" description="Настройки поведения и структуры контентных проектов."/>} />
            <Route path="content" element={<SettingsPlaceholder title="Типы контента" description="Настройки типов контента."/>} />
            <Route path="automation" element={<SettingsPlaceholder title="Автоматизация" description="Правила, шаблоны и автоматические действия."/>} />
            <Route path="integrations" element={<SettingsPlaceholder title="Площадки" description="Подключение YouTube, Telegram и других платформ."/>} />
            <Route path="team" element={<SettingsPlaceholder title="Команда" description="Участники, роли и права доступа."/>} />
            <Route path="ai" element={<SettingsPlaceholder title="AI" description="Настройки AI-функций."/>} />
          </Route>
        </Routes>
      </AppShell>
    </BrowserRouter>
  );
}
export default App;