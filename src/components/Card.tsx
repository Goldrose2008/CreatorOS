import type {HTMLAttributes} from "react";

type CardProps = HTMLAttributes<HTMLDivElement>;

function Card({
    className = "",
    ...props
}: CardProps) {

    const classes = [
        "ui-card",
        className
    ]
        .filter(Boolean)
        .join(" ");

    return (
        <div className={classes} {...props}/>
    );
}
export default Card;