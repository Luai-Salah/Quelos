using Quelos;

namespace Sandbox
{
    public class Camera : Entity
    {
        public float DistanceFromPlayer = 10f;
        
        private Player m_Player;

        private void OnStart() => m_Player = FindEntityByName("Player").As<Player>();

        private void OnUpdate(float _)
        {
            if (!m_Player)
            {
                print("Player is null! trying to find the entity...");
                m_Player = FindEntityByName("Player").As<Player>();
                return;
            }

            Transform.Position = new float3(m_Player.Position.xy, Transform.Position.z);
        }
    }
}
