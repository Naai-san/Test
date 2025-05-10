#include "pch.h"
#include "Test.h"
#include "GlobalCVarManager.h"


// Constructeur
Test::Test()
    : enableCvar(nullptr), // Initialize CVarWrapper members with nullptr
    indicatorSizeCvar(nullptr),
    opacityCvar(nullptr)
{
}



// Plugin registration
#define PLUGIN_VERSION "1.0"
BAKKESMOD_PLUGIN(Test, "Test Plugin", PLUGIN_VERSION, PLUGINTYPE_FREEPLAY)

void Test::onLoad()
{
    _globalCvarManager = cvarManager; // Initialisation de la variable globale
    InitCVars();
    InitHooks();
    cvarManager->log("Test plugin loaded successfully!");
}

void Test::onUnload()
{
    _globalCvarManager.reset(); // Nettoyage de la variable globale
    cvarManager->log("Test plugin unloaded.");
}

void Test::InitCVars()
{

    enableCvar = std::make_shared<CVarWrapper>(
        cvarManager->registerCvar("test_enabled", "1", "Enable/Disable plugin", true, true, 0, true, 1));
    indicatorSizeCvar = std::make_shared<CVarWrapper>(
        cvarManager->registerCvar("test_size", "30.0", "Size of visual indicators", true, true, 10.0, true, 100.0));
    opacityCvar = std::make_shared<CVarWrapper>(
        cvarManager->registerCvar("test_opacity", "0.7", "Opacity of indicators", true, true, 0.1, true, 1.0));


    pitchUpColor = { 0, 1, 0, 1 };
    pitchDownColor = { 1, 0, 0, 1 };
    yawLeftColor = { 0, 0, 1, 1 };
    yawRightColor = { 1, 1, 0, 1 };
    rollLeftColor = { 1, 0, 1, 1 };
    rollRightColor = { 0, 1, 1, 1 };
}

void Test::InitHooks()
{
    gameWrapper->HookEventWithCaller<CarWrapper>(
        "Function TAGame.Car_TA.SetVehicleInput",
        std::bind(&Test::OnPreProcessInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    gameWrapper->RegisterDrawable(std::bind(&Test::Render, this, std::placeholders::_1));
}

void Test::OnPreProcessInput(CarWrapper car, void* params, std::string eventName)
{
    if (!enableCvar->getBoolValue() || !gameWrapper->IsInGame()) return;
}

void Test::Render(CanvasWrapper canvas)
{
    if (!enableCvar->getBoolValue() || !gameWrapper->IsInGame()) return;

    auto tutorial = gameWrapper->GetGameEventAsTutorial();
    if (tutorial.IsNull()) return;

    CarWrapper car = tutorial.GetGameCar();
    if (car.IsNull()) return;

    float size = indicatorSizeCvar->getFloatValue();
    float opacity = opacityCvar->getFloatValue();

    auto adjustOpacity = [opacity](LinearColor color) -> LinearColor {
        color.A = opacity;
        return color;
        };

    // Récupération des vecteurs d'orientation de la voiture
    auto rotation = car.GetRotation();
    auto location = car.GetLocation();

    // Calcul des vecteurs de direction
    Vector forward = RotateVectorWithQuat(Vector(1, 0, 0), RotatorToQuat(rotation));
    Vector right = RotateVectorWithQuat(Vector(0, 1, 0), RotatorToQuat(rotation));
    Vector up = RotateVectorWithQuat(Vector(0, 0, 1), RotatorToQuat(rotation));

    // Longueur des axes
    float axisLength = size * 5.0f;

    // Dessiner les axes de pitch (avant/arrière)
    Vector pitchUpEnd = location + forward * axisLength;
    Vector pitchDownEnd = location - forward * axisLength;

    Vector2F screenPitchUp = canvas.ProjectF(pitchUpEnd);
    Vector2F screenPitchDown = canvas.ProjectF(pitchDownEnd);
    Vector2F screenLocation = canvas.ProjectF(location);

    // Dessiner les axes de yaw (gauche/droite)
    Vector yawLeftEnd = location + right * axisLength;
    Vector yawRightEnd = location - right * axisLength;

    Vector2F screenYawLeft = canvas.ProjectF(yawLeftEnd);
    Vector2F screenYawRight = canvas.ProjectF(yawRightEnd);

    // Dessiner les axes de roll (roulis)
    Vector rollLeftEnd = location + up * axisLength;
    Vector rollRightEnd = location - up * axisLength;

    Vector2F screenRollLeft = canvas.ProjectF(rollLeftEnd);
    Vector2F screenRollRight = canvas.ProjectF(rollRightEnd);

    // Vérification du canvas
    if (!enableCvar->getBoolValue() || !gameWrapper->IsInGame()) return;


    // Dessiner les lignes avec leurs couleurs respectives
    canvas.SetColor(adjustOpacity(pitchUpColor));
    canvas.DrawLine(screenLocation, screenPitchUp, 3.0f);

    canvas.SetColor(adjustOpacity(pitchDownColor));
    canvas.DrawLine(screenLocation, screenPitchDown, 3.0f);

    canvas.SetColor(adjustOpacity(yawLeftColor));
    canvas.DrawLine(screenLocation, screenYawLeft, 3.0f);

    canvas.SetColor(adjustOpacity(yawRightColor));
    canvas.DrawLine(screenLocation, screenYawRight, 3.0f);

    canvas.SetColor(adjustOpacity(rollLeftColor));
    canvas.DrawLine(screenLocation, screenRollLeft, 3.0f);

    canvas.SetColor(adjustOpacity(rollRightColor));
    canvas.DrawLine(screenLocation, screenRollRight, 3.0f);
}

Vector Test::RotatorToVector(Rotator rotation)
{
    // Convertir la rotation en radians
    float pitchRad = rotation.Pitch * CONST_UnrRotToRad;
    float yawRad = rotation.Yaw * CONST_UnrRotToRad;

    // Calculer les sinus et cosinus
    float cp = cosf(pitchRad);
    float sp = sinf(pitchRad);
    float cy = cosf(yawRad);
    float sy = sinf(yawRad);

    // Retourner le vecteur normalisé
    return Vector(cp * cy, cp * sy, sp);
}

Vector2F Test::WorldToScreen(Vector location, CanvasWrapper canvas)
{
    return canvas.ProjectF(location);
}
