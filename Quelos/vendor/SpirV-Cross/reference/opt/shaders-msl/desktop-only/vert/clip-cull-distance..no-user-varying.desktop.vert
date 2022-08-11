#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct main0_out
{
    float4 gl_Position [[position]];
    float gl_ClipDistance [[clip_distance]] [2];
};

vertex main0_out main0()
{
    main0_out out = {};
    out.gl_Position = float4(10.0);
    out.gl_ClipDistance[0] = 1.0;
    out.gl_ClipDistance[1] = 4.0;
    return out;
}

