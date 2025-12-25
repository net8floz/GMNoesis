function GMNoesisDelegate() constructor {
	static handle_id = 1;
	
	handles = [];
	
	on = function(fn) {
		var _id = GMNoesisDelegate.handle_id;
		handle_id++;
		
		array_push(handles, { id: _id, fn: fn });	
		
		return _id;
	}
	
	off = function(_handle_id) {
		for (var _i = 0; _i < array_length(handles); _i++) {
			if (handles[_i].id == _handle_id) {
				array_delete(handles, _i, 1);
				return true;
			}
		}
		
		return false;
	}
	
	invoke = function(_args) {
		for (var _i = 0; _i < array_length(handles); _i++) {
			handles[_i].fn(_args);
		}
	}
	
	reset = function() {
		handles = [];	
	}
}