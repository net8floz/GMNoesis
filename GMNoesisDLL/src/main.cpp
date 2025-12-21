
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <dxgi.h>
#include <ostream>
#include <dxgi1_2.h>
#include <thread>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IView.h>
#include <NsRender/D3D11Factory.h>
#include <NsApp/LocalXamlProvider.h>
#include <NsApp/LocalFontProvider.h>
#include <NsApp/LocalTextureProvider.h>
#include <NsApp/ThemeProviders.h>
#include <NsGui/IRenderer.h>
#include <NsGui/Uri.h>
#include <NsGui/FrameworkElement.h>
#include <NsCore/HighResTimer.h>
#include <NsApp/Interaction.h>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <NsCore/Package.h>
#include <NsGui/ObservableCollection.h>

#include "DynamicViewModel.h"
#include "GeneratedVMTypeData.h"
#include "NineSliceImage.h"
#include "VMWriteMessage.h"
#include "Windows/WindowsKeys.h"
#include <dcomp.h>     


static HWND noesis_hwnd = nullptr;
static HWND game_hwnd = nullptr;

static IDXGISwapChain1* swap_chain = nullptr;
static ID3D11Device* d3d_device = nullptr;
static ID3D11DeviceContext* d3d_context = nullptr;
static ID3D11RenderTargetView* render_target_view = nullptr;
static ID3D11DepthStencilView* depth_stencil_view = nullptr;

static int g_width = 1280;
static int g_height = 720;

static Noesis::Ptr<Noesis::IView> view;
static Noesis::Ptr<Noesis::RenderDevice> render_device;

static HHOOK hHook;
static uint64_t start_ticks;

NoesisApp::BehaviorCollection* behavior_collection;

static void render_frame()
{
    static bool view_init = false;
    
     if (!view_init)
     {
         if (view)
         {
             view->GetRenderer()->Init(render_device);
             view_init = true;
         }
     }
    
     if (view)
     {
         view->GetRenderer()->UpdateRenderTree();
         view->GetRenderer()->RenderOffscreen();
     }

    
    constexpr float clear_color[4] = { 0, 0, 0, 0 }; 
    d3d_context->OMSetRenderTargets(1, &render_target_view, nullptr);
    d3d_context->ClearRenderTargetView(render_target_view, clear_color);

    if (view)
    {
        view->GetRenderer()->Render();
    }
    D3D11_VIEWPORT viewport = {0, 0, (float)(g_width), (float)(g_height), 0.0f, 1.0f};
    d3d_context->RSSetViewports(1, &viewport);
    swap_chain->Present(1, 0);

}

LRESULT CALLBACK overlay_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    return DefWindowProc(hwnd, msg, w_param, l_param);
}

