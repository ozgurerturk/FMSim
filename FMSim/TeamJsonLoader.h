#pragma once

#include "Team.h"
#include <filesystem>

Team loadTeamFromJsonFile(const std::filesystem::path& filePath);
