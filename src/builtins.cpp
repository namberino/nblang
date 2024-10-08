//------------------------------------//
// Bản quyền © 2024 Nguyễn Bình Nam
// Được cấp phép theo Giấy phép Apache v2.0
//------------------------------------//

#include "builtins.hpp"

int NativeClock::arity()
{
    return 0;
}

std::any NativeClock::call(Interpreter& interpreter, std::vector<std::any> args)
{
    auto ticks = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration<double>{ticks}.count() / 100.0;
}

std::string NativeClock::to_string()
{
    return "<hàm đồng_hồ>";
}


int NativeTime::arity()
{
    return 0;
}

std::any NativeTime::call(Interpreter& interpreter, std::vector<std::any> args)
{
    std::time_t current_time = std::time(nullptr);
    char mbstr[100];

    std::locale::global(std::locale("vi_VN.UTF-8"));
    if (std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&current_time)))
        return std::string(mbstr);

    return "Can't access time";
}

std::string NativeTime::to_string()
{
    return "<hàm thời_gian>";
}


int NativeInput::arity()
{
    return 1;
}

std::any NativeInput::call(Interpreter& interpreter, std::vector<std::any> args)
{
    std::string prompt = std::any_cast<std::string>(args[0]);
    std::cout << prompt;

    std::string input;
    std::getline(std::cin, input);

    // stream the string into different variables
    std::istringstream iss(input);
    double result;

    if (!(iss >> result)) // input is not a double
        return input;

    return result;
}

std::string NativeInput::to_string()
{
    return "<hàm đầu_vào>";
}


int NativeExit::arity()
{
    return param_count;
}

std::any NativeExit::call(Interpreter& interpreter, std::vector<std::any> args)
{
    if (args.size() > 0)
        exit((int)std::any_cast<double>(args[0]));
    else
        exit(0);
}

std::string NativeExit::to_string()
{
    return "<hàm thoát>";
}


int NativeFloorDiv::arity()
{
    return 2;
}

std::any NativeFloorDiv::call(Interpreter& interpreter, std::vector<std::any> args)
{
    double div_res = std::any_cast<double>(args[0]) / std::any_cast<double>(args[1]);
    return floor(div_res);
}

std::string NativeFloorDiv::to_string()
{
    return "<hàm chia_nguyên>";
}

int NativeArrayLen::arity()
{
    return 1;
}

std::any NativeArrayLen::call(Interpreter& interpreter, std::vector<std::any> args)
{
    return (double)std::any_cast<std::shared_ptr<ListType>>(args[0])->get_length();
}

std::string NativeArrayLen::to_string()
{
    return "<hàm độ_dài>";
}
