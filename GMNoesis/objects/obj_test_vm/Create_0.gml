
noesis_load_view("MainPage.xaml");

vm = new PlayerViewModel(); 

vm.set_name("Player One!");
vm.set_score(69);

noesis_set_view_vm(vm.handle);

