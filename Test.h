#ifndef TEST_H
#define TEST_H

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/wrappers/GameEvent/TutorialWrapper.h"
#include "bakkesmod/wrappers/CanvasWrapper.h"
#include "bakkesmod/wrappers/wrapperstructs.h"

class Test : public BakkesMod::Plugin::BakkesModPlugin
{
public:
    Test();

    virtual void onLoad() override;
    virtual void onUnload() override;

private:
    void InitCVars();
    void InitHooks();
    void OnPreProcessInput(CarWrapper car, void* params, std::string eventName);
    void Render(CanvasWrapper canvas);
    Vector RotatorToVector(Rotator rotation);
    Vector2F WorldToScreen(Vector location, CanvasWrapper canvas);

    std::string plugin_version = "1.0";

    CVarWrapper enableCvar;
    CVarWrapper indicatorSizeCvar;
    CVarWrapper opacityCvar;

    LinearColor pitchUpColor;
    LinearColor pitchDownColor;
    LinearColor yawLeftColor;
    LinearColor yawRightColor;
    LinearColor rollLeftColor;
    LinearColor rollRightColor;
};

#endif // TEST_H
