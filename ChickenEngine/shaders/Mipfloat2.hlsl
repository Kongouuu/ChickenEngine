#define BLOCK_SIZE 8

#define WIDTH_HEIGHT_EVEN 0     // Both the width and the height of the texture are even.
#define WIDTH_ODD_HEIGHT_EVEN 1 // The texture width is odd and the height is even.
#define WIDTH_EVEN_HEIGHT_ODD 2 // The texture width is even and the height is odd.
#define WIDTH_HEIGHT_ODD 3      // Both the width and height of the texture are odd.


struct ComputeShaderInput
{
    uint3 GroupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  GroupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

cbuffer GenerateMipsCB : register(b0)
{
    uint SrcMipLevel; // Texture level of source mip
    uint NumMipLevels; // Number of OutMips to write: [1-4]
    uint SrcDimension;  // Width and height of the source texture are even or odd.
    float TexelSizeX; // 1.0 / OutMip1.Dimensions
    float TexelSizeY; // 1.0 / OutMip1.Dimensions
}

// Source mip map.
Texture2D<float2> SrcMip : register(t0);

// Write up to 4 mip map levels.
RWTexture2D<float2> OutMip1 : register(u0);  //
RWTexture2D<float2> OutMip2 : register(u1);  //
RWTexture2D<float2> OutMip3 : register(u2);  //
RWTexture2D<float2> OutMip4 : register(u3);  //

// Linear clamp sampler.
SamplerState gSamLinearClamp : register(s3);

// The reason for separating channels is to reduce bank conflicts in the
// local data memory controller.  A large stride will cause more threads
// to collide on the same memory bank.
groupshared float gs_R[64];
groupshared float gs_G[64];

void StoreColor(uint Index, float2 Color)
{
    gs_R[Index] = Color.r;
    gs_G[Index] = Color.g;
}

float2 LoadColor(uint Index)
{
    return float2(gs_R[Index], gs_G[Index]);
}

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CS(ComputeShaderInput IN)
{
    float2 Src1 = (float2)0;
    float2 UV = float2(TexelSizeX, TexelSizeY);
    UV = UV *(IN.DispatchThreadID.xy + 0.5);

    Src1 = SrcMip.SampleLevel(gSamLinearClamp, UV, SrcMipLevel);
    OutMip1[IN.DispatchThreadID.xy] = Src1;

    // A scalar (constant) branch can exit all threads coherently.
    if (NumMipLevels == 1)
        return;

    // Without lane swizzle operations, the only way to share data with other
    // threads is through LDS.
    StoreColor(IN.GroupIndex, Src1);

    // This guarantees all LDS writes are complete and that all threads have
    // executed all instructions so far (and therefore have issued their LDS
    // write instructions.)
    GroupMemoryBarrierWithGroupSync();

    //001001
    if ((IN.GroupIndex & 0x9) == 0)
    {
        float2 Src2 = LoadColor(IN.GroupIndex + 0x01);
        float2 Src3 = LoadColor(IN.GroupIndex + 0x08);
        float2 Src4 = LoadColor(IN.GroupIndex + 0x09);
        Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

        OutMip2[IN.DispatchThreadID.xy / 2] = Src1;
        StoreColor(IN.GroupIndex, Src1);
    }

    if (NumMipLevels == 2)
        return;

    GroupMemoryBarrierWithGroupSync();

    // This bit mask (binary: 011011) checks that X and Y are multiples of four.
    if ((IN.GroupIndex & 0x1B) == 0)
    {
        float2 Src2 = LoadColor(IN.GroupIndex + 0x02);
        float2 Src3 = LoadColor(IN.GroupIndex + 0x10);
        float2 Src4 = LoadColor(IN.GroupIndex + 0x12);
        Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

        OutMip3[IN.DispatchThreadID.xy / 4] = Src1;
        StoreColor(IN.GroupIndex, Src1);
    }


    if (NumMipLevels == 3)
        return;

    GroupMemoryBarrierWithGroupSync();
    // This bit mask would be 111111 (X & Y multiples of 8), but only one
   // thread fits that criteria.
    if (IN.GroupIndex == 0)
    {
        float2 Src2 = LoadColor(IN.GroupIndex + 0x04);
        float2 Src3 = LoadColor(IN.GroupIndex + 0x20);
        float2 Src4 = LoadColor(IN.GroupIndex + 0x24);
        Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

        OutMip4[IN.DispatchThreadID.xy / 8] = Src1;
    }
}
