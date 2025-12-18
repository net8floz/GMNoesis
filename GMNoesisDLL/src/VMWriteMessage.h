#pragma once
#include <cstdint>
#include <xstring>
#include <NsCore/Ptr.h>

namespace Noesis
{
    class BaseComponent;
}

struct VMWriteMessage
{
    uint32_t id = 0;
    bool is_event = false;
    std::string property_name;
    Noesis::Ptr<Noesis::BaseComponent> boxed_value;

    static char* out_buffer_current;
    static char* out_buffer_start;
    static size_t buffer_size;

    static void reset_write_buffer();
    static void prepare_write_buffer_for_reading();
    static void write_to_buffer(const VMWriteMessage& msg);
};
