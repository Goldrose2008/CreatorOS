import ru from "./locales/ru.js";
import en from "./locales/en.js";

const catalogs = {
    ru,
    en
};

const defaultLocale = "ru";

let currentLocale = defaultLocale;

export function getLocale() {return currentLocale;}

export function setLocale(locale) {
    if (!catalogs[locale]) {return false;}

    currentLocale = locale;
    document.documentElement.lang = locale;
    return true;
}

export function translate(key) {
    const currentCatalog = catalogs[currentLocale] ?? catalogs[defaultLocale];

    return currentCatalog[key] ?? catalogs[defaultLocale][key] ?? key;
}