
#pragma once
#include <NsCore/TypeClass.h>
#include <NsCore/Boxing.h>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <unordered_map>
#include <string>
#include <vector>
#include <NsGui/ICommand.h>
#include <NsCore/Ptr.h>
#include <functional>
#include <NsCore/Delegate.h>
#include <NsCore/BaseComponent.h>
#include <NsCore/TypeProperty.h>

#include "GeneratedVMTypeData.h"

class DynamicEvent : public Noesis::BaseComponent
{
public:
    Noesis::EventHandler event_handler;
};

class DynamicObject : public NoesisApp::NotifyPropertyChangedBase
{
public:
    DynamicObject(uint32_t in_handle, const std::string& type_name, const Noesis::TypeClass* in_type);

    const Noesis::TypeClass* GetClassType() const override;

    void ExecuteEvent(const char* name);
    void SetValue(const std::string& name, Noesis::Ptr<BaseComponent> value);
    void SetValueNoEvent(const std::string& name, Noesis::Ptr<BaseComponent> value);
    Noesis::Ptr<BaseComponent> GetValue(const std::string& name) const;
    Noesis::EventHandler* GetEvent(const std::string& name) const;

    void register_commands();
    void register_events();

    std::string GetTypeName() const { return type_name; }

    static const Noesis::TypeClass* create_dynamic_type(
        const std::string& type_name,
        const std::vector<GeneratedVMTypeProperty>& properties,
        const std::vector<GeneratedVMTypeProperty>& commands,
        const std::vector<GeneratedVMTypeProperty>& events);


private:
    std::unordered_map<std::string, Noesis::Ptr<DynamicEvent>> events;
    std::unordered_map<std::string, Noesis::Ptr<BaseComponent>> values;
    uint32_t handle = 0;
    std::string type_name;
    const Noesis::TypeClass* type;
    bool b_skip_events = false;
};

class DynamicCommand : public Noesis::BaseComponent, public Noesis::ICommand
{
public:
    DynamicCommand(std::function<void(Noesis::BaseComponent*)> in_callback)
        : on_execute(in_callback)
    {}

    bool CanExecute(Noesis::BaseComponent* param) const override { return true; }

    void Execute(Noesis::BaseComponent* param) const override
    {
        on_execute(param);
    }

    Noesis::Delegate<void(Noesis::BaseComponent*, const Noesis::EventArgs&)>& CanExecuteChanged() override
    {
        return on_can_execute_changed;
    }

    NS_IMPLEMENT_INTERFACE_FIXUP

private:
    std::function<void(Noesis::BaseComponent*)> on_execute;
    Noesis::Delegate<void(Noesis::BaseComponent*, const Noesis::EventArgs&)> on_can_execute_changed;

    NS_DECLARE_REFLECTION(DynamicCommand, Noesis::BaseComponent)
};

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

    void* GetContent(const void* ptr) const override
    {
        return nullptr;
    }
};

class DynamicEventProperty : public Noesis::TypeProperty
{
public:
    DynamicEventProperty(Noesis::Symbol name)
        : Noesis::TypeProperty(name, nullptr) {}

    void* GetContent(const void* ptr) const override;
    bool IsReadOnly() const override { return true; }
    const void* Get(const void* ptr) const override;
};