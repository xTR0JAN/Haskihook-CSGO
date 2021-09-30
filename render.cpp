#include "includes.h"
#include <cmath>
#include <math.h>       /* round, floor, ceil, trunc */

namespace render {
	Font menu;;
	Font SkeetIcon;;
	Font menu_shade;;
	Font esp;;
	Font esp_grenade;;
	Font esp_small;;
	Font esp_info;;
	Font hud;;
	Font cs;;
	Font cs_c4;;
	Font indicator;;
	Font logevent;;
	Font watermark;;
	Font spectators;;
	Font esp_name;;
}

vgui::HFont F::ESPInfo;
vgui::HFont F::Icons;
vgui::HFont F::Skeet;

const char* render::LastFontName;

void render::init() {
	LastFontName = "";

	//menu = Font(XOR("AstriumWep"), 12, FW_NORMAL, FONTFLAG_NONE);
	SkeetIcon = Font(XOR("menu_font"), 65, FW_NORMAL, FONTFLAG_ANTIALIAS);
	menu = Font(XOR("Tahoma"), 12, FW_NORMAL, FONTFLAG_NONE);
	menu_shade = Font(XOR("Tahoma"), 12, FW_NORMAL, FONTFLAG_DROPSHADOW);
	esp = Font(XOR("Segoe UI"), 14, FW_NORMAL, FONTFLAG_ANTIALIAS); // skeet name font ("Verdana"), 12, FW_NORMAL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW, pandora font Font(XOR("Segoe UI"), 14, FW_NORMAL, FONTFLAG_ANTIALIAS); //
	esp_grenade = Font(XOR("Segoe UI"), 18, FW_NORMAL, FONTFLAG_ANTIALIAS);
	esp_small = Font(XOR("Segoe UI"), 12, FW_NORMAL, FONTFLAG_ANTIALIAS); // weapon string font
	esp_info = Font(XOR("Segoe UI"), 12, FW_NORMAL, FONTFLAG_ANTIALIAS); // drop weapons
	hud = Font(XOR("Verdana"), 16, FW_NORMAL, FONTFLAG_ANTIALIAS); // indicators
	cs = Font(XOR("undefeated"), 15, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW); // weapon icon (it's useless cuz just watch down)
	cs_c4 = Font(XOR("Counter-Strike"), 28, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	indicator = Font(XOR("Verdana"), 26, FW_BOLD, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	logevent = Font(XOR("Segoe UI"), 14, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW); //skeet  Font(XOR("Lucida Console"), 10, FW_DONTCARE, FONTFLAG_DROPSHADOW 
	watermark = Font(XOR("Verdana"), 14, FW_NORMAL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	spectators = Font(XOR("Verdana"), 13, FW_NORMAL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	esp_name = Font(XOR("Segoe UI"), 14, FW_NORMAL, FONTFLAG_ANTIALIAS);

	LastFontName = "Verdana";
	g_csgo.m_surface->SetFontGlyphSet(F::ESPInfo = g_csgo.m_surface->CreateFont(), XOR("Small Fonts"), 8, 400, NULL, NULL, FONTFLAG_OUTLINE);
	LastFontName = "undefeated"; // weapon icon
	g_csgo.m_surface->SetFontGlyphSet(F::Icons = g_csgo.m_surface->CreateFont(), XOR("undefeated"), 13, FW_NORMAL, NULL, NULL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	LastFontName = "astriumwep 05_55"; // menu icon
	g_csgo.m_surface->SetFontGlyphSet(F::Skeet = g_csgo.m_surface->CreateFont(), XOR("astriumwep 05_55"), 13, FW_NORMAL, NULL, NULL, FONTFLAG_DROPSHADOW);

	LastFontName = "";
}

void render::gradient1337(int x, int y, int w, int h, Color color1, Color color2, bool isHorizontal) {
	g_csgo.m_surface->DrawSetColor(color1);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, 255, 0, isHorizontal);
}

void render::gradient1337reverse(int x, int y, int w, int h, Color color1, Color color2, bool isHorizontal) {
	g_csgo.m_surface->DrawSetColor(color2);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, 0, 255, isHorizontal);
}

bool render::WorldToScreen(const vec3_t& world, vec2_t& screen) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix();

	// check if it's in view first.
	// note - dex; w is below 0 when world position is around -90 / +90 from the player's camera on the y axis.
	w = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];
	if (w < 0.001f)
		return false;

	// calculate x and y.
	screen.x = matrix[0][0] * world.x + matrix[0][1] * world.y + matrix[0][2] * world.z + matrix[0][3];
	screen.y = matrix[1][0] * world.x + matrix[1][1] * world.y + matrix[1][2] * world.z + matrix[1][3];

	screen /= w;

	// calculate screen position.
	screen.x = (g_cl.m_width / 2) + (screen.x * g_cl.m_width) / 2;
	screen.y = (g_cl.m_height / 2) - (screen.y * g_cl.m_height) / 2;

	return true;
}

void render::DrawOutlinedRect(int x, int y, int w, int h, Color col)
{
	g_csgo.m_surface->DrawSetColor(col);
	g_csgo.m_surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void render::DrawRectGradientVertical(int x, int y, int width, int height, Color color1, Color color2)
{
	float flDifferenceR = (float)(color2.r() - color1.r()) / (float)height;
	float flDifferenceG = (float)(color2.g() - color1.g()) / (float)height;
	float flDifferenceB = (float)(color2.b() - color1.b()) / (float)height;
	float flDifferenceA = (float)(color2.a() - color1.a()) / (float)height;

	for (float i = 0.f; i < height; i++)
	{
		Color colGradient = Color(color1.r() + (flDifferenceR * i), color1.g() + (flDifferenceG * i), color1.b() + (flDifferenceB * i), color1.a() + (flDifferenceA * i));
		render::rect(x, y + i, width, 1, colGradient);
	}
}

void render::DrawRectGradientHorizontal(int x, int y, int width, int height, Color color1, Color color2)
{
	float flDifferenceR = (float)(color2.r() - color1.r()) / (float)width;
	float flDifferenceG = (float)(color2.g() - color1.g()) / (float)width;
	float flDifferenceB = (float)(color2.b() - color1.b()) / (float)width;
	float flDifferenceA = (float)(color2.a() - color1.a()) / (float)width;

	for (float i = 0.f; i < width; i++)
	{
		Color colGradient = Color(color1.r() + (flDifferenceR * i), color1.g() + (flDifferenceG * i), color1.b() + (flDifferenceB * i), color1.a() + (flDifferenceA * i));
		render::rect(x + i, y, 1, height, colGradient);
	}
}

void render::DrawLine(int x0, int y0, int x1, int y1, Color col, bool shadow)
{
	g_csgo.m_surface->DrawSetColor(col);
	g_csgo.m_surface->DrawLine(x0, y0, x1, y1);
}

bool render::WorldToScreen2(const vec3_t& world, vec3_t& screen) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix();

	// check if it's in view first.
	// note - dex; w is below 0 when world position is around -90 / +90 from the player's camera on the y axis.
	w = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];
	if (w < 0.001f)
		return false;

	// calculate x and y.
	screen.x = matrix[0][0] * world.x + matrix[0][1] * world.y + matrix[0][2] * world.z + matrix[0][3];
	screen.y = matrix[1][0] * world.x + matrix[1][1] * world.y + matrix[1][2] * world.z + matrix[1][3];

	screen /= w;

	// calculate screen position.
	screen.x = (g_cl.m_width / 2) + (screen.x * g_cl.m_width) / 2;
	screen.y = (g_cl.m_height / 2) - (screen.y * g_cl.m_height) / 2;

	return true;
}


