#ifndef GAME_HPP
#define GAME_HPP

#include "game_field.hpp"
#include <string>

class Game {
private:
    GameField board_{};

    static auto get_bot_respone(const std::string& path) noexcept -> std::size_t;
    static void write_bot_response(const std::string& path, const std::string& value) noexcept;
    static void trunc_file(const std::string& path) noexcept;
public:
    Game() noexcept = default;
    ~Game() noexcept = default;

    void mainloop() noexcept;
};

#endif // GAME_HPP
