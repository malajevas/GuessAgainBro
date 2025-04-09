#include <iostream>
#include "IndexController.h"
#include "GameController.h"
#include <httplib.h>
int main()
{
    httplib::Server svr;
    svr.new_task_queue = [] { return new httplib::ThreadPool(4); };
    IndexController indexController(svr);
    GameController gameController(svr);

    std::cout << "Server listening on port 1337..." << std::endl;
    svr.listen("localhost", 1337);
}