void render::line(vec2_t v0, vec2_t v1, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawLine(v0.x, v0.y, v1.x, v1.y);
}

void render::line(int x0, int y0, int x1, int y1, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawLine(x0, y0, x1, y1);
}

void render::rect(int x, int y, int w, int h, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void render::rect_filled(int x, int y, int w, int h, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawFilledRect(x, y, x + w, y + h);
}

void render::rect_filled_fade(int x, int y, int w, int h, Color color, int a1, int a2) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, a1, a2, false);
}

void render::rect_outlined(int x, int y, int w, int h, Color color, Color color2) {
	rect(x, y, w, h, color);
	rect(x - 1, y - 1, w + 2, h + 2, color2);
	rect(x + 1, y + 1, w - 2, h - 2, color2);
}


void render::arccircle(int x, int y, int r1, int r2, int s, int d, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	for (int i = s; i < s + d; i++) {

		float rad = i * math::pi / 180;

		g_csgo.m_surface->DrawLine(x + cos(rad) * r1, y + sin(rad) * r1, x + cos(rad) * r2, y + sin(rad) * r2);
	}
}

void render::drawCircle(int x, int y, int angle, Color color) {
	float granularity = 2 * math::pi / 700;
	float step = 2 * math::pi / 100;
	float inner = 6;

	for (int radius = 8; inner < -1; radius--) {
		for (int angle = 0; angle * step < -1; granularity++) {
			int px = round(radius * cos(angle));
			int py = round(radius * sin(angle));

			g_csgo.m_surface->DrawLine(px + x, py + y, px + 1 + x, py + 1 + y);
		}

	}

}

