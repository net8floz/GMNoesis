// ======================= Implementation ===========================
#include "DynamicViewModel.h"
#include <D2DBaseTypes.h>
#include <NsApp/BehaviorCollection.h>
#include <NsApp/DataEventTrigger.h>
#include <NsApp/InvokeCommandAction.h>
#include <NsApp/TriggerBase.h>
#include <NsCore/ReflectionImplement.h>
#include <NsGui/ObservableCollection.h>
#include "VMWriteMessage.h"
#include "GeneratedVMTypeData.h"

DynamicObject::DynamicObject(const uint32_t in_handle, const std::string& in_type_name, const Noesis::TypeClass* in_type)
    : handle(in_handle), type_name(in_type_name), type(in_type)
{
    PropertyChanged() += [this](Noesis::BaseComponent* sender, const Noesis::PropertyChangedEventArgs& args)
    {
        const char* changed_property = args.propertyName.Str();
        const auto* obj = dynamic_cast<DynamicObject*>(sender);
        if (obj->b_skip_events) return;

        const Noesis::Ptr<BaseComponent> current_value = obj->GetValue(changed_property);
        VMWriteMessage::write_to_buffer({ handle, false, changed_property, current_value });
    };
}

void DynamicObject::register_commands()
{
    auto data = GeneratedVMTypeData::registery.find(type_name);
    for (const auto& command_property : data->second.commands)
    {
        values[command_property.property_name] = *new DynamicCommand([this, command_name = command_property.property_name](Noesis::BaseComponent* param)
        {
            VMWriteMessage::write_to_buffer({ handle, true, command_name, Noesis::Ptr<BaseComponent>(param) });
        });
    }
}

void DynamicObject::register_events()
{
    auto data = GeneratedVMTypeData::registery.find(type_name);
    for (const auto& command_property : data->second.events)
    {
        events[command_property.property_name] = *new DynamicEvent();
    }
}

const Noesis::TypeClass* DynamicObject::GetClassType() const
{
    return type;
}

void DynamicObject::SetValue(const std::string& name, Noesis::Ptr<BaseComponent> value)
{
    values[name] = value;
    OnPropertyChanged(name.c_str());
}

void DynamicObject::SetValueNoEvent(const std::string& name, Noesis::Ptr<BaseComponent> value)
{
    b_skip_events = true;
    values[name] = value;
    OnPropertyChanged(name.c_str());
    b_skip_events = false;
}

void DynamicObject::ExecuteEvent(const char* name)
{
    events[name]->event_handler.Invoke(this, Noesis::EventArgs::Empty);   
}

Noesis::Ptr<Noesis::BaseComponent> DynamicObject::GetValue(const std::string& name) const
{
    auto it = values.find(name);
    return (it != values.end()) ? it->second : nullptr;
}

const Noesis::TypeClass* DynamicObject::create_dynamic_type(
    const std::string& type_name,
    const std::vector<GeneratedVMTypeProperty>& properties,
    const std::vector<GeneratedVMTypeProperty>& commands,
    const std::vector<GeneratedVMTypeProperty>& events)
{
    Noesis::Symbol type_sym(type_name.c_str());
    if (Noesis::Reflection::IsTypeRegistered(type_sym))
        return static_cast<const Noesis::TypeClass*>(Noesis::Reflection::GetType(type_sym));

    Noesis::TypeClass* type = new Noesis::TypeClass(type_sym, false);
    Noesis::Reflection::RegisterType(type);

    auto builder = (Noesis::TypeClassBuilder*)type;
    builder->AddBase(Noesis::TypeOf<NoesisApp::NotifyPropertyChangedBase>());

    for (auto& prop : properties)
        builder->AddProperty(new DynamicTypeProperty(Noesis::Symbol(prop.property_name.c_str()), prop.dynamic_noesis_type));

    for (auto& command : commands)
        builder->AddProperty(new DynamicTypeProperty(Noesis::Symbol(command.property_name.c_str()), Noesis::TypeOf<Noesis::ICommand>()));

    for (auto& event : events)
        builder->AddEvent(new DynamicEventProperty(Noesis::Symbol(event.property_name.c_str())));
    
    GeneratedVMTypeData::registery.emplace(type_name, GeneratedVMTypeData{ type_name, properties, commands, events });
    return type;
}

NS_IMPLEMENT_REFLECTION(DynamicCommand)
{
    NsImpl<Noesis::ICommand>();
}

const void* DynamicEventProperty::Get(const void* ptr) const
{
    const DynamicObject* obj = static_cast<const DynamicObject*>(ptr);
    return obj->GetEvent(GetName().Str());
}

void* DynamicEventProperty::GetContent(const void* ptr) const
{
    DynamicObject* obj = const_cast<DynamicObject*>(static_cast<const DynamicObject*>(ptr));
    return obj->GetEvent(GetName().Str());
}

Noesis::EventHandler* DynamicObject::GetEvent(const std::string& name) const
{
    auto it = events.find(name);
    if (it != events.end() && it->second != nullptr)
    {
        return &(it->second->event_handler); 
    }
    return nullptr;
}


