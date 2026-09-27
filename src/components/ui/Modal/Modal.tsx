import {
    useEffect,
    type ReactNode,
} from "react";

import { createPortal } from "react-dom";
import { X } from "lucide-react";
import styles from "./Modal.module.css";

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
        <div className={styles.overlay} onMouseDown={handleOverlayClick}>
            <div className={[styles.modal, styles[width]].join(" ")} role="dialog" aria-modal="true" aria-labelledby="modal-title" onMouseDown={(event) => event.stopPropagation() }>
                <header className={styles.header}>
                    <h2 id="modal-title" className={styles.title}>
                        {title}
                    </h2>

                    <button type="button" className={styles.closeButton} onClick={onClose} aria-label="Закрыть">
                        <X size={18} />
                    </button>
                </header>

                <div className={styles.content}>
                    {children}
                </div>
            </div>
        </div>,
        document.body
    );
}

export default Modal;