namespace Quelos
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class Transform : Component
    {
        public float3 Position
        {
            get
            {
                InternalCalls.Transform_GetPosition(Entity.ID, out float3 outPosition);
                return outPosition;
            }

            set => InternalCalls.Transform_SetPosition(Entity.ID, ref value);
        }
    }

    public class Rigidbody2D : Component
    {
        public enum BodyType
        {
            Static = 0,
            Dynamic = 1,
            Kinematic = 2
        }

        public BodyType Type
        {
            get => InternalCalls.Rigidbody2D_GetBodyType(Entity.ID);
            set => InternalCalls.Rigidbody2D_SetBodyType(Entity.ID, value);
        }

        public float2 Velocity
        {
            get
            {
                InternalCalls.Rigidbody2D_GetLinearVelocity(Entity.ID, out float2 outVelocity);
                return outVelocity;
            }
            set
            {
                InternalCalls.Rigidbody2D_SetLinearVelocity(Entity.ID, ref value);
            }
        }

        public void ApplyLinearImpulse(float2 impulse, bool wake = true)
            => InternalCalls.Rigidbody2D_AddLinearImpulseToCenter(Entity.ID, ref impulse, wake);
    }
}