LRESULT CALLBACK hook_proc(int n_code, WPARAM w_param, LPARAM l_param)
{
    if (n_code >= 0 && w_param == PM_REMOVE)
    {
        MSG* msg = (MSG*)l_param;

        if (view)
        {

            switch (msg->message)
            {
            
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                {
                    if (w_param <= 0xff && GMNoesis::Windows::g_key_table[msg->wParam] != 0)
                    {
                        view->KeyDown(static_cast<Noesis::Key>(GMNoesis::Windows::g_key_table[msg->wParam]));
                        break;
                    }
                }
            case WM_KEYUP:
            case WM_SYSKEYUP:
                if (w_param <= 0xff && GMNoesis::Windows::g_key_table[msg->wParam] != 0)
                {
                    view->KeyUp(static_cast<Noesis::Key>(GMNoesis::Windows::g_key_table[msg->wParam]));
                    break;
                }
                
            case WM_CHAR:
                view->Char((uint32_t)msg->wParam);
                break;

            case WM_MOUSEMOVE:
                view->MouseMove(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));
                break;

            case WM_LBUTTONDOWN:
                view->MouseButtonDown(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam), Noesis::MouseButton_Left);
                break;

            case WM_LBUTTONUP:
                view->MouseButtonUp(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam), Noesis::MouseButton_Left);
                break;

            case WM_RBUTTONDOWN:
                view->MouseButtonDown(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam), Noesis::MouseButton_Right);
                break;

            case WM_RBUTTONUP:
                view->MouseButtonUp(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam), Noesis::MouseButton_Right);
                break;

            case WM_MOUSEWHEEL:
                view->MouseWheel(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam), GET_WHEEL_DELTA_WPARAM(msg->wParam));
                break;
            
            case WM_SIZE:
            case WM_MOVE:
            case WM_MOVING:
                RECT parentRect;
    
                if (GetWindowRect(game_hwnd, &parentRect))
                {
                    g_width = parentRect.right - parentRect.left;
                    g_height = parentRect.bottom - parentRect.top;

                    view->SetSize(g_width, g_height);
                    
                    SetWindowPos(noesis_hwnd, HWND_TOPMOST,
                                 parentRect.left, parentRect.top,
                                 g_width, g_height,
                                 SWP_NOACTIVATE | SWP_NOZORDER);
                }

                break;
            }
        }

        if (msg->message == WM_TIMER && msg->wParam == 1)
        {
            double time = Noesis::HighResTimer::Seconds(Noesis::HighResTimer::Ticks() - start_ticks);
            if (view)
            {


                RECT parentRect;
    
                if (GetWindowRect(game_hwnd, &parentRect))
                {
                    g_width = parentRect.right - parentRect.left;
                    g_height = parentRect.bottom - parentRect.top;

                    view->SetSize(g_width, g_height);
                    
                    SetWindowPos(noesis_hwnd, HWND_TOPMOST,
                                 parentRect.left, parentRect.top,
                                 g_width, g_height,
                                 SWP_NOACTIVATE | SWP_NOZORDER);
                }
                
                view->Update(time);

            }
        }
    }

    return CallNextHookEx(NULL, n_code, w_param, l_param);
}

