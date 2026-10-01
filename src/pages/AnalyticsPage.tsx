import PageLayout from "../components/ui/layout/PageLayout";
import PageHeader from "../components/ui/layout/PageHeader";

function AnalyticsPage() {
    return (
        <PageLayout
            header={
                <PageHeader title="Аналитика" />
            }
        >
            <p>Здесь будет аналитика</p>
        </PageLayout>
    );
}

export default AnalyticsPage;