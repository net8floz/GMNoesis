noisis_loicense(
	extension_get_option_value("GMNoesis", "loicense_name"),
	extension_get_option_value("GMNoesis", "loicense_key")
);

noesis_initialize(window_handle())
noesis_load_view("MainPage.xaml");


vm = new MyViewModel(); 

vm.set_name("Player One!");
vm.set_score(69);



noesis_set_view_vm(vm.handle);



