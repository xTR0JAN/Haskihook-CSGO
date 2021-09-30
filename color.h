#pragma once

class Color {
private:
	// easy reinterpret.
	union {
		struct {
			uint8_t m_r;
			uint8_t m_g;
			uint8_t m_b;
			uint8_t m_a;
		};

		uint32_t m_rgba;
	};

public:
	// ctors.
	__forceinline Color() : m_r{ 0 }, m_g{ 0 }, m_b{ 0 }, m_a{ 0 }, m_rgba{} {}
	__forceinline Color(int r, int g, int b, int a = 255) : m_r{ (uint8_t)r }, m_g{ (uint8_t)g }, m_b{ (uint8_t)b }, m_a{ (uint8_t)a } {}
	__forceinline Color(uint32_t rgba) : m_rgba{ rgba } {}

	static Color hsl_to_rgb(float h, float s, float l) {
		float q;

		if (l < 0.5f)
			q = l * (s + 1.f);

		else
			q = l + s - (l * s);

		float p = 2 * l - q;

		float rgb[3];
		rgb[0] = h + (1.f / 3.f);
		rgb[1] = h;
		rgb[2] = h - (1.f / 3.f);

		for (int i = 0; i < 3; ++i) {
			if (rgb[i] < 0)
				rgb[i] += 1.f;

			if (rgb[i] > 1)
				rgb[i] -= 1.f;

			if (rgb[i] < (1.f / 6.f))
				rgb[i] = p + ((q - p) * 6 * rgb[i]);
			else if (rgb[i] < 0.5f)
				rgb[i] = q;
			else if (rgb[i] < (2.f / 3.f))
				rgb[i] = p + ((q - p) * 6 * ((2.f / 3.f) - rgb[i]));
			else
				rgb[i] = p;
		}

		return {
			int(rgb[0] * 255.f),
			int(rgb[1] * 255.f),
			int(rgb[2] * 255.f)
		};
	}

	static Color from_hsb(float hue, float saturation, float brightness) {
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - (saturation * (1.0f - f)));

		if (h < 1) {
			return Color(
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 2) {
			return Color(
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255)
			);
		}
		else if (h < 3) {
			return Color(
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255)
			);
		}
		else if (h < 4) {
			return Color(
				(unsigned char)(p * 255),
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else if (h < 5) {
			return Color(
				(unsigned char)(t * 255),
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255)
			);
		}
		else {
			return Color(
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255),
				(unsigned char)(q * 255)
			);
		}
	}

	Color malpha(float _a)
	{
		if (_a > 1.0f)
			_a = 1.0f;
		else if (_a < 0.0f)
			_a = 0.0f;

		return Color(this->r(), this->g(), this->b(), this->a() * _a);
	}


	static Color Pulsate(float& clr, float min, float max, float pulsate_speed)
	{
		static bool state = false;

		if (!state)
		{
			if (clr < max)
				clr += pulsate_speed;
			else
			{
				clr = max; state = !state;
			};
		}
		else
		{
			if (clr > min)
				clr -= pulsate_speed;
			else
			{
				clr = min; state = !state;
			};
		};
	};

	// member accessors.
	__forceinline uint8_t& r() { return m_r; }
	__forceinline uint8_t& g() { return m_g; }
	__forceinline uint8_t& b() { return m_b; }
	__forceinline uint8_t& a() { return m_a; }
	__forceinline uint32_t& rgba() { return m_rgba; }

	// operators.
	__forceinline operator uint32_t() { return m_rgba; }
};

namespace colors {
	static Color null{ 0,0,0,0 };
	static Color white{ 255, 255, 255, 255 };
	static Color black{ 0, 0, 0, 255 };
	static Color yellowgreen{ 182,231,23,255 };
	static Color red{ 255, 0, 0, 255 };
	static Color burgundy{ 0xff2d00b3 };
	static Color light_blue{ 95, 174, 227, 255 };
	static Color orange{ 243, 156, 18, 255 };
	static Color green{ 149, 184, 6, 255 };
	static Color purple{ 180, 60, 120, 255 };
	static Color transparent_green{ 0, 255, 0, 200 };
	static Color transparent_yellow{ 255, 255, 0, 200 };
	static Color transparent_red{ 255, 0, 0, 200 };
	static Color cyangreen{ 154, 205, 50 };
}