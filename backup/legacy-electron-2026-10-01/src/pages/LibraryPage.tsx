import PageLayout from "../components/ui/layout/PageLayout";
import PageHeader from "../components/ui/layout/PageHeader";

function LibraryPage() {
    return (
        <PageLayout
            header={
                <PageHeader title="Библиотека" />
            }
        >
            <p>Здесь будет библиотека</p>
        </PageLayout>
    );
}

export default LibraryPage;