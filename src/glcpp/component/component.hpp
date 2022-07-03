#ifndef GLCPP_COMPONENT_COMPONENT_HPP
#define GLCPP_COMPONENT_COMPONENT_HPP

namespace glcpp
{
    class Component
    {
    public:
        virtual ~Component() = default;
        virtual int get_type() const = 0;
    };

    template <class T>
    class ComponentBase : public Component
    {
    public:
        virtual ~ComponentBase() = default;
        static int type;
        int get_type() const override { return T::type; }
    };

    static int componentTypeNum = 0;
    template <typename T>
    int ComponentBase<T>::type(componentTypeNum++);
}

#endif