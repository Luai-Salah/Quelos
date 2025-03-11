using Quelos;

namespace Sandbox
{
    public class Player : Entity
    {
        public float Speed = 20f;
        public float Time;

        private Rigidbody2D m_Rigidbody;

        private void OnStart()
        {
            m_Rigidbody = GetComponent<Rigidbody2D>();
        }

        private void OnUpdate(float ts)
        {
            float2 velocity = float2.zero;

            if (Input.GetKey(KeyCode.D))
                velocity.x = 1f;
            if (Input.GetKey(KeyCode.A))
                velocity.x = -1f;

            velocity *= Speed * ts;
            m_Rigidbody.ApplyLinearImpulse(velocity);

            Time += ts; 
        }
    }
}
