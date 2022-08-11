using System;

namespace Quelos
{
    public struct Vector2
    {
        public float X, Y;

        public Vector2(float x, float y)
        {
            this.X = x;
            this.Y = y;
        }

        public Vector2(float scaler)
        {
            X = scaler;
            Y = scaler;
        }

        public static Vector2 Zero => new Vector2(0.0f);

        public Vector2 Normalized => this / Magnitude;

        public float Magnitude => (float)Math.Sqrt((X * X) + (Y * Y));

        public static Vector2 operator +(Vector2 a, Vector2 b)
        {
            return new Vector2(a.X + b.X, a.Y + b.Y);
        }

        public static Vector2 operator *(Vector2 vector, float scaler)
        {
            return new Vector2(vector.X * scaler, vector.Y * scaler);
        }

        public static Vector2 operator /(Vector2 v, float i)
        {
            return new Vector2(v.X / i, v.Y / i);
        }

        public override string ToString()
        {
            return $"{X}, {Y}";
        }
    }
}
