#include "py_manager.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define IS_WINDOWS
#include <Windows.h> // Needed by GetModuleFileNameW
#else
#include <libgen.h> // Needed by readlink
#endif

#include <filesystem>

#include <iostream>

#ifdef _DEBUG
#define _DEBUG_WAS_DEFINED 1
#undef _DEBUG
#endif
#include <Python.h>
#ifdef _DEBUG_WAS_DEFINED
#define _DEBUG 1
#endif

#include <pybind11/pybind11.h>
#include <pybind11/eval.h>
#include <pybind11/embed.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;
using namespace py::literals;

///=============================================================================
#ifdef IS_WINDOWS
std::wstring getExecutableDir()
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    const auto executableDir = std::wstring(exePath);
    const auto pos = executableDir.find_last_of('\\');
    if (pos != std::string::npos)
        return executableDir.substr(0, pos);
    return L"\\";
}
#else
std::wstring getExecutableDir()
{
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    if (count != -1)
    {
        const auto path = std::string(dirname(result));
        return std::wstring(path.begin(), path.end());
    }
    return L"/";
}
#endif

namespace anim
{
    PyManager *PyManager::get_instance()
    {
        if (instance_ == nullptr)
        {
            instance_ = new PyManager();
        }
        return instance_;
    }

    PyManager::PyManager()
    {
        const auto exe_path = std::filesystem::path(getExecutableDir());
        auto python_path = exe_path / "python";

#ifdef IS_WINDOWS
        const std::wstring pythonHome = python_path.wstring();
        auto lib_path = python_path / "Lib";
        auto dll_path = python_path / "DLLs";
        auto site_path = lib_path / "site-packages";
        auto app_path = exe_path / "py_module";

        const std::wstring pythonPath = lib_path.wstring() + L";" +
                                        dll_path.wstring() + L";" +
                                        site_path.wstring() + L";" +
                                        app_path.wstring() + L";" +
                                        python_path.wstring() + L";" +
                                        exe_path.wstring();
#ifndef NDEBUG
#endif
        std::wcout << pythonPath << "\n";
#else
        // TODO: implement for Linux
#endif
        Py_OptimizeFlag = 1;
        Py_SetPath(pythonPath.c_str());
        Py_SetPythonHome(pythonHome.c_str());
        py::initialize_interpreter();

        py::exec(R"(
            import json
            import py_module.mp_manager
            import py_module.mp_helper
            mpm = py_module.mp_manager.MediapipeManager()
         )");
    }
    PyManager::~PyManager()
    {
        if (instance_ != nullptr)
        {
            delete instance_;
            instance_ = nullptr;
        }
        py::finalize_interpreter();
    }

    void PyManager::get_mediapipe_pose(const MediapipeInfo &mp_info)
    {
        try
        {
            auto locals = py::dict("video_path"_a = mp_info.video_path,
                                   "model"_a = mp_info.model_info,
                                   "output_path"_a = mp_info.output_path,
                                   "show_plot"_a = mp_info.show_plot,
                                   "model_complexity"_a = mp_info.model_complexity,
                                   "min_detection_confidence"_a = mp_info.min_detection_confidence,
                                   "min_visibility"_a = mp_info.min_visibility,
                                   "custom_fps"_a = mp_info.fps);

            py::exec(R"(
                mpm.set_key(model_complexity, False, min_detection_confidence)
                mpm.is_show_result = show_plot
                mpm.min_visibility = min_visibility
                mpm.fps = custom_fps
                
                _, animjson = py_module.mp_helper.mediapipe_to_mixamo(mpm, model, video_path)
                with open(output_path, 'w') as f:
                    json.dump(animjson, f, indent=2)
            )",
                     py::globals(), locals);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

}
