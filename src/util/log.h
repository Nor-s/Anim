#include <iostream>
#include <string>

namespace anim
{
    inline static void LOG(const std::string &msg, bool is_ignore = false)
    {
#ifndef NDEBUG
        if (!is_ignore)
        {
            std::cout << msg << std::endl;
        }
#endif
    }
}