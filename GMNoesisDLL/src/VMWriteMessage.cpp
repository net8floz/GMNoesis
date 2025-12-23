#include "VMWriteMessage.h"

#include <cassert>
#include <NsCore/TypeProperty.h>
#include <NsCore/BaseComponent.h>
#include <NsCore/Type.h>
#include <NsCore/Boxing.h>
#include <iostream>

char* VMWriteMessage::out_buffer_current = nullptr;
char* VMWriteMessage::out_buffer_start = nullptr;
size_t VMWriteMessage::buffer_size = 0;

void VMWriteMessage::reset_write_buffer()
{
    out_buffer_current = out_buffer_start;
    constexpr uint32_t zero = 0;
    memcpy(out_buffer_start, &zero, sizeof(uint32_t));
}

void VMWriteMessage::prepare_write_buffer_for_reading()
{
    if (out_buffer_current != nullptr)
    {
        constexpr uint32_t zero = 0;
        memcpy(out_buffer_current, &zero, sizeof(uint32_t));
    }
}

void VMWriteMessage::write_to_buffer(const VMWriteMessage& msg)
{
    if (out_buffer_current == nullptr)
    {
        out_buffer_current = out_buffer_start;
    }
    
    memcpy(out_buffer_current, &msg.id, sizeof(uint32_t));
    out_buffer_current += sizeof(uint32_t);

    memcpy(out_buffer_current, &msg.is_event, sizeof(uint8_t));
    out_buffer_current += sizeof(uint8_t);
    
    memcpy(out_buffer_current, msg.property_name.data(), msg.property_name.size());
    out_buffer_current += msg.property_name.size();
    *out_buffer_current = '\0';
    out_buffer_current += 1;
    
    const Noesis::Type* type = msg.boxed_value->GetClassType();

    if (type == Noesis::TypeOf<Noesis::Boxed<int>>())
    {
        float val = static_cast<float>(Noesis::Boxing::Unbox<int>(msg.boxed_value));
        memcpy(out_buffer_current, &val, sizeof(float));
        out_buffer_current += sizeof(float);
    }
    else if (type == Noesis::TypeOf<Noesis::Boxed<float>>())
    {
        float val = Noesis::Boxing::Unbox<float>(msg.boxed_value);
        memcpy(out_buffer_current, &val, sizeof(float));
        out_buffer_current += sizeof(float);
    }
    else if (type == Noesis::TypeOf<Noesis::Boxed<bool>>())
    {
        float val = Noesis::Boxing::Unbox<bool>(msg.boxed_value);
        memcpy(out_buffer_current, &val, sizeof(bool));
        out_buffer_current += sizeof(bool);
    }
    else if (type == Noesis::TypeOf<Noesis::Boxed<Noesis::String>>())
    {
        Noesis::String val = Noesis::Boxing::Unbox<Noesis::String>(msg.boxed_value);
        memcpy(out_buffer_current, val.Str(), val.Size());
        out_buffer_current += val.Size();
        *out_buffer_current = '\0';
        out_buffer_current += 1;
    }
    else
    {
        std::cout << "[DynamicViewModel] unknown type\n";
    }
}