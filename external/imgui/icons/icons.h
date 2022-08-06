#pragma once
#include "folder.h"
#include "IconsFontaudio.h"
#include "IconsFontAwesome6.h"
#include "IconsFontAwesome6Brands.h"
#include "IconsForkAwesome.h"
#include "IconsMaterialDesign.h"
#include "IconsKenney.h"

#define ICON_MD 0
#define ICON_FA 1
namespace ImGui
{
    IMGUI_API bool LoadInternalIcons(ImFontAtlas *atlas);
}