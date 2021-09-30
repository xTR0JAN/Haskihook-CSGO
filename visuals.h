#pragma once

struct OffScreenDamageData_t {
    float m_time, m_color_step;
    Color m_color;

    __forceinline OffScreenDamageData_t() : m_time{ 0.f }, m_color{ colors::white } {}
    __forceinline OffScreenDamageData_t(float time, float m_color_step, Color color) : m_time{ time }, m_color{ color } {}
};

struct WorldHitmarkerData_t {
    float m_time, m_alpha;
    float m_pos_x, m_pos_y, m_pos_z;

    vec2_t m_world_pos;
    bool m_world_to_screen;
};

class Visuals {
private:

    //you should always store the entity index rather than the ptr
    struct sound_t {
        float m_time;
        vec3_t m_pos;
        int m_ent;
    };
public:
    std::array< bool, 64 >                  m_draw;
    std::array< float, 2048 >               m_opacities;
    std::array< OffScreenDamageData_t, 64 > m_offscreen_damage;
    std::vector< WorldHitmarkerData_t >     m_world_hitmarkers;
    std::vector< sound_t > m_sounds;
    vec2_t                                  m_crosshair;
    bool                                    m_thirdperson;
    int                                    fakeangels[64];
    float					                m_hit_start, m_hit_end, m_hit_duration;

    // info about planted c4.
    bool                                    planted;
    bool                                    bombexploded;
    bool                                    bombedefused;
    std::string                             m_bombsite;
    bool                                    bombeindefuse;
    float                                   m_planted_c4_defuse_time;
    bool        m_c4_planted;
    Entity* m_planted_c4;
    float       m_planted_c4_explode_time;
    vec3_t      m_planted_c4_explosion_origin;
    float       m_planted_c4_damage;
    float       m_planted_c4_radius;
    float       m_planted_c4_radius_scaled;
    float       miss;
    float       miss1;
    float       miss2;
    std::string m_last_bombsite;

    std::array<const wchar_t*, 900u> weapon_names = {};

    IMaterial* smoke1;
    IMaterial* smoke2;
    IMaterial* smoke3;
    IMaterial* smoke4;

    std::unordered_map< int, char > m_weapon_icons = {
        { DEAGLE, 'F' },
        { ELITE, 'S' },
        { FIVESEVEN, 'U' },
        { GLOCK, 'C' },
        { AK47, 'B' },
        { AUG, 'E' },
        { AWP, 'R' },
        { FAMAS, 'T' },
        { G3SG1, 'I' },
        { GALIL, 'V' },
        { M249, 'Z' },
        { M4A4, 'W' },
        { MAC10, 'L' },
        { P90, 'M' },
        { UMP45, 'Q' },
        { XM1014, ']' },
        { BIZON, 'D' },
        { MAG7, 'K' },
        { NEGEV, 'Z' },
        { SAWEDOFF, 'K' },
        { TEC9, 'C' },
        { ZEUS, 'Y' },
        { P2000, 'Y' },
        { MP7, 'X' },
        { MP9, 'D' },
        { NOVA, 'K' },
        { P250, 'Y' },
        { SCAR20, 'I' },
        { SG553, '[' },
        { SSG08, 'N' },
        { KNIFE_CT, 'J' },
        { FLASHBANG, 'G' },
        { HEGRENADE, 'H' },
        { SMOKE, 'P' },
        { MOLOTOV, 'H' },
        { DECOY, 'G' },
        { FIREBOMB, 'H' },
        { C4, '\\' },
        { KNIFE_T, 'J' },
        { M4A1S, 'W' },
        { USPS, 'Y' },
        { CZ75A, 'Y' },
        { REVOLVER, 'F' },
        { KNIFE_BAYONET, 'J' },
        { KNIFE_FLIP, 'J' },
        { KNIFE_GUT, 'J' },
        { KNIFE_KARAMBIT, 'J' },
        { KNIFE_M9_BAYONET, 'J' },
        { KNIFE_HUNTSMAN, 'J' },
        { KNIFE_FALCHION, 'J' },
        { KNIFE_BOWIE, 'J' },
        { KNIFE_BUTTERFLY, 'J' },
        { KNIFE_SHADOW_DAGGERS, 'J' },
    };
public:
    static void ModulateWorld();
    void ThirdpersonThink();
    void IndicateAngles();
    void DrawDamageIndicator();
    void Keybinds();
    void Hitmarker();
    void HitmarkerWorld();
    void NoSmoke();
    void think();
    void Spectators();
    void StatusIndicators();
    void SpreadCrosshair();
    void ManualAntiAim();
    void PenetrationCrosshair();
    void DrawPlantedC4();
    void draw(Entity* ent);
    void DrawProjectile(Weapon* ent);
    void DrawItem(Weapon* item);
    void OffScreen(Player* player, int alpha);
    void DrawPlayer(Player* player);
    bool GetPlayerBoxRect(Player* player, Rect& box);
    void DrawHistorySkeleton(Player* player, int opacity);
    void DrawSkeleton(Player* player, int opacity);
    void RenderGlow();
    void DrawHitboxMatrix(LagRecord* record, Color col, float time);
    //void DrawHitboxMatrix(LagRecord* record, Color col, float time);
    void DrawBeams();
    void DebugAimbotPoints(Player* player);
    void update_positions();
    void store_sound(int index, vec3_t origin);
    void draw_sound();
};

extern Visuals g_visuals;