using Quelos;

namespace Sandbox
{
    public class Player : Entity
    {
        private float m_Speed = 20f;

        private Rigidbody2DComponent m_Rigidbody;

        private void OnStart()
        {
            InternalCalls.NativeLog($"Player.OnCreate - {ID}");
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
        }

        private void OnUpdate(float ts)
        {
            Vector2 velocity = Vector2.Zero;

            if (Input.GetKey(KeyCode.D))
                velocity.X = 1f;
            if (Input.GetKey(KeyCode.A))
                velocity.X = -1f;

            velocity *= m_Speed * ts;
            m_Rigidbody.ApplyLinearImpulse(velocity);
        }
    }
}
