////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_LAYOUTSCALER_H__
#define __APP_LAYOUTSCALER_H__


#include <NsCore/Noesis.h>
// #include <NsApp/ToolkitApi.h>
#include <NsGui/FrameworkElement.h>


namespace Noesis
{
struct Size;
}

namespace NoesisApp
{

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Calculates a scale based on the selected mode and applies a LayoutTransform to this element
////////////////////////////////////////////////////////////////////////////////////////////////////
class LayoutScaler final: public Noesis::FrameworkElement
{
public:
    LayoutScaler();

    /// Gets or sets the resolution the UI layout is designed for. If the screen resolution is
    /// larger, the UI will be scaled up, and if it's smaller, the UI will be scaled down.
    //@{
    const Noesis::Size& GetReferenceResolution() const;
    void SetReferenceResolution(const Noesis::Size& v);
    //@}

    /// Gets or sets an extra factor used to scale all UI elements in the screen
    //@{
    float GetScaleFactor() const;
    void SetScaleFactor(float v);
    //@}

    /// Gets or sets the child element
    //@{
    UIElement* GetChild() const;
    void SetChild(UIElement* child);
    //@}

public:
    /// Dependency properties
    //@{
    static const Noesis::DependencyProperty* ReferenceResolutionProperty;
    static const Noesis::DependencyProperty* ScaleFactorProperty;
    //@}

private:
    /// From Visual
    //@{
    uint32_t GetVisualChildrenCount() const override;
    Visual* GetVisualChild(uint32_t index) const override;
    //@}

    /// From FrameworkElement
    //@{
    void CloneOverride(FrameworkElement* clone, Noesis::FrameworkTemplate* template_) const override;
    uint32_t GetLogicalChildrenCount() const override;
    Noesis::Ptr<BaseComponent> GetLogicalChild(uint32_t index) const override;
    void OnTemplatedParentChanged(FrameworkElement* old_, FrameworkElement* new_) override;
    Noesis::Size MeasureOverride(const Noesis::Size& availableSize) override;
    Noesis::Size ArrangeOverride(const Noesis::Size& finalSize) override;
    //@}

    NS_DECLARE_REFLECTION(LayoutScaler, FrameworkElement)
};

}

#endif
