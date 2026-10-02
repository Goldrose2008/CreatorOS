import ru from "./locales/ru.js";
import en from "./locales/en.js";

const catalogs = {
    ru,
    en
};

let currentLocale = "ru";

export function getLocale() {return currentLocale;}

export function setLocale(locale) {

    if (!catalogs[locale]) {return false;}

    currentLocale = locale;
    document.documentElement.lang = locale;
    return true;
}

export function translate(key, params = {}) {

    const currentCatalog = catalogs[currentLocale] ?? catalogs.ru;

    let value = currentCatalog[key] ?? catalogs.ru[key] ?? key;

    Object.entries(params).forEach(
        ([name, replacement]) => {
            value = value.replaceAll(`{${name}}`, replacement);
        }
    );

    return value;
}