using System.Runtime.CompilerServices;

namespace Quelos
{
    public struct float3
    {
        public float x, y, z;

        public float3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public float3(float scaler)
        {
            x = scaler;
            y = scaler;
            z = scaler;
        }

        public float3(float2 xy, float z)
        {
            x = xy.x;
            y = xy.y;
            this.z = z;
        }

        public float2 xy { get => new float2(x, y); set { x = value.x; y = value.y; } }

        public static float3 zero => new float3(0.0f);


        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float3 operator +(float3 a, float3 b)
        {
            return new float3(a.x + b.x, a.y + b.y, a.z + b.z);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float3 operator -(float3 a, float3 b)
        {
            return new float3(a.x - b.x, a.y - b.y, a.z - b.z);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float3 operator *(float3 vector, float scaler)
        {
            return new float3(vector.x * scaler, vector.y * scaler, vector.z * scaler);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float3 operator /(float3 v, float i)
        {
            return new float3(v.x / i, v.y / i, v.z / i);
        }

        public override string ToString()
        {
            return $"({x}, {y}, {z})";
        }
    }
}
