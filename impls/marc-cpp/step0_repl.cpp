#include <iostream>

std::string &read(std::string &line)
{
    return line;
}

std::string &eval(std::string &line)
{
    return line;
}

std::string &print(std::string &line)
{
    return line;
}

bool rep()
{
    std::cout << "user> ";

    std::string line;
    if (!std::getline(std::cin, line))
    {
        return false;
    }

    auto intermediate_read = read(line);
    auto intermediate_eval = eval(intermediate_read);
    auto intermediate_print = print(intermediate_eval);
    std::cout << intermediate_print << std::endl;
    return true;
}

int main(int argc, char **argv)
{
    while (rep())
        ;
    return 0;
}