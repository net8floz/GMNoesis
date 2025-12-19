enum GMNoesisVMType
{
	string,
	number,
	view_model
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
			
			var _is_collection = is_array(_type);
			if (_is_collection) {
				_type = _type[0];	
			}
			
			var _is_vm = is_struct(_type);
			if (_is_vm) {
				var _vm_names = struct_get_names(_type);
				var _vm_type_name = _vm_names[0];
				_type = GMNoesisVMType.view_model;
				noesis_vm_type_add_vm_definition(_name, _vm_type_name, _is_collection);
			} else {
				var _is_command = false;
				noesis_vm_type_add_definition(_name, _type, _is_collection, _is_command);
			}
		}
		
		if (is_struct(_definition[$ "commands"])) {
			var _commands = _definition.commands;
			
			var _names = struct_get_names(_commands);
			for (var _i = 0; _i < array_length(_names); _i++) {
				var _name = _names[_i];
				var _type = _commands[$ _name];
				
				var _type = _definition[$ _name];
			
				var _is_collection = is_array(_type);
				if (_is_collection) {
					_type = _type[0];	
				}
		
				var _is_command = true;
				noesis_vm_type_add_definition(_name, _type, _is_collection,  _is_command);
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
		var _is_collection = is_array(_type);
		
		if (_is_collection) {
			_type = _type[0];	
		}
		
		if (is_struct(_type)) {
			_type = GMNoesisVMType.view_model;
		}
		
		
		self[$ $"set_{_name}"] = method({this: _this, name: _name, type: _type, is_collection : _is_collection}, function(_val) {
			this[$ name] = _val;
			
			buffer_write(GMNoesis.read_buffer, buffer_u32, this.handle);
			buffer_write(GMNoesis.read_buffer, buffer_string, name);
			
			if (is_collection) {
				buffer_write(GMNoesis.read_buffer, buffer_u32, array_length(_val));
				
				for (var _i = 0; _i < array_length(_val); _i++) {
					switch (type) {
						case GMNoesisVMType.string:
							buffer_write(GMNoesis.read_buffer, buffer_string, string(_val[_i]));
							break;
						case GMNoesisVMType.number:
							buffer_write(GMNoesis.read_buffer, buffer_f32, _val[_i]);
							break;
						case GMNoesisVMType.view_model:
							buffer_write(GMNoesis.read_buffer, buffer_u32, _val[_i].handle);
							break;
					}
				}
			} else {
			
				switch (type) {
					case GMNoesisVMType.string:
						buffer_write(GMNoesis.read_buffer, buffer_string, string(_val));
						break;
					case GMNoesisVMType.number:
						buffer_write(GMNoesis.read_buffer, buffer_f32, _val);
						break;
					case GMNoesisVMType.view_model:
						buffer_write(GMNoesis.read_buffer, buffer_u32, _val.handle);
					break;
				}
			}
		});
		
		self[$ $"get_{_name}"] = method({this: _this, name: _name}, function() {
			return this[$ name];
		});
		
		if (_is_collection) {
			self[$ _name] = [];
		} else {
			switch (_type) {
				case GMNoesisVMType.string:
					self[$ _name] = "";
					break;
				case GMNoesisVMType.number:
					self[$ _name] = 0;
					break;
				case GMNoesisVMType.view_model:
					self[$ _name] = undefined; // nothing to really do here
					break;
			}
		}
	}
}

GMNoesis();