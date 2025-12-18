
#include "DynamicViewModel.h"
#include <NsCore/TypeProperty.h>

DynamicObject::DynamicObject(const Noesis::TypeClass* in_type) : type(in_type) {}

const Noesis::TypeClass* DynamicObject::GetClassType() const
{
    return type;
}

void DynamicObject::SetValue(const std::string& name, Noesis::Ptr<BaseComponent> value)
{
    values[name] = value;
    OnPropertyChanged(name.c_str());
}

Noesis::Ptr<Noesis::BaseComponent> DynamicObject::GetValue(const std::string& name) const
{
    auto it = values.find(name);
    return it != values.end() ? it->second : nullptr;
}

class DynamicTypeProperty : public Noesis::TypeProperty
{
public:
    DynamicTypeProperty(Noesis::Symbol name, const Noesis::Type* type)
        : Noesis::TypeProperty(name, type) {}

    Noesis::Ptr<Noesis::BaseComponent> GetComponent(const void* ptr) const override
    {
        const DynamicObject* obj = static_cast<const DynamicObject*>(ptr);
        return obj->GetValue(GetName().Str());
    }

    void SetComponent(void* ptr, Noesis::BaseComponent* value) const override
    {
        DynamicObject* obj = static_cast<DynamicObject*>(ptr);
        obj->SetValue(GetName().Str(), Noesis::Ptr<Noesis::BaseComponent>(value));
    }

    void* GetContent(const void* ptr) const override { return nullptr; }
};


const Noesis::TypeClass* DynamicObject::create_dynamic_type(
    const std::string& typeName,
    const std::vector<std::pair<std::string, const Noesis::Type*>>& properties)
{
    Noesis::Symbol typeSym(typeName.c_str());
    if (Noesis::Reflection::IsTypeRegistered(typeSym))
        return static_cast<const Noesis::TypeClass*>(Noesis::Reflection::GetType(typeSym));

    Noesis::TypeClass* type = new Noesis::TypeClass(typeSym, false);
    Noesis::Reflection::RegisterType(type);

    auto builder = (Noesis::TypeClassBuilder*)type;
    builder->AddBase(Noesis::TypeOf<NoesisApp::NotifyPropertyChangedBase>());

    for (auto& prop : properties)
    {
        builder->AddProperty(new DynamicTypeProperty(Noesis::Symbol(prop.first.c_str()), prop.second));
    }

    return type;
}