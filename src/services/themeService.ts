const ACCENT_STORAGE_KEY = "creatoros.accentColor";

export const DEFAULT_ACCENT_COLOR = "#0F8B8D";

export const ACCENT_PRESETS = [
    {
        name: "Бирюзовый",
        value: "#0F8B8D"
    },
    {
        name: "Фиолетовый",
        value: "#6C63CE"
    },
    {
        name: "Коралловый",
        value: "#D86457"
    },
    {
        name: "Синий",
        value: "#3578E5"
    },
    {
        name: "Лаймовый",
        value: "#7AAE2E"
    }
];

export function getAccentColor(): string {
    const savedColor = localStorage.getItem(ACCENT_STORAGE_KEY);
    return savedColor || DEFAULT_ACCENT_COLOR;
}

export function saveAccentColor(color: string): void {
    localStorage.setItem(ACCENT_STORAGE_KEY, color);
}

export function applyAccentColor(color: string): void {
    const root = document.documentElement;

    root.style.setProperty("--color-accent", color);
    root.style.setProperty("--color-accent-hover", `color-mix(in srgb, ${color} 82%, black)`);
    root.style.setProperty("--color-accent-soft", `color-mix(in srgb, ${color} 10%, white)`);
    root.style.setProperty("--color-focus", `color-mix(in srgb, ${color} 22%, transparent)`);
}