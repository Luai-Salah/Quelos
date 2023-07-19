using System;
using System.Runtime.CompilerServices;

namespace Quelos
{
    internal class InternalCalls
    {
        #region Entity
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_HasComponent(ulong entityID, Type Type);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong Entity_FindEntityByName(string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern object Entity_GetScriptInstance(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void NativeLog(string text);

        #endregion

        #region Transform Component

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetPosition(ulong entityID, out float3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_SetPosition(ulong entityID, ref float3 position);

        #endregion

        #region Rigidbody2D Component

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Rigidbody2D.BodyType Rigidbody2D_GetBodyType(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Rigidbody2D_SetBodyType(ulong entityID, Rigidbody2D.BodyType type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Rigidbody2D_GetLinearVelocity(ulong entityID, out float2 outVelocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Rigidbody2D_SetLinearVelocity(ulong entityID, ref float2 velocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Rigidbody2D_AddLinearImpulseToCenter(ulong entityID, ref float2 impulse, bool wake);

        #endregion

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Input_GetKey(KeyCode keyCode);
    }
}