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

#define ATTEMPTS 6

enum HttpStatus {
    OK = 200,
    Created = 201,
	NoContent = 204,
    BadRequest = 400,
	Forbidden = 403,
    NotFound = 404,
    ServerError = 500,
	NotImplemented = 501
};

class GameController : public Controller {
	using json = nlohmann::json;
private:
    std::vector<std::string> valid_words;
	std::unordered_map<std::string, Game> sessions;

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
					return std::make_pair(NotFound, json({ {"error", "Session not found"} }).dump());
				}
		
				Game& game = sessions[session_id];
		
				if (!game.getIs_active()) {
					if(game.getIs_victory())
						return std::make_pair(Forbidden, json({ {"error", "Victory! Game over"} }).dump());
					else
						return std::make_pair(Forbidden, json({ {"error", "Defeat! Game over"} }).dump());
				}
		
				if (std::find(valid_words.begin(), valid_words.end(), guess) == valid_words.end()) {
					return std::make_pair(NotFound, json({ {"error", "Word not in dictionary"} }).dump());
				}
				
				logger.Info("Guessed word: " + guess);

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
		
				game.setAttempts(game.getAttempts() + 1);

				if (guess == game.getTarget_word()) {
					game.setIs_active(false);
					game.setIs_victory(true);
					result["message"] = "Correct! You won!";
				}

				if (!(game.getIs_victory()) && game.getAttempts() == ATTEMPTS) {
					game.setIs_active(false);
					game.setIs_victory(false);
					result["message"] = "Attempts finished. You lost!";
				}

		
				return std::make_pair(OK, result.dump());
		
			} 
			catch (const json::parse_error& e) {
				return std::make_pair(BadRequest, json({ {"error", "Invalid JSON format"} }).dump());
			}
			catch (const std::exception& e) {
				return std::make_pair(ServerError, json({ {"error", "Internal server error"} }).dump());
			}
		});
		

		Get("/start", [this]() {
			srand((unsigned)(time(nullptr)));

			if (valid_words.empty()) {
				return std::make_pair(ServerError, json({ {"error", "Word list is empty"} }).dump());
			}
		
			std::string session_id = GenerateSessionId();

			std::string target = valid_words[rand() % valid_words.size()];
		
			sessions[session_id] = Game{target};
		
			logger.Info("Started new session: " + session_id + " | Word: " + target);
		
			return std::make_pair(OK, json({
				{"message", "New game started"},
				{"session_id", session_id}
			}).dump());
		});

		Get("/stats", [this](const httplib::Request& req) {
			// Get session_id from query parameter (e.g. GET /game/stats?session_id=abc123)
			auto it = req.params.find("session_id");
			if (it == req.params.end()) {
				return std::make_pair(BadRequest, json({ {"error", "Missing session_id"} }).dump());
			}
		
			const std::string& session_id = it->second;
		
			if (sessions.find(session_id) == sessions.end()) {
				return std::make_pair(NotFound, json({ {"error", "Session not found"} }).dump());
			}
		
			const Game& game = sessions[session_id];
		
			json stats = {
				{"attempts", game.getAttempts()},
				{"is_active", game.getIs_active()},
				{"is_victory", game.getIs_victory()}
			};
		
			return std::make_pair(OK, stats.dump());
		});
		
		
	}
};