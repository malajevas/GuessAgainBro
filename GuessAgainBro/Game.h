#pragma once
#include <string>
#include <vector>

struct Game {
    private:
        std::string target_word;
        int attempts;
        bool is_active;
        bool is_victory;

    public:
        Game(const std::string& word) {
            target_word = word;
            attempts = 0;
            is_active = true;
            is_victory = false;
        }

        Game() {
            attempts = 0;
            is_active = true;
            is_victory = false;
        }

        void setTarget_word(const std::string& target_word) { this->target_word = target_word; }
        void setAttempts(int attempts) { this->attempts = attempts; }
        void setIs_active(bool is_active) { this->is_active = is_active; }
        void setIs_victory(bool is_victory) { this->is_victory = is_victory; }

        std::string getTarget_word() const { return this->target_word; }
        int getAttempts() const { return this->attempts; }
        bool getIs_active() const { return this->is_active; }
        bool getIs_victory() const { return this->is_victory; }
};
