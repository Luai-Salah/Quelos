using System;
using System.Runtime.CompilerServices;

namespace Quelos.Mathematics
{
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Style", "IDE1006:Naming Styles", Justification = "<Pending>")]
    public static class math
    {
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float lensq(float2 v)
        {
            return (v.x * v.x) + (v.y * v.y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float lensq(float3 v)
        {
            return (v.x * v.x) + (v.y * v.y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float len(float2 v)
        {
            return (float)Math.Sqrt(lensq(v));
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float len(float3 v)
        {
            return (float)Math.Sqrt(lensq(v));
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float2 normalize(float2 v)
        {
            return v / len(v);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float3 normalize(float3 v)
        {
            return v / len(v);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float lerp(float a, float b, float t)
        {
            return a * (1 - t) + (b * t);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float2 lerp(float2 a, float2 b, float t)
        {
            return a * (1f - t) + (b * t);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float3 lerp(float3 a, float3 b, float t)
        {
            return a * (1f - t) + (b * t);
        }
    }
}
