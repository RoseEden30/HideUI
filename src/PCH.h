#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

#include <Windows.h>

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <format>
#include <string>

// Needed by the plugin declaration CMake generates, which uses "..."sv.
using namespace std::literals;
