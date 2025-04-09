#pragma once
#include "Controller.h"
#include "httplib.h"
#include "json.hpp"

class GameController : public Controller {
	using json = nlohmann::json;
public:
	GameController(httplib::Server& srv) : Controller(srv) {
		SetRoute("/game");
		std::string todays_word = "guess";
		Post("/word", [todays_word](const httplib::Request& req) {
			try{
				auto json_body = json::parse(req.body);
				std::string word = json_body["word"];

				json result;
				for (size_t i = 0; i < word.length(); ++i) {
					if (word[i] == todays_word[i]) {
						result[std::to_string(i + 1)] = 1;
					}
					else if (todays_word.find(word[i]) != std::string::npos) {
						result[std::to_string(i + 1)] = 2;
					}
					else {
						result[std::to_string(i + 1)] = 0;
					}
				}
				return std::make_pair(200, result.dump());
			}
			catch (const json::parse_error& e) {
				return std::make_pair(400, json({ {"error", "Invalid JSON format"} }).dump());
			}
			catch (const std::exception& e) {
				return std::make_pair(500, json({ {"error", "Internal server error"} }).dump());
			}
		});
	}
};