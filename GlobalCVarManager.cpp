#include "pch.h" // Inclusion de l'en-t�te pr�compil�
#include "GlobalCVarManager.h"

// D�finition de la variable globale _globalCvarManager
std::shared_ptr<CVarManagerWrapper> _globalCvarManager = nullptr;
