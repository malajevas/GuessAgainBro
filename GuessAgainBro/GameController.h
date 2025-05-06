#pragma once
#include "Controller.h"
#include "httplib.h"
#include "json.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>  

class GameController : public Controller {
	using json = nlohmann::json;
private:
    std::vector<std::string> valid_words;
    std::string todays_word;

	void LoadWordsFromFile(const std::string& filename) {
		std::ifstream file(filename);
		std::string word;
		while (file >> word) {
			valid_words.push_back(word);
		}
		if (!valid_words.empty()) {
			srand(static_cast<unsigned int>(time(nullptr)));
			todays_word = valid_words[rand() % valid_words.size()];
		} 
		else {
			throw std::runtime_error("Word list is empty.");
		}
	}
public:
	GameController(httplib::Server& srv) : Controller(srv) {
		SetRoute("/game");

		LoadWordsFromFile("words.txt");
		std::cout << "TESTING: today's word is " << todays_word << std::endl;

		Post("/word", [this](const httplib::Request& req) {
			try{
				auto json_body = json::parse(req.body);
				std::string word = json_body["word"];

				// Check if word is valid
				if (std::find(valid_words.begin(), valid_words.end(), word) == valid_words.end()) {
					return std::make_pair(404, json({ {"error", "Word not in the list"} }).dump());
				}

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