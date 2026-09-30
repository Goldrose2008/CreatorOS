export function formatDate(value?: string | null): string {
    if (!value) {
        return "Не указана";
    }

    const datePart = value.slice(0, 10);
    const [year, month, day] = datePart.split("-");

    if (!year || !month || !day) {
        return value;
    }

    return `${day}.${month}.${year}`;
}

export function formatDateTime(value?: string | null): string {
    if (!value) {
        return "Не указана";
    }

    return formatDate(value);
}
