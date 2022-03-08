

#include "game.hpp"

#include <algorithm>
#include <fstream>
#include <charconv>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <random>

auto random_string(std::size_t length) -> std::string {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string random_string;
    srand(time(nullptr));
    for (std::size_t i = 0; i < length; ++i)
    {
        random_string += CHARACTERS[rand() % CHARACTERS.size()]; // CHARACTERS[distribution(generator)];
    }

    return random_string;
}

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

Game::Game() noexcept {
    using namespace std::chrono_literals;

    auto connect = [](sio::event& e){};

    auto disconnect = [](sio::event& e){};

    auto setup = [](sio::event& e){};

    auto state_update = [](sio::event& e){};

    client_black_.socket()->on("connect", connect);
    client_white_.socket()->on("connect", connect);

    client_black_.socket()->on("disconnect", disconnect);
    client_white_.socket()->on("disconnect", disconnect);

    client_black_.socket()->on("setup", setup);
    client_white_.socket()->on("setup", setup);

    client_black_.socket()->on("state_update", state_update);
    client_white_.socket()->on("state_update", state_update);

    client_black_.connect(kAddress);
    std::this_thread::sleep_for(0.25s);
    client_white_.connect(kAddress);
    session_id_ = random_string(12);
    sio::message::ptr m = sio::object_message::create();
    m->get_map()["sessionId"] = sio::string_message::create(session_id_);
    sio::message::list li{m};

    std::this_thread::sleep_for(0.5s);
    client_black_.socket()->emit("setup", li);
    std::this_thread::sleep_for(0.25s);
    client_white_.socket()->emit("setup", li);
    std::this_thread::sleep_for(0.5s);
}

void Game::mainloop() noexcept {
    using namespace std::chrono_literals;
    const std::string player_1_file = "./black";
    const std::string player_2_file = "./white";
    std::string file;

    std::cout << "http://0.0.0.0:8080/#" << session_id_ << std::endl;
    std::string user_input;
    std::getline(std::cin, user_input);

    write_bot_response(player_1_file, "start");
    while (not board_.is_finished()) {
        auto current_player = board_.get_game_turn();
        file = not current_player ? player_1_file : player_2_file;

        std::cout << "Waiting for Player: " << file << std::endl;
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

        write_to_website(current_player ? GamePosition::Black : GamePosition::White , column);
        board_.player_move(column);

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
        std::this_thread::sleep_for(0.25s);
        // now the bot flushes the file
    }
    trunc_file(player_1_file);
    trunc_file(player_2_file);
    std::this_thread::sleep_for(1s);
}

void Game::trunc_file(const std::string &path) noexcept {
    std::ofstream file_handle(path, std::ios::trunc);
    file_handle.close();
}

void Game::write_to_website(GamePosition player, int column){
    auto m = sio::int_message::create(column);
    sio::message::list li{m};

    if(player == GamePosition::Black){
        client_black_.socket()->emit("placement", m);
    } else {
        client_white_.socket()->emit("placement", m);
    }

}

