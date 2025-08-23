// FranticDreamer 2022-2025
//
// imgui_style_candy.h
// Header-only ImGui style that loosely inspired by the RayGui "Candy" theme:
// https://github.com/raysan5/raygui/tree/6530ee136b3c5af86c5640151f07837a604308ec/styles/candy
// Generated using ImThemes:
// https://github.com/Patitotective/ImThemes
//
// Font: BoldPixels1.4.ttf by Yuki Pixels
// Embedded using a generated header file.
// Command: `xxd -i BoldPixels1.4.ttf > BoldPixels1_4.hpp`
//
// Quick usage:
//  1. Include this header after ImGui is included in your project.
//  2. Call `ImGui::Styles::Candy::ApplyStyle(io)` after `ImGui::CreateContext()` to apply the style.
// 

#pragma once
#ifndef IMGUI_API
	#include "imgui.h"
#endif
#include "fonts/BoldPixels1_4.hpp"

namespace ImGui
{
	namespace Styles
	{
		namespace Candy
		{
			constexpr int CANDY_TEXT_SIZE_PX = 15;	// DEFAULT_TEXT_SIZE 
			constexpr int CANDY_TEXT_SPACING = 0;	// DEFAULT_TEXT_SPACING    

			inline void ApplyStyle(ImGuiIO& io, float size_px = (float)CANDY_TEXT_SIZE_PX, const ImWchar* ranges = nullptr)
			{
                ImFontConfig cfg;
                cfg.FontDataOwnedByAtlas = false; // Don't free the font data, we own it
                io.Fonts->AddFontFromMemoryTTF((void*)BoldPixels1_4_ttf, (int)BoldPixels1_4_ttf_len, size_px, &cfg, ranges ? ranges : io.Fonts->GetGlyphRangesDefault());

                // Candy style from ImThemes
                ImGuiStyle& style = ImGui::GetStyle();

                style.Alpha = 1.0f;
                style.DisabledAlpha = 0.6000000238418579f;
                style.WindowPadding = ImVec2(6.0f, 3.0f);
                style.WindowRounding = 0.0f;
                style.WindowBorderSize = 1.0f;
                style.WindowMinSize = ImVec2(32.0f, 32.0f);
                style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
                style.WindowMenuButtonPosition = ImGuiDir_Left;
                style.ChildRounding = 0.0f;
                style.ChildBorderSize = 1.0f;
                style.PopupRounding = 0.0f;
                style.PopupBorderSize = 1.0f;
                style.FramePadding = ImVec2(5.0f, 1.0f);
                style.FrameRounding = 0.0f;
                style.FrameBorderSize = 1.0f;
                style.ItemSpacing = ImVec2(8.0f, 4.0f);
                style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
                style.CellPadding = ImVec2(4.0f, 2.0f);
                style.IndentSpacing = 21.0f;
                style.ColumnsMinSpacing = 6.0f;
                style.ScrollbarSize = 13.0f;
                style.ScrollbarRounding = 0.0f;
                style.GrabMinSize = 20.0f;
                style.GrabRounding = 0.0f;
                style.TabRounding = 0.0f;
                style.TabBorderSize = 1.0f;
                style.ColorButtonPosition = ImGuiDir_Right;
                style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
                style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

                style.Colors[ImGuiCol_Text] = ImVec4(0.9882352948188782f, 0.4117647111415863f, 0.3333333432674408f, 1.0f);
                style.Colors[ImGuiCol_TextDisabled] = ImVec4(9.999899930335232e-07f, 9.999999974752427e-07f, 9.999938583860057e-07f, 1.0f);
                style.Colors[ImGuiCol_WindowBg] = ImVec4(0.9882352948188782f, 0.8470588326454163f, 0.3568627536296844f, 1.0f);
                style.Colors[ImGuiCol_ChildBg] = ImVec4(9.999999974752427e-07f, 9.999899930335232e-07f, 9.999899930335232e-07f, 0.0f);
                style.Colors[ImGuiCol_PopupBg] = ImVec4(0.9882352948188782f, 0.8470588326454163f, 0.3568627536296844f, 1.0f);
                style.Colors[ImGuiCol_Border] = ImVec4(0.9333333373069763f, 0.5058823823928833f, 0.2470588237047195f, 1.0f);
                style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
                style.Colors[ImGuiCol_FrameBg] = ImVec4(0.9882352948188782f, 0.8470588326454163f, 0.3568627536296844f, 1.0f);
                style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3529411852359772f, 0.1702422052621841f, 0.1702422052621841f, 1.0f);
                style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4392156898975372f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
                style.Colors[ImGuiCol_TitleBg] = ImVec4(0.125490203499794f, 0.1568627506494522f, 0.1490196138620377f, 1.0f);
                style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2478431463241577f, 0.3098039329051971f, 0.2944959700107574f, 1.0f);
                style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 0.75f);
                style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.953361988067627f, 0.3607842922210693f, 1.0f);
                style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.9882352948188782f, 0.8470588326454163f, 0.3568627536296844f, 1.0f);
                style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.9882352948188782f, 0.8470588326454163f, 0.3568627536296844f, 1.0f);
                style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 0.7799999713897705f);
                style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
                style.Colors[ImGuiCol_CheckMark] = ImVec4(0.9882352948188782f, 0.4117647111415863f, 0.3333333432674408f, 1.0f);
                style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.5960784554481506f, 0.2475066184997559f, 0.2010303735733032f, 1.0f);
                style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.7297819852828979f, 0.3372548818588257f, 1.0f);
                style.Colors[ImGuiCol_Button] = ImVec4(0.9882352948188782f, 0.8470588326454163f, 0.3568627536296844f, 1.0f);
                style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.5882353186607361f, 0.2860438227653503f, 0.2860438227653503f, 1.0f);
                style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4392156898975372f, 0.2101345509290695f, 0.2101345509290695f, 1.0f);
                style.Colors[ImGuiCol_Header] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 0.7599999904632568f);
                style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 0.8600000143051147f);
                style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.501960813999176f, 0.07450980693101883f, 0.2549019753932953f, 1.0f);
                style.Colors[ImGuiCol_Separator] = ImVec4(0.9333333373069763f, 0.5058823823928833f, 0.2470588237047195f, 1.0f);
                style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.501960813999176f, 0.07450980693101883f, 0.2549019753932953f, 0.6566523313522339f);
                style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.501960813999176f, 0.07450980693101883f, 0.2549019753932953f, 1.0f);
                style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.501960813999176f, 0.07450980693101883f, 0.2549019753932953f, 1.0f);
                style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 0.7799999713897705f);
                style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
                style.Colors[ImGuiCol_Tab] = ImVec4(0.9882352948188782f, 0.8470588326454163f, 0.3568627536296844f, 1.0f);
                style.Colors[ImGuiCol_TabHovered] = ImVec4(0.5882353186607361f, 0.2862745225429535f, 0.2862745225429535f, 1.0f);
                style.Colors[ImGuiCol_TabActive] = ImVec4(0.4392156898975372f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
                style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.9333333373069763f, 0.5058823823928833f, 0.2470588237047195f, 1.0f);
                style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.9333333373069763f, 0.5058823823928833f, 0.2470588237047195f, 1.0f);
                style.Colors[ImGuiCol_PlotLines] = ImVec4(0.4392156898975372f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
                style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9333333373069763f, 0.5058823823928833f, 0.2470588237047195f, 1.0f);
                style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.4392156898975372f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
                style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.9333333373069763f, 0.5058823823928833f, 0.2470588237047195f, 1.0f);
                style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.4392156898975372f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
                style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.9333333373069763f, 0.5058823823928833f, 0.2470588237047195f, 1.0f);
                style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.9333333373069763f, 0.5058823823928833f, 0.2470588237047195f, 1.0f);
                style.Colors[ImGuiCol_TableRowBg] = ImVec4(1.0f, 0.9140945672988892f, 0.615686297416687f, 1.0f);
                style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.1459227204322815f);
                style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.501960813999176f, 0.07450980693101883f, 0.2549019753932953f, 1.0f);
                style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.9882352948188782f, 0.8470588326454163f, 0.3568627536296844f, 1.0f);
                style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.4392156898975372f, 0.2117647081613541f, 0.2117647081613541f, 1.0f);
                style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
                style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3347639441490173f);
                style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
			}
		}
	}
}
