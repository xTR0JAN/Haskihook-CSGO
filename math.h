#pragma once

namespace math {
    // pi constants.
    constexpr float pi = 3.1415926535897932384f; // pi
    constexpr float pi_2 = pi * 2.f;               // pi * 2
    static const float twopi = 6.283185f;

#ifndef min1
#define min1(a,b)            (((a) < (b)) ? (a) : (b))
#define M_PI		3.14159265358979323846f
#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

    static std::random_device rd;
    static std::mt19937 gen(rd());

    // degrees to radians.
    __forceinline constexpr float deg_to_rad(float val) {
        return val * (pi / 180.f);
    }

    // radians to degrees.
    __forceinline constexpr float rad_to_deg(float val) {
        return val * (180.f / pi);
    }

    // angle mod ( shitty normalize ).
    __forceinline float AngleMod(float angle) {
        return (360.f / 65536) * ((int)(angle * (65536.f / 360.f)) & 65535);
    }

    __forceinline static float interpolate(const float from, const float to, const float percent)
    {
        if (from == to)
            return from;

        return to * percent + from * (1.f - percent);
    }

    template< class T, class Y, class X >
    inline T clamp1(T const& val, Y const& minVal, X const& maxVal)
    {
        if (val < minVal)
            return minVal;
        else if (val > maxVal)
            return maxVal;
        else
            return val;
    }

    template < typename t >
    __forceinline t random_number(t min, t max) {
        if constexpr (!std::is_integral_v< t >) {
            std::uniform_real_distribution< t > dist(min, max);
            return dist(gen);
        }
        else {
            std::uniform_int_distribution< t > dist(min, max);
            return dist(gen);
        }
    }

    template< class T >
    __forceinline T Max(T const& val1, T const& val2)
    {
        return val1 > val2 ? val1 : val2;
    }

    template<typename T>
    void LimitValue(T& value, const T& min, const T& max)
    {
        if (value > max)
            value = max;
        else if (value < min)
            value = min;
    }

    template<typename T>
    T ToRadians(T degrees)
    {
        return (degrees * (static_cast<T>(RadPi) / static_cast<T>(DegPi)));
    }

    typedef __declspec(align(16)) union {
        float f[4];
        __m128 v;
    } m128;

    inline __m128 sqrt_ps(const __m128 squared) {
        return _mm_sqrt_ps(squared);
    }

    void AngleMatrix(const ang_t& ang, const vec3_t& pos, matrix3x4_t& out);

    // normalizes an angle.
    void NormalizeAngle(float& angle);

    float NormalizeYaw(float angle);

    __forceinline float NormalizedAngle(float angle) {
        NormalizeAngle(angle);
        return angle;
    }

    __forceinline float normalize_angle(float angle)
    {
        //return remainderf(angle, 360.0f);
        float rtn = angle;

        for (; rtn > 180.0f; rtn = rtn - 360.0f)
            ;
        for (; rtn < -180.0f; rtn = rtn + 360.0f)
            ;

        return rtn;
    }

    static float normalize_float(float angle)
    {
        auto revolutions = angle / 360.f;
        if (angle > 180.f || angle < -180.f)
        {
            revolutions = round(abs(revolutions));
            if (angle < 0.f)
                angle = (angle + 360.f * revolutions);
            else
                angle = (angle - 360.f * revolutions);
            return angle;
        }
        return angle;
    }

    __forceinline float SimpleSpline(float value) {
        float valueSquared = value * value;

        // Nice little ease-in, ease-out spline-like curve
        return (3 * valueSquared - 2 * valueSquared * value);
    }

    __forceinline float SimpleSplineRemapValClamped(float val, float A, float B, float C, float D) {
        if (A == B)
            return val >= B ? D : C;
        float cVal = (val - A) / (B - A);
        cVal = std::clamp(cVal, 0.0f, 1.0f);
        return C + (D - C) * SimpleSpline(cVal);
    }

    vec3_t CalcAngle(const vec3_t& vecSource, const vec3_t& vecDestination);
    void CalcAngle3(const vec3_t src, const vec3_t dst, ang_t& angles);
    void MatrixSetOrigin(vec3_t pos, matrix3x4_t& matrix);
    vec3_t MatrixGetOrigin(const matrix3x4_t& src);
    float ApproachAngle(float target, float value, float speed);
    void  VectorAngles(const vec3_t& forward, ang_t& angles, vec3_t* up = nullptr);
    void VectorAngles2(const vec3_t& vecForward, vec3_t& vecAngles);
    void NormalizeVector(vec3_t& vec);
    void AngleVectors(const ang_t& angles, vec3_t& forward);
    void  AngleVectors(const ang_t& angles, vec3_t* forward, vec3_t* right = nullptr, vec3_t* up = nullptr);
    inline void SinCos(float radians, float* sine, float* cosine);
    void AngleVectorKidua(ang_t& vAngle, vec3_t& vForward);
    float GetFOV(const ang_t& view_angles, const vec3_t& start, const vec3_t& end);
    void  VectorTransform(const vec3_t& in, const matrix3x4_t& matrix, vec3_t& out);
    float SegmentToSegment(const vec3_t s1, const vec3_t s2, const vec3_t k1, const vec3_t k2);
    void  VectorITransform(const vec3_t& in, const matrix3x4_t& matrix, vec3_t& out);
    void  MatrixAngles(const matrix3x4_t& matrix, ang_t& angles);
    void  MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out);
    float random_float(float min, float max);
    int random_int(int min, int max);
    void random_seed(int seed);
    void fast_rsqrt(float a, float* out);
    float fast_vec_normalize(vec3_t& vec);
    void  ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out);

    // computes the intersection of a ray with a box ( AABB ).
    bool IntersectRayWithBox(const vec3_t& start, const vec3_t& delta, const vec3_t& mins, const vec3_t& maxs, float tolerance, BoxTraceInfo_t* out_info);
    bool IntersectRayWithBox(const vec3_t& start, const vec3_t& delta, const vec3_t& mins, const vec3_t& maxs, float tolerance, CBaseTrace* out_tr, float* fraction_left_solid = nullptr);

    // computes the intersection of a ray with a oriented box ( OBB ).
    bool IntersectRayWithOBB(const vec3_t& start, const vec3_t& delta, const matrix3x4_t& obb_to_world, const vec3_t& mins, const vec3_t& maxs, float tolerance, CBaseTrace* out_tr);
    bool IntersectRayWithOBB(const vec3_t& start, const vec3_t& delta, const vec3_t& box_origin, const ang_t& box_rotation, const vec3_t& mins, const vec3_t& maxs, float tolerance, CBaseTrace* out_tr);

    // returns whether or not there was an intersection of a sphere against an infinitely extending ray. 
    // returns the two intersection points.
    bool IntersectInfiniteRayWithSphere(const vec3_t& start, const vec3_t& delta, const vec3_t& sphere_center, float radius, float* out_t1, float* out_t2);

    // returns whether or not there was an intersection, also returns the two intersection points ( clamped 0.f to 1.f. ).
    // note: the point of closest approach can be found at the average t value.
    bool IntersectRayWithSphere(const vec3_t& start, const vec3_t& delta, const vec3_t& sphere_center, float radius, float* out_t1, float* out_t2);

    vec3_t Interpolate(const vec3_t from, const vec3_t to, const float percent);

    template <class T>
    __forceinline T Lerp(float flPercent, T const& A, T const& B)
    {
        return A + (B - A) * flPercent;
    }

    template < typename t >
    __forceinline void clamp(t& n, const t& lower, const t& upper) {
        n = std::max(lower, std::min(n, upper));
    }
}