void render::draw_arc(int x, int y, int radius, int start_angle, int percent, int thickness, Color color) {
	float precision = (2 * math::pi) / 30;
	float step = math::pi / 180;
	float inner = radius - thickness;
	float end_angle = (start_angle + percent) * step;
	float start_angle1337 = (start_angle * math::pi) / 180;

	for (; radius > inner; --radius) {
		for (float angle = start_angle1337; angle < end_angle; angle += precision) {
			float cx = round(x + radius * cos(angle));
			float cy = round(y + radius * sin(angle));

			float cx2 = round(x + radius * cos(angle + precision));
			float cy2 = round(y + radius * sin(angle + precision));

			g_csgo.m_surface->DrawSetColor(color);
			g_csgo.m_surface->DrawLine(cx, cy, cx2, cy2);
		}
	}
}

void render::circle(int x, int y, int radius, int segments, Color color) {
	static int texture = g_csgo.m_surface->CreateNewTextureID(true);

	g_csgo.m_surface->DrawSetTextureRGBA(texture, &colors::white, 1, 1);
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawSetTexture(texture);

	std::vector< Vertex > vertices{};

	float step = math::pi_2 / segments;
	for (float i{ 0.f }; i < math::pi_2; i += step)
		vertices.emplace_back(vec2_t{ x + (radius * std::cos(i)), y + (radius * std::sin(i)) });

	g_csgo.m_surface->DrawTexturedPolygon(vertices.size(), vertices.data());
}

void render::TexturedPolygon(int n, std::vector< Vertex > vertice, Color color) {
	static int texture_id = g_csgo.m_surface->CreateNewTextureID(true); // 
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	g_csgo.m_surface->DrawSetTextureRGBA(texture_id, buf, 1, 1); //
	g_csgo.m_surface->DrawSetColor(color); //
	g_csgo.m_surface->DrawSetTexture(texture_id); //
	g_csgo.m_surface->DrawTexturedPolygon(n, vertice.data()); //
}

void render::DrawFilledCircle(int x, int y, int radius, int segments, Color color) {
	std::vector< Vertex > vertices;

	float step = math::pi * 2.0f / segments;

	for (float a = 0; a < (math::pi * 2.0f); a += step)
		vertices.emplace_back(radius * cosf(a) + x, radius * sinf(a) + y);

	TexturedPolygon(vertices.size(), vertices, color);
}
#define ZERO vec3_t(0.0f, 0.0f, 0.0f)

