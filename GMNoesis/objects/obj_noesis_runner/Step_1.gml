
if (noesis_vm_prepare_write_buffer_for_reading()) {
	var _buffer = GMNoesis.write_buffer;

	buffer_seek(_buffer, 0, 0);

	for (;;) {
		var _vm_id = buffer_read(_buffer, buffer_u32);

		if (_vm_id == 0) {
			noesis_vm_clear_write_buffer();
			break;
		}
	
		var _vm = GMNoesis.vm_collection[$ _vm_id];
	
		var _property_name = buffer_read(_buffer, buffer_string);
	
		switch (_vm.definition[$ _property_name]) {
			case GMNoesisVMType.string:
				var _str = buffer_read(_buffer, buffer_string);
				_vm[$ _property_name] = _str;
				// todo: events
				break;
			case GMNoesisVMType.number:
				throw "Not implemented!";
				break;
		}
	
	}
}

