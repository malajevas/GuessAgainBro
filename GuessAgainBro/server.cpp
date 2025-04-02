#include "server.h"
#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <httplib.h>
#include "json.hpp"

using json = nlohmann::json;

#pragma comment(lib, "ws2_32.lib")

void start_server() {
    
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        // Send the status code as 200 OK
        res.status = 200;

        // Set the Content-Type header to text/html (for HTML response)
        res.set_header("Content-Type", "text/html");

        // HTML content to be displayed
        std::string html_content = "<p>Hello Idiot</p>";

        // Set the content to HTML
        res.set_content(html_content, "text/html");
        });

    svr.Post("/data", [](const httplib::Request& req, httplib::Response& res) {
        json response = {
            {"method", "POST"},
            {"path", req.path},
            {"status", "OK"},
            {"body", req.body}
        };
        res.set_content(response.dump(), "application/json");
        });

    std::cout << "Server listening on port 1337..." << std::endl;
    svr.listen("localhost", 1337);
}