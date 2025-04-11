#include <iostream>
#include "IndexController.h"
#include "GameController.h"
#include <httplib.h>
int main()
{
    httplib::Server svr;
    svr.new_task_queue = [] { return new httplib::ThreadPool(4); };

    // allow corse all
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "http://localhost:3000"},
        {"Access-Control-Allow-Methods", "POST, GET, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"},   
    });

    // Handle OPTIONS requests globally
    svr.Options("/game/word", [](const httplib::Request& req, httplib::Response& res) {
        res.status = 204;
    });
    IndexController indexController(svr);
    GameController gameController(svr);

    std::cout << "Server listening on port 1337..." << std::endl;
    svr.listen("localhost", 1337);
}