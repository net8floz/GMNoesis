
#pragma once
#include <array>
#include <Windows.h>
#include <NsGui/InputEnums.h>

namespace GMNoesis::Windows
{
    using namespace Noesis;
    
    inline constexpr std::array<int, 256> g_key_table = []() {
        std::array<int, 256> table{};
        
        table[VK_BACK] = Key_Back;
        table[VK_TAB] = Key_Tab;
        table[VK_CLEAR] = Key_Clear;
        table[VK_RETURN] = Key_Return;
        table[VK_PAUSE] = Key_Pause;

        table[VK_SHIFT] = Key_LeftShift;
        table[VK_LSHIFT] = Key_LeftShift;
        table[VK_RSHIFT] = Key_RightShift;
        table[VK_CONTROL] = Key_LeftCtrl;
        table[VK_LCONTROL] = Key_LeftCtrl;
        table[VK_RCONTROL] = Key_RightCtrl;
        table[VK_MENU] = Key_LeftAlt;
        table[VK_LMENU] = Key_LeftAlt;
        table[VK_RMENU] = Key_RightAlt;
        table[VK_LWIN] = Key_LWin;
        table[VK_RWIN] = Key_RWin;
        table[VK_ESCAPE] = Key_Escape;

        table[VK_SPACE] = Key_Space;
        table[VK_PRIOR] = Key_Prior;
        table[VK_NEXT] = Key_Next;
        table[VK_END] = Key_End;
        table[VK_HOME] = Key_Home;
        table[VK_LEFT] = Key_Left;
        table[VK_UP] = Key_Up;
        table[VK_RIGHT] = Key_Right;
        table[VK_DOWN] = Key_Down;
        table[VK_SELECT] = Key_Select;
        table[VK_PRINT] = Key_Print;
        table[VK_EXECUTE] = Key_Execute;
        table[VK_SNAPSHOT] = Key_Snapshot;
        table[VK_INSERT] = Key_Insert;
        table[VK_DELETE] = Key_Delete;
        table[VK_HELP] = Key_Help;
        
        table['0'] = Key_D0;
        table['1'] = Key_D1;
        table['2'] = Key_D2;
        table['3'] = Key_D3;
        table['4'] = Key_D4;
        table['5'] = Key_D5;
        table['6'] = Key_D6;
        table['7'] = Key_D7;
        table['8'] = Key_D8;
        table['9'] = Key_D9;
        
        table[VK_NUMPAD0] = Key_NumPad0;
        table[VK_NUMPAD1] = Key_NumPad1;
        table[VK_NUMPAD2] = Key_NumPad2;
        table[VK_NUMPAD3] = Key_NumPad3;
        table[VK_NUMPAD4] = Key_NumPad4;
        table[VK_NUMPAD5] = Key_NumPad5;
        table[VK_NUMPAD6] = Key_NumPad6;
        table[VK_NUMPAD7] = Key_NumPad7;
        table[VK_NUMPAD8] = Key_NumPad8;
        table[VK_NUMPAD9] = Key_NumPad9;

        table[VK_MULTIPLY] = Key_Multiply;
        table[VK_ADD] = Key_Add;
        table[VK_SEPARATOR] = Key_Separator;
        table[VK_SUBTRACT] = Key_Subtract;
        table[VK_DECIMAL] = Key_Decimal;
        table[VK_DIVIDE] = Key_Divide;
        
        table['A'] = Key_A;
        table['B'] = Key_B;
        table['C'] = Key_C;
        table['D'] = Key_D;
        table['E'] = Key_E;
        table['F'] = Key_F;
        table['G'] = Key_G;
        table['H'] = Key_H;
        table['I'] = Key_I;
        table['J'] = Key_J;
        table['K'] = Key_K;
        table['L'] = Key_L;
        table['M'] = Key_M;
        table['N'] = Key_N;
        table['O'] = Key_O;
        table['P'] = Key_P;
        table['Q'] = Key_Q;
        table['R'] = Key_R;
        table['S'] = Key_S;
        table['T'] = Key_T;
        table['U'] = Key_U;
        table['V'] = Key_V;
        table['W'] = Key_W;
        table['X'] = Key_X;
        table['Y'] = Key_Y;
        table['Z'] = Key_Z;
        
        table[VK_F1] = Key_F1;
        table[VK_F2] = Key_F2;
        table[VK_F3] = Key_F3;
        table[VK_F4] = Key_F4;
        table[VK_F5] = Key_F5;
        table[VK_F6] = Key_F6;
        table[VK_F7] = Key_F7;
        table[VK_F8] = Key_F8;
        table[VK_F9] = Key_F9;
        table[VK_F10] = Key_F10;
        table[VK_F11] = Key_F11;
        table[VK_F12] = Key_F12;
        table[VK_F13] = Key_F13;
        table[VK_F14] = Key_F14;
        table[VK_F15] = Key_F15;
        table[VK_F16] = Key_F16;
        table[VK_F17] = Key_F17;
        table[VK_F18] = Key_F18;
        table[VK_F19] = Key_F19;
        table[VK_F20] = Key_F20;
        table[VK_F21] = Key_F21;
        table[VK_F22] = Key_F22;
        table[VK_F23] = Key_F23;
        table[VK_F24] = Key_F24;

        table[VK_NUMLOCK] = Key_NumLock;
        table[VK_SCROLL] = Key_Scroll;
        
        table[VK_OEM_1] = Key_Oem1;
        table[VK_OEM_PLUS] = Key_OemPlus;
        table[VK_OEM_COMMA] = Key_OemComma;
        table[VK_OEM_MINUS] = Key_OemMinus;
        table[VK_OEM_PERIOD] = Key_OemPeriod;
        table[VK_OEM_2] = Key_Oem2;
        table[VK_OEM_3] = Key_Oem3;
        table[VK_OEM_4] = Key_Oem4;
        table[VK_OEM_5] = Key_Oem5;
        table[VK_OEM_6] = Key_Oem6;
        table[VK_OEM_7] = Key_Oem7;
        table[VK_OEM_8] = Key_Oem8;
        table[VK_OEM_102] = Key_Oem102;

        table[VK_CAPITAL] = Key_CapsLock;
        table[VK_APPS] = Key_Apps;
        table[VK_SLEEP] = Key_Sleep;

        return table;
    }();
}