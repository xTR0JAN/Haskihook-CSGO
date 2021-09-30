#include "includes.h"

void GradientLine(int x, int y, int w, int h, Color c1, Color c2)
{

	render::rect_filled(x, y, w, h, c1);
	BYTE first = c2.r();
	BYTE second = c2.g();
	BYTE third = c2.b();
	for (int i = 0; i < w; i++)
	{
		float fi = i, fw = w;
		float a = fi / fw;
		DWORD ia = a * 255;
		render::rect_filled(x + i, y, 1, h, Color(first, second, third, ia));
	}

}

void Form::draw() {
	// opacity should reach 1 in 500 milliseconds.
	constexpr float frequency = 1.f / 0.5f;

	// the increment / decrement per frame.
	float step = frequency * g_csgo.m_globals->m_frametime;

	// if open		-> increment
	// if closed	-> decrement
	m_open ? m_opacity += step : m_opacity -= step;

	// clamp the opacity.
	math::clamp(m_opacity, 0.f, 1.f);

	m_alpha = 0xff * m_opacity;
	if (!m_alpha)
		return;

	// get gui color.
	Color color = g_gui.m_color;
	color.a() = m_alpha;

	// background.
	render::rect_filled(m_x, m_y, m_width, m_height, { 12, 12, 12, m_alpha });

	// lbgt line
	//render::gradient1337(m_x + 7, m_y + 7, m_width / 2 - 7, 1, { 99, 160, 200, m_alpha }, { 179, 102, 181, m_alpha });
	//render::gradient1337(m_x + (m_width / 2), m_y + 7, m_width / 2 - 7, 1, { 179, 102, 181, m_alpha }, { 230, 217, 100, m_alpha });

	//render::gradient1337(m_x + 7, m_y + 7 + 1, m_width / 2 - 7, 1, { 49, 79, 99, m_alpha }, { 89, 50, 90, m_alpha });
	//render::gradient1337(m_x + (m_width / 2), m_y + 7 + 1, m_width / 2 - 7, 1, { 89, 50, 90, m_alpha }, { 114, 108, 49, m_alpha });

	// border.
	/*render::rect(m_x, m_y, m_width, m_height, {5, 5, 5, m_alpha});
	render::rect( m_x + 1, m_y + 1, m_width - 2, m_height - 2, { 60, 60, 60, m_alpha } );
	render::rect( m_x + 2, m_y + 2, m_width - 4, m_height - 4, { 40, 40, 40, m_alpha } );
	render::rect( m_x + 3, m_y + 3, m_width - 6, m_height - 6, { 40, 40, 40, m_alpha } );
	render::rect( m_x + 4, m_y + 4, m_width - 8, m_height - 8, { 40, 40, 40, m_alpha } );
	render::rect( m_x + 5, m_y + 5, m_width - 10, m_height - 10, { 60, 60, 60, m_alpha } );
	*/

	// border.
	render::rect(m_x, m_y, m_width, m_height, color);

	static int s, v, i;
	static float h, r, g, b, f, p, q, t;

	h = g_csgo.m_globals->m_realtime * 0.1f;
	s = 1;
	v = 1;

	i = floor(h * 6);
	f = h * 6 - i;
	p = v * (1 - s);
	q = v * (1 - f * s);
	t = v * (1 - (1 - f) * s);

	switch (i % 6)
	{
	case 0: r = v, g = t, b = p; break;
	case 1: r = q, g = v, b = p; break;
	case 2: r = p, g = v, b = t; break;
	case 3: r = p, g = q, b = v; break;
	case 4: r = t, g = p, b = v; break;
	case 5: r = v, g = p, b = q; break;
	}

	r = round(r * 255), g = round(g * 255), b = round(b * 255);

	GradientLine(m_x + 6, m_y + 6, (m_width - 12) / 2, 2, Color(g, b, r, 255), Color(r, g, b, 255));
	GradientLine(m_x + 6 + (m_width - 12) / 2, m_y + 6, (m_width - 12) / 2, 2, Color(r, g, b, 255), Color(b, r, g, 255));


	// draw tabs if we have any.
	if (!m_tabs.empty()) {
		// tabs background and border.
		Rect tabs_area = GetTabsRect();

		render::rect_filled(tabs_area.x, tabs_area.y, tabs_area.w, tabs_area.h, { 17, 17, 17, m_alpha });
		render::rect(tabs_area.x, tabs_area.y, tabs_area.w, tabs_area.h, { 0, 0, 0, m_alpha });
		render::rect(tabs_area.x + 1, tabs_area.y + 1, tabs_area.w - 2, tabs_area.h - 2, { 48, 48, 48, m_alpha });

		for (size_t i{}; i < m_tabs.size(); ++i) {
			const auto& t = m_tabs[i];

			render::menu_shade.string(tabs_area.x + 10, tabs_area.y + 5 + (i * 16),
				t == m_active_tab ? color : Color{ 152, 152, 152, m_alpha },
				t->m_title);

			// render icons, comment this to back the default text tabs
/*	if (i == 0)
	{
		render::SkeetIcon.string(tabs_area.x + 20, tabs_area.y + 5 + (i * 16),
			t == m_active_tab ? color : Color{ 152, 152, 152, m_alpha },
			"A");
	}
	else if (i == 1)
	{
		render::SkeetIcon.string(tabs_area.x + 8, tabs_area.y + 55 + (i * 16),
			t == m_active_tab ? color : Color{ 152, 152, 152, m_alpha },
			"B");
	}
	else if (i == 2)
	{
		render::SkeetIcon.string(tabs_area.x + 6, tabs_area.y + 110 + (i * 16),
			t == m_active_tab ? color : Color{ 152, 152, 152, m_alpha },
			"C");
	}
	else if (i == 3)
	{
		render::SkeetIcon.string(tabs_area.x + 6, tabs_area.y + 160 + (i * 16),
			t == m_active_tab ? color : Color{ 152, 152, 152, m_alpha },
			"D");
	}
	else if (i == 4)
	{
		render::SkeetIcon.string(tabs_area.x + 6, tabs_area.y + 210 + (i * 16),
			t == m_active_tab ? color : Color{ 152, 152, 152, m_alpha },
			"U");
	}
	else if (i == 5)
	{
		render::SkeetIcon.string(tabs_area.x + 6, tabs_area.y + 265 + (i * 16),
			t == m_active_tab ? color : Color{ 152, 152, 152, m_alpha },
			"k");
	}
	else if (i == 6)
	{
		render::SkeetIcon.string(tabs_area.x + 6, tabs_area.y + 320 + (i * 16),
			t == m_active_tab ? color : Color{ 152, 152, 152, m_alpha },
			"A");
	}*/
		}

		// this tab has elements.
		if (!m_active_tab->m_elements.empty()) {
			// elements background and border.
			Rect el = GetElementsRect();

			render::rect_filled(el.x, el.y, el.w, el.h, { 17, 17, 17, m_alpha });
			render::rect(el.x, el.y, el.w, el.h, { 0, 0, 0, m_alpha });
			render::rect(el.x + 1 / 2, el.y + 1 / 2, el.w - 2 / 2, el.h - 2 / 2, { 48, 48, 48, m_alpha });

			std::string time = XOR(__TIME__);
			std::string date = XOR(__DATE__);

			//std::string text = tfm::format( XOR( "%s | %s | build time: %s" ), date.c_str( ), g_cl.m_user, time.c_str());
			//render::menu_shade.string( el.x + el.w - 5, el.y + el.h - 16, { 205, 205, 205, m_alpha }, text, render::ALIGN_RIGHT );

			// iterate elements to display.
			for (const auto& e : m_active_tab->m_elements) {

				// draw the active element last.
				if (!e || (m_active_element && e == m_active_element))
					continue;

				if (!e->m_show)
					continue;

				// this element we dont draw.
				if (!(e->m_flags & ElementFlags::DRAW))
					continue;

				e->draw();
			}

			// we still have to draw one last fucker.
			if (m_active_element && m_active_element->m_show)
				m_active_element->draw();
		}
	}
};