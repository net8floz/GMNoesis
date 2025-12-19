if (noesis_vm_prepare_write_buffer_for_reading()) {
	var _buffer = GMNoesis.write_buffer;

	buffer_seek(_buffer, 0, 0);

	for (;;) {
		var _vm_id = buffer_read(_buffer, buffer_u32);

		if (_vm_id == 0) {
			noesis_vm_clear_write_buffer();
			break;
		}
		
		var _is_event = buffer_read(_buffer, buffer_u8);
	
		var _vm = GMNoesis.vm_collection[$ _vm_id];
	
		var _property_name = buffer_read(_buffer, buffer_string);
	
		var _property_value = 0;
		
		var _collection = _is_event
			? _vm.definition.commands
			: _vm.definition;
		
		var _property_type = _collection[$ _property_name];
		
		switch (_property_type) {
			case GMNoesisVMType.string:
				_property_value = buffer_read(_buffer, buffer_string);
				break;
			case GMNoesisVMType.number:
				throw "Not implemented!";
				break;
		}
		
		if (_is_event) {
			_vm[$ $"handle_{_property_name}"](_property_value);
		} else {
			_vm[$ _property_name] = _property_value;
		}
		
	
	}
}

