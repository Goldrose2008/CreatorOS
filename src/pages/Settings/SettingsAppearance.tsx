import { useState } from "react";
import { APP_NAME } from "../../config/appConfig";
import {
    ACCENT_PRESETS,
    DEFAULT_ACCENT_COLOR,
    applyAccentColor,
    getAccentColor,
    saveAccentColor
} from "../../services/themeService";
import Button from "../../components/ui/primitives/Button";
import Card from "../../components/ui/layout/Card";
import styles from "../../styles/settings/Settings.module.css";

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
        <div className={styles.page}>
            <header className={styles.pageHeader}>
                <h2 className={styles.pageTitle}>Внешний вид</h2>
                <p className={styles.pageDescription}>Настройки визуального оформления {APP_NAME}.</p>
            </header>
            <Card className={styles.card}>
                <div className={styles.cardHeader}>
                    <h3>Основной цвет</h3>
                    <p>Используется для кнопок, активных элементов, ссылок и выделений.</p>
                </div>
                <div className={styles.field}>
                    <label className={styles.label}>Готовые варианты</label>
                    <div className={styles.accentPresets}>
                        {ACCENT_PRESETS.map((preset) => (
                            <button key={preset.value} type="button" className={[styles.accentPreset, accentColor.toUpperCase() === preset.value.toUpperCase() ? styles.accentPresetActive : ""].filter(Boolean).join(" ")} onClick={() => changeAccentColor(preset.value)}>
                                <span className={styles.accentPresetColor} style={{backgroundColor: preset.value}}/>
                                <span>{preset.name}</span>
                            </button>
                        ))}
                    </div>
                    <div className={styles.customAccent}>
                        <label className={styles.label} htmlFor="accent-color">Свой цвет</label>
                        <div className={styles.customAccentControl}>
                            <input id="accent-color" type="color" value={accentColor} onChange={(e) => changeAccentColor(e.target.value)}/>
                            <code>{accentColor.toUpperCase()}</code>
                        </div>
                    </div>
                    <Button variant="secondary" onClick={resetAccentColor}>Вернуть цвет по-умолчанию</Button>
                </div>
            </Card>
            <Card className={styles.card}>
                <div className={styles.cardHeader}>
                    <h3>Интерфейс</h3>
                    <p>Базовые параметры интерфейса {APP_NAME}.</p>
                </div>
                <div className={styles.placeholder}>
                    <span>Светлая рабочая область</span>
                    <span>Активно</span>
                </div>
                <div className={styles.placeholder}>
                    <span>Тёмная навигация</span>
                    <span>Активно</span>
                </div>
            </Card>
        </div>
    );
}
export default SettingsAppearance;