static void overlay_thread()
{
    HINSTANCE h_instance = GetModuleHandle(NULL);
    
    WNDCLASS wc = {};
    wc.lpfnWndProc = overlay_proc;
    wc.hInstance = h_instance;
    wc.lpszClassName = TEXT("overlay_class");
    wc.hbrBackground = nullptr;
    RegisterClass(&wc);

    noesis_hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT,
        TEXT("overlay_class"), TEXT(""),
        WS_VISIBLE | WS_POPUP,
        0, 0, g_width, g_height,
        nullptr, nullptr, h_instance, NULL);

    if (!noesis_hwnd) {
        std::cerr << "CreateWindowEx failed: " << GetLastError() << std::endl;
        return;
    }

    SetWindowLongPtr(noesis_hwnd, GWLP_HWNDPARENT, (LONG_PTR)game_hwnd);
    
    D3D_FEATURE_LEVEL feature_level;
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &d3d_device, &feature_level, &d3d_context);

    if (FAILED(hr)) {
        std::cerr << "D3D11CreateDevice failed: 0x" << std::hex << hr << std::endl;
        return;
    }
 
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
    swap_chain_desc.Width = g_width;
    swap_chain_desc.Height = g_height;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferCount = 2;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swap_chain_desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    IDXGIFactory2* dxgi_factory = nullptr;
    hr = CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), (void**)&dxgi_factory);
    if (FAILED(hr)) {
        std::cerr << "CreateDXGIFactory2 failed: 0x" << std::hex << hr << std::endl;
        return;
    }

    hr = dxgi_factory->CreateSwapChainForComposition(d3d_device, &swap_chain_desc, nullptr, &swap_chain);
    dxgi_factory->Release();
    if (FAILED(hr)) {
        std::cerr << "CreateSwapChainForComposition failed: 0x" << std::hex << hr << std::endl;
        return;
    }
    
    IDXGIDevice* dxgi_device = nullptr;
    hr = d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device);
    if (FAILED(hr)) {
        std::cerr << "QueryInterface for IDXGIDevice failed: 0x" << std::hex << hr << std::endl;
        return;
    }
    
    IDCompositionDevice* dcomp_device = nullptr;
    hr = DCompositionCreateDevice(dxgi_device, __uuidof(IDCompositionDevice), (void**)&dcomp_device);
    dxgi_device->Release();
    if (FAILED(hr)) {
        std::cerr << "DCompositionCreateDevice failed: 0x" << std::hex << hr << std::endl;
        return;
    }
    
    IDCompositionTarget* dcomp_target = nullptr;
    hr = dcomp_device->CreateTargetForHwnd(noesis_hwnd, TRUE, &dcomp_target);
    if (FAILED(hr)) {
        std::cerr << "CreateTargetForHwnd failed: 0x" << std::hex << hr << std::endl;
        return;
    }

    IDCompositionVisual* dcomp_visual = nullptr;
    hr = dcomp_device->CreateVisual(&dcomp_visual);
    if (FAILED(hr)) {
        std::cerr << "CreateVisual failed: 0x" << std::hex << hr << std::endl;
        return;
    }

    hr = dcomp_visual->SetContent(swap_chain);
    if (FAILED(hr)) {
        std::cerr << "SetContent failed: 0x" << std::hex << hr << std::endl;
        return;
    }

    hr = dcomp_target->SetRoot(dcomp_visual);
    if (FAILED(hr)) {
        std::cerr << "SetRoot failed: 0x" << std::hex << hr << std::endl;
        return;
    }

    hr = dcomp_device->Commit();
    if (FAILED(hr)) {
        std::cerr << "Commit failed: 0x" << std::hex << hr << std::endl;
        return;
    }
    
    ID3D11Texture2D* back_buffer = nullptr;
    swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer);
    d3d_device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
    back_buffer->Release();

    render_device = NoesisApp::D3D11Factory::CreateDevice(d3d_context, false);
    
    MSG msg = {};

    for (;;)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        render_frame();
    }
}

extern "C" void NsRegisterReflectionAppInteractivity();
extern "C" void NsInitPackageAppInteractivity();

static char* message_read_buffer_start = nullptr; 
static char* message_read_buffer_current = nullptr;

extern "C" __declspec(dllexport) 
double gm_function_initialize(char* ptr, const double fps, char *event_read_buffer, char* event_write_buffer, const double buffer_size)
{
    game_hwnd = (HWND)ptr;

    VMWriteMessage::buffer_size = static_cast<size_t>(buffer_size);
    VMWriteMessage::out_buffer_start = event_write_buffer;
    VMWriteMessage::reset_write_buffer();

    message_read_buffer_start = event_read_buffer;

    HWND parent_hwnd = GetParent(game_hwnd);  
    RECT rect;
    if (GetClientRect(parent_hwnd, &rect))
    {
        g_width = rect.right - rect.left;
        g_height = rect.bottom - rect.top;
    }
    
    Noesis::GUI::SetLogHandler([](const char*, uint32_t, uint32_t level, const char*, const char* msg) 
    {
        const char* prefixes[] = { "T", "D", "I", "W", "E" };
        std::cout << "[NOESIS/" << prefixes[level] << "] " << msg << std::endl;
    });
    
    Noesis::GUI::Init();
    NsRegisterReflectionAppInteractivity();
    NsInitPackageAppInteractivity();

    NS_REGISTER_COMPONENT(NineSlice::NineSliceImage)
    
    Noesis::GUI::SetXamlProvider(Noesis::MakePtr<NoesisApp::LocalXamlProvider>("./Screens"));
    Noesis::GUI::SetFontProvider(Noesis::MakePtr<NoesisApp::LocalFontProvider>("./Screens"));
    const char* fonts[] = {"Fonts/#PT Root UI", "Arial", "Segoe UI Emoji"};
    Noesis::GUI::SetFontFallbacks(fonts, 3);
    Noesis::GUI::SetFontDefaultProperties(15.0f, Noesis::FontWeight_Normal, Noesis::FontStretch_Normal, Noesis::FontStyle_Normal);
    Noesis::GUI::SetTextureProvider(Noesis::MakePtr<NoesisApp::LocalTextureProvider>("./Screens"));
    NoesisApp::SetThemeProviders();
    Noesis::GUI::LoadApplicationResources(NoesisApp::Theme::DarkBlue());
    
    start_ticks = Noesis::HighResTimer::Ticks();
    
    DWORD gameThreadId = GetWindowThreadProcessId(game_hwnd, nullptr);
    hHook = SetWindowsHookEx(WH_GETMESSAGE, hook_proc, nullptr, gameThreadId);

    SetTimer(game_hwnd, 1, 1, nullptr);
    
    std::thread thread(overlay_thread);
    thread.detach();

    return 1;
}

