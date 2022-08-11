using System;
using System.Runtime.CompilerServices;

namespace Quelos
{
    internal class InternalCalls
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_HasComponent(ulong entityID, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void NativeLog(string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void TransformComponent_GetPosition(ulong entityID, out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void TransformComponent_SetPosition(ulong entityID, ref Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Rigidbody2DComponent_AddLinearImpulseToCenter
            (ulong entityID, ref Vector2 impulse, bool wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Input_GetKey(KeyCode keyCode);
    }
}