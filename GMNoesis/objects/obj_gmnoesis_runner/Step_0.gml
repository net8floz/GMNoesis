if (keyboard_check_pressed(vk_enter) && keyboard_check(vk_alt)) {
	noesis_prep_for_fullscreen_change();
	window_set_fullscreen(!window_get_fullscreen());
}