extern "C" __declspec(dllexport)
double gm_function_load_xaml(const char* filename)
{
    if (Noesis::Ptr<Noesis::FrameworkElement> xaml = Noesis::GUI::LoadXaml<Noesis::FrameworkElement>(filename))
    {
        view = Noesis::GUI::CreateView(xaml);
        view->SetSize(g_width, g_height);
        view->Activate();
        return 1;
    } else
    {
        std::cout << "Failed to load Xaml\n";
    }

    return 0;
}

extern "C" __declspec(dllexport)
double gm_function_unload_xaml(char* ptr)
{
    if (view)
    {
        view->Deactivate();
    }

    view = nullptr;
    return 1;
}

extern "C" __declspec(dllexport)
double gm_function_is_ready()
{
    return true;
}

extern "C" __declspec(dllexport)
double gm_function_set_license(char* name, char* key)
{
    Noesis::GUI::SetLicense(name, key);
    return 1;
}

struct VMTypeBuilderState
{
    std::string type_name;
    std::vector<GeneratedVMTypeProperty> properties;
    std::vector<GeneratedVMTypeProperty> commands;
    std::vector<GeneratedVMTypeProperty> events;
};

static VMTypeBuilderState g_builder_state;
static const Noesis::TypeClass* g_last_created_type = nullptr;
static std::unordered_map<std::string, const Noesis::TypeClass*> g_registered_types;

extern "C" __declspec(dllexport)
double gm_function_create_vm_type_begin(char* type_name)
{
    g_builder_state.type_name = type_name;
    g_builder_state.properties.clear();
    g_builder_state.commands.clear();
    return 1; 
}

extern "C" __declspec(dllexport)
double gm_function_create_vm_type_add_vm_definition(char* property_name, char* vm_type_name, double is_collection)
{
    std::vector<GeneratedVMTypeProperty>& collection = g_builder_state.properties;
    
    if (static_cast<bool>(is_collection))
    {
        collection.emplace_back(GeneratedVMTypeProperty
        {
            Noesis::TypeOf<Noesis::BaseObservableCollection>(),
            property_name,
            vm_type_name,
            VMParamType::view_model,
            true
        });
    } else
    {
        collection.emplace_back(GeneratedVMTypeProperty
           {
               Noesis::TypeOf<Noesis::BaseComponent>(),
               property_name,
               vm_type_name,
                VMParamType::view_model,
               false
           });
    }

    return 1;
}

extern "C" __declspec(dllexport)
double gm_function_create_vm_type_add_event(char* property_name)
{
    g_builder_state.events.emplace_back(GeneratedVMTypeProperty{
        Noesis::TypeOf<Noesis::BaseComponent>(),
        property_name,
        "",
        VMParamType::number,
        false
    });

    return 0;
}

