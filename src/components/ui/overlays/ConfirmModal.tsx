import type { ReactNode } from "react";
import { AlertTriangle } from "lucide-react";
import Button from "../primitives/Button";
import Modal from "../overlays/Modal";

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
            <div className="ui-confirm-modal">
                <div className="ui-confirm-modal__icon">
                    <AlertTriangle size={20} />
                </div>

                <div className="ui-confirm-modal__message">
                    {message}
                </div>

                <div className="ui-confirm-modal__actions">
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