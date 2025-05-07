#pragma once
#include "Controller.h"
#include "httplib.h"
#include "json.hpp"
#include "Logger.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>  
#include "Game.h"
#include <unordered_map>
#include <random>
#include <sstream>


class GameController : public Controller {
	using json = nlohmann::json;
private:
    std::vector<std::string> valid_words;
	std::unordered_map<std::string, Game> sessions;
	Logger logger;

	void LoadWordsFromFile(const std::string& filename) {
		std::ifstream file(filename);
		std::string word;
		while (file >> word) {
			valid_words.push_back(word);
		}
		if (valid_words.empty()) {
			throw std::runtime_error("Word list is empty.");
		} 
	}

	std::string GenerateSessionId() {
		std::stringstream ss;
		static const char charset[] = "0123456789abcdef";
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, 15);
	
		for (int i = 0; i < 16; ++i)
			ss << charset[dist(gen)];
	
		return ss.str();
	}	

public:
	GameController(httplib::Server& srv) : Controller(srv) {
		SetRoute("/game");

		LoadWordsFromFile("words.txt");

		Post("/word", [this](const httplib::Request& req) {
			try {
				auto json_body = json::parse(req.body);
				std::string session_id = json_body["session_id"];
				std::string guess = json_body["word"];
		
				if (sessions.find(session_id) == sessions.end()) {
					return std::make_pair(404, json({ {"error", "Session not found"} }).dump());
				}
		
				Game& game = sessions[session_id];
		
				if (!game.getIs_active()) {
					if(game.getIs_victory())
						return std::make_pair(403, json({ {"error", "Victory! Game over"} }).dump());
					else
						return std::make_pair(403, json({ {"error", "Defeat! Game over"} }).dump());
				}
		
				if (std::find(valid_words.begin(), valid_words.end(), guess) == valid_words.end()) {
					return std::make_pair(404, json({ {"error", "Word not in dictionary"} }).dump());
				}
		
				json result;
				for (size_t i = 0; i < guess.length(); ++i) {
					if (guess[i] == game.getTarget_word()[i]) {
						result[std::to_string(i)] = 1;
					} 
					else if (game.getTarget_word().find(guess[i]) != std::string::npos) {
						result[std::to_string(i)] = 2;
					} 
					else {
						result[std::to_string(i)] = 0;
					}
				}
		
				game.setattempts(game.getattempts() + 1);
				if (game.getattempts() == 6) {
					game.setIs_active(false);
					game.setIs_victory(false);
					result["message"] = "Attempts finished. You lost!";
				}
				if (guess == game.getTarget_word()) {
					game.setIs_active(false);
					game.setIs_victory(true);
					result["message"] = "Correct! You won!";
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
		

		Get("/start", [this]() {
			if (valid_words.empty()) {
				return std::make_pair(500, json({ {"error", "Word list is empty"} }).dump());
			}
		
			std::string session_id = GenerateSessionId();
			std::string target = valid_words[rand() % valid_words.size()];
		
			sessions[session_id] = Game{target};
		
			logger.Info("Started new session: " + session_id + " | Word: " + target);
		
			return std::make_pair(200, json({
				{"message", "New game started"},
				{"session_id", session_id}
			}).dump());
		});
		
	}
};