import { spawn, execFileSync } from "node:child_process";
import path from "node:path";
import process from "node:process";

const isWindows = process.platform === "win32";
const npmCommand = isWindows ? "npm.cmd" : "npm";
const shellCommand = isWindows
    ? (process.env.ComSpec ?? "cmd.exe")
    : npmCommand;

const electronCommand = path.resolve(
    process.cwd(),
    "node_modules",
    "electron",
    "dist",
    process.platform === "win32" ? "electron.exe" : "electron"
);

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

const viteArgs = [
    "run",
    "dev",
    "--",
    "--host",
    "127.0.0.1",
    "--port",
    "5173",
    "--strictPort",
];

const vite = spawn(
    shellCommand,
    isWindows ? ["/d", "/s", "/c", npmCommand, ...viteArgs] : viteArgs,
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
    const buildElectronArgs = ["run", "build:electron"];

    if (isWindows) {
        execFileSync(shellCommand, [
            "/d",
            "/s",
            "/c",
            npmCommand,
            ...buildElectronArgs,
        ], {
            stdio: "inherit",
        });
    } else {
        execFileSync(npmCommand, buildElectronArgs, {
            stdio: "inherit",
        });
    }

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
