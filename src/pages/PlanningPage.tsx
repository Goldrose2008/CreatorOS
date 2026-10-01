import PageLayout from "../components/ui/layout/PageLayout";
import PageHeader from "../components/ui/layout/PageHeader";

function PlanningPage() {
    return (
        <PageLayout
            header={
                <PageHeader title="Планирование" />
            }
        >
            <p>Здесь будет контент-план</p>
        </PageLayout>
    );
}

export default PlanningPage;