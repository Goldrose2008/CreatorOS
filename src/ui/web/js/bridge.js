export function connectBridge() {
    return new Promise((resolve, reject) => {
        if (typeof qt === "undefined" || !qt.webChannelTransport) 
        {
            reject(new Error("Qt WebChannel transport недоступен."));
            return;
        }

        new QWebChannel(qt.webChannelTransport, (channel) => {
                resolve(channel.objects.bridge);
            }
        );
    });
}