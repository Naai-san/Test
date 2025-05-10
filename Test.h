#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/wrappers/GameObject/CarWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"

constexpr auto plugin_version = "1.0";

class Test : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    // M�thodes principales du plugin
    virtual void onLoad() override;
    virtual void onUnload() override;

private:
    // Variables de configuration (pas initialis�es ici, configur�es dans InitCVars)
    CVarWrapper enableCvar;
    CVarWrapper indicatorSizeCvar;
    CVarWrapper opacityCvar;

    // Couleurs des indicateurs
    LinearColor pitchUpColor;
    LinearColor pitchDownColor;
    LinearColor yawLeftColor;
    LinearColor yawRightColor;
    LinearColor rollLeftColor;
    LinearColor rollRightColor;

    // M�thodes internes
    void Render(CanvasWrapper canvas);
    void OnPreProcessInput();
    void InitCVars();
    void InitHooks();

    // Utilitaires
    Vector RotatorToVector(Rotator rotation);
    Vector2F WorldToScreen(Vector location, CanvasWrapper canvas);
};