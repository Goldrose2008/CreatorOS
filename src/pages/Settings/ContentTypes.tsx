import {
    useCallback,
    useEffect,
    useState,
} from "react";
import Button from "../../components/ui/primitives/Button";
import Card from "../../components/ui/layout/Card";
import Modal from "../../components/ui/overlays/Modal";
import EntityForm from "../../components/ui/forms/EntityForm";
import EmptyState from "../../components/ui/states/EmptyState";
import ConfirmModal from "../../components/ui/overlays/ConfirmModal";
import type { ContentType } from "../../models/ContentType";
import {
    CONTENT_TYPE_FORM_FIELDS,
    type ContentTypeFormValues,
} from "../../config/entities/contentConfig";
import {
    createContentType,
    deleteContentType,
    getContentTypes,
    updateContentType,
} from "../../services/contentTypeService";
import styles from "../../styles/settings/ContentTypes.module.css";

function getCreateValues(): ContentTypeFormValues {
    return {
        name: "",
        description: "",
    };
}

function getEditValues(contentType: ContentType): ContentTypeFormValues {
    return {
        name: contentType.name,
        description: contentType.description ?? "",
    };
}

function ContentTypes() {
    const [contentTypes, setContentTypes] = useState<ContentType[]>([]);
    const [loading, setLoading] = useState(true);
    const [createOpen, setCreateOpen] = useState(false);
    const [editingType, setEditingType] = useState<ContentType | null>(null);
    const [saving, setSaving] = useState(false);
    const [formError, setFormError] = useState("");
    const loadContentTypes = useCallback(async () => {
        const data = await getContentTypes();
        setContentTypes(data);
    }, []);

    useEffect(() => {
        async function loadInitialData() {
            try {
                await loadContentTypes();
            }
            catch (error) {
                console.error("Ошибка загрузки типов контента:", error);
            }
            finally { setLoading(false); }
        }

        loadInitialData();
    }, [loadContentTypes]);

    function openCreateModal() {
        setFormError("");
        setCreateOpen(true);
    }

    function closeCreateModal() {
        if (saving) {return;}

        setFormError("");
        setCreateOpen(false);
    }

    function openEditModal(contentType: ContentType) {
        setFormError("");
        setEditingType(contentType);
    }

    function closeEditModal() {
        if (saving) {return;}

        setFormError("");
        setEditingType(null);
    }

    async function handleCreate(values: ContentTypeFormValues) {
        try {
            setSaving(true);
            setFormError("");

            await createContentType(
                values.name.trim(),
                values.description.trim()
            );

            await loadContentTypes();
            setCreateOpen(false);
        }
        catch (error) {
            console.error("Ошибка создания типа контента:", error);
            setFormError("Не удалось создать тип контента. Возможно, такое название уже существует.");
        }
        finally { setSaving(false); }
    }

    async function handleEdit(values: ContentTypeFormValues) {
        if (!editingType) { return; }

        try {
            setSaving(true);
            setFormError("");

            await updateContentType(
                editingType.id,
                values.name.trim(),
                values.description.trim()
            );

            await loadContentTypes();
            setEditingType(null);
        }
        catch (error) {
            console.error("Ошибка обновления типа контента:", error);
            setFormError("Не удалось сохранить изменения. Возможно, такое название уже существует.");
        }
        finally { setSaving(false); }
    }

    function handleDelete(contentType: ContentType) {
        ConfirmModal.StartEvent({
            title: "Удаление типа контента",
            message: (
                <>
                    Удалить тип контента{" "}
                    <strong>«{contentType.name}»</strong>?
                    <br />
                    Это действие нельзя отменить.
                </>
            ),
            confirmLabel: "Удалить",
            savingLabel: "Удаление...",
            onConfirm: async () => {
                try {
                    await deleteContentType(contentType.id);
                    await loadContentTypes();
                }
                catch (error) {
                    console.error(
                        "Ошибка удаления типа контента:",
                        error
                    );

                    throw error;
                }
            },
        });
    }

    return (
        <div className={styles.page}>
            <header className={styles.header}>
                <div>
                    <h2 className={styles.title}>
                        Типы контента
                    </h2>

                    <p className={styles.description}>
                        Определи форматы контента, которые используются в CreatorOS.
                    </p>
                </div>

                <Button onClick={openCreateModal}>
                    Добавить тип
                </Button>
            </header>

            <section className={styles.list}>
                {loading ? (
                    <EmptyState description="Загрузка типов контента..."/>
                ) : contentTypes.length === 0 ? (
                    <Card className={styles.emptyCard}>
                        <EmptyState
                            title="Типов контента пока нет"
                            description="Добавь первый тип контента."
                            action={
                                <Button onClick={openCreateModal}>
                                    Добавить тип
                                </Button>
                            }
                        />
                    </Card>
                ) : (
                    contentTypes.map((contentType) => (
                        <Card
                            key={contentType.id}
                            className={styles.item}
                        >
                            <div className={styles.itemMain}>
                                <h3 className={styles.itemTitle}>
                                    {contentType.name}
                                </h3>

                                {contentType.description && (
                                    <p className={styles.itemDescription}>
                                        {contentType.description}
                                    </p>
                                )}
                            </div>

                            <div className={styles.actions}>
                                <Button
                                    variant="secondary"
                                    onClick={() => openEditModal(contentType)}
                                >
                                    Редактировать
                                </Button>

                                <Button
                                    variant="danger"
                                    onClick={() => handleDelete(contentType)}
                                >
                                    Удалить
                                </Button>
                            </div>
                        </Card>
                    ))
                )}
            </section>

            <Modal
                open={createOpen}
                title="Новый тип контента"
                onClose={closeCreateModal}
            >
                <EntityForm<ContentTypeFormValues>
                    key="create-content-type"
                    fields={CONTENT_TYPE_FORM_FIELDS}
                    initialValues={getCreateValues()}
                    submitLabel="Добавить"
                    saving={saving}
                    error={formError}
                    onSubmit={handleCreate}
                    onCancel={closeCreateModal}
                />
            </Modal>

            <Modal
                open={editingType !== null}
                title="Редактирование типа контента"
                onClose={closeEditModal}
            >
                {editingType && (
                    <EntityForm<ContentTypeFormValues>
                        key={`edit-content-type-${editingType.id}`}
                        fields={CONTENT_TYPE_FORM_FIELDS}
                        initialValues={getEditValues(editingType)}
                        submitLabel="Сохранить"
                        saving={saving}
                        error={formError}
                        onSubmit={handleEdit}
                        onCancel={closeEditModal}
                    />
                )}
            </Modal>
        </div>
    );
}

export default ContentTypes;