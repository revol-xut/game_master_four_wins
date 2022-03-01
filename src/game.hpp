#ifndef GAME_HPP
#define GAME_HPP

#include "game_field.hpp"

#include <sio_client.h>
#include <string>

constexpr const char* kAddress = "http://0.0.0.0:8080";

class Game {
private:
    GameField board_{};
    sio::client client_black_;
    sio::client client_white_;
    std::string session_id_;

    static auto get_bot_respone(const std::string& path) noexcept -> std::size_t;
    static void write_bot_response(const std::string& path, const std::string& value) noexcept;
    static void trunc_file(const std::string& path) noexcept;
    void write_to_website(GamePosition player, int column);
public:
    Game() noexcept;
    ~Game() noexcept = default;

    void mainloop() noexcept;
};

#endif // GAME_HPP
