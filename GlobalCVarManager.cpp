#include "pch.h" // Inclusion de l'en-tête précompilé
#include "GlobalCVarManager.h"

// Définition de la variable globale _globalCvarManager
std::shared_ptr<CVarManagerWrapper> _globalCvarManager = nullptr;
