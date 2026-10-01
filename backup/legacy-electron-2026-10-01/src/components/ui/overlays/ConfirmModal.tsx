import {
    useEffect,
    useState,
    type ReactElement,
    type ReactNode,
} from "react";
import { AlertTriangle } from "lucide-react";
import Button from "../primitives/Button";
import Modal from "./Modal";

export interface ConfirmModalRequest {
    title: string;
    message: ReactNode;
    confirmLabel?: string;
    cancelLabel?: string;
    savingLabel?: string;
    onConfirm: () => Promise<void> | void;
}

type ConfirmModalListener = (
    request: ConfirmModalRequest
) => void;

let confirmModalListener: ConfirmModalListener | null = null;

interface ConfirmModalProps {}

interface ConfirmModalComponent {
    (props: ConfirmModalProps): ReactElement;
    StartEvent: (
        request: ConfirmModalRequest
    ) => void;
}

const ConfirmModal: ConfirmModalComponent = Object.assign(
    function ConfirmModal(_props: ConfirmModalProps) {
        const [request, setRequest] =
            useState<ConfirmModalRequest | null>(null);

        const [saving, setSaving] = useState(false);
        const [error, setError] = useState("");

        useEffect(() => {
            const listener: ConfirmModalListener = (
                nextRequest
            ) => {
                setError("");
                setSaving(false);
                setRequest(nextRequest);
            };

            confirmModalListener = listener;

            return () => {
                if (confirmModalListener === listener) {
                    confirmModalListener = null;
                }
            };
        }, []);

        function close() {
            if (saving) {
                return;
            }

            setError("");
            setRequest(null);
        }

        async function handleConfirm() {
            if (!request) {
                return;
            }

            try {
                setSaving(true);
                setError("");

                await request.onConfirm();

                setRequest(null);
            }
            catch (confirmError) {
                console.error(
                    "Ошибка выполнения подтверждённого действия:",
                    confirmError
                );

                setError(
                    "Не удалось выполнить действие."
                );
            }
            finally {
                setSaving(false);
            }
        }

        return (
            <Modal
                open={request !== null}
                title={request?.title ?? ""}
                onClose={close}
                width="small"
            >
                {request && (
                    <div className="ui-confirm-modal">
                        <div className="ui-confirm-modal__icon">
                            <AlertTriangle size={20} />
                        </div>

                        <div className="ui-confirm-modal__message">
                            {request.message}
                        </div>

                        {error && (
                            <div className="ui-confirm-modal__error">
                                {error}
                            </div>
                        )}

                        <div className="ui-confirm-modal__actions">
                            <Button
                                variant="danger"
                                onClick={handleConfirm}
                                disabled={saving}
                            >
                                {saving
                                    ? (
                                        request.savingLabel
                                        ?? "Выполнение..."
                                    )
                                    : (
                                        request.confirmLabel
                                        ?? "Подтвердить"
                                    )}
                            </Button>

                            <Button
                                variant="secondary"
                                onClick={close}
                                disabled={saving}
                            >
                                {request.cancelLabel ?? "Отмена"}
                            </Button>
                        </div>
                    </div>
                )}
            </Modal>
        );
    },
    {
        StartEvent(request: ConfirmModalRequest) {
            if (!confirmModalListener) {
                console.warn(
                    "ConfirmModal.StartEvent(): экземпляр ConfirmModal не смонтирован."
                );

                return;
            }

            confirmModalListener(request);
        },
    }
);

export default ConfirmModal;