enum GMNoesisVMType
{
	string,
	number
}

function GMNoesis() {
	static defined_types = {};
	
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
			var _type = _definition[$ _name];
		
			switch (_type) {
				case GMNoesisVMType.number:
					noesis_vm_type_add_number(_name);
					break;
				case GMNoesisVMType.string:
					noesis_vm_type_add_string(_name);
					break;
				default:
					throw "unknown type";
					break;
			}
		}
	
		noesis_vm_type_end();
	
		GMNoesis.defined_types[$ _type_name] = _definition;
	}
}

function GMNoesisVM(_type_name, _definition) constructor {
	definition = _definition;
	type_name = _type_name;
	
	GMNoesis.noesis_define_vm(_type_name, _definition);
	
	// todo: must destroy VMs
	handle = noesis_create_vm(_type_name);
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