
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
#include <NsApp/Interaction.h>
#include <string>
#include <iostream>

static HWND noesis_hwnd = nullptr;
static HWND game_hwnd = nullptr;

static IDXGISwapChain* swap_chain = nullptr;
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
    if (view)
    {
        view->GetRenderer()->UpdateRenderTree();
        view->GetRenderer()->RenderOffscreen();
    }
    
    d3d_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
    
    constexpr float clear_color[4] = {0, 0,0, 0};
    d3d_context->ClearRenderTargetView(render_target_view, clear_color);
    if (depth_stencil_view)
    {
        d3d_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);
    }

    D3D11_VIEWPORT viewport = {0, 0, (float)(g_width), (float)(g_height), 0.0f, 1.0f};
    d3d_context->RSSetViewports(1, &viewport);

    if (view)
    {
        view->GetRenderer()->Render();
    }

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
                view->KeyDown((Noesis::Key)msg->wParam);
                break;

            case WM_KEYUP:
            case WM_SYSKEYUP:
                view->KeyUp((Noesis::Key)msg->wParam);
                break;

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
                g_width = LOWORD(l_param);
                g_height = HIWORD(l_param);
                if (view)
                {
                    view->SetSize(g_width, g_height);
                }
                break;
            }
        }

        if (msg->message == WM_TIMER && msg->wParam == 1)
        {
            double time = Noesis::HighResTimer::Seconds(Noesis::HighResTimer::Ticks() - start_ticks);
            if (view)
            {
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
    RegisterClass(&wc);
    

    noesis_hwnd = CreateWindowEx(
        WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        TEXT("overlay_class"), TEXT(""),
        WS_CHILD | WS_VISIBLE | WS_DISABLED,
        0, 0,
        g_width, g_height,
        game_hwnd, NULL, h_instance, NULL);

    if (!noesis_hwnd) return;
    
    SetLayeredWindowAttributes(noesis_hwnd, 0, 255, LWA_ALPHA);
    
    DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
    swap_chain_desc.BufferCount = 1;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = noesis_hwnd;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.Windowed = TRUE;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL feature_level;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        nullptr, 0, D3D11_SDK_VERSION,
        &swap_chain_desc, &swap_chain, &d3d_device, &feature_level, &d3d_context);
    if (FAILED(hr)) return;

    ID3D11Texture2D* back_buffer = nullptr;
    swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer);
    d3d_device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
    back_buffer->Release();

    D3D11_TEXTURE2D_DESC depth_desc = {};
    depth_desc.Width = g_width;
    depth_desc.Height = g_height;
    depth_desc.MipLevels = 1;
    depth_desc.ArraySize = 1;
    depth_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_desc.SampleDesc.Count = 1;
    depth_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depth_tex = nullptr;
    d3d_device->CreateTexture2D(&depth_desc, nullptr, &depth_tex);
    d3d_device->CreateDepthStencilView(depth_tex, nullptr, &depth_stencil_view);
    depth_tex->Release();

    render_device = NoesisApp::D3D11Factory::CreateDevice(d3d_context, false);
    view->GetRenderer()->Init(render_device);
    
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

static std::thread t;

extern "C" void NsRegisterReflectionAppInteractivity();
extern "C" void NsInitPackageAppInteractivity();

extern "C" __declspec(dllexport)
double gm_function_initialize(char* ptr)
{
    game_hwnd = (HWND)ptr;

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
    Noesis::GUI::SetXamlProvider(Noesis::MakePtr<NoesisApp::LocalXamlProvider>("./Screens"));
    Noesis::GUI::SetFontProvider(Noesis::MakePtr<NoesisApp::LocalFontProvider>("./Screens"));
    const char* fonts[] = {"Fonts/#PT Root UI", "Arial", "Segoe UI Emoji"};
    Noesis::GUI::SetFontFallbacks(fonts, 3);
    Noesis::GUI::SetFontDefaultProperties(15.0f, Noesis::FontWeight_Normal, Noesis::FontStretch_Normal, Noesis::FontStyle_Normal);
    Noesis::GUI::SetTextureProvider(Noesis::MakePtr<NoesisApp::LocalTextureProvider>("./Screens"));
    Noesis::GUI::LoadApplicationResources(NoesisApp::Theme::DarkBlue());
    start_ticks = Noesis::HighResTimer::Ticks();
    
    DWORD gameThreadId = GetWindowThreadProcessId(game_hwnd, NULL);
    hHook = SetWindowsHookEx(WH_GETMESSAGE, hook_proc, NULL, gameThreadId);

    SetTimer(game_hwnd, 1, 1, NULL);
    
    t = std::thread(overlay_thread);
    t.detach();

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

