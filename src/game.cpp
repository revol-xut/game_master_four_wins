

#include "game.hpp"

#include <fstream>
#include <charconv>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

auto Game::get_bot_respone(const std::string& path) noexcept -> std::size_t {
    std::ifstream file_handle;
    std::string file_contents;
    do {
        file_handle.open(path);
        file_handle >> file_contents;
        file_handle.close();
    } while (file_contents.empty() or file_contents[0] == '<');
    file_contents = file_contents.substr(1, file_contents.size());

    std::size_t number;
    auto result = std::from_chars(file_contents.data(), file_contents.data() + file_contents.size(), number);
    if (result.ec == std::errc::invalid_argument) {
        std::cerr << "Could not convert.";
    }

    return number;
}

void Game::write_bot_response(const std::string& path, const std::string& value) noexcept {
    std::ofstream file_handle(path, std::ios::trunc);
    file_handle << "<" + value;
    file_handle.close();
}

void Game::mainloop() noexcept {
    using namespace std::chrono_literals;
    const std::string player_1_file = "./black";
    const std::string player_2_file = "./white";
    std::string file;

    write_bot_response(player_1_file, "start");
    while (not board_.is_finished()) {
        auto current_player = board_.get_game_turn();
        file = not current_player ? player_1_file : player_2_file;

        auto start_point = std::chrono::steady_clock::now();
        auto value = get_bot_respone(file);
        auto end_point = std::chrono::steady_clock::now();
        trunc_file(file);

        if( end_point - start_point > 60s) {
            std::cerr << "Player took to long !" << std::endl;
        }

        if(value > 15 or value < 0) {
            std::cerr << "Invalid Index !" << std::endl;
        }

        auto column = static_cast<unsigned char>(value);

        std::cout << "Player: " << current_player << " Column: " << std::to_string(column) << std::endl;
        board_.player_move(column);
        board_.write_data("live_feed.csv");

        if(board_.is_finished()) {
            write_bot_response(player_1_file, "end");
            write_bot_response(player_2_file, "end");

            std::cout << "GAME OVER !" << std::endl;
            auto to_string = [](GamePosition player) {
                switch (player) {
                    case GamePosition::Tie: return "Tie";
                    case GamePosition::Black: return "Black";
                    case GamePosition::White: return "White";
                }
                return "Unknown";
            };
            std::cout << to_string(board_.get_winner()) << std::endl;
            return;
        }

        file = current_player ? player_1_file : player_2_file;
        write_bot_response(file, std::to_string(column));
        std::this_thread::sleep_for(1s);
        // now the bot flushes the file
    }
    trunc_file(player_1_file);
    trunc_file(player_2_file);
}

void Game::trunc_file(const std::string &path) noexcept {
    std::ofstream file_handle(path, std::ios::trunc);
    file_handle.close();
}
