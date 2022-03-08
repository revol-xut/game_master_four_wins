#include "game_field.hpp"

#include <iostream>
#include <fstream>

GameField::Column::Column() noexcept {
    std::fill(std::begin(data_), std::end(data_), static_cast<unsigned char>(GamePosition::Empty));
}

auto GameField::Column::get_fill() const noexcept -> std::size_t {
    for(auto i = 0; i < 4; i++){
        if((*this)[i] == GamePosition::Empty) {
            return i;
        }
    }
    return 4;
}

void GameField::Column::add_stone(GamePosition player) noexcept {
    auto pos = get_fill();
    data_[pos] = static_cast<unsigned char>(player);
}

auto GameField::Column::is_full() const noexcept -> bool {
    return get_fill() == 4;
}

void GameField::place_stone(std::size_t position) noexcept {
    // Invalid Move Terminating 
    if(data_[position].is_full()) {
        winner_ = not game_turn_ ? GamePosition::White : GamePosition::Black;
        game_finished_state_ = true;
        std::cout << "Column is full !: " << data_[position].get_fill() << " Col: " << std::to_string(position) << std::endl;
        data_[position].print();
        return;
    }
    auto player = game_turn_ ? GamePosition::White : GamePosition::Black;
    data_[position].add_stone(player);
}

void GameField::player_move(std::size_t position) noexcept {
    if(no_place_left()){
        winner_ = GamePosition::Tie;
        game_finished_state_ = true;
    } else{
        place_stone(position);
    }

    if(not game_finished_state_) { // was therefore a valid move
        auto player = game_turn_ ? GamePosition::White : GamePosition::Black;
        auto winning_move = is_player_winning(player);

        if(winning_move){
            winner_ = player;
            game_finished_state_ = true;
            print_solution();
            return;
        }

        // it is possible to play for a tie
    }

    game_turn_ = not game_turn_; // next player 
}

