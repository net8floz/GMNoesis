////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "LayoutScaler.h"
#include <NsGui/Decorator.h>
#include <NsGui/ScaleTransform.h>
#include <NsGui/UIElementData.h>
#include <NsGui/FrameworkPropertyMetadata.h>
#include <NsGui/ContentPropertyMetaData.h>

#ifdef NS_HAVE_STUDIO
#include <NsGui/StudioMeta.h>
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
NoesisApp::LayoutScaler::LayoutScaler()
{
    Noesis::Ptr<Noesis::Decorator> decorator = *new Noesis::Decorator(false);
    decorator->SetLayoutTransform(Noesis::MakePtr<Noesis::ScaleTransform>());
    SetSingleVisualChild(decorator);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const Noesis::Size& NoesisApp::LayoutScaler::GetReferenceResolution() const
{
    return GetValue<Noesis::Size>(ReferenceResolutionProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void NoesisApp::LayoutScaler::SetReferenceResolution(const Noesis::Size& v)
{
    SetValue<Noesis::Size>(ReferenceResolutionProperty, v);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
float NoesisApp::LayoutScaler::GetScaleFactor() const
{
    return GetValue<float>(ScaleFactorProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void NoesisApp::LayoutScaler::SetScaleFactor(float v)
{
    SetValue<float>(ScaleFactorProperty, v);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::UIElement* NoesisApp::LayoutScaler::GetChild() const
{
    return ((Noesis::Decorator*)GetSingleVisualChild())->GetChild();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void NoesisApp::LayoutScaler::SetChild(UIElement* child)
{
    Noesis::Decorator* decorator = (Noesis::Decorator*)GetSingleVisualChild();
    Noesis::Ptr<UIElement> oldChild(decorator->GetChild());
    if (oldChild != child)
    {
        decorator->SetChild(0);
        RemoveLogicalChild(oldChild.GetPtr());

        AddLogicalChild(child);
        decorator->SetChild(child);

        InvalidateMeasure();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t NoesisApp::LayoutScaler::GetVisualChildrenCount() const
{
    return 1u;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Visual* NoesisApp::LayoutScaler::GetVisualChild(uint32_t index) const
{
    NS_ASSERT(index == 0u);
    return GetSingleVisualChild();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void NoesisApp::LayoutScaler::CloneOverride(FrameworkElement* clone_,
    Noesis::FrameworkTemplate* template_) const
{
    FrameworkElement* child = Noesis::DynamicCast<FrameworkElement*>(GetChild());
    if (child != nullptr)
    {
        LayoutScaler* clone = (LayoutScaler*)clone_;
        if (clone->GetChild() == nullptr)
        {
            FrameworkElement* templatedParent = clone->GetTemplatedParent();
            Noesis::Decorator* decorator = (Noesis::Decorator*)clone->GetSingleVisualChild();
            decorator->SetTemplatedParent(templatedParent, 0);
            Noesis::Ptr<FrameworkElement> childClone = child->Clone(clone, templatedParent, template_);
            clone->SetChild(childClone);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t NoesisApp::LayoutScaler::GetLogicalChildrenCount() const
{
    return GetChild() != 0 ? 1u : 0u;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<Noesis::BaseComponent> NoesisApp::LayoutScaler::GetLogicalChild(uint32_t) const
{
    return Noesis::Ptr<BaseComponent>(GetChild());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void NoesisApp::LayoutScaler::OnTemplatedParentChanged(FrameworkElement* old_, FrameworkElement* new_)
{
    if (old_ != 0 && new_ == 0)
    {
        // clean templated parent from decorator
        Noesis::Decorator* decorator = (Noesis::Decorator*)GetSingleVisualChild();
        decorator->SetTemplatedParent(0, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static float CalculateScale(const Noesis::Size& size, const Noesis::Size& ref, float scaleFactor)
{
    float hRatio = Noesis::IsInfinity(size.width) ? 1.0f : (size.width / ref.width);
    float vRatio = Noesis::IsInfinity(size.height) ? 1.0f : (size.height / ref.height);
    return Noesis::Max(0.1f, scaleFactor) * Noesis::Min(hRatio, vRatio);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static void UpdateScale(Noesis::ScaleTransform* transform, float scale)
{
    NS_ASSERT(transform != nullptr);
    transform->SetScaleX(scale);
    transform->SetScaleY(scale);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Size NoesisApp::LayoutScaler::MeasureOverride(const Noesis::Size& availableSize)
{
    Noesis::Decorator* decorator = (Noesis::Decorator*)GetSingleVisualChild();
    if (decorator != nullptr)
    {
        float scale = CalculateScale(availableSize, GetReferenceResolution(), GetScaleFactor());
        UpdateScale((Noesis::ScaleTransform*)decorator->GetLayoutTransform(), scale);

        decorator->Measure(availableSize);
    }

    return availableSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Size NoesisApp::LayoutScaler::ArrangeOverride(const Noesis::Size& finalSize)
{
    Noesis::Decorator* decorator = (Noesis::Decorator*)GetSingleVisualChild();
    if (decorator != nullptr)
    {
        float scale = CalculateScale(finalSize, GetReferenceResolution(), GetScaleFactor());
        UpdateScale((Noesis::ScaleTransform*)decorator->GetLayoutTransform(), scale);

        decorator->Arrange(Noesis::Rect(finalSize));
    }

    return finalSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION(NoesisApp::LayoutScaler, "NoesisGUIExtensions.LayoutScaler")
{
  #ifdef NS_HAVE_STUDIO
    NsMeta<Noesis::StudioOrder>(1100, "Panel and Layout");
    NsMeta<Noesis::StudioDesc>("LayoutScaler\nScales the contents of this element according to the reference resolution");
    NsMeta<Noesis::StudioHelpUri>("https://www.noesisengine.com/docs/App.Toolkit._LayoutScaler.html");
    NsMeta<Noesis::StudioIcon>(Noesis::Uri::Pack("Toolkit", "#ToolkitIcons"), 0xE90A);

    NsProp("ReferenceResolution", &LayoutScaler::GetReferenceResolution,
        &LayoutScaler::SetReferenceResolution)
        .Meta<Noesis::StudioOrder>(0);
    NsProp("ScaleFactor", &LayoutScaler::GetScaleFactor,
        &LayoutScaler::SetScaleFactor)
        .Meta<Noesis::StudioOrder>(1)
        .Meta<Noesis::StudioRange>(0.1f, 5.0f);
  #endif

    NsMeta<Noesis::ContentPropertyMetaData>("Child");
    NsProp("Child", &NoesisApp::LayoutScaler::GetChild, &NoesisApp::LayoutScaler::SetChild);

    Noesis::UIElementData* data = NsMeta<Noesis::UIElementData>(Noesis::TypeOf<SelfClass>());

    data->RegisterProperty<Noesis::Size>(ReferenceResolutionProperty, "ReferenceResolution",
        Noesis::FrameworkPropertyMetadata::Create(Noesis::Size(1920.0f, 1080.0f),
            Noesis::FrameworkPropertyMetadataOptions_AffectsMeasure |
            Noesis::FrameworkPropertyMetadataOptions_AffectsArrange));
    data->RegisterProperty<float>(ScaleFactorProperty, "ScaleFactor",
        Noesis::FrameworkPropertyMetadata::Create(1.0f,
            Noesis::FrameworkPropertyMetadataOptions_AffectsMeasure |
            Noesis::FrameworkPropertyMetadataOptions_AffectsArrange));
}

NS_END_COLD_REGION

////////////////////////////////////////////////////////////////////////////////////////////////////
const Noesis::DependencyProperty* NoesisApp::LayoutScaler::ReferenceResolutionProperty;
const Noesis::DependencyProperty* NoesisApp::LayoutScaler::ScaleFactorProperty;
