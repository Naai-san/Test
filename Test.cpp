#include "pch.h"
#include "Test.h"
#include "bakkesmod/wrappers/GameEvent/TutorialWrapper.h" // Ajout pour GetGameEventAsTutorial

// Constructeur
Test::Test() : enableCvar(0), indicatorSizeCvar(0), opacityCvar(0) {
    // Les membres CVarWrapper sont initialisés avec 0 (nullptr pour uintptr_t)
    // pour satisfaire le compilateur s'il ne trouve pas de constructeur par défaut.
    // Ils seront correctement enregistrés dans InitCVars.
    // LinearColor a un constructeur par défaut et sera initialisé.
}

BAKKESMOD_PLUGIN(Test, "Test Plugin", plugin_version, PLUGINTYPE_FREEPLAY)

void Test::onLoad()
{
    // Initialisation des CVars
    InitCVars();

    // Initialisation des hooks
    InitHooks();

    // Message dans la console de BakkesMod
    cvarManager->log("Test plugin loaded successfully!");
}

void Test::onUnload()
{
    cvarManager->log("Test plugin unloaded.");
    // Il est bon de unhook les events ici si nécessaire, bien que BakkesMod le fasse souvent automatiquement.
    // gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
    // gameWrapper->UnregisterDrawables();
}

void Test::InitCVars()
{
    enableCvar = cvarManager->registerCvar("test_enabled", "1", "Enable/Disable plugin", true, true, 0, true, 1);
    indicatorSizeCvar = cvarManager->registerCvar("test_size", "30.0", "Size of visual indicators", true, true, 10.0, true, 100.0);
    opacityCvar = cvarManager->registerCvar("test_opacity", "0.7", "Opacity of indicators", true, true, 0.1, true, 1.0);

    // Couleurs par défaut des indicateurs
    pitchUpColor = { 0, 1, 0, 1 };    // Vert
    pitchDownColor = { 1, 0, 0, 1 };  // Rouge
    yawLeftColor = { 0, 0, 1, 1 };    // Bleu
    yawRightColor = { 1, 1, 0, 1 };   // Jaune
    rollLeftColor = { 1, 0, 1, 1 };   // Magenta
    rollRightColor = { 0, 1, 1, 1 };  // Cyan
}

void Test::InitHooks()
{
    // Hook pour capturer les entrées utilisateur
    gameWrapper->HookEventWithCaller<CarWrapper>(
        "Function TAGame.Car_TA.SetVehicleInput",
        std::bind(&Test::OnPreProcessInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3) // Utilisation des placeholders
    );

    // Hook pour effectuer le rendu
    gameWrapper->RegisterDrawable(std::bind(&Test::Render, this, std::placeholders::_1));
}

// Signature et arguments mis à jour
void Test::OnPreProcessInput(CarWrapper car, void* params, std::string eventName)
{
    if (!enableCvar.getBoolValue() || !gameWrapper->IsInGame()) return;

    // car: instance de CarWrapper pour laquelle SetVehicleInput a été appelée.
    // params: pointeur vers la structure FControllerInput (nécessite un cast si utilisé).
    // eventName: la chaîne "Function TAGame.Car_TA.SetVehicleInput".
    // Ajoutez ici la logique pour traiter les entrées utilisateur si nécessaire.
}

void Test::Render(CanvasWrapper canvas)
{
    if (!enableCvar.getBoolValue() || !gameWrapper->IsInGame()) return;

    TutorialWrapper tutorialGame = gameWrapper->GetGameEventAsTutorial(); // Type explicite
    if (tutorialGame.IsNull()) return;

    CarWrapper car = tutorialGame.GetGameCar(); // Type explicite
    if (car.IsNull()) return;

    float size = indicatorSizeCvar.getFloatValue();
    float opacity = opacityCvar.getFloatValue();

    // Ajuste l'opacité des couleurs
    auto adjustOpacity = [opacity](LinearColor color) -> LinearColor {
        color.A = opacity;
        return color;
        };

    auto rotation = car.GetRotation();
    auto forward = RotatorToVector(rotation);
    auto right = Vector(forward.Y, -forward.X, 0).GetNormalized(); // Attention: ceci est un "right" 2D dans le plan XY
    auto up = Vector::Cross(forward, right).GetNormalized(); // Utilisation de Vector::Cross

    // canvas.GetSize() retourne Vector2, dont les composantes X et Y sont des entiers.
    Vector2 center = { canvas.GetSize().X / 2, canvas.GetSize().Y / 2 };

    // Exemple d'indicateur pour le pitch
    canvas.SetColor(adjustOpacity(pitchUpColor));
    canvas.DrawLine(Vector2{ center.X, center.Y }, // Initialisateur C++11 pour Vector2
        Vector2{ center.X + 100, center.Y }, 3.0f);
}

Vector Test::RotatorToVector(Rotator rotation)
{
    // Conversion des angles de Unreal Rotator (unités entières) en radians
    float pitchRad = rotation.Pitch * (3.14159265359f / 32768.0f);
    float yawRad = rotation.Yaw * (3.14159265359f / 32768.0f);
    // Roll n'est pas utilisé pour un vecteur forward simple

    float cp = cos(pitchRad);
    float sp = sin(pitchRad);
    float cy = cos(yawRad);
    float sy = sin(yawRad);

    return Vector(cp * cy, cp * sy, sp);
}

Vector2F Test::WorldToScreen(Vector location, CanvasWrapper canvas)
{
    // Correction: Utilisation de !canvas.IsValid()
    if (!gameWrapper || !canvas.IsValid())
    {
        return { 0.0f, 0.0f };
    }

    // Correction: canvas.Project retourne Vector2 (int X, int Y)
    Vector2 projectedInt = canvas.Project(location);

    // Conversion explicite de Vector2 (int) vers Vector2F (float)
    return { static_cast<float>(projectedInt.X), static_cast<float>(projectedInt.Y) };
}
