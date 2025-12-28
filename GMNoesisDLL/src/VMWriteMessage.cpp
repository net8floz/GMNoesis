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
    // std::cout << "resetting write buffer " << std::endl;
    out_buffer_current = nullptr;
}

void VMWriteMessage::prepare_write_buffer_for_reading()
{
    if (out_buffer_current != nullptr)
    {
        // game is going to read the write buffer so add a zero to mark end
        // std::cout << "writing zero end" << std::endl;
        constexpr uint32_t zero = 0;
        memcpy(out_buffer_current, &zero, sizeof(uint32_t));
    }
}

void VMWriteMessage::write_to_buffer(const VMWriteMessage& msg)
{
    size_t size = 0;
    if (out_buffer_current == nullptr)
    {
        // std::cout << "seeking to start of buffer" << std::endl;
        out_buffer_current = out_buffer_start;
    }

    // std::cout << "writing vm id " << msg.id << std::endl;
    memcpy(out_buffer_current, &msg.id, sizeof(uint32_t));
    out_buffer_current += sizeof(uint32_t);
    size += sizeof(uint32_t);
    // std::cout << "size is " << size << std::endl;
    
    // std::cout << "writing bool" << std::endl;
    memcpy(out_buffer_current, &msg.is_event, sizeof(bool));
    out_buffer_current += sizeof(bool);
    size += sizeof(bool);
    // std::cout << "size is " << size << std::endl;

    // std::cout << "writing string" << std::endl;
    memcpy(out_buffer_current, msg.property_name.c_str(), msg.property_name.size() + 1);
    out_buffer_current += msg.property_name.size() + 1;
    size += msg.property_name.size() + 1;
    // std::cout << "size is " << size << std::endl;
    
    const Noesis::Type* type = msg.boxed_value->GetClassType();

    if (type == Noesis::TypeOf<Noesis::Boxed<int>>())
    {
        // std::cout << "writing float" << std::endl;
        float val = static_cast<float>(Noesis::Boxing::Unbox<int>(msg.boxed_value));
        memcpy(out_buffer_current, &val, sizeof(float));
        out_buffer_current += sizeof(float);
        size += sizeof( float);
        // std::cout << "size is " << size << std::endl;
    }
    else if (type == Noesis::TypeOf<Noesis::Boxed<float>>())
    {
        // std::cout << "writing float" << std::endl;
        float val = Noesis::Boxing::Unbox<float>(msg.boxed_value);
        memcpy(out_buffer_current, &val, sizeof(float));
        out_buffer_current += sizeof(float);
        size += sizeof( float);
        // std::cout << "size is " << size << std::endl;
    }
    else if (type == Noesis::TypeOf<Noesis::Boxed<bool>>()) 
    {
        // std::cout << "writing bool" << std::endl;
        float val = Noesis::Boxing::Unbox<bool>(msg.boxed_value);
        memcpy(out_buffer_current, &val, sizeof(bool));
        out_buffer_current += sizeof(bool);
        size += sizeof( bool);
        // std::cout << "size is " << size << std::endl;
    }
    else if (type == Noesis::TypeOf<Noesis::Boxed<Noesis::String>>())
    {
        // std::cout << "writing string" << std::endl;
        Noesis::String val = Noesis::Boxing::Unbox<Noesis::String>(msg.boxed_value);
        memcpy(out_buffer_current, val.Str(), val.Size() + 1);
        out_buffer_current += val.Size() + 1;
        size += val.Size() + 1;
        // std::cout << "size is " << size << std::endl;
    }
    else
    {
        std::cout << "[DynamicViewModel] unknown type\n";
    }
}