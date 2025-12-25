
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
		
		if (_name == "commands") {
			var _command_names = struct_get_names(_definition.commands);
			for (var _ii = 0; _ii < array_length(_command_names); _ii++) {
				var _command_name = _command_names[_ii];
				self[$ _command_name] = new GMNoesisDelegate();
			}
		}
		
		if (_name == "events") {
			
			var _event_names = struct_get_names(_definition.events);
			for (var _ii = 0; _ii < array_length(_event_names); _ii++) {
				var _event_name = _event_names[_ii];
				var _this = self;
				self[$ $"execute_{_event_name}"] = method({ this: _this, event_name: _event_name}, function(){
					buffer_write(GMNoesis.read_buffer, buffer_u32, this.handle);
					buffer_write(GMNoesis.read_buffer, buffer_u8, /* is_event = */ 1);
					buffer_write(GMNoesis.read_buffer, buffer_string, event_name);
				});
			
			}
			
			continue;
		}
		
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
			buffer_write(GMNoesis.read_buffer, buffer_u8, /* is_event = */ 0);
			buffer_write(GMNoesis.read_buffer, buffer_string, name);
			
			if (is_collection) {
				buffer_write(GMNoesis.read_buffer, buffer_u32, array_length(_val));
				
				for (var _i = 0; _i < array_length(_val); _i++) {
					switch (type) {
						case GMNoesisVMType.string:
							buffer_write(GMNoesis.read_buffer, buffer_string, string(_val[_i]));
							break;
						case GMNoesisVMType.boolean:
							buffer_write(GMNoesis.read_buffer, buffer_bool, _val[_i] ? 1 : 0);
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
					case GMNoesisVMType.boolean:
						buffer_write(GMNoesis.read_buffer, buffer_bool, _val ? 1 : 0);
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
				case GMNoesisVMType.boolean:
					self[$ _name] = 0;
					break;
				case GMNoesisVMType.view_model:
					self[$ _name] = undefined; // nothing to really do here
					break;
			}
		}
	}
}