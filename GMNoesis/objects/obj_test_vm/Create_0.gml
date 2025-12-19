
noesis_load_view("MainPage.xaml");

vm = new PlayerViewModel(); 

//vm.set_name("Player One!");
//vm.set_score(69);

vm.set_favorite_colors([
	"red", "green", "blue"	
]);

noesis_set_view_vm(vm.handle);

