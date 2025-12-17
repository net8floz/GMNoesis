
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
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
#include <string>
#include <NsCore/HighResTimer.h>
#include <mutex>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

std::atomic<bool> is_ready{false};

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

static std::mutex view_mutex;

static void render_frame(double current_time)
{
    std::lock_guard<std::mutex> lock(view_mutex);
    
    if (view && !view->Update(current_time))
    {
        return;
    }

    if (view)
    {
        view->GetRenderer()->UpdateRenderTree();
        view->GetRenderer()->RenderOffscreen();

        d3d_context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
    }
    constexpr float clear_color[4] = {0, 0, 0, 0};
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

    if (FAILED(swap_chain->Present(1, 0)))
    {
        std::cout << "swap chain failed\n";
    }
}

LRESULT CALLBACK overlay_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
{
    
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_MOUSEMOVE:
        if (view)
        {
            view->MouseMove(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param));
        }
        SendMessage(game_hwnd, msg, w_param, l_param);

        break;

    case WM_LBUTTONDOWN:
        if (view && !view->MouseButtonDown(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), Noesis::MouseButton_Left))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_LBUTTONUP:
        if (view && !view->MouseButtonUp(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), Noesis::MouseButton_Left))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_LBUTTONDBLCLK:
        if (view && !view->MouseDoubleClick(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), Noesis::MouseButton_Left))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_RBUTTONDOWN:
        if (view && !view->MouseButtonDown(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), Noesis::MouseButton_Right))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_RBUTTONUP:
        if (view && !view->MouseButtonUp(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), Noesis::MouseButton_Right))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_RBUTTONDBLCLK:
        if (view && !view->MouseDoubleClick(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), Noesis::MouseButton_Right))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_MBUTTONDOWN:
        if (view && !view->MouseButtonDown(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), Noesis::MouseButton_Middle))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_MBUTTONUP:
        if (view && !view->MouseButtonUp(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), Noesis::MouseButton_Middle))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_MBUTTONDBLCLK:
        if (view && !view->MouseDoubleClick(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), Noesis::MouseButton_Middle))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_MOUSEWHEEL:
        if (view && !view->MouseWheel(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), GET_WHEEL_DELTA_WPARAM(w_param)))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_MOUSEHWHEEL:
        if (view && !view->MouseHWheel(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), GET_WHEEL_DELTA_WPARAM(w_param)))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (view && !view->KeyDown((Noesis::Key)w_param))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (view && !view->KeyUp((Noesis::Key)w_param))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_CHAR:
        if (view && !view->Char((uint32_t)w_param))
            SendMessage(game_hwnd, msg, w_param, l_param);
        break;

    case WM_ACTIVATE:
        if (LOWORD(w_param) == WA_INACTIVE) {
            if (view)
            {
                view->Deactivate();
            }
        }
        else if (view)
        {
            view->Activate();
        }
        return 0;

    case WM_SIZE:
        g_width = LOWORD(l_param);
        g_height = HIWORD(l_param);
        if (view)
        {
            view->SetSize(g_width, g_height);
        }
        break;
    }

    return DefWindowProc(hwnd, msg, w_param, l_param);
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
        WS_EX_TRANSPARENT,
        TEXT("overlay_class"), TEXT(""),
        WS_CHILD | WS_VISIBLE,
        0, 0,
        g_width, g_height,
        game_hwnd, NULL, h_instance, NULL);

    if (!noesis_hwnd) return;

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
    
    Noesis::GUI::Init();
    Noesis::GUI::SetXamlProvider(Noesis::MakePtr<NoesisApp::LocalXamlProvider>("./Screens"));
    Noesis::GUI::SetFontProvider(Noesis::MakePtr<NoesisApp::LocalFontProvider>("./Screens"));
    const char* fonts[] = {"Fonts/#PT Root UI", "Arial", "Segoe UI Emoji"};
    Noesis::GUI::SetFontFallbacks(fonts, 3);
    Noesis::GUI::SetFontDefaultProperties(15.0f, Noesis::FontWeight_Normal, Noesis::FontStretch_Normal, Noesis::FontStyle_Normal);
    Noesis::GUI::SetTextureProvider(Noesis::MakePtr<NoesisApp::LocalTextureProvider>("./Screens"));

    Noesis::GUI::LoadApplicationResources(NoesisApp::Theme::DarkBlue());


    is_ready = true;
    MSG msg = {};
    const uint64_t start_ticks = Noesis::HighResTimer::Ticks();

    for (;;)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        render_frame(Noesis::HighResTimer::Seconds(Noesis::HighResTimer::Ticks() - start_ticks));
        
    }
}

static std::thread t;

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

    t = std::thread(overlay_thread);
    t.detach();
    
    return 1;
}

extern "C" __declspec(dllexport)
double gm_function_load_xaml(const char* filename)
{
    if (Noesis::Ptr<Noesis::FrameworkElement> xaml = Noesis::GUI::LoadXaml<Noesis::FrameworkElement>(filename))
    {
        std::lock_guard<std::mutex> lock(view_mutex); 
        view = Noesis::GUI::CreateView(xaml);
        view->SetSize(g_width, g_height);
        view->GetRenderer()->Init(render_device);
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
   return is_ready;
}