var _buffer = GMNoesis.read_buffer;

if (buffer_tell(_buffer) > 0) {
	// indicate we are finished writing
	buffer_write(_buffer, buffer_u32, 0);
	noesis_vm_process_read_buffer();
	buffer_seek(_buffer, 0, 0);
}

noesis_update_view(timer);
timer += (1/60);