void render::Draw3DFilledCircle(const vec3_t& origin, float radius, Color color) // OUTLINE 
{
	auto prevScreenPos = ZERO; //-V656
	auto step = math::pi * 2.0f / 72.0f;

	auto screenPos = ZERO;
	auto screen = ZERO;

	if (!render::WorldToScreen2(origin, screen))
		return;

	for (auto rotation = 0.0f; rotation <= math::pi * 2.0f; rotation += step) //-V1034
	{
		vec3_t pos(radius * cos(rotation) + origin.x, radius * sin(rotation) + origin.y, origin.z);

		if (render::WorldToScreen2(pos, screenPos))
		{
			if (!prevScreenPos.IsZero() && prevScreenPos.IsValid() && screenPos.IsValid() && prevScreenPos != screenPos)
			{
				render::DrawLine(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, color, true);
			}

			prevScreenPos = screenPos;
		}
	}
}

void render::Draw3DFilledCircle2(const vec3_t& origin, float radius, Color color, bool active_line, Color active) // FILLEDCIRCLE WITH OUTLINE
{
	auto prevScreenPos = ZERO; //-V656
	auto step = math::pi * 2.0f / 72.0f;

	auto screenPos = ZERO;
	auto screen = ZERO;

	if (!render::WorldToScreen2(origin, screen))
		return;

	for (auto rotation = 0.0f; rotation <= math::pi * 2.0f; rotation += step) //-V1034
	{
		vec3_t pos(radius * cos(rotation) + origin.x, radius * sin(rotation) + origin.y, origin.z);

		if (render::WorldToScreen2(pos, screenPos))
		{
			bool check = prevScreenPos.x != screenPos.x || prevScreenPos.y != screenPos.y || prevScreenPos.z != screenPos.z;
			if (!prevScreenPos.IsZero() && prevScreenPos.IsValid() && screenPos.IsValid() && check)
			{
				render::DrawLine(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, active_line ? active : color, true);
				triangle(vec2_t(screen.x, screen.y), vec2_t(screenPos.x, screenPos.y), vec2_t(prevScreenPos.x, prevScreenPos.y), color.a() / 2);
			}

			prevScreenPos = screenPos;
		}
	}
}

__forceinline std::wstring ascii_to_unicode(const std::string& ascii) {
	std::wstring unicode_str(ascii.begin(), ascii.end());
	return unicode_str;
}

void render::get_text_size(int& w, int& h, const char* msg, ...) {
	char* buffer = (char*)_alloca(1024);
	va_list list{ };

	memset(buffer, 0, 1024);

	__crt_va_start(list, msg);
	vsprintf_s(buffer, 1024, msg, list);
	__crt_va_end(list);

	g_csgo.m_surface->GetTextSize(F::ESPInfo, ascii_to_unicode(buffer).c_str(), w, h);
}


void render::gradient(int x, int y, int w, int h, Color color1, Color color2) {
	g_csgo.m_surface->DrawSetColor(color1);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, color1.a(), 0, false);

	g_csgo.m_surface->DrawSetColor(color2);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, 0, color2.a(), false);
}

void render::sphere(vec3_t origin, float radius, float angle, float scale, Color color) {
	std::vector< Vertex > vertices{};

	// compute angle step for input radius and precision.
	float step = (1.f / radius) + math::deg_to_rad(angle);

	for (float lat{}; lat < (math::pi * scale); lat += step) {
		// reset.
		vertices.clear();

		for (float lon{}; lon < math::pi_2; lon += step) {
			vec3_t point{
				origin.x + (radius * std::sin(lat) * std::cos(lon)),
				origin.y + (radius * std::sin(lat) * std::sin(lon)),
				origin.z + (radius * std::cos(lat))
			};

			vec2_t screen;
			if (WorldToScreen(point, screen))
				vertices.emplace_back(screen);
		}

		if (vertices.empty())
			continue;

		g_csgo.m_surface->DrawSetColor(color);
		g_csgo.m_surface->DrawTexturedPolyLine(vertices.size(), vertices.data());
	}
}

