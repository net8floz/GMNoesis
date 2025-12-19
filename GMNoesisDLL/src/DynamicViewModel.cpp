
#include "DynamicViewModel.h"

#include <D2DBaseTypes.h>
#include <NsApp/InvokeCommandAction.h>
#include <NsCore/ReflectionImplement.h>
#include <NsCore/TypeProperty.h>
#include <NsGui/ObservableCollection.h>

#include "GeneratedVMTypeData.h"
#include "VMWriteMessage.h"



DynamicObject::DynamicObject(const uint32_t in_handle, const std::string& in_type_name, const Noesis::TypeClass* in_type)
    : handle(in_handle)
    , type_name(in_type_name)
    , type(in_type)
{
    PropertyChanged() += [this](Noesis::BaseComponent* sender, const Noesis::PropertyChangedEventArgs& args)
    {
        const char* changed_property = args.propertyName.Str();
        
        const auto* obj = dynamic_cast<DynamicObject*>(sender);
        if (obj->b_skip_events)
        {
            return;
        }
        
        const Noesis::Ptr<BaseComponent> current_value = obj->GetValue(changed_property);
        
        VMWriteMessage::write_to_buffer({
            handle,
            false,
            changed_property,
            current_value
        });
    };
}

void DynamicObject::register_commands()
{
    auto data = GeneratedVMTypeData::registery.find(type_name);
    for (const auto& command_property : data->second.commands)
    {
        const Noesis::Ptr<DynamicCommand> cmd = *new DynamicCommand([this, command_name = command_property.property_name](Noesis::BaseComponent* param)
        {
            Noesis::BaseComponent* out_param = param;

            if (param == nullptr)
            {
                out_param = Noesis::Boxing::Box<float>(0);
            }
            
            VMWriteMessage::write_to_buffer({
                handle,
                true,
                command_name,
                Noesis::Ptr<BaseComponent>(out_param) 
            });
        });

        values[command_property.property_name] = cmd;
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

Noesis::Ptr<Noesis::BaseComponent> DynamicObject::GetValue(const std::string& name) const
{
    auto it = values.find(name);
    if (it != values.end())
    {
        auto val = it->second;
    }
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

    void* GetContent(const void* ptr) const override
    {
        return nullptr;
    }
};

const Noesis::TypeClass* DynamicObject::create_dynamic_type(
    const std::string& type_name,
    const std::vector<GeneratedVMTypeProperty>& properties,
    const std::vector<GeneratedVMTypeProperty>& commands)
{
    Noesis::Symbol type_sym(type_name.c_str());
    if (Noesis::Reflection::IsTypeRegistered(type_sym))
        return static_cast<const Noesis::TypeClass*>(Noesis::Reflection::GetType(type_sym));

    Noesis::TypeClass* type = new Noesis::TypeClass(type_sym, false);
    Noesis::Reflection::RegisterType(type);

    auto builder = (Noesis::TypeClassBuilder*)type;
    builder->AddBase(Noesis::TypeOf<NoesisApp::NotifyPropertyChangedBase>());

    for (auto& prop : properties)
    {
        builder->AddProperty(new DynamicTypeProperty(Noesis::Symbol(prop.property_name.c_str()), prop.dynamic_noesis_type));   
    }

    for (auto& command : commands)
    {
        builder->AddProperty(new DynamicTypeProperty(Noesis::Symbol(command.property_name.c_str()),  Noesis::TypeOf<Noesis::ICommand>()));
    }

    GeneratedVMTypeData::registery.emplace(type_name, GeneratedVMTypeData{
        type_name,
        properties,
        commands
    });
    
    return type;
}

NS_IMPLEMENT_REFLECTION(DynamicCommand)
{
    NsImpl<Noesis::ICommand>();
}