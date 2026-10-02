const DEFAULT_LOCALE = "ru";

let catalogs = {};
let currentLocale = DEFAULT_LOCALE;

function parseLocalizationTable(source) {
    const cleanSource = source.replace(/^\uFEFF/, "");

    const lines = cleanSource.split(/\r?\n/).filter(line => {
        const trimmed = line.trim();
        return trimmed.length > 0 && !trimmed.startsWith("#");
    });

    if (lines.length === 0) {throw new Error("Localization table is empty.");}

    const headers = lines[0].split("\t");
    const idColumn = headers.indexOf("id");

    if (idColumn === -1) {throw new Error("Localization table has no id column.");}

    const locales = headers.filter(header => header !== "id");
    const result = {};

    locales.forEach(locale => {result[locale] = {};});

    for (let index = 1; index < lines.length; index++) {
        const values = lines[index].split("\t");
        const id = values[idColumn]?.trim();

        if (!id) {continue;}

        locales.forEach((locale, localeIndex) => {
            const value = values[localeIndex + 1] ?? "";
            result[locale][id] = value;
        });
    }

    return result;
}

export async function initializeLocalization(locale = DEFAULT_LOCALE) {
    const response = await fetch("qrc:///ui/i18n/localization.tsv");

    if (!response.ok) { throw new Error(`Cannot load localization table: ${response.status}`);}

    const source = await response.text();

    catalogs = parseLocalizationTable(source);

    if (!catalogs[locale]) {locale = DEFAULT_LOCALE;}
    if (!catalogs[locale]) {throw new Error("Default localization is unavailable.");}

    currentLocale = locale;
    document.documentElement.lang = currentLocale;
}

export function getLocale() {return currentLocale;}

export function setLocale(locale) {
    if (!catalogs[locale]) {return false;}

    currentLocale = locale;
    document.documentElement.lang = currentLocale;
    return true;
}

export function translate(
    id,
    params = {}
) {

    const currentCatalog = catalogs[currentLocale] ?? {};
    const defaultCatalog = catalogs[DEFAULT_LOCALE] ?? {};

    let value = currentCatalog[id] ?? defaultCatalog[id] ?? id;

    Object.entries(params).forEach(
        ([name, replacement]) => {

            value = value.replaceAll(
                `{${name}}`,
                String(replacement)
            );
        }
    );

    return value;
}