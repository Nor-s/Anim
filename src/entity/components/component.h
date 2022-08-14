#ifndef ANIM_ENTITY_COMPONENT_COMPONENT_H
#define ANIM_ENTITY_COMPONENT_COMPONENT_H

#include <memory>

namespace anim
{
    class Component
    {
    public:
        virtual ~Component() = default;
        virtual int get_type() const = 0;
        virtual void update() = 0;
    };

    template <class T>
    class ComponentBase : public Component
    {
    public:
        virtual ~ComponentBase() = default;
        virtual void update() = 0;
        static int type;
        int get_type() const override { return T::type; }
    };

    static int componentTypeNum = 0;
    template <typename T>
    int ComponentBase<T>::type(componentTypeNum++);
}

#endif