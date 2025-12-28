// ////////////////////////////////////////////////////////////////////////////////////////////////////
// // NoesisGUI - http://www.noesisengine.com
// // Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
// ////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// #include "ThicknessConverter.h"
//
// #include <NsCore/ReflectionImplement.h>
// #include <NsCore/ReflectionImplementEnum.h>
// #include <NsCore/ArrayRef.h>
// #include <NsDrawing/Thickness.h>
//
// #ifdef NS_HAVE_STUDIO
// #include <NsCore/Category.h>
// #endif
//
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// static inline float ExtractFloat(Noesis::BaseComponent* boxed)
// {
//     if (Noesis::Boxing::CanUnbox<float>(boxed))
//     {
//         return Noesis::Boxing::Unbox<float>(boxed);
//     }
//     else if (Noesis::Boxing::CanUnbox<Noesis::String>(boxed))
//     {
//         const Noesis::String& val = Noesis::Boxing::Unbox<Noesis::String>(boxed);
//         uint32_t charParsed = 0;
//         return Noesis::StrToFloat(val.Str(), &charParsed);
//     }
//     return 0.0f;
// }
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisApp::ThicknessConverter::ThicknessConverter(): mFormat(ThicknessFormat_Uniform)
// {
// }
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// bool NoesisApp::ThicknessConverter::TryConvert(Noesis::BaseComponent* value, const Noesis::Type*, Noesis::BaseComponent*,
//     Noesis::Ptr<Noesis::BaseComponent>& result)
// {
//     if (value == nullptr)
//     {
//         return false;
//     }
//
//     float f = ExtractFloat(value);
//
//     switch (mFormat)
//     {
//         case ThicknessFormat_Uniform:
//         {
//             result = Noesis::Boxing::Box(Noesis::Thickness(f));
//             break;
//         }
//         case ThicknessFormat_LeftRight:
//         {
//             result = Noesis::Boxing::Box(Noesis::Thickness(f, 0.0f));
//             break;
//         }
//         case ThicknessFormat_TopBottom:
//         {
//             result = Noesis::Boxing::Box(Noesis::Thickness(0.0f, f));
//             break;
//         }
//         case ThicknessFormat_Left:
//         {
//             result = Noesis::Boxing::Box(Noesis::Thickness(f, 0.0f, 0.0f, 0.0f));
//             break;
//         }
//         case ThicknessFormat_Right:
//         {
//             result = Noesis::Boxing::Box(Noesis::Thickness(0.0f, 0.0f, f, 0.0f));
//             break;
//         }
//         case ThicknessFormat_Top:
//         {
//             result = Noesis::Boxing::Box(Noesis::Thickness(0.0f, f, 0.0f, 0.0f));
//             break;
//         }
//         case ThicknessFormat_Bottom:
//         {
//             result = Noesis::Boxing::Box(Noesis::Thickness(0.0f, 0.0f, 0.0f, f));
//             break;
//         }
//         default: NS_ASSERT_UNREACHABLE;
//     }
//
//     return true;
// }
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// bool NoesisApp::ThicknessConverter::TryConvert(Noesis::ArrayRef<Noesis::BaseComponent*> values,
//     const Noesis::Type*, Noesis::BaseComponent*, Noesis::Ptr<Noesis::BaseComponent>& result)
// {
//     float l = 0.0f, t = 0.0f, r = 0.0f, b = 0.0f;
//
//     if (values.Size() == 1)
//     {
//         // Uniform
//         l = t = r = b = ExtractFloat(values[0]);
//     }
//     else if (values.Size() == 2)
//     {
//         // LeftRight
//         l = r = ExtractFloat(values[0]);
//
//         // TopBottom
//         t = b = ExtractFloat(values[1]);
//     }
//     else if (values.Size() == 4)
//     {
//         // Left
//         l = ExtractFloat(values[0]);
//
//         // Top
//         t = ExtractFloat(values[1]);
//
//         // Right
//         r = ExtractFloat(values[2]);
//
//         // Bottom
//         b = ExtractFloat(values[3]);
//     }
//
//     result = Noesis::Boxing::Box(Noesis::Thickness(l, t, r, b));
//     return true;
// }
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// bool NoesisApp::ThicknessConverter::TryConvertBack(Noesis::BaseComponent*,
//     Noesis::ArrayRef<const Noesis::Type*>, Noesis::BaseComponent*,
//     Noesis::BaseVector<Noesis::Ptr<Noesis::BaseComponent>>&)
// {
//     return false;
// }
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisApp::ThicknessFormat NoesisApp::ThicknessConverter::GetFormat() const
// {
//     return mFormat;
// }
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// void NoesisApp::ThicknessConverter::SetFormat(ThicknessFormat format)
// {
//     mFormat = format;
// }
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// NS_BEGIN_COLD_REGION
//
// NS_IMPLEMENT_REFLECTION(NoesisApp::ThicknessConverter, "NoesisGUIExtensions.ThicknessConverter")
// {
// #ifdef NS_HAVE_STUDIO
//     NsMeta<Noesis::Category>("Converters");
// #endif
//
//     NsImpl<IMultiValueConverter>();
//
//     NsProp("Format", &NoesisApp::ThicknessConverter::GetFormat, &NoesisApp::ThicknessConverter::SetFormat);
// }
//
// NS_IMPLEMENT_REFLECTION_ENUM(NoesisApp::ThicknessFormat, "NoesisGUIExtensions.ThicknessFormat")
// {
//     NsVal("Uniform", NoesisApp::ThicknessFormat_Uniform);
//     NsVal("LeftRight", NoesisApp::ThicknessFormat_LeftRight);
//     NsVal("TopBottom", NoesisApp::ThicknessFormat_TopBottom);
//     NsVal("Left", NoesisApp::ThicknessFormat_Left);
//     NsVal("Right", NoesisApp::ThicknessFormat_Right);
//     NsVal("Top", NoesisApp::ThicknessFormat_Top);
//     NsVal("Bottom", NoesisApp::ThicknessFormat_Bottom);
// }
//
// NS_END_COLD_REGION
