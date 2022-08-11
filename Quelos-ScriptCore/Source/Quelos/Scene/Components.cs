namespace Quelos
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Position
        {
            get
            {
                InternalCalls.TransformComponent_GetPosition(Entity.ID, out Vector3 outPosition);
                return outPosition;
            }

            set => InternalCalls.TransformComponent_SetPosition(Entity.ID, ref value);
        }
    }

    public class Rigidbody2DComponent : Component
    {
        public void ApplyLinearImpulse(Vector2 impulse, bool wake = true)
            => InternalCalls.Rigidbody2DComponent_AddLinearImpulseToCenter(Entity.ID, ref impulse, wake);
    }
}
