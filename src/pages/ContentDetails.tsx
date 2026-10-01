import {
    useEffect,
    useState,
} from "react";
import {
    ArrowLeft,
    CalendarDays,
    FileText,
    Pencil,
} from "lucide-react";
import { Link, useParams } from "react-router-dom";
import EntityHeader from "../components/entity/EntityHeader";
import Button from "../components/ui/primitives/Button";
import Card from "../components/ui/layout/Card";
import EmptyState from "../components/ui/states/EmptyState";
import EntityForm from "../components/ui/forms/EntityForm";
import Modal from "../components/ui/overlays/Modal";
import PageLayout from "../components/ui/layout/PageLayout";
import type { Content } from "../models/Content";
import type { ContentType } from "../models/ContentType";
import {
    getContentFormFields,
    type ContentFormValues,
} from "../config/entities/contentConfig";
import {
    getContentById,
    updateContent,
} from "../services/contentService";
import { getContentTypes } from "../services/contentTypeService";
import styles from "./ContentDetails.module.css";
import { formatDate } from "../utils/date";

function getRoleLabel(content: Content): string {
    return content.content_role === "main"
        ? "Основной контент"
        : "Дополнительный контент";
}

function ContentDetails() {
    const { id } = useParams<{ id: string }>();
    const [content, setContent] = useState<Content | null>(null);
    const [contentTypes, setContentTypes] = useState<ContentType[]>([]);
    const [loading, setLoading] = useState(true);
    const [error, setError] = useState("");
    const [editOpen, setEditOpen] = useState(false);
    const [saving, setSaving] = useState(false);
    const [formError, setFormError] = useState("");

    useEffect(() => {
        async function loadContent() {
            if (!id) {
                setError("Идентификатор контента не указан.");
                setLoading(false);
                return;
            }

            const contentId = Number(id);

            if (!Number.isInteger(contentId)) {
                setError("Некорректный идентификатор контента.");
                setLoading(false);
                return;
            }

            try {
                const [
                    contentData,
                    types,
                ] = await Promise.all([
                    getContentById(contentId),
                    getContentTypes(),
                ]);

                setContent(contentData);
                setContentTypes(types);

                if (!contentData) { setError("Контент не найден."); }
            }
            catch (loadError) {
                console.error("Ошибка загрузки контента:", loadError);
                setError("Не удалось загрузить контент.");
            }
            finally { setLoading(false); }
        }

        loadContent();
    }, [id]);

    function openEditModal() {
        setFormError("");
        setEditOpen(true);
    }

    function closeEditModal() {
        if (saving) { return; }

        setFormError("");
        setEditOpen(false);
    }

    function getContentType(): ContentType | undefined {
        if (!content) { return undefined; }
        return contentTypes.find((type) => type.id === content.content_type_id);
    }

    function getEditValues(): ContentFormValues {
        return {
            contentTypeId: content?.content_type_id ?? 0,
            name: content?.name ?? "",
            description: content?.description ?? "",
        };
    }

    async function handleSave(values: ContentFormValues) {
        if (!content) { return; }

        try {
            setSaving(true);
            setFormError("");

            await updateContent(
                content.id,
                values.contentTypeId,
                values.name.trim(),
                values.description.trim()
            );

            const updatedContent = await getContentById(content.id);

            setContent(updatedContent);
            setEditOpen(false);
        }
        catch (saveError) {
            console.error("Ошибка сохранения контента:", saveError);
            setFormError("Не удалось сохранить изменения.");
        }
        finally { setSaving(false); }
    }

    if (loading) {
        return (
            <PageLayout>
                <div className={styles.page}>
                    <EmptyState description="Загрузка контента..." />
                </div>
            </PageLayout>
        );
    }

    if (error || !content) {
        return (
            <PageLayout>
                <div className={styles.page}>
                    <Link to="/projects" className={styles.backLink}>
                        <ArrowLeft size={16} />
                        Вернуться к проектам
                    </Link>

                    <EmptyState title="Контент не найден" description={error || "Контент не найден."}/>
                </div>
            </PageLayout>
        );
    }

    const contentType = getContentType();

    return (
        <PageLayout
            navigation={
                <Link to={`/projects/${content.project_id}`} className={styles.backLink}>
                    <ArrowLeft size={16} />
                    Вернуться к проекту
                </Link>
            }
        >
            <div className={styles.page}>
                <EntityHeader
                    title={content.name}
                    description={content.description}
                    meta={[
                        <span key="type">
                            <FileText size={14} />
                            Тип:{" "}
                            {contentType?.name ?? "Неизвестный тип"}
                        </span>,

                        <span key="role">
                            Роль: {getRoleLabel(content)}
                        </span>,

                        <span key="release">
                            <CalendarDays size={14} />
                            Планируемый выход:{" "}
                            {formatDate(content.planned_release_at)}
                        </span>,

                        <span key="progress">
                            Прогресс: {content.progress}%
                        </span>,
                    ]}
                    actions={
                        <Button onClick={openEditModal} disabled={saving}>
                            <Pencil size={15} />
                            Редактировать
                        </Button>
                    }
                />

                {formError && (
                    <div className={styles.error}>
                        {formError}
                    </div>
                )}

                <Card className={styles.section}>
                    <h2 className={styles.sectionTitle}>
                        Контент
                    </h2>

                    <div className={styles.infoGrid}>
                        <div>
                            <span className={styles.label}>
                                Тип контента
                            </span>
                            <strong className={styles.value}>
                                {contentType?.name ?? "Неизвестный тип"}
                            </strong>
                        </div>

                        <div>
                            <span className={styles.label}>
                                Роль
                            </span>
                            <strong className={styles.value}>
                                {getRoleLabel(content)}
                            </strong>
                        </div>

                        <div>
                            <span className={styles.label}>
                                Планируемый выход
                            </span>
                            <strong className={styles.value}>
                                {formatDate(content.planned_release_at)}
                            </strong>
                        </div>

                        <div>
                            <span className={styles.label}>
                                Прогресс
                            </span>
                            <strong className={styles.value}>
                                {content.progress}%
                            </strong>
                        </div>
                    </div>
                </Card>

                <Modal
                    open={editOpen}
                    title="Редактирование контента"
                    onClose={closeEditModal}
                >
                    <EntityForm<ContentFormValues>
                        fields={getContentFormFields(contentTypes)}
                        initialValues={getEditValues()}
                        submitLabel="Сохранить"
                        saving={saving}
                        error={formError}
                        onSubmit={handleSave}
                        onCancel={closeEditModal}
                    />
                </Modal>
            </div>
        </PageLayout>
    );
}

export default ContentDetails;