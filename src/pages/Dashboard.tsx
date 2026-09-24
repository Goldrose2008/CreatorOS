import { APP_NAME } from "../config/appConfig";

function Dashboard() {
  return (
    <div>
      <h1>Главная</h1>
      <p>Главная {APP_NAME}</p>
    </div>
  );
}

export default Dashboard;