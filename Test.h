#pragma once
#pragma comment(lib, "PluginSDK.lib")

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/wrappers/GameObject/CarWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"
#include "bakkesmod/wrappers/cvarmanagerwrapper.h"
#include "bakkesmod/wrappers/WrapperStructs.h"

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
    std::shared_ptr<CVarWrapper> enableCvar;
    std::shared_ptr<CVarWrapper> indicatorSizeCvar;
    std::shared_ptr<CVarWrapper> opacityCvar;

    // Couleurs des indicateurs
    LinearColor pitchUpColor;
    LinearColor pitchDownColor;
    LinearColor yawLeftColor;
    LinearColor yawRightColor;
    LinearColor rollLeftColor;
    LinearColor rollRightColor;

    // Méthodes internes
    void Render(CanvasWrapper canvas);
    void OnPreProcessInput(CarWrapper car, void* params, std::string eventName);
    void InitCVars();
    void InitHooks();

    // Utilitaires
    Vector RotatorToVector(Rotator rotation);
    Vector2F WorldToScreen(Vector location, CanvasWrapper canvas);
};
