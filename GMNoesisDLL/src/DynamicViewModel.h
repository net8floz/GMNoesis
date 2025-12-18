
#pragma once
#include <NsCore/TypeClass.h>
#include <NsCore/Boxing.h>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <unordered_map>
#include <string>
#include <vector>

class DynamicObject : public NoesisApp::NotifyPropertyChangedBase
{
public:
    DynamicObject(const Noesis::TypeClass* in_type);

    const Noesis::TypeClass* GetClassType() const override;

    void SetValue(const std::string& name, Noesis::Ptr<BaseComponent> value);
    Noesis::Ptr<BaseComponent> GetValue(const std::string& name) const;

    static const Noesis::TypeClass* create_dynamic_type(
        const std::string& type_name,
        const std::vector<std::pair<std::string, const Noesis::Type*>>& properties);
private:
    const Noesis::TypeClass* type;
    std::unordered_map<std::string, Noesis::Ptr<BaseComponent>> values;
};
