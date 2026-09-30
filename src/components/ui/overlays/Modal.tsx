import {
    useEffect,
    type ReactNode,
} from "react";

import { createPortal } from "react-dom";
import { X } from "lucide-react";

interface ModalProps {
    open: boolean;
    title: string;
    children: ReactNode;
    onClose: () => void;
    width?: "small" | "medium" | "large";
}

function Modal({
    open,
    title,
    children,
    onClose,
    width = "medium",
}: ModalProps) {

    useEffect(() => {
        if (!open) { return; }

        function handleKeyDown(event: KeyboardEvent) {
            if (event.key === "Escape") {
                onClose();
            }
        }

        document.addEventListener(
            "keydown",
            handleKeyDown
        );

        const previousOverflow = document.body.style.overflow;
        document.body.style.overflow = "hidden";

        return () => {
            document.removeEventListener("keydown", handleKeyDown);
            document.body.style.overflow = previousOverflow;
        };

    }, [open, onClose]);

    if (!open) { return null; }

    function handleOverlayClick(event: React.MouseEvent<HTMLDivElement>) {
        if (event.target === event.currentTarget) { onClose(); }
    }

    return createPortal(
        <div className="ui-modal__overlay" onMouseDown={handleOverlayClick}>
            <div className={["ui-modal", `ui-modal--${width}`].join(" ")} role="dialog" aria-modal="true" aria-labelledby="modal-title" onMouseDown={(event) => event.stopPropagation() }>
                <header className="ui-modal__header">
                    <h2 id="modal-title" className="ui-modal__title">
                        {title}
                    </h2>

                    <button type="button" className="ui-modal__close-button" onClick={onClose} aria-label="Закрыть">
                        <X size={18} />
                    </button>
                </header>

                <div className="ui-modal__content">
                    {children}
                </div>
            </div>
        </div>,
        document.body
    );
}

export default Modal;