#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <sstream>
#include <iostream>
#include <utility>

#include "Quelos/Core/Core.h"
#include "Quelos/Core/AssetsManager.h"
#include "Quelos/Core/Log.h"

#include "Quelos/Core/KeyCodes.h"
#include "Quelos/Core/MouseButtonCodes.h"

#include "Quelos/Debug/Instrumentor.h"
#include "Quelos/Core/Time.h"
#include "Quelos/Core/TimeStep.h"

#include "Quelos\Math\Math.h"

#ifdef QS_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
