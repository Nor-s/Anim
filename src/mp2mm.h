#ifndef SRC_PROCESS_H
#define SRC_PROCESS_H

#include <iostream>
#include <filesystem>
#include <thread>
#include <atomic>

class Mp2mm
{
public:
        Mp2mm() = default;
        ~Mp2mm();
        static void executeProcess(const char *process_name, std::string arg, std::atomic<bool> &is_done);
        bool open();

private:
        std::thread worker_;
        std::atomic<bool> is_done_{true};
};

#endif