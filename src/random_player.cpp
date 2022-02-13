#include <vector>
#include <iostream>
#include <fstream>
#include <charconv>
#include <thread>

auto get_game_master_respone(const std::string& path) noexcept -> std::string {
    std::ifstream file_handle;
    std::string file_contents;
    do {
        file_handle.open(path);
        file_handle >> file_contents;
        file_handle.close();
    } while (file_contents.empty() or file_contents[0] == '>');

    return file_contents.substr(1, file_contents.size());
}

void trunc_file(const std::string& path) {
    std::ofstream file_handle(path, std::ios::trunc);
    file_handle.close();
}

void write_response(const std::string& path, unsigned short value) noexcept {
    std::ofstream file_handle(path, std::ios::trunc);
    file_handle << ">" + std::to_string(value);
    file_handle.close();
}

int main(int argc, char* argv[]) {
    using namespace std::chrono_literals;
    if(argc < 3) {
        std::cerr << "Not enough arguments supplied !" << std::endl;
        return 1;
    }
    const char* player = argv[1];
    std::string file = std::string(argv[2]);

    std::vector<unsigned char> field(16, 0);
    std::size_t number;
    srand(time(nullptr));

    while (true) {
        auto input = get_game_master_respone(file);
        trunc_file(file);

        if (input == "end") {
            break;
        } else if (input != "start"){
            std::cout << input << std::endl;
            auto result = std::from_chars(input.data(), input.data() + input.size(), number);
            if (result.ec == std::errc::invalid_argument) {
                std::cerr << "Could not convert.";
                break;
            }
            field[number] = field[number] + 1;

        }
        auto index = static_cast<unsigned short>(rand() % 16);
        while(field[index] == 3) {
            index = rand() % 16;
        }

        std::cout << "Selected Column: "<< index << std::endl;

        field[index] = field[index] + 1;
        write_response(file, index);
        std::this_thread::sleep_for(1s);
    }

    return 0;
}
