///----------------------------------------------------------------------------|
/// Счётчик билдов. 2025©
///----------------------------------------------------------------------------:
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <string>

#define l(a) std::cout << #a << " = " << (a) << '\n';

struct  Task
{       Task(){ go(); }

private:
    std::string_view mess{      "Counted-building: "};
    const char*  filename{"../../Counted-building.txt"};

    void go()
    {   size_t res = read();
        std::cout
            << "///---------------------------|\n"
            << "/// "<< std::format("{}{:5}   |\n", mess, ++res)
            << "///---------------------------|" << std::endl;
        std::ofstream f(filename);
        f << mess << res << std::endl;
    }

    size_t read()
    {   if(std::ifstream f(filename); f.is_open())
        {
            std::string s; std::getline(f, s, '\f');

            size_t p = s.find(mess);
            if(std::string::npos != p)
            try
            {   p += mess.size();
                size_t res = std::stoll(&s[p]);
                return res;
            }
            catch(...)
            {   return 0;
            }
        }
        return 0;
    }
};

int main(){ Task task; }
