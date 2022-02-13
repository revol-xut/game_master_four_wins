
#ifndef GAME_FIELD_HPP
#define GAME_FIELD_HPP

#include <array>
#include <iostream>

enum class GamePosition : unsigned char {
    Empty = 0,
    White = 1,
    Black = 2,
    Tie = 3
};

struct Point {
    std::size_t x_, y_, z_;
};

struct WinningLine {
    Point points_1_;
    Point points_2_;
    Point points_3_;
    Point points_4_;
};

class GameField {
private:
    class Column {
        private:
            unsigned char data_[4];

        public:
            Column() noexcept;
            ~Column() noexcept = default;
            auto operator[] (std::size_t index) const noexcept -> GamePosition {
                if( index > 3 ){
                    std::cerr << "Out of bounds" << std::endl;
                }
                return GamePosition(data_[index]);
            };

            void print() const noexcept {
                std::cout << "Column:" << std::endl;
                for (unsigned char i : data_) {
                    std::cout << std::to_string(i) << " ";
                } 
                std::cout << std::endl;
            } 
            auto get_fill() const noexcept -> std::size_t;
            void add_stone(GamePosition player) noexcept;
            auto is_full() const noexcept -> bool;
    };

    std::array<Column, 16> data_;
    bool game_finished_state_ = false;
    bool game_turn_ = false;
    GamePosition winner_ = GamePosition::Tie;
    WinningLine winning_line_{};
public:
    GameField() noexcept = default;
    ~GameField() noexcept = default;

    void place_stone(std::size_t position) noexcept;
    auto is_player_winning(GamePosition player) noexcept -> bool;
    void player_move(std::size_t position) noexcept;
    void write_data(const std::string& path) const noexcept;
    void print_solution() const noexcept;
    auto no_place_left() const noexcept -> bool;

    inline auto get_winner() const noexcept -> GamePosition {
        return winner_;
    };
    inline auto is_finished() const noexcept -> bool {
        return game_finished_state_;
    };
    inline auto get_game_turn() const noexcept -> bool {
        return game_turn_;
    };
};

#endif //GAME_FIELD_HPP
