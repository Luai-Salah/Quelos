using Quelos;

namespace Sandbox
{
    public class Camera : Entity
    {
        private float m_Speed = 10f;

        private void OnUpdate(float ts)
        {
            Vector3 velocity = Vector3.Zero;

            if (Input.GetKey(KeyCode.Up))
                velocity.Y = 1f;
            if (Input.GetKey(KeyCode.Down))
                velocity.Y = -1f;
            if (Input.GetKey(KeyCode.Right))
                velocity.X = 1f;
            if (Input.GetKey(KeyCode.Left))
                velocity.X = -1f;

            velocity *= m_Speed * ts;
            Transform.Position += velocity;
        }
    }
}
