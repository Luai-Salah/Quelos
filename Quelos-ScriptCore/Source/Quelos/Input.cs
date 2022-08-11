namespace Quelos
{
    public class Input
    {
        public static bool GetKey(KeyCode keyCode) => InternalCalls.Input_GetKey(keyCode);
    }
}
