enum GMNoesisVMType
{
	string,
	number
}

function GMNoesis() {
	static buffer_size = 1024 * 1024 * 10;
	static read_buffer = buffer_create(buffer_size, buffer_fixed, 1);
	static write_buffer = buffer_create(buffer_size, buffer_fixed, 1);
	
	static defined_types = {};
	static vm_collection = {};
	
	static assert = function(_cnd) {
		if (!_cnd) {
			throw "assert failed";	
		}
	}
	
	static noesis_define_vm = function(_type_name, _definition) {
		GMNoesis.assert(is_struct(_definition));
	
		noesis_vm_type_begin(_type_name);
	
		var _names = struct_get_names(_definition);
		for (var _i = 0; _i < array_length(_names); _i++) {
			var _name = _names[_i];
			
			if (_name == "commands") {
				continue;	
			}
			
			var _type = _definition[$ _name];
		
			noesis_vm_type_add_definition(_name, _type,  /* is_command */ false);
		}
		
		if (is_struct(_definition[$ "commands"])) {
			var _commands = _definition.commands;
			
			var _names = struct_get_names(_commands);
			for (var _i = 0; _i < array_length(_names); _i++) {
				var _name = _names[_i];
				var _type = _commands[$ _name];
				noesis_vm_type_add_definition(_name, _type,  /* is_command */ true);
			}
		}
	
		noesis_vm_type_end();
	
		GMNoesis.defined_types[$ _type_name] = _definition;
	}
	
	noisis_loicense(
		extension_get_option_value("GMNoesis", "loicense_name"),
		extension_get_option_value("GMNoesis", "loicense_key")
	);

	noesis_initialize(window_handle(), 120, buffer_get_address(read_buffer),  buffer_get_address(write_buffer));
}

function GMNoesisVM(_type_name, _definition) constructor {
	definition = _definition;
	type_name = _type_name;
	
	
	GMNoesis.noesis_define_vm(_type_name, _definition);
	
	// todo: must destroy VMs
	handle = noesis_create_vm(_type_name);
	
	// todo: weak pointers
	GMNoesis.vm_collection[$ handle] = self;
	
	var _this = self;
	var _names = struct_get_names(_definition);
	for (var _i = 0; _i < array_length(_names); _i++) {
		var _name = _names[_i];
		var _type = definition[$ _name];
		
		self[$ $"set_{_name}"] = method({this: _this, name: _name, type: _type}, function(_val) {
			this[$ name] = _val;
			
			switch (type) {
				case GMNoesisVMType.string:
					noesis_vm_set_string(this.handle, name, string(_val));
				break;
			case GMNoesisVMType.number:
					noesis_vm_set_number(this.handle, name, _val);
				break;
			}
		});
		
		self[$ $"get_{_name}"] = method({this: _this, name: _name}, function() {
			return this[$ name];
		});
		
		switch (_type) {
			case GMNoesisVMType.string:
				self[$ _name] = "";
				break;
			case GMNoesisVMType.number:
				self[$ _name] = 0;
				break;
		}
	}
}

GMNoesis();