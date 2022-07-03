#include "mp2mm.h"

Mp2mm::~Mp2mm()
{
    if (worker_.joinable())
    {
#ifndef NDEBUG
        std::cout << "Mp2mm::~Mp2mm : Detacth worker thread" << std::endl;
#endif
        worker_.detach();
    }
}
void Mp2mm::executeProcess(const char *process_name, std::string arg, std::atomic<bool> &is_done)
{

    std::filesystem::path process = std::filesystem::u8path(process_name);
    std::string abs_process = std::filesystem::absolute(process).string();

#ifdef _WIN32
    abs_process += ".exe";
#endif
    abs_process += " " + arg;
    try
    {
#ifndef NDEBUG
        std::cout << "executeProcess: " << abs_process << std::endl;
        std::cout << "begin: executeProcess-----------------\n";
#endif
        system(abs_process.c_str());
#ifndef NDEBUG
        std::cout << "end: executeProcess-----------------" << std::endl;
#endif
    }
    catch (std::exception &e)
    {
#ifndef NDEBUG

        std::cout << "Error: " << e.what() << std::endl;
#endif
    }
    is_done = true;
}
bool Mp2mm::open()
{
    if (is_done_)
    {
        is_done_ = false;
        if (worker_.joinable())
        {
#ifndef NDEBUG
            std::cout << "begin: thread.join\n";
#endif
            worker_.join();
#ifndef NDEBUG
            std::cout << "end: thread.join\n";
#endif
        }
        std::filesystem::path path_default_model = std::filesystem::u8path("model.json");
        std::filesystem::path path_default_anim = std::filesystem::u8path("anim.json");
        std::string arg = "--arg1 " + std::filesystem::absolute(path_default_model).string() + " --arg2 " +
                          std::filesystem::absolute(path_default_anim).string();

        worker_ = std::thread(executeProcess, "mp2mm/mp2mm", arg, std::ref(is_done_));

        return true;
    }
    else
    {
#ifndef NDEBUG
        std::cout << "mp2mm is running.\n";
#endif
    }
    return false;
}