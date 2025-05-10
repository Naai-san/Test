#include "pch.h"
#include "Test.h"

// Constructeur par défaut
Test::Test() {
    // Ce constructeur peut être vide.
    // Les membres CVarWrapper et LinearColor seront initialisés par défaut,
    // puis configurés dans InitCVars lors de l'appel à onLoad.
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
        std::bind(&Test::OnPreProcessInput, this)
    );

    // Hook pour effectuer le rendu
    gameWrapper->RegisterDrawable(std::bind(&Test::Render, this, std::placeholders::_1));
}

void Test::OnPreProcessInput()
{
    if (!enableCvar.getBoolValue() || !gameWrapper->IsInGame()) return;

    // Ajoutez ici la logique pour traiter les entrées utilisateur si nécessaire
}

void Test::Render(CanvasWrapper canvas)
{
    if (!enableCvar.getBoolValue() || !gameWrapper->IsInGame()) return;

    auto tutorialGame = gameWrapper->GetGameEventAsTutorial();
    if (tutorialGame.IsNull()) return;

    auto car = tutorialGame.GetGameCar();
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
    auto right = Vector(forward.Y, -forward.X, 0).getNormalized();
    auto up = Vector::cross(forward, right).getNormalized();

    // canvas.GetSize() retourne Vector2, dont les composantes X et Y sont des entiers.
    Vector2 center = { canvas.GetSize().X / 2, canvas.GetSize().Y / 2 };

    // Exemple d'indicateur pour le pitch
    canvas.SetColor(adjustOpacity(pitchUpColor));
    // Les casts static_cast<int> ne sont pas nécessaires ici car center.X et center.Y sont déjà des int.
    canvas.DrawLine(Vector2(center.X, center.Y),
        Vector2(center.X + 100, center.Y), 3.0f);
}

Vector Test::RotatorToVector(Rotator rotation)
{
    float pitch = rotation.Pitch * 3.1415926f / 32768.0f;
    float yaw = rotation.Yaw * 3.1415926f / 32768.0f;

    float CP = cos(pitch);
    float SP = sin(pitch);
    float CY = cos(yaw);
    float SY = sin(yaw);

    return Vector(CP * CY, CP * SY, SP);
}

Vector2F Test::WorldToScreen(Vector location, CanvasWrapper canvas)
{
    if (!gameWrapper || canvas.IsNull()) // Ajout d'une vérification pour canvas.IsNull() par sécurité
    {
        return { 0.0f, 0.0f }; // Utilisation de littéraux flottants pour Vector2F
    }

    Vector projected = canvas.Project(location); // Renvoie Vector2 (int X, int Y)
    // Conversion explicite de Vector2 vers Vector2F
    return { static_cast<float>(projected.X), static_cast<float>(projected.Y) };
}