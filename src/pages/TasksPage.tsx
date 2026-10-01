import PageLayout from "../components/ui/layout/PageLayout";
import PageHeader from "../components/ui/layout/PageHeader";

function TasksPage() {
    return (
        <PageLayout
            header={
                <PageHeader title="Задачи" />
            }
        >
            <p>Здесь будут задачи</p>
        </PageLayout>
    );
}

export default TasksPage;