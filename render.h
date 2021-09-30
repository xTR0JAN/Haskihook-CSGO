#pragma once

struct Point {
	int x;
	int y;
};

struct Rect {
	int x;
	int y;
	int w;
	int h;
};

namespace vgui
{
	typedef unsigned long HFont;
	typedef unsigned long VPANEL;
};

namespace F
{
	extern vgui::HFont ESPInfo;
	extern vgui::HFont Icons;
	extern vgui::HFont Skeet;
};

namespace render {
	struct FontSize_t {
		int m_width;
		int m_height;
	};

	enum StringFlags_t {
		ALIGN_LEFT = 0,
		ALIGN_RIGHT,
		ALIGN_CENTER
	};

	class Font {
	public:
		HFont      m_handle;
		FontSize_t m_size;

	public:
		__forceinline Font() : m_handle{}, m_size{} {};

		// ctor.
		__forceinline Font(const std::string& name, int s, int w, int flags) {
			m_handle = g_csgo.m_surface->CreateFont();
			g_csgo.m_surface->SetFontGlyphSet(m_handle, name.data(), s, w, 0, 0, flags);
			m_size = size(XOR("A"));
		}

		// ctor.
		__forceinline Font(HFont font) {
			m_handle = font;
			m_size = size(XOR("A"));
		}

		void string(int x, int y, Color color, const std::string& text, StringFlags_t flags = ALIGN_LEFT);
		void string(int x, int y, Color color, const std::stringstream& text, StringFlags_t flags = ALIGN_LEFT);
		void wstring(int x, int y, Color color, const std::wstring& text, StringFlags_t flags = ALIGN_LEFT);
		render::FontSize_t size(const std::string& text);
		FontSize_t wsize(const std::wstring& text);
	};

	extern Font SkeetIcon;
	extern Font menu;
	extern Font menu_shade;
	extern Font esp;
	extern Font esp_grenade;
	extern Font esp_small;
	extern Font esp_info;
	extern Font hud;
	extern Font cs;
	extern Font cs_c4;
	extern Font indicator;
	extern Font logevent;
	extern Font watermark;
	extern Font spectators;
	extern Font esp_name;;

	void init();
	bool WorldToScreen(const vec3_t& world, vec2_t& screen);
	void DrawOutlinedRect(int x, int y, int w, int h, Color col);
	void DrawRectGradientVertical(int x, int y, int width, int height, Color color1, Color color2);
	void DrawRectGradientHorizontal(int x, int y, int width, int height, Color color1, Color color2);
	void DrawLine(int x0, int y0, int x1, int y1, Color col, bool shadow);
	bool WorldToScreen2(const vec3_t& world, vec3_t& screen);
	void line(vec2_t v0, vec2_t v1, Color color);
	void line(int x0, int y0, int x1, int y1, Color color);
	void rect(int x, int y, int w, int h, Color color);
	void rect_filled(int x, int y, int w, int h, Color color);
	void arccircle(int x, int y, int r1, int r2, int s, int d, Color color);
	void drawCircle(int x, int y, int angle, Color color);
	void draw_arc(int x, int y, int radius, int start_angle, int percent, int thickness, Color color);
	void rect_filled_fade(int x, int y, int w, int h, Color color, int a1, int a2);
	void rect_outlined(int x, int y, int w, int h, Color color, Color color2);
	void circle(int x, int y, int radius, int segments, Color color);
	void TexturedPolygon(int n, std::vector<Vertex> vertice, Color color);
	void DrawPolygon(int count, Vertex* vertices, const Color& col);
	void DrawFilledCircle(int x, int y, int radius, int segments, Color color);
	void Draw3DFilledCircle(const vec3_t& origin, float radius, Color color);
	void Draw3DFilledCircle2(const vec3_t& origin, float radius, Color color, bool active_line, Color active);
	void get_text_size(int& w, int& h, const char* msg, ...);
	void gradient(int x, int y, int w, int h, Color color1, Color color2);
	RECT GetTextSize(vgui::HFont font, const wchar_t* text);
	void DrawStringUnicode(vgui::HFont font, int x, int y, Color color, bool bCenter, const wchar_t* msg, ...);
	void gradient1337(int x, int y, int w, int h, Color color1, Color color2, bool isHorizontal);
	void gradient1337reverse(int x, int y, int w, int h, Color color1, Color color2, bool isHorizontal);
	void sphere(vec3_t origin, float radius, float angle, float scale, Color color);
	void triangle(vec2_t point_one, vec2_t point_two, vec2_t point_three, Color color);
	Vertex RotateVertex(const vec2_t& p, const Vertex& v, float angle);
	void DrawString(vgui::HFont font, int x, int y, Color color, DWORD alignment, const char* msg, ...);

	void Draw3DBox(vec3_t* boxVectors, Color color);

	void set_random_seed(int seed);

	float get_random_float(float min, float max);

	int get_random_int(int min, int max);

	extern const char* LastFontName;
}

// pride month <3
