using Quelos;

namespace Sandbox
{
    public class Player : Entity
    {
        public float Speed = 20f;
        public float Time;

        private Rigidbody2DComponent m_Rigidbody;

        private void OnStart()
        {
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
        }

        private void OnUpdate(float ts)
        {
            Vector2 velocity = Vector2.Zero;

            if (Input.GetKey(KeyCode.D))
                velocity.X = 1f;
            if (Input.GetKey(KeyCode.A))
                velocity.X = -1f;

            velocity *= Speed * ts;
            m_Rigidbody.ApplyLinearImpulse(velocity);

            Time += ts; 
        }
    }
}
