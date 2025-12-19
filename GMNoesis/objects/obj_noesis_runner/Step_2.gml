var _buffer = GMNoesis.read_buffer;

// indicate we are finished writing
buffer_write(_buffer, buffer_u8, 0);
noesis_vm_process_read_buffer();
buffer_seek(_buffer, 0, 0);