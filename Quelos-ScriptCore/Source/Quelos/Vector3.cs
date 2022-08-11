using System;

namespace Quelos
{
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float x, float y, float z)
        {
            this.X = x;
            this.Y = y;
            this.Z = z;
        }

        public Vector3(float scaler)
        {
            X = scaler;
            Y = scaler;
            Z = scaler;
        }

        public Vector2 XY { get => new Vector2(X, Y); set { X = value.X; Y = value.Y; } }

        public static Vector3 Zero => new Vector3(0.0f);

        public Vector3 Normalized => this / Magnitude;

        public float Magnitude => (float)Math.Sqrt((X * X) + (Y * Y) + (Z * Z));

        public static Vector3 operator +(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }

        public static Vector3 operator *(Vector3 vector, float scaler)
        {
            return new Vector3(vector.X * scaler, vector.Y * scaler, vector.Z * scaler);
        }

        public static Vector3 operator /(Vector3 v, float i)
        {
            return new Vector3(v.X / i, v.Y / i, v.Z / i);
        }

        public override string ToString()
        {
            return $"{X}, {Y}, {Z}";
        }
    }
}
