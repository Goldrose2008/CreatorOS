import type { ReactNode } from "react";
import { AlertTriangle } from "lucide-react";
import Button from "../Button/Button";
import Modal from "../Modal/Modal";
import styles from "./ConfirmModal.module.css";

interface ConfirmModalProps {
    open: boolean;
    title: string;
    message: ReactNode;
    confirmLabel?: string;
    cancelLabel?: string;
    saving?: boolean;
    onConfirm: () => Promise<void> | void;
    onCancel: () => void;
}

function ConfirmModal({
    open,
    title,
    message,
    confirmLabel = "Удалить",
    cancelLabel = "Отмена",
    saving = false,
    onConfirm,
    onCancel,
}: ConfirmModalProps) {
    return (
        <Modal open={open} title={title} onClose={onCancel} width="small">
            <div className={styles.content}>
                <div className={styles.icon}>
                    <AlertTriangle size={20} />
                </div>

                <div className={styles.message}>
                    {message}
                </div>

                <div className={styles.actions}>
                    <Button variant="danger" onClick={onConfirm} disabled={saving}>
                        {saving ? "Удаление..." : confirmLabel}
                    </Button>

                    <Button variant="secondary" onClick={onCancel} disabled={saving}>
                        {cancelLabel}
                    </Button>
                </div>
            </div>
        </Modal>
    );
}

export default ConfirmModal;