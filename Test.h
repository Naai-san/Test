#pragma once
//2316
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/wrappers/GameObject/CarWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h" // Ajout pour CVarWrapper
#include "bakkesmod/wrappers/WrapperStructs.h"    // Ajout pour Vector, LinearColor, etc.

constexpr auto plugin_version = "1.0";

class Test : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    Test(); // Constructeur

    // Méthodes principales du plugin
    virtual void onLoad() override;
    virtual void onUnload() override;

private:
    // Variables de configuration
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

    // Méthodes internes
    void Render(CanvasWrapper canvas);
    void OnPreProcessInput(CarWrapper car, void* params, std::string eventName); // Signature mise à jour
    void InitCVars();
    void InitHooks();

    // Utilitaires
    Vector RotatorToVector(Rotator rotation);
    Vector2F WorldToScreen(Vector location, CanvasWrapper canvas);
};