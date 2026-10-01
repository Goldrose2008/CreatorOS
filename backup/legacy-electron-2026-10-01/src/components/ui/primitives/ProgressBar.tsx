import type { CSSProperties } from "react";

interface ProgressBarProps {
    value: number;
    showValue?: boolean;
}

function ProgressBar({
    value,
    showValue = true,
}: ProgressBarProps) {

    const normalizedValue = Math.min(100, Math.max(0, value));

    const valueStyle: CSSProperties = {
        width: `${normalizedValue}%`,
    };

    return (
        <div className="ui-progress">
            <div className="ui-progress__track">
                <div className="ui-progress__value" style={valueStyle}/>
            </div>

            {showValue && (
                <span className="ui-progress__label">
                    {normalizedValue}%
                </span>
            )}
        </div>
    );
}

export default ProgressBar;
