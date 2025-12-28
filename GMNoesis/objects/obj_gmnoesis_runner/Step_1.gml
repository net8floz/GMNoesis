var _commands_to_execute = [];

var _buffer = GMNoesis.write_buffer;
buffer_seek(_buffer, 0, 0);
	
if (noesis_vm_prepare_write_buffer_for_reading()) {
	//show_debug_message("has data to read");


	for (;;) {
		//show_debug_message("reading u32");
		var _vm_id = buffer_read(_buffer, buffer_u32);
		//show_debug_message("size is " + string(buffer_tell(_buffer)));

		if (_vm_id == 0) {
			//show_debug_message("done reading buffer");
			noesis_vm_clear_write_buffer();
			break;
		}
		
		//show_debug_message("reading bool");
		var _is_event = buffer_read(_buffer, buffer_bool);
		//show_debug_message("size is " + string(buffer_tell(_buffer)));
		
		var _vm = GMNoesis.vm_collection[$ _vm_id];
	
		assert(is_struct(_vm), $"reading vm {_vm_id} is this valid??");
		
		//show_debug_message("reading string");
		var _property_name = buffer_read(_buffer, buffer_string);
		//show_debug_message(_property_name);
		//show_debug_message("size is " + string(buffer_tell(_buffer)));
	
		var _property_value = 0;
		
		var _definitions = _is_event
			? _vm.definition.commands
			: _vm.definition;
		
		// todo: handle collections
		var _property_type = _definitions[$ _property_name];
		
		switch (_property_type) {
			case GMNoesisVMType.string:
				//show_debug_message("reading string");
				_property_value = buffer_read(_buffer, buffer_string);
				//show_debug_message(_property_value);
		//show_debug_message("size is " + string(buffer_tell(_buffer)));
				break;
			case GMNoesisVMType.number:
			//show_debug_message("reading f32");
				_property_value = buffer_read(_buffer, buffer_f32);
					//show_debug_message("size is " + string(buffer_tell(_buffer)));
				break;
			case GMNoesisVMType.boolean:
			//show_debug_message("reading bool");
				_property_value = buffer_read(_buffer, buffer_bool);
		//show_debug_message("size is " + string(buffer_tell(_buffer)));
				break;
		}
		
		if (_is_event) {
			array_push(_commands_to_execute, [_vm, _property_name, _property_value]);
		} else {
			_vm[$ _property_name] = _property_value;
		}
	}
}

for (var _i = 0; _i < array_length(_commands_to_execute); _i++) {
	var _c = _commands_to_execute[_i];
	var _vm = _c[0];
	var _property_name = _c[1];
	var _property_value = _c[2];
	
	var _delegate = _vm[$ _property_name];
	_delegate.invoke(_property_value);	
}

