import { Card } from "../../components/ui";
import { APP_NAME } from "../../config/appConfig";
import styles from "./Settings.module.css";

interface Props {
    title: string;
    description: string;
}

function SettingsPlaceholder({title, description}: Props) {
    return (
        <div className={styles.page}>
            <header className={styles.pageHeader}>
                <h2 className={styles.pageTitle}>{title}</h2>
                <p className={styles.pageDescription}>{description}</p>
            </header>
            <Card className={styles.card}>
                <div className={styles.placeholderPage}>
                    <h3>Раздел находится в разработке</h3>
                    <p>Здесь появятся соответствующие настройки {APP_NAME}.</p>
                </div>
            </Card>
        </div>
    );
}
export default SettingsPlaceholder;