auto GameField::is_player_winning(GamePosition player) noexcept -> bool {
    // maybe add some output where the line was

    // very ugly open for alternatives
    auto check_vertical = [this](std::size_t offset, GamePosition player) {
        for (auto i = 0; i < 4; i++){
            if(data_[offset][i] != player){
                return false;
            }
        }
        winning_line_ = WinningLine{
                Point{offset % 4, offset / 4, 0},
                Point{offset % 4, offset / 4, 1},
                Point{offset % 4, offset / 4, 2},
                Point{offset % 4, offset / 4, 3}
        };

        return true;
    };

    auto check_horizontal_x = [this](std::size_t x, std::size_t z, GamePosition player) {
        for (auto i = 0; i < 4; i++){
            if(data_[i * 4 +  x][z] != player){
                return false;
            }
        }
        winning_line_ = WinningLine{
                Point{x, 0, z},
                Point{x, 1, z},
                Point{x, 2, z},
                Point{x, 3, z},
        };

        return true;
    };

    auto check_horizontal_y = [this](std::size_t y, std::size_t z, GamePosition player) {
        for (auto i = 0; i < 4; i++){
            if(data_[i + y * 4][z] != player){
                return false;
            }
        }

        winning_line_ = WinningLine{
                Point{0, y, z},
                Point{1, y, z},
                Point{2, y, z},
                Point{3, y, z},
        };

        return true;
    };

    auto check_diagonal_x_l_to_h = [this](std::size_t x, GamePosition player) {
        for (auto i = 0; i < 4; i++){
            if(data_[i * 4 +  x][i] != player){
                return false;
            }
        }

        winning_line_ = WinningLine{
                Point{x, 0, 0},
                Point{x, 1, 1},
                Point{x, 2, 2},
                Point{x, 3, 3},
        };

        return true;
    };

    auto check_diagonal_x_h_to_l = [this](std::size_t x, GamePosition player) {
        for (auto i = 0; i < 4; i++){
            if(data_[i * 4 +  x][3 - i] != player){
                return false;
            }
        }

        winning_line_ = WinningLine{
                Point{x, 0, 3},
                Point{x, 1, 2},
                Point{x, 2, 1},
                Point{x, 3, 0},
        };

        return true;
    };

    auto check_diagonal_y_l_to_h = [this](std::size_t y, GamePosition player) {
        for (auto i = 0; i < 4; i++){
            if(data_[i + y][i] != player){
                return false;
            }
        }

        winning_line_ = WinningLine{
                Point{0, y, 0},
                Point{1, y, 1},
                Point{2, y, 2},
                Point{3, y, 3},
        };

        return true;
    };

    auto check_diagonal_y_h_to_l = [this](std::size_t y, GamePosition player) {
        for (auto i = 0; i < 4; i++){
            if(data_[i + y][3 - i] != player){
                return false;
            }
        }

        winning_line_ = WinningLine{
                Point{0, y, 3},
                Point{1, y, 2},
                Point{2, y, 1},
                Point{3, y, 0},
        };

        return true;
    };

    auto diagonals_1 = [this] (std::size_t z, GamePosition player) {
        for (auto i = 0; i < 4; i++){
            if(data_[5 * i][z] != player){
                return false;
            }
        }

        winning_line_ = WinningLine{
                Point{0, 0, z},
                Point{1, 1, z},
                Point{2, 2, z},
                Point{3, 3, z},
        };

        return true;
    };

    auto diagonals_2 = [this] (std::size_t z, GamePosition player) {
        for (auto i = 0; i < 4; i++){
            if(data_[3 * (i + 1)][z] != player){
                return false;
            }
        }

        winning_line_ = WinningLine{
                Point{0, 1, z},
                Point{1, 2, z},
                Point{2, 3, z},
                Point{3, 4, z},
        };

        return true;
    };

    auto check_diagonal_through_cube = [this](GamePosition player) {
        std::size_t i1 = 0, i2 = 0, i3 = 0, i4 = 0;

        while (data_[i1 * 5][i1] == player and i1 < 4) {
            i1++;
        }

        if( i1 == 4) {
            winning_line_ = WinningLine{
                    Point{0, 0, 0},
                    Point{1, 1, 1},
                    Point{2, 2, 2},
                    Point{3, 3, 3},
            };

            return true;
        }

        while (data_[(i2 + 1) * 3][i2] == player and i2 < 4) {
            i2++;
        }

        if( i2 == 4) {
            winning_line_ = WinningLine{
                    Point{3, 0, 0},
                    Point{2, 1, 1},
                    Point{1, 2, 2},
                    Point{0, 3, 3},
            };

            return true;
        }

        while (data_[12 - i3 * 3][i3] == player and i3 < 4) {
            i3++;
        }

        if(i3 == 4) {
            winning_line_ = WinningLine{
                    Point{0, 3, 0},
                    Point{1, 2, 1},
                    Point{2, 1, 2},
                    Point{3, 0, 3},
            };

            return true;
        }

        while (data_[15 - i4 * 5][i4] == player and i4 < 4) {
            i4++;
        }

        if( i4 == 4) {
            winning_line_ = WinningLine{
                    Point{3, 3, 0},
                    Point{2, 2, 1},
                    Point{1, 1, 2},
                    Point{0, 0, 3},
            };

            return true;
        }

        return false;
    };
    // checking the 16 columns
    for (auto i = 0; i < 16; i++){
        if(check_vertical(i, player)){
            return true;
        }
    }

    // checking horizontal in the x direction
    for (auto i = 0; i < 16; i++){
        if(check_horizontal_x(i / 4, i % 4, player)){
            return true;
        }
    }

    // checking horizontal in the y direction
    for (auto i = 0; i < 16; i++){
        if(check_horizontal_y(i / 4, i % 4, player)){
            return true;
        }
    }

    // checking for diagonals in the x direction from low to high
    for (auto i = 0; i < 4; i++){
        if(check_diagonal_x_l_to_h(i, player)){
            return true;
        }
    }

    // checking for diagonals in the x direction from high to low
    for (auto i = 0; i < 4; i++){
        if(check_diagonal_x_h_to_l(i, player)){
            return true;
        }
    }

    // checking for diagonals in the y direction from low to high
    for (auto i = 0; i < 4; i++){
        if(check_diagonal_y_l_to_h(i, player)){
            return true;
        }
    }

    // checking for diagonals in the y direction from high to low
    for (auto i = 0; i < 4; i++){
        if(check_diagonal_y_h_to_l(i, player)){
            return true;
        }
    }

    // checking for diagonals with variables 4
    for (auto i = 0; i < 4; i++){
        if(diagonals_1(i, player)){
            return true;
        }
    }

    // checking for diagonals with variables 4
    for (auto i = 0; i < 4; i++){
        if(diagonals_2(i, player)){
            return true;
        }
    }

    // checking diagonals that go from corner to corner
    if(check_diagonal_through_cube(player)){
        return true;
    } else {
        return false;
    }
}

void GameField::write_data(const std::string& path) const noexcept {
    std::ofstream file_handle(path, std::ios::trunc);
    std::string content = "x; y; z; t\n";

    for(auto i = 0; i < 16; i++){
        for(auto j = 0; j < 4; j++){
            if(data_[i][j] != GamePosition::Empty){
                content += std::to_string(i / 4) + ";"
                        + std::to_string(i % 4) + ";"
                        + std::to_string(j) + ";"
                        + (data_[i][j] == GamePosition::Black ? "b\n" : "w\n");
            }
        }
    }

    file_handle << content;
    file_handle.close();
}

void GameField::print_solution() const noexcept {
    std::cout << "Winning Line:\n";
    std::cout << "x:" << std::to_string(winning_line_.points_1_.x_)
              << " y: " << std::to_string(winning_line_.points_1_.y_)
              << " z: " << std::to_string(winning_line_.points_1_.z_) << std::endl;
    std::cout << "x:" << std::to_string(winning_line_.points_2_.x_)
              << " y: " << std::to_string(winning_line_.points_2_.y_)
              << " z: " << std::to_string(winning_line_.points_2_.z_) << std::endl;
    std::cout << "x:" << std::to_string(winning_line_.points_3_.x_)
              << " y: " << std::to_string(winning_line_.points_3_.y_)
              << " z: " << std::to_string(winning_line_.points_3_.z_) << std::endl;
    std::cout << "x:" << std::to_string(winning_line_.points_4_.x_)
              << " y: " << std::to_string(winning_line_.points_4_.y_)
              << " z: " << std::to_string(winning_line_.points_4_.z_) << std::endl;

}

auto GameField::no_place_left() const noexcept -> bool {
    for (auto i = 0; i < 16; i++){
        if(not data_[i].is_full()) {
            return false;
        }
    }
    return true;
}
