#include "Networking/Servers/HttpServer.h"
#include <ESPmDNS.h>

HttpServer::HttpServer(ServerConfig* config)
{
    this->config_ = config;  
    this->server_ = nullptr;
}

void HttpServer::stopHttpServer(){
    if (server_ != nullptr) {
        server_->stop();
        delete server_;
        server_ = nullptr;
    }
}


void HttpServer::startHttpServer()
{
    stopHttpServer();
    server_ = new WebServer(config_->getHttpPort());
    server_->on("/test", [this]() { handlePage(); });
    server_->on("/log", HTTP_GET, [this]() { server_->send(200, "text/html", logger.buildLog()); });
    server_->begin();
    Serial.println("SRV: HTTP Server started on port: " + String(config_->getHttpPort()));
}

/**
 * @brief	Services pending HTTP requests (if the HTTP server is running)
 * 			and updates the mDNS responder. Should be called every loop().
 */
void HttpServer::handleHttpServer()
{
    if (server_ != nullptr) server_->handleClient();
    //MDNS.update();
}

void HttpServer::handlePage()
{
    String page = R"(
    <html>
    <head>
        <style>
            body { background: #121212; color: #ccc; font-family: monospace; }
        </style>
    </head>

    <body>

        <div id="log"></div>

        <script>
        setInterval(() => {
            fetch('/log')
            .then(r => r.text())
            .then(data => {
                document.getElementById('log').innerHTML = data;
            });
        }, 300);
        </script>

    </body>
    </html>
    )";

    server_->send(200, "text/html", page);
}

void HttpServer::clearHttpServer(){ logger.freeList(); }

void HttpServer::getMsg(const String& msg){ logger.putMsgInBuffer(msg); }