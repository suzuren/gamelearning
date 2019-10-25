#pragma once

#include "../lib_spdlog/spdlogheader.h"

#ifdef _DEBUG
#pragma comment (lib,"../Debug/lib_spdlog.lib")
#else
#pragma comment (lib,"../Release/lib_spdlog.lib")
#endif