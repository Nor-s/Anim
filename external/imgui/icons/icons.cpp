#include "imgui.h"
#include "imgui_internal.h"
#include "icons.h"

extern const char StepMath_compressed_data_base85[];
#define ICON_MIN_MATH 0x2000
#define ICON_MAX_MATH 0x2A80

bool ImGui::LoadInternalIcons(ImFontAtlas *atlas)
{
    float icons_size = 13.0f;
    ImFontConfig icons_config;
    icons_config.OversampleH = 2;
    icons_config.OversampleV = 2;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.EllipsisChar = (ImWchar)0x0085;

    icons_config.SizePixels = icons_size * 1.0f;
    icons_config.GlyphOffset.y = 1.0f * IM_FLOOR(icons_config.SizePixels / icons_size); // Add +1 offset per 16 units

    static const ImWchar ki_icons_ranges[] = {ICON_MIN_KI, ICON_MAX_KI, 0};
    atlas->AddFontFromMemoryCompressedBase85TTF(kenney_compressed_data_base85, icons_size, &icons_config, ki_icons_ranges);

    icons_size = 16.0f;
    icons_config.SizePixels = icons_size * 1.0f;
    icons_config.GlyphOffset.y = 4.0f * IM_FLOOR(icons_config.SizePixels / icons_size); // Add +1 offset per 16 units

    // Audio Icons
    static const ImWchar fad_icons_ranges[] = {ICON_MIN_FAD, ICON_MAX_FAD, 0};
    atlas->AddFontFromMemoryCompressedBase85TTF(fontaudio_compressed_data_base85, icons_size, &icons_config, fad_icons_ranges);

    static const ImWchar md_icons_ranges[] = {ICON_MIN_MD, ICON_MAX_16_MD, 0};
    atlas->AddFontFromMemoryCompressedBase85TTF(MaterialIcons_compressed_data_base85, icons_size, &icons_config, md_icons_ranges);
    // Fork Material Design Icons
    // FileDialog Icons
    static const ImWchar icons_ranges[] = {ICON_MIN_IGFD, ICON_MAX_IGFD, 0};
    atlas->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_IGFD, icons_size, &icons_config, icons_ranges);
    // Awesome brands Icons
    static const ImWchar fab_icons_ranges[] = {ICON_MIN_FAB, ICON_MAX_FAB, 0};
    atlas->AddFontFromMemoryCompressedBase85TTF(fa_brands_compressed_data_base85, icons_size, &icons_config, fab_icons_ranges);
    icons_config.GlyphOffset.y = 1.0f * IM_FLOOR(icons_config.SizePixels / icons_size); // Add +1 offset per 16 units

    // Fork Awesome Icons
    static const ImWchar fk_icons_ranges[] = {ICON_MIN_FK, ICON_MAX_FK, 0};
    atlas->AddFontFromMemoryCompressedBase85TTF(fork_webfont_compressed_data_base85, icons_size, &icons_config, fk_icons_ranges);

    // StepMath
    static const ImWchar math_icons_ranges[] = {ICON_MIN_MATH, ICON_MAX_MATH, 0};
    atlas->AddFontFromMemoryCompressedBase85TTF(StepMath_compressed_data_base85, icons_size, &icons_config, math_icons_ranges);

    // Awesome Icons
    static const ImWchar fa_icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
    atlas->AddFontFromFileTTF("./resources/font/D2Coding.ttf", 16.0f, NULL, atlas->GetGlyphRangesKorean());
    atlas->AddFontFromMemoryCompressedBase85TTF(fa_regular_compressed_data_base85, icons_size, &icons_config, fa_icons_ranges);
    atlas->AddFontFromMemoryCompressedBase85TTF(fa_solid_compressed_data_base85, icons_size, &icons_config, fa_icons_ranges);

    return true;
}