extern "C" __declspec(dllexport)
double gm_function_create_vm_type_add_definition(char* property_name, double type_enum, double is_collection, double is_command)
{
    const VMParamType type = static_cast<VMParamType>(static_cast<int>(type_enum)); 

    std::vector<GeneratedVMTypeProperty>& collection =
        static_cast<bool>(is_command)
        ? g_builder_state.commands
        : g_builder_state.properties;


    if (static_cast<bool>(is_collection))
    {
        collection.emplace_back(GeneratedVMTypeProperty
        {
            Noesis::TypeOf<Noesis::BaseObservableCollection>(),
            property_name,
            "",
            type,
            true
        });
    }
    else
    {
        switch (type)
        {
        case VMParamType::string:
            collection.emplace_back(GeneratedVMTypeProperty
            {
                Noesis::TypeOf<Noesis::String>(),
                property_name,
                "",
                type,
                false
            });
        
            break;
        case VMParamType::number:
            collection.emplace_back(GeneratedVMTypeProperty
            {
                Noesis::TypeOf<float>(),
                property_name,
                "",
                type,
                false
            });
            
            break;
        }
    }

    return 1;
}

extern "C" __declspec(dllexport)
double gm_function_create_vm_type_end()
{
    g_last_created_type = DynamicObject::create_dynamic_type(
        g_builder_state.type_name,
        g_builder_state.properties,
        g_builder_state.commands,
        g_builder_state.events);

    g_builder_state.properties.clear();
    g_builder_state.commands.clear();
    g_builder_state.events.clear();
    
    if (g_last_created_type)
    {
        g_registered_types.emplace(g_builder_state.type_name, g_last_created_type);
    }
    return g_last_created_type ? 1 : 0;
}

static std::unordered_map<int, DynamicObject*> g_vm_instances;
static int g_next_id = 1;

extern "C" __declspec(dllexport)
double gm_function_create_vm(char* type_name)
{
    auto it = g_registered_types.find(type_name);
    if (it == g_registered_types.end())
    {
        return -1;
    }

    const int id = g_next_id++;
    DynamicObject* instance = new DynamicObject(id, type_name, it->second);
    instance->register_commands();
    instance->register_events();
    g_vm_instances[id] = instance;

    return static_cast<double>(id);
}

extern "C" __declspec(dllexport)
double gm_function_destroy_vm(double id)
{
    int int_id = static_cast<int>(id);
    auto it = g_vm_instances.find(int_id);
    if (it != g_vm_instances.end())
    {
        delete it->second;
        g_vm_instances.erase(it);
        return 1;
    }
    return 0;
}

extern "C" __declspec(dllexport)
double gm_function_set_view_vm(double id)
{
    int int_id = static_cast<int>(id);
    auto it = g_vm_instances.find(int_id);
    if (it != g_vm_instances.end())
    {
        if (view)
        {
            view->GetContent()->SetDataContext(it->second);
            view->Activate();
            return 1;
        }
    }
    return 0;
}

extern "C" __declspec(dllexport)
double gm_function_vm_clear_write_buffer()
{
    VMWriteMessage::reset_write_buffer();
    return 1;
}

extern "C" __declspec(dllexport)
double gm_function_vm_prepare_write_buffer()
{
    VMWriteMessage::prepare_write_buffer_for_reading();
    return VMWriteMessage::out_buffer_current == nullptr ? 0 : 1;
}

