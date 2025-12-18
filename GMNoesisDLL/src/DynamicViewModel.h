
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

class DynamicObject : public NoesisApp::NotifyPropertyChangedBase
{
public:
    DynamicObject(uint32_t in_handle, const std::string& type_name, const Noesis::TypeClass* in_type);

    const Noesis::TypeClass* GetClassType() const override;

    void SetValue(const std::string& name, Noesis::Ptr<BaseComponent> value);
    void SetValueNoEvent(const std::string& name, Noesis::Ptr<BaseComponent> value);
    Noesis::Ptr<BaseComponent> GetValue(const std::string& name) const;
    void register_commands();

    static const Noesis::TypeClass* create_dynamic_type(
        const std::string& type_name,
        const std::vector<std::pair<std::string, const Noesis::Type*>>& properties,
        const std::vector<std::pair<std::string, const Noesis::Type*>>& commands);

private:
    uint32_t handle = 0;
    std::string type_name;
    const Noesis::TypeClass* type;
    std::unordered_map<std::string, Noesis::Ptr<BaseComponent>> values;
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



