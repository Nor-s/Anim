// // #define PY_NO_LINK_LIB
// // #ifdef _DEBUG
// // #undef _DEBUG
// // #include <Python.h>
// // // #include <pybind11/embed.h> // everything needed for embedding
// // #define _DEBUG
// // #else
// // #include <Python.h>
// // // #include <pybind11/embed.h> // everything needed for embedding
// // #endif
// #ifdef _DEBUG
// #define _DEBUG_WAS_DEFINED 1
// #undef _DEBUG
// #endif
// #include <Python.h>
// #include <pybind11/pybind11.h>
// #include <pybind11/eval.h>
// #include <pybind11/embed.h>
// #ifdef _DEBUG_WAS_DEFINED
// #define _DEBUG 1
// #endif

// namespace py = pybind11;
// // using namespace py::literals;
// int main()
// {
//     // https://github.com/pybind/pybind11/issues/2369
//     // Py_SetPythonHome(L"C:/Users/No/anaconda3/envs/mp_gui_build");
//     Py_OptimizeFlag = 1;

//     Py_SetPythonHome(L"D:\\Study\\Anim\\build\\bin\\python");
//     // Py_SetPath(L"D:\\Study\\Anim\\build\\bin\\python\\DLLs;D:\\Study\\Anim\\build\\bin\\python\\Lib;D:\\Study\\Anim\\build\\bin\\python");
//     Py_Initialize();
//     PyEval_InitThreads();
//     PyRun_SimpleString("from time import time,ctime\n"
//                        "print('Today is', ctime(time()))\n");
//     PyObject *strret, *mymod, *strfunc, *strargs;

//     mymod = PyImport_ImportModule("cv2");
//     PyRun_SimpleString("from time import time,ctime\n"
//                        "print('Today is', ctime(time()))\n");
//     if (Py_FinalizeEx() < 0)
//     {
//         exit(120);
//     }
//     PyMem_RawFree(L"C:/vcpkg/installed/x64-windows/tools/python3");
//     return 0;
//     // Py_Finalize();
//     // py::scoped_interpreter guard{}; // start the interpreter and keep it alive
//     // py::module_ sys = py::module_::import("cv2");

//     // py::scoped_interpreter guard{};
//     // py::module_ sys = py::module_::import("cv2");
//     // py::print("Hello, World!"); // use the Python API
// }