extern "C" __declspec(dllexport)
double gm_function_vm_process_read_buffer()
{
    message_read_buffer_current = message_read_buffer_start;
    
    for (;;)
    {
        uint32_t view_model_id  = 0;
        memcpy(&view_model_id, message_read_buffer_current, sizeof(uint32_t));
        message_read_buffer_current += sizeof(uint32_t);

        if (view_model_id == 0)
        {
            message_read_buffer_current = nullptr;
            // end of message
            break;
        }

        uint8_t is_event  = 0;
        memcpy(&is_event, message_read_buffer_current, sizeof(uint8_t));
        message_read_buffer_current += sizeof(uint8_t);
        
        auto vm = g_vm_instances.find(static_cast<int>(view_model_id))->second;
        
        const std::string& type_name = g_vm_instances.find(static_cast<int>(view_model_id))->second->GetTypeName();
        const auto& type_data = GeneratedVMTypeData::registery.find(type_name)->second;
        
        const char* str_start = message_read_buffer_current;
        size_t length = std::strlen(str_start);
        std::string param_name(str_start, length);
        message_read_buffer_current += length + 1;

        if (static_cast<bool>(is_event))
        {
            vm->ExecuteEvent(param_name.c_str());   
        }
        else
        {
            const GeneratedVMTypeProperty* property_data = type_data.find_command_by_name(param_name);

            if (property_data->is_collection)
            {
                uint32_t expected_count  = 0;
                memcpy(&expected_count, message_read_buffer_current, sizeof(uint32_t));
                message_read_buffer_current += sizeof(uint32_t);

                Noesis::Ptr<Noesis::ObservableCollection<Noesis::BaseComponent>> collection = *new Noesis::ObservableCollection<Noesis::BaseComponent>();
                
                for (uint32_t index = 0; index < expected_count; index++)
                {
                    switch (property_data->vm_param_type)
                    {
                    case VMParamType::string:
                        {
                            const char* val_str_start = message_read_buffer_current;
                            size_t val_length = std::strlen(val_str_start);
                            std::string value(val_str_start, val_length);
                            message_read_buffer_current += val_length + 1;
                
                            collection->Add(Noesis::Boxing::Box<Noesis::String>(value.c_str()));
                            break;
                        }
                    case VMParamType::number:
                        {
                            float value  = 0;
                            memcpy(&value, message_read_buffer_current, sizeof(float));
                            message_read_buffer_current += sizeof(float);

                            collection->Add(Noesis::Boxing::Box<float>(value));
                            break;
                        }
                    
                    case VMParamType::view_model:
                        {
                            uint32_t ref_id  = 0;
                            memcpy(&ref_id, message_read_buffer_current, sizeof(uint32_t));
                            message_read_buffer_current += sizeof(uint32_t);

                            auto ref_vm = g_vm_instances.find(static_cast<int>(ref_id))->second;
                            collection->Add(ref_vm);
                            break;
                        }
                        break;
                    }
                }

                vm->SetValueNoEvent(param_name, collection);
            }
            else
            {
                switch (property_data->vm_param_type)
                {
                case VMParamType::string:
                    {
                        const char* val_str_start = message_read_buffer_current;
                        size_t val_length = std::strlen(val_str_start);
                        std::string value(val_str_start, val_length);
                        message_read_buffer_current += val_length + 1;
                
                        vm->SetValueNoEvent(param_name, Noesis::Boxing::Box<Noesis::String>(value.c_str()));
                        break;
                    }
                case VMParamType::number:
                    {
                        float value  = 0;
                        memcpy(&value, message_read_buffer_current, sizeof(float));
                        message_read_buffer_current += sizeof(float);
                        vm->SetValueNoEvent(param_name, Noesis::Boxing::Box(value));
                        break;
                    }
                case VMParamType::view_model:
                    {
                        uint32_t ref_id  = 0;
                        memcpy(&ref_id, message_read_buffer_current, sizeof(uint32_t));
                        message_read_buffer_current += sizeof(uint32_t);

                        auto ref_vm = g_vm_instances.find(static_cast<int>(ref_id))->second;
                        vm->SetValueNoEvent(param_name, Noesis::Ptr<DynamicObject>(ref_vm));
                        break;
                    }
                }
            }
        }
    }

    return 1;
}

extern "C" __declspec(dllexport)
double gm_function_load_application_resources(char* path)
{
    Noesis::GUI::LoadApplicationResources(path);
    return 1;
}