void render::triangle(vec2_t point_one, vec2_t point_two, vec2_t point_three, Color color)
{
	Vertex verts[3] = {
		Vertex(point_one),
		Vertex(point_two),
		Vertex(point_three)
	};

	static int texture = g_csgo.m_surface->CreateNewTextureID(true);
	unsigned char buffer[4] = { 255, 255, 255, 255 };

	g_csgo.m_surface->DrawSetTextureRGBA(texture, buffer, 1, 1);
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawSetTexture(texture);

	g_csgo.m_surface->DrawTexturedPolygon(3, verts);
}

Vertex render::RotateVertex(const vec2_t& p, const Vertex& v, float angle) {
	// convert theta angle to sine and cosine representations.
	float c = std::cos(math::deg_to_rad(angle));
	float s = std::sin(math::deg_to_rad(angle));

	return {
		p.x + (v.m_pos.x - p.x) * c - (v.m_pos.y - p.y) * s,
		p.y + (v.m_pos.x - p.x) * s + (v.m_pos.y - p.y) * c
	};
}

void render::Font::string(int x, int y, Color color, const std::string& text, StringFlags_t flags /*= render::ALIGN_LEFT */) {
	wstring(x, y, color, util::MultiByteToWide(text), flags);
}

void render::Font::string(int x, int y, Color color, const std::stringstream& text, StringFlags_t flags /*= render::ALIGN_LEFT */) {
	wstring(x, y, color, util::MultiByteToWide(text.str()), flags);
}

void render::Font::wstring(int x, int y, Color color, const std::wstring& text, StringFlags_t flags /*= render::ALIGN_LEFT */) {
	int w, h;

	g_csgo.m_surface->GetTextSize(m_handle, text.c_str(), w, h);
	g_csgo.m_surface->DrawSetTextFont(m_handle);
	g_csgo.m_surface->DrawSetTextColor(color);

	if (flags & ALIGN_RIGHT)
		x -= w;
	if (flags & render::ALIGN_CENTER)
		x -= w / 2;

	g_csgo.m_surface->DrawSetTextPos(x, y);
	g_csgo.m_surface->DrawPrintText(text.c_str(), (int)text.size());
}

RECT render::GetTextSize(vgui::HFont font, const wchar_t* text)
{
	RECT rect;

	rect.left = rect.right = rect.bottom = rect.top = 0;

	int x, y;
	g_csgo.m_surface->GetTextSize(font, text, x, y);
	rect.left = x; rect.bottom = y;
	rect.right = x; rect.top = y;

	return rect;
}

void render::DrawStringUnicode(vgui::HFont font, int x, int y, Color color, bool bCenter, const wchar_t* msg, ...)
{
	int r = 255, g = 255, b = 255, a = 255;

	int iWidth, iHeight;

	g_csgo.m_surface->GetTextSize(font, msg, iWidth, iHeight);
	g_csgo.m_surface->DrawSetTextFont(font);
	g_csgo.m_surface->DrawSetTextColor(color);
	g_csgo.m_surface->DrawSetTextPos(x, y);
	g_csgo.m_surface->DrawPrintText(msg, wcslen(msg));
}

