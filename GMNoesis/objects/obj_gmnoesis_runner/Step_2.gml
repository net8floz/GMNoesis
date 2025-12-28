var _buffer = GMNoesis.read_buffer;

if (buffer_tell(_buffer) > 0) {
	// indicate we are finished writing
	buffer_write(_buffer, buffer_u32, 0);
	noesis_vm_process_read_buffer();
	buffer_seek(_buffer, 0, 0);
}

noesis_update_view(timer);

if (GMNoesis.use_delta_time) {
	timer += delta_time / 1000000;
} else {
	timer += (1/game_get_speed(gamespeed_fps));
}