using System;

namespace Quelos
{
    public class Entity
    {
        public readonly ulong ID;

        protected Entity() => ID = 0;

        internal Entity(ulong id)
        {
            ID = id;
            Transform = new TransformComponent { Entity = this };
        }

        public TransformComponent Transform { get; }

        public Vector3 Position
        {
            get
            {
                InternalCalls.TransformComponent_GetPosition(ID, out Vector3 position);
                return position;
            }

            set => InternalCalls.TransformComponent_SetPosition(ID, ref value);
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (!HasComponent<T>())
                return null;

            return new T { Entity = this };
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            Type componentType = typeof(T);
            return InternalCalls.Entity_HasComponent(ID, componentType);
        }

        public void Log(object text) => InternalCalls.NativeLog(text.ToString());
    }
}