void render::DrawString(vgui::HFont font, int x, int y, Color color, DWORD alignment, const char* msg, ...)
{
	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf_s(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;

	int width, height;
	g_csgo.m_surface->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	g_csgo.m_surface->DrawSetTextFont(font);

	if (width > 65536)
		return;

	if (alignment & FONT_OUTLINE)
	{
		g_csgo.m_surface->DrawSetTextColor(Color(0, 0, 0, color.a()));
		g_csgo.m_surface->DrawSetTextPos(x + 1, y /*+ 1*/);
		g_csgo.m_surface->DrawPrintText(wbuf, wcslen(wbuf));

		g_csgo.m_surface->DrawSetTextColor(Color(0, 0, 0, color.a()));
		g_csgo.m_surface->DrawSetTextPos(x - 1, y /*- 1*/);
		g_csgo.m_surface->DrawPrintText(wbuf, wcslen(wbuf));
	}

	g_csgo.m_surface->DrawSetTextColor(color);
	g_csgo.m_surface->DrawSetTextPos(x, y /*- height / 2*/);
	g_csgo.m_surface->DrawPrintText(wbuf, wcslen(wbuf));
}

void render::Draw3DBox(vec3_t* boxVectors, Color color)
{
	vec2_t boxVectors0, boxVectors1, boxVectors2, boxVectors3,
		boxVectors4, boxVectors5, boxVectors6, boxVectors7;


	if (render::WorldToScreen(boxVectors[0], boxVectors0) &&
		render::WorldToScreen(boxVectors[1], boxVectors1) &&
		render::WorldToScreen(boxVectors[2], boxVectors2) &&
		render::WorldToScreen(boxVectors[3], boxVectors3) &&
		render::WorldToScreen(boxVectors[4], boxVectors4) &&
		render::WorldToScreen(boxVectors[5], boxVectors5) &&
		render::WorldToScreen(boxVectors[6], boxVectors6) &&
		render::WorldToScreen(boxVectors[7], boxVectors7))
	{

		/*
		.+--5---+
		.8 4    6'|
		+--7---+'  11
		9   |  10  |
		|  ,+--|3--+
		|.0    | 2'
		+--1---+'
		*/

		vec2_t lines[12][2];
		//bottom of box
		lines[0][0] = { boxVectors0.x, boxVectors0.y };
		lines[0][1] = { boxVectors1.x, boxVectors1.y };
		lines[1][0] = { boxVectors1.x, boxVectors1.y };
		lines[1][1] = { boxVectors2.x, boxVectors2.y };
		lines[2][0] = { boxVectors2.x, boxVectors2.y };
		lines[2][1] = { boxVectors3.x, boxVectors3.y };
		lines[3][0] = { boxVectors3.x, boxVectors3.y };
		lines[3][1] = { boxVectors0.x, boxVectors0.y };

		lines[4][0] = { boxVectors0.x, boxVectors0.y };
		lines[4][1] = { boxVectors6.x, boxVectors6.y };

		// top of box
		lines[5][0] = { boxVectors6.x, boxVectors6.y };
		lines[5][1] = { boxVectors5.x, boxVectors5.y };
		lines[6][0] = { boxVectors5.x, boxVectors5.y };
		lines[6][1] = { boxVectors4.x, boxVectors4.y };
		lines[7][0] = { boxVectors4.x, boxVectors4.y };
		lines[7][1] = { boxVectors7.x, boxVectors7.y };
		lines[8][0] = { boxVectors7.x, boxVectors7.y };
		lines[8][1] = { boxVectors6.x, boxVectors6.y };


		lines[9][0] = { boxVectors5.x, boxVectors5.y };
		lines[9][1] = { boxVectors1.x, boxVectors1.y };

		lines[10][0] = { boxVectors4.x, boxVectors4.y };
		lines[10][1] = { boxVectors2.x, boxVectors2.y };

		lines[11][0] = { boxVectors7.x, boxVectors7.y };
		lines[11][1] = { boxVectors3.x, boxVectors3.y };

		for (int i = 0; i < 12; i++)
		{
			render::DrawLine(lines[i][0].x, lines[i][0].y, lines[i][1].x, lines[i][1].y, color, true);
		}
	}
}

render::FontSize_t render::Font::size(const std::string& text) {
	return wsize(util::MultiByteToWide(text));
}

render::FontSize_t render::Font::wsize(const std::wstring& text) {
	FontSize_t res;
	g_csgo.m_surface->GetTextSize(m_handle, text.data(), res.m_width, res.m_height);
	return res;
}

void render::set_random_seed(int seed) {
	using fn = int(__cdecl*)(int);
	fn fn_ptr = (fn)(GetProcAddress(
		GetModuleHandleA(XOR("vstdlib.dll")),
		XOR("RandomSeed")));

	fn_ptr(seed);
}

float render::get_random_float(float min, float max) {
	using fn = float(*)(float, float);
	fn fn_ptr = (fn)(GetProcAddress(
		GetModuleHandleA(XOR("vstdlib.dll")),
		XOR("RandomFloat")));

	return fn_ptr(min, max);
}

int render::get_random_int(int min, int max) {
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<> distr(min, max);

	return distr(eng);
}