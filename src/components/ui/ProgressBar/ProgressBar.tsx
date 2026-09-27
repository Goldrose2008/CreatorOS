interface ProgressBarProps {
    value: number;
    showValue?: boolean;
}

import styles from "./ProgressBar.module.css";

function ProgressBar({
    value,
    showValue = true,
}: ProgressBarProps) {

    const normalizedValue = Math.min(100, Math.max(0, value));

    return (
        <div className={styles.wrapper}>
            <div className={styles.track}>
                <div className={styles.value} style={{width: `${normalizedValue}%`}}/>
            </div>

            {showValue && (
                <span className={styles.label}>
                    {normalizedValue}%
                </span>
            )}
        </div>
    );
}

export default ProgressBar;