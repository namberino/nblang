//------------------------------------//
// Bản quyền © 2024 Nguyễn Bình Nam
// Được cấp phép theo Giấy phép Apache v2.0
//------------------------------------//

#include <iostream>

#include "util.hpp"

Interpreter interpreter{};

int main(int argc, char* argv[])
{
    if (argc > 2) // too many arguments
    {
        std::cout << "Cách dùng: vila <script>.vila\n";
        exit(1);
    }
    else if (argc == 2) // run script file
    {
        char* point = strrchr(argv[1], '.');

        if(point != NULL)
        {
            if(strcmp(point, ".vila") != 0 && strcmp(point, ".vl") != 0) // ends with nbl
            {
                std::cout << "Định dạng file không hợp lệ, xin hãy dùng file với định dạng '.vila'\n";
                exit(1);
            }
        }
        else
        {
            std::cout << "Định dạng file không hợp lệ, xin hãy dùng file với định dạng '.vila'\n";
            exit(1);
        }

        run_file(argv[1], interpreter);
    }
    else // run interactive mode
    {
        run_prompt(interpreter);
        // prompt_load("./example/function/function-8.nbl");
    }

    return 0;
}
