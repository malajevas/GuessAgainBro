#pragma once
#include <httplib.h>
#include <string>
#include <functional>
#include "HttpStatus.h"
#include "Logger.h"

class Controller {
    httplib::Server& server;
    std::string route = "";
	Logger logger;
protected:
    Controller(httplib::Server& srv) : server(srv) {
    }

    void Get(const std::string path, std::function<std::pair<int, std::string>()> handler) {
        server.Get(route + path, [this, handler](const httplib::Request& req, httplib::Response& res) {
			logger.Info("Received GET" + req.path);

            auto [status, body] = handler();
            res.status = status;
            res.set_header("Content-Type", "text/html");
            res.set_content(body, "text/html");
        });
    }
	void Post(const std::string path, std::function<std::pair<int, std::string>(const httplib::Request&)> handler) {
		server.Post(route + path, [this, handler](const httplib::Request& req, httplib::Response& res) {
			logger.Info("Received POST" + req.path);

			auto [status, body] = handler(req);
			res.status = status;
			res.set_header("Content-Type", "application/json");
			res.set_content(body, "application/json");
		});
	}
	void SetRoute(const std::string& newRoute) {
		route = newRoute;
	}
};