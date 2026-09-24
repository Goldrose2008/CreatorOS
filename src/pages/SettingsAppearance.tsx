import { useState } from "react";
import { APP_NAME } from "../config/appConfig";
import {
    ACCENT_PRESETS,
    DEFAULT_ACCENT_COLOR,
    applyAccentColor,
    getAccentColor,
    saveAccentColor
} from "../services/themeService";
import Button from "../components/Button";
import Card from "../components/Card";

function SettingsAppearance() {
    const [accentColor, setAccentColor] = useState(getAccentColor());

    function changeAccentColor(color: string) {
        setAccentColor(color);
        saveAccentColor(color);
        applyAccentColor(color);
    }

    function resetAccentColor() {
        changeAccentColor(DEFAULT_ACCENT_COLOR);
    }

    return (
        <div className="settings-page">
            <header className="settings-page__header">
                <h2 className="settings-page__title">Внешний вид</h2>
                <p className="settings-page__description">Настройки визуального оформления {APP_NAME}.</p>
            </header>
            <Card className="settings-card">
                <div className="settings-card__header">
                    <h3>Основной цвет</h3>
                    <p>Используется для кнопок, активных элементов, ссылок и выделений.</p>
                </div>
                <div className="settings-field">
                    <label className="settings-label">Готовые варианты</label>
                    <div className="accent-presets">
                        {ACCENT_PRESETS.map((preset) => (
                            <button key={preset.value} type="button" className={["accent-preset", accentColor.toUpperCase() === preset.value.toUpperCase() ? "accent-preset--active" : ""].filter(Boolean).join(" ")} onClick={() => changeAccentColor(preset.value)}>
                                <span className="accent-preset__color" style={{backgroundColor: preset.value}}/>
                                <span>{preset.name}</span>
                            </button>
                        ))}
                    </div>
                    <div className="custom-accent">
                        <label className="settings-label" htmlFor="accent-color">Свой цвет</label>
                        <div className="custom-accent__control">
                            <input id="accent-color" type="color" value={accentColor} onChange={(e) => changeAccentColor(e.target.value)}/>
                            <code>{accentColor.toUpperCase()}</code>
                        </div>
                    </div>
                    <Button variant="secondary" onClick={resetAccentColor}>Вернуть цвет по-умолчанию</Button>
                </div>
            </Card>
            <Card className="settings-card">
                <div className="settings-card__header">
                    <h3>Интерфейс</h3>
                    <p>Базовые параметры интерфейса {APP_NAME}.</p>
                </div>
                <div className="settings-placeholder">
                    <span>Светлая рабочая область</span>
                    <span>Активно</span>
                </div>
                <div className="settings-placeholder">
                    <span>Тёмная навигация</span>
                    <span>Активно</span>
                </div>
            </Card>
        </div>
    );
}
export default SettingsAppearance;