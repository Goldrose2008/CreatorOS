import { spawn, execFileSync } from "node:child_process";
import process from "node:process";

const npmCommand = process.platform === "win32" ? "npm.cmd" : "npm";
const electronCommand = process.platform === "win32"
    ? "node_modules/.bin/electron.cmd"
    : "node_modules/.bin/electron";

const rendererUrl = "http://127.0.0.1:5173";

function waitForRenderer() {
    return new Promise((resolve, reject) => {
        const startedAt = Date.now();

        const check = async () => {
            try {
                await fetch(rendererUrl);
                resolve();
                return;
            }
            catch {
                if (Date.now() - startedAt > 30000) {
                    reject(new Error("Vite не запустил renderer за 30 секунд."));
                    return;
                }

                setTimeout(check, 250);
            }
        };

        check();
    });
}

const vite = spawn(
    npmCommand,
    ["run", "dev", "--", "--host", "127.0.0.1", "--port", "5173", "--strictPort"],
    {
        stdio: "inherit",
    }
);

function cleanup() {
    if (!vite.killed) {
        vite.kill();
    }
}

process.on("SIGINT", cleanup);
process.on("SIGTERM", cleanup);

try {
    execFileSync(npmCommand, ["run", "build:electron"], {
        stdio: "inherit",
    });

    await waitForRenderer();

    const electron = spawn(electronCommand, ["."], {
        env: {
            ...process.env,
            ELECTRON_RENDERER_URL: rendererUrl,
        },
        stdio: "inherit",
    });

    electron.on("exit", (code) => {
        cleanup();
        process.exit(code ?? 0);
    });
}
catch (error) {
    cleanup();
    console.error(error);
    process.exit(1);
}
