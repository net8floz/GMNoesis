noisis_loicense(
	extension_get_option_value("GMNoesis", "loicense_name"),
	extension_get_option_value("GMNoesis", "loicense_key")
);

noesis_initialize(window_handle())
noesis_load_view("MainPage.xaml");
