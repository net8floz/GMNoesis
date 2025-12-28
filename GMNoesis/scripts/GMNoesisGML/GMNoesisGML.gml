enum GMNoesisVMType
{
	string,
	number,
	boolean,
	view_model
}

function GMNoesis() {
	static buffer_size = 1024 * 1024 * 10;
	static read_buffer = buffer_create(buffer_size, buffer_fixed, 1);
	static write_buffer = buffer_create(buffer_size, buffer_fixed, 1);
	
	static defined_types = {};
	static vm_collection = {};
	
	static use_delta_time = false;
	
	static nassert = function(_cnd) {
		if (!_cnd) {
			throw "assert failed";	
		}
	}

	static load_view = function(_path) {
		noesis_load_view(_path);	
	}
	
	static set_view_model = function(_vm) {
		noesis_set_view_vm(_vm.handle);
	}
	
	static load_resources = function(_path) {
		noesis_load_application_resources(_path);	
	}
	
	static noesis_define_vm = function(_type_name, _definition) {
		GMNoesis.nassert(is_struct(_definition));
	
		noesis_vm_type_begin(_type_name);
	
		var _names = struct_get_names(_definition);
		for (var _i = 0; _i < array_length(_names); _i++) {
			var _name = _names[_i];

			
			if (_name == "commands") {
				continue;	
			}
			
			if (_name == "events") {
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
		
		if (is_struct(_definition[$ "events"])) {
			var _events = _definition.events;
			
			var _names = struct_get_names(_events);
			for (var _i = 0; _i < array_length(_names); _i++) {
				var _name = _names[_i];
				
				// todo: support arguments
				//var _type = _events[$ _name];
				//var _type = _definition[$ _name];
			
				//var _is_collection = is_array(_type);
				//if (_is_collection) {
				//	_type = _type[0];	
				//}

				noesis_vm_type_add_event(_name);
			}
		}
	
		noesis_vm_type_end();
	
		GMNoesis.defined_types[$ _type_name] = _definition;
	}
	
	var _res_buffer = buffer_create(1024 * 1024, buffer_fixed, 1);
	noesis_get_supported_resolutions(buffer_get_address(_res_buffer));
	

	static supported_resolutions = [];
	var _total_resolutions = noesis_get_supported_resolutions(buffer_get_address(_res_buffer));

	repeat (_total_resolutions) {
	    var _str = buffer_read(_res_buffer, buffer_string);
	    var parts = string_split(_str, "@");
	    var res_part = parts[0];
	    var fps_part = parts[1];

	    parts = string_split(res_part, "x");
	    var width = real(parts[0]);
	    var height = real(parts[1]);
	    var refresh = real(string_replace(fps_part, "Hz", ""));

	    var _found = false;
	    for (var i = 0; i < array_length(supported_resolutions); i++) {
	        if (supported_resolutions[i].width == width && supported_resolutions[i].height == height) {
	            array_push(supported_resolutions[i].refresh_rates, refresh);
	            _found = true;
	            break;
	        }
	    }

	    if (!_found) {
	        var resolution_info = {
	            width: width,
	            height: height,
	            refresh_rates: [refresh]
	        };
	        array_push(supported_resolutions, resolution_info);
	    }
	}
	
	noisis_loicense(
		extension_get_option_value("GMNoesis", "loicense_name"),
		extension_get_option_value("GMNoesis", "loicense_key")
	);

	noesis_initialize(window_handle(), 120, buffer_get_address(read_buffer),  buffer_get_address(write_buffer));
}


GMNoesis();