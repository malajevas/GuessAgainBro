#pragma once
#include "Controller.h"
#include "httplib.h"

class IndexController : public Controller {
public:
    IndexController(httplib::Server& srv) : Controller(srv) {
        Get("/", []() {
            return std::make_pair(200, "<p>Cia IndexController.h vidus, manau kai kursi frontenda reikes tsg prapassint visa &lt;html&gt; vietoj sito teksto, kartu su visais &lt;style&gt; ir visais &lt;script&gt;. <br/>Manau visa logika tame &lt;script&gt; tada darytu POST i musu kitus endpointus ir thats about it :).</p>");
        });
    }
};