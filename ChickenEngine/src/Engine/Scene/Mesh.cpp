#include "pch.h"
#include "Engine/Scene/Mesh.h"

namespace ChickenEngine
{
    Mesh MeshManager::GenerateBox()
    {
        LOG_INFO("MeshManager - Generate box");
        Mesh boxMesh;
        boxMesh.vertices = std::vector<Vertex>(24);
       
        // front
        boxMesh.vertices[0]  = Vertex({ -5.0, 5.0, -5.0 }, { 0.0,0.0,-1.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 }); // left up
        boxMesh.vertices[1]  = Vertex({ 5.0, 5.0, -5.0 },  { 0.0,0.0,-1.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 });  // right up
        boxMesh.vertices[2]  = Vertex({ -5.0, -5.0, -5.0 },{ 0.0,0.0,-1.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 }); // left down
        boxMesh.vertices[3]  = Vertex({ 5.0, -5.0, -5.0 }, { 0.0,0.0,-1.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 }); // right down

        // left
        boxMesh.vertices[4]  = Vertex({ -5.0, 5.0, 5.0 },  { -1.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 });
        boxMesh.vertices[5]  = Vertex({ -5.0, 5.0, -5.0 }, { -1.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 });
        boxMesh.vertices[6]  = Vertex({ -5.0, -5.0, 5.0 }, { -1.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 });
        boxMesh.vertices[7]  = Vertex({ -5.0, -5.0, -5.0 },{ -1.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 });

        // right
        boxMesh.vertices[8]  = Vertex({ 5.0, 5.0, -5.0 },  { 1.0,0.0,0.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 });
        boxMesh.vertices[9]  = Vertex({ 5.0, 5.0, 5.0 },   { 1.0,0.0,0.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 });
        boxMesh.vertices[10] = Vertex({ 5.0, -5.0, -5.0 }, { 1.0,0.0,0.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 });
        boxMesh.vertices[11] = Vertex({ 5.0, -5.0, 5.0 },  { 1.0,0.0,0.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 });

        // back
        boxMesh.vertices[12] = Vertex({ 5.0, 5.0, 5.0 },  { 0.0,0.0,1.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 }); 
        boxMesh.vertices[13] = Vertex({ -5.0, 5.0, 5.0 }, { 0.0,0.0,1.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 }); 
        boxMesh.vertices[14] = Vertex({ 5.0, -5.0, 5.0 }, { 0.0,0.0,1.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 }); 
        boxMesh.vertices[15] = Vertex({ -5.0, -5.0, 5.0 },{ 0.0,0.0,1.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 }); 

        // top
        boxMesh.vertices[16] = Vertex({ -5.0, 5.0, 5.0 },  { 0.0,1.0,0.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 }); 
        boxMesh.vertices[17] = Vertex({ 5.0, 5.0, 5.0 },   { 0.0,1.0,0.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 });  
        boxMesh.vertices[18] = Vertex({ -5.0, 5.0, -5.0 }, { 0.0,1.0,0.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 }); 
        boxMesh.vertices[19] = Vertex({ 5.0, 5.0, -5.0 },  { 0.0,1.0,0.0 },  { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 });

        // bottom
        boxMesh.vertices[20] = Vertex({ -5.0, -5.0, 5.0 }, { 0.0,-1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 });
        boxMesh.vertices[21] = Vertex({ -5.0, -5.0, -5.0 },  { 0.0,-1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 });
        boxMesh.vertices[22] = Vertex({ 5.0, -5.0, 5.0 },{ 0.0,-1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 });
        boxMesh.vertices[23] = Vertex({ 5.0,- 5.0, -5.0 }, { 0.0,-1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 });



        uint32_t i[36];

        i[0] = 0; i[1] = 1; i[2] = 2;
        i[3] = 2; i[4] = 1; i[5] = 3;

        i[6] = 4; i[7] = 5; i[8] = 6;
        i[9] = 6; i[10] = 5; i[11] = 7;

        i[12] = 8; i[13] = 9; i[14] = 10;
        i[15] = 10; i[16] = 9; i[17] = 11;

        i[18] = 12; i[19] = 13; i[20] = 14;
        i[21] = 14; i[22] = 13; i[23] = 15;

        i[24] = 16; i[25] = 17; i[26] = 18;
        i[27] = 18; i[28] = 17; i[29] = 19;

        i[30] = 20; i[31] = 21; i[32] = 22;
        i[33] = 22; i[34] = 21; i[35] = 23;
        boxMesh.indices.assign(&i[0], &i[36]);

        return boxMesh;
    }

    Mesh MeshManager::GeneratePlane()
    {
        LOG_INFO("MeshManager - Generate plane");
        Mesh planeMesh;
        planeMesh.vertices = std::vector<Vertex>(8);

        // top
        planeMesh.vertices[0] = Vertex({ -5.0, 0.0, 5.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 });
        planeMesh.vertices[1] = Vertex({ 5.0, 0.0, 5.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 });
        planeMesh.vertices[2] = Vertex({ -5.0, 0.0, -5.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 });
        planeMesh.vertices[3] = Vertex({ 5.0, 0.0, -5.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 });

        // bottom
        planeMesh.vertices[4] = Vertex({ -5.0, 0.0, 5.0 }, { 0.0,-1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 });
        planeMesh.vertices[5] = Vertex({ -5.0, 0.0, -5.0 }, { 0.0,-1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 });
        planeMesh.vertices[6] = Vertex({ 5.0, 0.0, 5.0 }, { 0.0,-1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 });
        planeMesh.vertices[7] = Vertex({ 5.0,0.0, -5.0 }, { 0.0,-1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 });



        uint32_t i[12];

        i[0] = 0; i[1] = 1; i[2] = 2;
        i[3] = 2; i[4] = 1; i[5] = 3;

        i[6] = 4; i[7] = 5; i[8] = 6;
        i[9] = 6; i[10] = 5; i[11] = 7;

        planeMesh.indices.assign(&i[0], &i[12]);

        return planeMesh;
    }

    Mesh MeshManager::GenerateDebugPlane()
    {
        LOG_INFO("MeshManager - Generate debug plane");
        Mesh planeMesh;
        planeMesh.vertices = std::vector<Vertex>(4);

        planeMesh.vertices[0] = Vertex({ 0.5, -0.5, 0.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0 });
        planeMesh.vertices[1] = Vertex({ 1.0, -0.5, 0.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,0.0 });
        planeMesh.vertices[2] = Vertex({ 0.5, -1.0, 0.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,1.0 });
        planeMesh.vertices[3] = Vertex({ 1.0, -1.0, -5.0 }, { 0.0,1.0,0.0 }, { 0.0,0.0,0.0 }, { 0.0,0.0,0.0 }, { 1.0,1.0 });

        uint32_t i[6];

        i[0] = 0; i[1] = 1; i[2] = 2;
        i[3] = 2; i[4] = 1; i[5] = 3;

        planeMesh.indices.assign(&i[0], &i[6]);
        return planeMesh;
    }

    void Mesh::AddDiffuseTexture(Texture tex)
    {
        diffuseMap = tex;
        diffuseMap.type = ETextureType::DIFFUSE;
    }

}
