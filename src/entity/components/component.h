#ifndef ANIM_ENTITY_COMPONENT_COMPONENT_H
#define ANIM_ENTITY_COMPONENT_COMPONENT_H

#include <memory>
#include <vector>

namespace anim
{
    typedef int *TypeID;
    class Component
    {
    public:
        virtual ~Component() = default;
        virtual TypeID get_type() const = 0;
        virtual void update() = 0;
    };

    template <class T>
    class ComponentBase : public Component
    {
    public:
        virtual ~ComponentBase() = default;
        virtual void update() override{};
        static TypeID type;
        TypeID get_type() const override { return T::type; }
    };
    template <typename T>
    TypeID ComponentBase<T>::type((TypeID)&T::type);
}
#endif