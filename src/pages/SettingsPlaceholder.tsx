import Card from "../components/Card";
import { APP_NAME } from "../config/appConfig";

interface Props {
    title: string;
    description: string;
}

function SettingsPlaceholder({title, description}: Props) {
    return (
        <div className="settings-page">
            <header className="settings-page__header">
                <h2 className="settings-page__title">{title}</h2>
                <p className="settings-page__description">{description}</p>
            </header>
            <Card className="settings-card">
                <div className="settings-placeholder-page">
                    <h3>Раздел находится в разработке</h3>
                    <p>Здесь появятся соответствующие настройки {APP_NAME}.</p>
                </div>
            </Card>
        </div>
    );
}
export default SettingsPlaceholder;