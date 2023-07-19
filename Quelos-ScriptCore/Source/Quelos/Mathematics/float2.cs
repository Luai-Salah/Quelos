using System.Runtime.CompilerServices;

namespace Quelos
{
    public struct float2
    {
        public float x, y;

        public float2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        public float2(float scaler)
        {
            x = scaler;
            y = scaler;
        }

        public static float2 zero => new float2(0.0f);

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float2 operator +(float2 a, float2 b)
        {
            return new float2(a.x + b.x, a.y + b.y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float2 operator -(float2 a, float2 b)
        {
            return new float2(a.x - b.x, a.y - b.y);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float2 operator *(float2 vector, float scaler)
        {
            return new float2(vector.x * scaler, vector.y * scaler);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float2 operator /(float2 v, float i)
        {
            return new float2(v.x / i, v.y / i);
        }

        public override string ToString()
        {
            return $"{x}, {y}";
        }
    }
}
