
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <dxgi.h>
#include <ostream>
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
#include <random>

static HWND game_hwnd = nullptr;

static ID3D11Device* game_device = nullptr;
static ID3D11DeviceContext* game_context = nullptr;

static ID3D11Texture2D* game_back_buffer = nullptr;
static ID3D11RenderTargetView* render_target_view = nullptr;

static int g_width = 0;
static int g_height = 0;

static Noesis::Ptr<Noesis::IView> view = nullptr;
static Noesis::Ptr<Noesis::RenderDevice> render_device = nullptr;

static HHOOK hHook;
static uint64_t start_ticks;

static bool init_renderer = false;

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
            }
        }
    
        if (msg->message == WM_TIMER && msg->wParam == 1)
        {
            if (view)
            {
                if (init_renderer)
                {
                    const double time = Noesis::HighResTimer::Seconds(Noesis::HighResTimer::Ticks() - start_ticks);
                    view->Update(time);
                    view->GetRenderer()->UpdateRenderTree();
                    view->GetRenderer()->RenderOffscreen();
                }
                else if (render_device)
                {
                    init_renderer = true;
                    view->GetRenderer()->Init(render_device);
                }
            }
        }
    }
    return CallNextHookEx(NULL, n_code, w_param, l_param);
}

extern "C" void NsRegisterReflectionAppInteractivity();
extern "C" void NsInitPackageAppInteractivity();

static char* message_read_buffer_start = nullptr; 
static char* message_read_buffer_current = nullptr;

ID3D11Device* gDevice = nullptr;
ID3D11DeviceContext* gContext = nullptr;
ID3D11RenderTargetView* gRTV = nullptr;

typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain*, UINT, UINT);
typedef HRESULT(__stdcall* ResizeBuffers_t)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

Present_t oPresent = nullptr;
ResizeBuffers_t oResizeBuffers = nullptr;

static void* present_hook_ptr = nullptr;
static void* resize_hook_ptr = nullptr;
void initialize_swap_hooks()
{
    // Create dummy device and swap chain to get vtable
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 1;
    sd.BufferDesc.Height = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow();

    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;

    if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context)))
    {
        void** vtable = *reinterpret_cast<void***>(swapChain);
        
        present_hook_ptr = vtable[8]; 
        resize_hook_ptr = vtable[13];
        swapChain->Release();
        device->Release();
        context->Release();
    }
}

HRESULT __stdcall present_hook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (game_device && view)
    {
        if (view && init_renderer)
        {
            game_context->OMSetRenderTargets(1, &render_target_view, nullptr);
            D3D11_VIEWPORT viewport = {0, 0, (float)(g_width), (float)(g_height), -1000, 1000.f};
            game_context->RSSetViewports(1, &viewport);
            
            view->GetRenderer()->Render();
        }
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT __stdcall resize_hook(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{
    if (render_target_view) { render_target_view->Release(); render_target_view = nullptr; }
    if (game_back_buffer) { game_back_buffer->Release(); game_back_buffer = nullptr; }
    
    HRESULT hr = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    
    if (SUCCEEDED(hr))
    {
        if (!game_device)
        {
            pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&game_device);
            game_device->GetImmediateContext(&game_context);
            render_device = NoesisApp::D3D11Factory::CreateDevice(game_context, /* srgb */ false);
        }

        if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&game_back_buffer)))
        {
            game_device->CreateRenderTargetView(game_back_buffer, nullptr, &render_target_view);
        }

        if (view)
        {
            g_width = Width;
            g_height = Height;
            view->SetSize(Width, Height);
        }
    }

    return hr;
}

void hook_swap_chain()
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 1;
    sd.BufferDesc.Height = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow();

    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    IDXGISwapChain* swapChain = nullptr;
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;

    if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context)))
    {
        void** vtable = *reinterpret_cast<void***>(swapChain);
        DWORD oldProtect;

        // present
        VirtualProtect(&vtable[8], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
        oPresent = reinterpret_cast<Present_t>(vtable[8]);
        vtable[8] = reinterpret_cast<void*>(&present_hook);
        VirtualProtect(&vtable[8], sizeof(void*), oldProtect, &oldProtect);

        // resize
        VirtualProtect(&vtable[13], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
        oResizeBuffers = reinterpret_cast<ResizeBuffers_t>(vtable[13]);
        vtable[13] = reinterpret_cast<void*>(&resize_hook);
        VirtualProtect(&vtable[13], sizeof(void*), oldProtect, &oldProtect);

        swapChain->Release();
        device->Release();
        context->Release();
    }
}

extern "C" __declspec(dllexport) 
double gm_function_initialize(char* ptr, const double fps, char *event_read_buffer, char* event_write_buffer, const double buffer_size)
{
    game_hwnd = (HWND)ptr;

    VMWriteMessage::buffer_size = static_cast<size_t>(buffer_size);
    VMWriteMessage::out_buffer_start = event_write_buffer;
    VMWriteMessage::reset_write_buffer();

    message_read_buffer_start = event_read_buffer;
    
    RECT rect;
    if (GetClientRect(game_hwnd, &rect))
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

    hHook = SetWindowsHookEx(WH_GETMESSAGE, hook_proc, nullptr, GetWindowThreadProcessId(game_hwnd, nullptr));
    
    hook_swap_chain();

    SetTimer(game_hwnd, 1, 1, nullptr);
    
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