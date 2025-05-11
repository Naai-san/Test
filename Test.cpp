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

/*void Test::InitHooks()
{
    cvarManager->log("Initializing hooks...");

    // Hook pour intercepter les entrées du véhicule
    try {
        gameWrapper->HookEventWithCaller<CarWrapper>(
            "Function TAGame.Car_TA.SetVehicleInput",
            std::bind(&Test::OnPreProcessInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
        );
        cvarManager->log("HookEventWithCaller for SetVehicleInput registered successfully.");
    }
    catch (...) {
        cvarManager->log("Error: Failed to register HookEventWithCaller for SetVehicleInput.");
    }

    // Enregistrement de la fonction de rendu
    try {
        gameWrapper->RegisterDrawable(std::bind(&Test::Render, this, std::placeholders::_1));
        cvarManager->log("RegisterDrawable for Render function registered successfully.");
    }
    catch (...) {
        cvarManager->log("Error: Failed to register RegisterDrawable for Render function.");
    }

    cvarManager->log("Hooks initialized.");
}*/
void Test::InitHooks()
{
    // Hook pour intercepter les entrées du véhicule
    gameWrapper->HookEventWithCaller<CarWrapper>(
        "Function TAGame.Car_TA.SetVehicleInput",
        std::bind(&Test::OnPreProcessInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    // Enregistrement de la fonction de rendu
    gameWrapper->RegisterDrawable(std::bind(&Test::Render, this, std::placeholders::_1));
}

void Test::OnPreProcessInput(CarWrapper car, void* params, std::string eventName)
{
    if (!enableCvar->getBoolValue() || !gameWrapper->IsInGame()) return;
}

/*void Test::Render(CanvasWrapper canvas)
{
    cvarManager->log("Render method called.");
    if (!gameWrapper->IsInFreeplay() && !gameWrapper->IsInCustomTraining()) {
        cvarManager->log("Render skipped: Not in Freeplay or Custom Training.");
        return;
    }

    // Vérification si le plugin est activé
    if (!enableCvar->getBoolValue()) {
        cvarManager->log("Render skipped: Plugin is disabled (test_enabled = 0).");
        return;
    }

    // Vérification si le jeu est en cours
    if (!gameWrapper->IsInGame()) {
        cvarManager->log("Render skipped: Not in a game.");
        return;
    }

    // Test de dessin simple
    try {
        canvas.SetColor(LinearColor(1, 0, 0, 1)); // Rouge opaque
        canvas.DrawLine(Vector2F(100, 100), Vector2F(200, 200), 5.0f); // Ligne diagonale
        cvarManager->log("Debug line drawn successfully.");
        
            cvarManager->log("Render method called (simplified test).");

            canvas.SetColor(LinearColor(0, 1, 0, 1)); // Vert opaque
            canvas.FillBox(Vector2F(100, 100)); // Dessine un carré de 100x100 pixels
            cvarManager->log("Test rectangle drawn successfully.");
        

    }
    catch (...) {
        cvarManager->log("Error: Failed to draw debug line.");
        return;
    }

    // Obtenir la voiture locale
    CarWrapper car = gameWrapper->GetLocalCar();
    if (car.IsNull()) {
        cvarManager->log("Render skipped: Local car is null.");
        return;
    }

    // Récupération des paramètres de configuration
    float size = indicatorSizeCvar->getFloatValue();
    float opacity = opacityCvar->getFloatValue();
    cvarManager->log("Indicator size: " + std::to_string(size) + ", Opacity: " + std::to_string(opacity));

    auto adjustOpacity = [opacity](LinearColor color) -> LinearColor {
        color.A = opacity;
        return color;
        };

    // Récupération des vecteurs d'orientation de la voiture
    auto rotation = car.GetRotation();
    auto location = car.GetLocation();
    cvarManager->log("Car location: X=" + std::to_string(location.X) + ", Y=" + std::to_string(location.Y) + ", Z=" + std::to_string(location.Z));

    // Calcul des vecteurs de direction
    Vector forward = RotateVectorWithQuat(Vector(1, 0, 0), RotatorToQuat(rotation));
    Vector right = RotateVectorWithQuat(Vector(0, 1, 0), RotatorToQuat(rotation));
    Vector up = RotateVectorWithQuat(Vector(0, 0, 1), RotatorToQuat(rotation));

    // Longueur des axes
    float axisLength = size * 5.0f;

    // Calcul des positions projetées
    Vector pitchUpEnd = location + forward * axisLength;
    Vector pitchDownEnd = location - forward * axisLength;
    Vector2F screenPitchUp = canvas.ProjectF(pitchUpEnd);
    Vector2F screenPitchDown = canvas.ProjectF(pitchDownEnd);
    Vector2F screenLocation = canvas.ProjectF(location);

    cvarManager->log("Screen location: X=" + std::to_string(screenLocation.X) + ", Y=" + std::to_string(screenLocation.Y));

    // Dessiner les axes
    try {
        // Axe de pitch (avant/arrière)
        canvas.SetColor(adjustOpacity(pitchUpColor));
        canvas.DrawLine(screenLocation, screenPitchUp, 3.0f);
        cvarManager->log("Pitch up line drawn successfully.");
        if (screenLocation.X < 0 || screenLocation.X > 1920 || screenLocation.Y < 0 || screenLocation.Y > 1080) {
            cvarManager->log("Screen location is out of bounds: X=" + std::to_string(screenLocation.X) + ", Y=" + std::to_string(screenLocation.Y));
            return;
        }

        canvas.SetColor(adjustOpacity(pitchDownColor));
        canvas.DrawLine(screenLocation, screenPitchDown, 3.0f);
        cvarManager->log("Pitch down line drawn successfully.");
        if (screenLocation.X < 0 || screenLocation.X > 1920 || screenLocation.Y < 0 || screenLocation.Y > 1080) {
            cvarManager->log("Screen location is out of bounds: X=" + std::to_string(screenLocation.X) + ", Y=" + std::to_string(screenLocation.Y));
            return;
        }

        // Axe de yaw (gauche/droite)
        Vector yawLeftEnd = location + right * axisLength;
        Vector yawRightEnd = location - right * axisLength;
        Vector2F screenYawLeft = canvas.ProjectF(yawLeftEnd);
        Vector2F screenYawRight = canvas.ProjectF(yawRightEnd);

        canvas.SetColor(adjustOpacity(yawLeftColor));
        canvas.DrawLine(screenLocation, screenYawLeft, 3.0f);
        cvarManager->log("Yaw left line drawn successfully.");
        if (screenLocation.X < 0 || screenLocation.X > 1920 || screenLocation.Y < 0 || screenLocation.Y > 1080) {
            cvarManager->log("Screen location is out of bounds: X=" + std::to_string(screenLocation.X) + ", Y=" + std::to_string(screenLocation.Y));
            return;
        }

        canvas.SetColor(adjustOpacity(yawRightColor));
        canvas.DrawLine(screenLocation, screenYawRight, 3.0f);
        cvarManager->log("Yaw right line drawn successfully.");
        if (screenLocation.X < 0 || screenLocation.X > 1920 || screenLocation.Y < 0 || screenLocation.Y > 1080) {
            cvarManager->log("Screen location is out of bounds: X=" + std::to_string(screenLocation.X) + ", Y=" + std::to_string(screenLocation.Y));
            return;
        }

        // Axe de roll (roulis)
        Vector rollLeftEnd = location + up * axisLength;
        Vector rollRightEnd = location - up * axisLength;
        Vector2F screenRollLeft = canvas.ProjectF(rollLeftEnd);
        Vector2F screenRollRight = canvas.ProjectF(rollRightEnd);

        canvas.SetColor(adjustOpacity(rollLeftColor));
        canvas.DrawLine(screenLocation, screenRollLeft, 3.0f);
        cvarManager->log("Roll left line drawn successfully.");
        if (screenLocation.X < 0 || screenLocation.X > 1920 || screenLocation.Y < 0 || screenLocation.Y > 1080) {
            cvarManager->log("Screen location is out of bounds: X=" + std::to_string(screenLocation.X) + ", Y=" + std::to_string(screenLocation.Y));
            return;
        }

        canvas.SetColor(adjustOpacity(rollRightColor));
        canvas.DrawLine(screenLocation, screenRollRight, 3.0f);
        cvarManager->log("Roll right line drawn successfully.");
        if (screenLocation.X < 0 || screenLocation.X > 1920 || screenLocation.Y < 0 || screenLocation.Y > 1080) {
            cvarManager->log("Screen location is out of bounds: X=" + std::to_string(screenLocation.X) + ", Y=" + std::to_string(screenLocation.Y));
            return;
        }
    }
    catch (...) {
        cvarManager->log("Error: Failed to draw one or more lines.");
    }
}*/

void Test::Render(CanvasWrapper canvas)
{
    // Vérification initiale : Assurez-vous que le contexte de rendu est valide
    if (&canvas == nullptr) {
        cvarManager->log("CanvasWrapper is null.");
        return;
    }
    Vector2 screenSize = canvas.GetSize();
    if (screenSize.X <= 0 || screenSize.Y <= 0) {
        cvarManager->log("Invalid CanvasWrapper: Screen size is zero or negative.");
        return;
    }
    // 4. Ajoutez un test de dessin minimaliste
    try {
        canvas.SetColor(LinearColor(1.0f, 0.0f, 0.0f, 1.0f)); // Rouge opaque
        canvas.DrawLine(Vector2F(0, 0), Vector2F(100, 100), 5.0f);
    }
    catch (...) {
        cvarManager->log("CanvasWrapper is not valid: Failed to draw.");
        return;
    }

    // Vérification des conditions de rendu
    if (!gameWrapper->IsInFreeplay() && !gameWrapper->IsInCustomTraining()) return;
    if (!enableCvar->getBoolValue()) return; // Vérifiez que le plugin est activé
    if (!gameWrapper->IsInGame()) return;

    // Dessin d'un rectangle rouge semi-transparent couvrant tout l'écran
    canvas.SetColor(LinearColor(1.0f, 0.0f, 0.0f, 0.5f)); // Rouge semi-transparent
    canvas.SetPosition(Vector2F(0, 0)); // Position en haut à gauche
    canvas.FillBox(Vector2F(1920, 1080)); // Taille du rectangle couvrant tout l'écran

    // Dessin d'un texte centré
    std::string testText = "TEST AFFICHAGE TEXTE";
    float textScale = 3.0f; // Taille du texte
    Vector2F textPos = { 960.0f - (testText.length() * 7.0f * textScale / 2.0f), 540.0f }; // Centré
    canvas.SetColor(LinearColor(1.0f, 1.0f, 1.0f, 1.0f)); // Blanc opaque
    canvas.SetPosition(textPos);
    canvas.DrawString(testText, textScale, textScale);

    // Dessin d'une ligne diagonale verte
    try
	{
		canvas.SetColor(LinearColor(0.0f, 1.0f, 0.0f, 1.0f)); // Vert opaque
		canvas.DrawLine(Vector2F(100.0f, 100.0f), Vector2F(300.0f, 300.0f), 5.0f);
	}
	catch (...)
	{
		cvarManager->log("CanvasWrapper is not valid: Failed to draw line.");
		return;
	}

    // Log périodique pour éviter de surcharger les logs
    static int renderCallCount = 0;
    renderCallCount++;
    if (renderCallCount % 500 == 0) { // Log toutes les 500 itérations
        cvarManager->log("Render function is being called periodically.");
    }
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
