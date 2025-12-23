#pragma once
#include <unordered_map>
#include <vector>
#include <xstring>
#include <NsCore/Ptr.h>

namespace Noesis
{
    class TypeClass;
    class Type;
}

enum class VMParamType 
{
    string,
    number,
    boolean,
    view_model
};

struct GeneratedVMTypeProperty
{
    const Noesis::Type* dynamic_noesis_type;
    std::string property_name;
    std::string vm_type_name;
    VMParamType vm_param_type;
    bool is_collection = false;
};

struct GeneratedVMTypeData
{
    std::string type_name;
    std::vector<GeneratedVMTypeProperty> properties;
    std::vector<GeneratedVMTypeProperty> commands;
    std::vector<GeneratedVMTypeProperty> events;

    const GeneratedVMTypeProperty* find_property_by_name(const std::string& property_name) const
    {
       for (auto& it : properties)
       {
           if (it.property_name == property_name)
           {
               return &it;
           }
       }

        return nullptr;
    }

    const GeneratedVMTypeProperty* find_command_by_name(const std::string& property_name) const
    {
        for (auto& it : properties)
        {
            if (it.property_name == property_name)
            {
                return &it;
            }
        }

        return nullptr;
    }

    const GeneratedVMTypeProperty* find_event_by_name(const std::string& property_name) const
    {
        for (auto& it : events)
        {
            if (it.property_name == property_name)
            {
                return &it;
            }
        }

        return nullptr;
    }
    
    static std::unordered_map<std::string, GeneratedVMTypeData> registery;
};