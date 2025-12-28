// ////////////////////////////////////////////////////////////////////////////////////////////////////
// // NoesisGUI - http://www.noesisengine.com
// // Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
// ////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
// #ifndef __APP_THICKNESSCONVERTERTER_H__
// #define __APP_THICKNESSCONVERTERTER_H__
//
//
// #include <NsCore/Noesis.h>
// #include <NsCore/String.h>
// #include <NsCore/ReflectionDeclareEnum.h>
// #include <NsGui/BaseValueConverter.h>
// #include <NsGui/IMultiValueConverter.h>
//
//
// namespace NoesisApp
// {
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// enum ThicknessFormat
// {
//     ThicknessFormat_Uniform,
//     ThicknessFormat_LeftRight,
//     ThicknessFormat_TopBottom,
//     ThicknessFormat_Left,
//     ThicknessFormat_Right,
//     ThicknessFormat_Top,
//     ThicknessFormat_Bottom
// };
//
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// class ThicknessConverter final : public Noesis::BaseValueConverter,
//     public Noesis::IMultiValueConverter
// {
// public:
//     ThicknessConverter();
//
//     bool TryConvert(Noesis::BaseComponent* value, const Noesis::Type* targetType,
//         Noesis::BaseComponent* parameter, Noesis::Ptr<Noesis::BaseComponent>& result) final override;
//
//     bool TryConvert(Noesis::ArrayRef<Noesis::BaseComponent*> values, const Noesis::Type* targetType,
//         Noesis::BaseComponent* parameter, Noesis::Ptr<Noesis::BaseComponent>& result) final override;
//
//     using BaseValueConverter::TryConvertBack;
//
//     bool TryConvertBack(Noesis::BaseComponent* value,
//         Noesis::ArrayRef<const Noesis::Type*> targetTypes, Noesis::BaseComponent* parameter,
//         Noesis::BaseVector<Noesis::Ptr<Noesis::BaseComponent>>& results) final override;
//
//     ThicknessFormat GetFormat() const;
//     void SetFormat(ThicknessFormat format);
//
//     NS_IMPLEMENT_INTERFACE_FIXUP
//
// private:
//     ThicknessFormat mFormat;
//
//     NS_DECLARE_REFLECTION(ThicknessConverter, BaseValueConverter)
// };
//
// }
//
// NS_DECLARE_REFLECTION_ENUM(NoesisApp::ThicknessFormat)
//
//
// #endif
