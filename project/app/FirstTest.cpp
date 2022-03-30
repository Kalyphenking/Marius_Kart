
#include "yoshix_fix_function.h"
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <winuser.h>
#include <cstdlib>
#include <cstdio>
#include <conio.h>
#include <string.h>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <vector>
#include <future>
#include <thread>
#include <chrono>
#include <functional>

using namespace gfx;

#define PI 3.14159265

namespace
{
    class CApplication : public IApplication
    {
    public:

        CApplication();
        virtual ~CApplication();

    private:

        float   m_FieldOfViewY;     // Vertical view angle of the camera

        BHandle m_pTriangleMesh;    // A pointer to a YoshiX mesh, which represents a single triangle.
        BHandle m_pFloorleMesh;
        BHandle m_pCubeMesh;
        BHandle m_pTestImportMesh;
        BHandle m_testTexture;
        



    private:

        virtual bool InternOnStartup();
        virtual bool InternOnCreateMeshes();
        virtual bool InternOnReleaseMeshes();
        virtual bool InternOnCreateTextures();
        virtual bool InternOnReleaseTextures();
        virtual bool InternOnResize(int _Width, int _Height);
        virtual bool InternOnKeyEvent(unsigned int _Key, bool _IsKeyDown, bool _IsAltDown);
        virtual bool InternOnMouseEvent(int _X, int _Y, int _Button, bool _IsButtonDown, bool _IsDoubleClick, int _WheelDelta);
        virtual bool InternOnUpdate();
        virtual bool InternOnFrame();
        bool LoadFromObjectFile(std::string sFilename, BHandle* _ppMesh);
        bool testFun(int a, int b);
    };

    
} // namespace

namespace
{
    CApplication::CApplication()
        : m_FieldOfViewY(60.0f)    // Set the vertical view angle of the camera to 60 degrees.
        , m_pTriangleMesh(nullptr)
        , m_pFloorleMesh(nullptr)
        , m_pTestImportMesh(nullptr)
        , m_pCubeMesh(nullptr)
        , m_testTexture(nullptr)
    {
    }

    POINT p = { 0,0 };
    POINT prevP = { 0,0 };
    POINT deltaP = { 0,0 };
    bool firstRunDone = false;
    int delay = 0;
    float posX = 0.0f;
    float posY = 0.0f;
    float sensitivity = 0.05f;
    bool mouseIsFree = false;
    int windowXDisplacement = -8;
    int windowYDisplaycement = 0;
    float scrollOffset = 0.0f;

    LPCTSTR WindowName = L"YoshiX-Example";

    HWND window;
    RECT rect = { 0 };

    // -----------------------------------------------------------------------------

    CApplication::~CApplication()
    {
    }

    

   bool fileLoaded = false;
   bool fileMeshCreated = false;

  

    bool CApplication::LoadFromObjectFile(std::string sFilename, BHandle* _ppMesh) {

        
        struct vec4d
        {
            float x, y, z, a;
        };




        std::fstream f(sFilename);

       /* std::vector<vec4d> ververticests;
        std::vector<vec4d> normals;
        std::vector<vec4d> indices;
        std::vector<vec4d> textureCoords;*/

        std::string textureFileName = "";

        BHandle s_texture = nullptr;

        bool fileExists = false;
        bool colorsDefined = false;
        bool textureDefined = false;
        bool textureExists = false;

        bool verticesDone = false;
        bool indicesDone = false;
        bool normalsDone = false;
        bool textureCoordsDone = false;

        int count = 0;
        int numberOfVertices = 0;
        int numberOfNormals = 0;
        int numberOfIndices = 0;
        int numberOfTextureCoords = 0;

        int verticesStartIndex = 0;
        int normalsStartIndex = 0;
        int indicesStartIndex = 0;
        int colorStartIndex = 0;
        int textureNameStartIndex = 0;
        int textureCoordsStartIndex = 0;

        const int maxNumberOfVertices = 30720;
        const int maxNumberOfNormals = 30720;
        const int maxNumberOfColors = 30720;
        const int maxNumberOfindices = 204800;
        const int maxNumberOfTexCoords = 204800;

        auto s_Vertices = new float[maxNumberOfVertices][3];
        auto s_Normals = new float[maxNumberOfNormals][3];
        auto s_Colors = new float[maxNumberOfColors][4];
        auto s_Indices = new int[maxNumberOfindices][3];
        auto s_TextureCoords = new float[maxNumberOfTexCoords][2];



        

        int arrayIndex = 0;

        if (!f.is_open()) {
            std::cout << "File does not exist\n";
            return false;
        }
        else {
            std::cout << "File exists\n";
            fileExists = true;
        }


        while (!f.eof() && fileExists) {
            char line[256];
            f.getline(line, 256);

            std::strstream s;
            s << line;

            char junk;

            std::string string;
            //std::cout << "line: " << line << "\n";

            string = line;

            if (string.find("\n") != std::string::npos) {
                std::cout << "line: " << line << "\n";
            }

            if (string.find("Mesh CINEMA4D_Mesh {") != std::string::npos) {
                verticesStartIndex = count + 3;
            }

            if (string.find("MeshNormals {") != std::string::npos) {
                normalsStartIndex = count + 3;
            }

            if (string.find("MeshTextureCoords {") != std::string::npos) {
                textureCoordsStartIndex = count + 3;
            }

            if (string.find("Mat {") != std::string::npos) {
                colorStartIndex = count + 1;
                colorsDefined = true;
                std::cout << "Material defined: " << std::endl;
            }

            if (string.find("TextureFilename {") != std::string::npos) {
                textureNameStartIndex = count + 1;
                textureDefined = true;
                std::cout << "texture defined: " << std::endl;
            }



            if (count == (verticesStartIndex - 2)) {
                s >> numberOfVertices;

                if (numberOfVertices > maxNumberOfVertices) {
                    std::cout << "numberOfVertices > maxNumberOfVertices: " << numberOfVertices << std::endl;
                    return false;
                }
            }

            if (count == (textureCoordsStartIndex - 2)) {
                s >> numberOfTextureCoords;

                if (numberOfTextureCoords > maxNumberOfTexCoords) {
                    std::cout << "numberOfTextureCoords > maxNumberOfTexCoords: " << numberOfVertices << std::endl;
                    return false;
                }
            }

            if (count == indicesStartIndex - 2) {
                s >> numberOfIndices;
                numberOfIndices *= 3;

                if (numberOfIndices > maxNumberOfindices) {
                    std::cout << "numberOfIndices > maxNumberOfindices: " << numberOfIndices << std::endl;
                    return false;
                }
            }

            if (count == normalsStartIndex - 2) {
                s >> numberOfNormals;

                if (numberOfNormals > maxNumberOfNormals) {
                    std::cout << "numberOfNormals > maxNumberOfNormals: " << numberOfIndices << std::endl;
                    return false;
                }
            }



            if (strlen(line) == 0 && verticesStartIndex != 0 && normalsStartIndex == 0 && count > (verticesStartIndex + numberOfVertices - 1)) {
                indicesStartIndex = count + 3;
                //verticesDone = true;
            }


            if (strlen(line) != 0 && verticesStartIndex != 0 && count >= verticesStartIndex && count <= (verticesStartIndex + numberOfVertices - 1)) {
                vec4d v;
                //std::cout << "verticesStartIndex: " << verticesStartIndex << "; Index: " << count << "; endIndex: " << (verticesStartIndex + numberOfVertices - 1) << "\n";
                //std::cout << "vertices: " << line << "\n";
                s >> v.x >> junk >> v.y >> junk >> v.z;

                arrayIndex = count - verticesStartIndex;

                s_Vertices[arrayIndex][0] = v.x;
                s_Vertices[arrayIndex][1] = v.y;
                s_Vertices[arrayIndex][2] = v.z;
            }


            if (strlen(line) != 0 && normalsStartIndex != 0  && count >= normalsStartIndex && count <= (normalsStartIndex + numberOfNormals - 1)) {
                vec4d v;
                //std::cout << "StartIndex: " << normalsStartIndex << "; Index: " << count << "; endIndex: " << (normalsStartIndex + numberOfNormals - 1) << "\n";
                //std::cout << "normals: " << line << "\n";
                s >> v.x >> junk >> v.y >> junk >> v.z;
                
                //std::cout << "v.x: " << v.x << "; v.y: " << v.y << "; v.z: " << v.z << "\n";

                arrayIndex = count - normalsStartIndex;

                s_Normals[arrayIndex][0] = v.x;
                s_Normals[arrayIndex][1] = v.y;
                s_Normals[arrayIndex][2] = v.z;
            }


            if (strlen(line) != 0 && indicesStartIndex != 0 && count >= indicesStartIndex && count <= (indicesStartIndex + (numberOfIndices / 3) - 1)) {
                vec4d v;
                //std::cout << "indicesStartIndex: " << indicesStartIndex << "; Index: " << count << "; endIndex: " << (indicesStartIndex + (numberOfIndices / 3) - 1) << "\n";
                //std::cout << "indices: " << line << "\n";
                s >> junk >> junk >> v.x >> junk >> v.y >> junk >> v.z;

                //std::cout << "v.x: " << v.x << "; v.y: " << v.y << "; v.z: " << v.z << "\n";

                arrayIndex = count - indicesStartIndex;

                s_Indices[arrayIndex][0] = v.x;
                s_Indices[arrayIndex][1] = v.z;
                s_Indices[arrayIndex][2] = v.y;
            }

            if (strlen(line) != 0 && textureCoordsStartIndex != 0 && count >= textureCoordsStartIndex && count <= (textureCoordsStartIndex + numberOfTextureCoords - 1)) {
                vec4d v;
                //std::cout << "StartIndex: " << normalsStartIndex << "; Index: " << count << "; endIndex: " << (normalsStartIndex + numberOfNormals - 1) << "\n";
                //std::cout << "normals: " << line << "\n";
                s >> v.x >> junk >> v.y;

                arrayIndex = count - textureCoordsStartIndex;
                int invertedArrayIndex = numberOfTextureCoords - (count - textureCoordsStartIndex);


                s_TextureCoords[arrayIndex][0] = v.x;
                s_TextureCoords[arrayIndex][1] = v.y;
            }

            if (strlen(line) != 0 && count == colorStartIndex && colorsDefined) {
                vec4d v;

                s >> v.x >> junk >> v.y >> junk >> v.z >> junk >> v.a;

                for (int index = 0; index < numberOfVertices; index++) {
                    s_Colors[index][0] = v.x;
                    s_Colors[index][1] = v.y;
                    s_Colors[index][2] = v.z;
                    s_Colors[index][3] = v.a;
                }
            }

            

            if (count == textureNameStartIndex && textureDefined) {
                s >> textureFileName;
            }

            count++;
        }

        
        
        if (fileExists) {


            for (int index = 0; index < numberOfTextureCoords; index++) {
                std::cout << "s_TextureCoords: " << s_TextureCoords[index][0] << "; " << s_TextureCoords[index][1] << "\n";
            }

            
            if (textureDefined) {
                std::string textureRootDirectory = "../project/src/textures/";
                //char* texturePath = textureRootDirectory.c_str() + textureFileName.c_str();

                textureFileName = textureFileName.substr(1, (textureFileName.length() - 3));

                std::string texturePath = textureRootDirectory + textureFileName;


                std::fstream textureFile(texturePath);
                std::cout << "texturePath: " << texturePath << std::endl;
                //std::cout << "texturePath: " << texturePath.c_str() << std::endl;


                
                if (textureFile.is_open()) {
                    textureExists = true;
                  
                    textureFile.close();
                    std::cout << "texture exists: " << s_texture << std::endl;
                    CreateTexture(texturePath.c_str() , &s_texture);
                    /*CreateTexture("..\\project\\src\\textures\\Mauer.dds", &s_texture);*/
                    //CreateTexture(texturePath, &s_texture);
                    //CreateTexture("..\\project\\src\\textures\\Mauer.dds", &m_testTexture);
                    //CreateTexture("..\\data\\images\\sky.dds", &m_testTexture);
                }
                else {
                    std::cout << "texture not exists: " << std::endl;
                }
            }

           

            SMeshInfo MeshInfo;

            MeshInfo.m_pVertices = &s_Vertices[0][0];
            MeshInfo.m_pNormals = &s_Normals[0][0];
            MeshInfo.m_pColors = colorsDefined && !textureExists ? &s_Colors[0][0] : nullptr;
            MeshInfo.m_pTexCoords = textureExists ? &s_TextureCoords[0][0] : nullptr;
            MeshInfo.m_NumberOfVertices = numberOfVertices;
            MeshInfo.m_NumberOfIndices = numberOfIndices;
            MeshInfo.m_pIndices = &s_Indices[0][0];
            MeshInfo.m_pTexture = textureExists ? s_texture : nullptr;
            //MeshInfo.m_pTexture = nullptr;

            CreateMesh(MeshInfo, _ppMesh);

            //std::cout << "m_pTestImportMesh: " << m_pTestImportMesh << "\n";
            //std::cout << "_ppMesh " << *_ppMesh << "\n";
            //CreateMesh(MeshInfo, &m_pTestImportMesh);
           
            //std::cout << "m_pTestImportMesh: " << m_pTestImportMesh << "\n";
            //std::cout << "_ppMesh: " << *_ppMesh << "\n";
        }
        
        delete[] s_Vertices;
        delete[] s_Normals;
        delete[] s_Colors;
        delete[] s_Indices;
        delete[] s_TextureCoords;

        return true;
    }

    bool test() {

        return true;
    }

    bool bufferedFileLooader(int a, int b) {
        return true; 
    }

    bool testLoad(std::string sFilename, BHandle* _ppMesh) {
        return true;
    }

    bool CApplication::testFun(int a, int b) {
        return true;
    }

    bool CApplication::InternOnCreateTextures()
    {
        std::cout << "InternOnCreateTextures\n";
        // -----------------------------------------------------------------------------
        // Load an image from the given path and create a YoshiX texture representing
        // the image.
        // -----------------------------------------------------------------------------
        //CreateTexture("..\\project\\src\\textures\\Mauer.dds", &m_testTexture);
        //CreateTexture("..\\data\\images\\sky.dds", &m_testTexture);

        return true;
    }

    bool CApplication::InternOnStartup()
    {
        std::cout << "startUp\n";

        //LoadFromObjectFile("../project/src/models/spitz.x");
        //LoadFromObjectFile("../project/src/models/Blender_Plane_Triangulate.x");
        /*LoadFromObjectFile("../project/src/models/C4D_Plane_Triangulate.x");*/
        //LoadFromObjectFile("../project/src/models/C4D-Test_Cube_Z.x", &m_pTestImportMesh);
        //LoadFromObjectFile("../project/src/models/KartExportTest.x", &m_pTestImportMesh);
        //LoadFromObjectFile("../project/src/models/Kugel_64.x", &m_pTestImportMesh);


        //std::string fileName = "../project/src/models/spitz.x";
        //std::string fileName = "../project/src/models/Kugel_64.x";

        //std::string fileName = "../project/src/models/Kart_Test.x";
        //std::string fileName = "../project/src/models/CollorTest.x";

        /*std::string fileName = "../project/src/models/C4D-Test_Cube_Z.x";*/

        //std::string fileName = "../project/src/models/cubeTextureTest.x";
        //std::string fileName = "../project/src/models/TextureTestCube.x";
        //std::string fileName = "../project/src/models/TextureTest.x";
        //std::string fileName = "../project/src/models/TextureTestSphere.x";
        std::string fileName = "../project/src/models/Track.x";
        

        


        //BHandle refMesh = std::ref(m_pTestImportMesh);
        //std::string refPath = std::ref("../project/src/models/spitz.x");

        //std::future<bool> backgroundThread = async(std::launch::async, &CApplication::LoadFromObjectFile, this, refPath, &refMesh);
        std::future<bool> backgroundThread = async(std::launch::async, &CApplication::LoadFromObjectFile, this, fileName, &m_pTestImportMesh);

        std::future_status status;   
        
        int count = 0;

        while (true) {
            //std::cout << "loading...: " << count << std::endl;
            status = backgroundThread.wait_for(std::chrono::microseconds(1));

            if (status == std::future_status::ready) {
               

                //fileMeshCreated = true;
                fileLoaded = backgroundThread.get();
                std::cout << "file loaded: " << fileLoaded << "\n";

                //if (backgroundThread.get() == 1) {
                //    //fileMeshCreated = true;
                //    std::cout << "backgroundThread\n";
                //}
                //std::cout << "fileMeshCreated: " << fileMeshCreated << "\n";
                //fileMeshCreated = true;
                /*std::cout << "fileMeshCreated: " << fileMeshCreated << "\n";*/
                break;
            }
            count ++;
        }

        //fileLoaded = true;
        //fileMeshCreated = true;

        // -----------------------------------------------------------------------------
        // Define the background color of the window. Colors are always 4D tuples,
        // whereas the components of the tuple represent the red, green, blue, and alpha 
        // channel. The alpha channel indicates the transparency of the color. A value
        // of 1 means the color is completely opaque. A value of 0 means the color is
        // completely transparent. The channels red, green, and blue also are values
        // 0..1 with 0 indicating the minimum intensity and 1 the maximum intensity.
        // -----------------------------------------------------------------------------
        float ClearColor[4] = {  0.1f, 0.1f, 0.1f, 1.0f, };

        SetClearColor(ClearColor);

        // -----------------------------------------------------------------------------
        // Define the position of the light source in 3D space. The fixed function 
        // pipeline of YoshiX only supports one light source.
        // -----------------------------------------------------------------------------
        float LightPosition[3] = {0.0f, 10.0f, 0.0f, };

        SetLightPosition(LightPosition);
        
        
        // -----------------------------------------------------------------------------
        // Define the ambient, diffuse, and specular color of the light source. 
        // -----------------------------------------------------------------------------
        //float LightAmbientColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f, };
        //float LightDiffuseColor[4] = { 0.6f, 0.6f, 0.6f, 1.0f, };
        //float LightSpecularColor[4] = { 0.9f, 0.9f, 0.9f, 1.0f, };

        float LightAmbientColor[4] = { 0.3, 0.3, 0.3, 1.0f, };
        float LightDiffuseColor[4] = { 0.6f, 0.6f, 0.6f, 1.0f, };
        float LightSpecularColor[4] = { 0.5, 0.5, 0.5, 1.0f, };


        SetLightColor(LightAmbientColor, LightDiffuseColor, LightSpecularColor, 200);
        std::cout << "Startup \n";
        return true;
    }

    // -----------------------------------------------------------------------------

    bool CApplication::InternOnCreateMeshes()
    {
        std::cout << "InternOnCreateMeshes\n";
        // -----------------------------------------------------------------------------
        // Define the vertices of the mesh and their attributes.
        // -----------------------------------------------------------------------------

        static float s_TriangleVertices[][3] =
        {
            { -2.0f,  -2.0f, 1.0f, },
            { 2.0f,  -2.0f, 1.0f, },
            { 0.0f,  2.0f, 1.0f, },
            { 0.0f,  -2.0f, -1.0f, },
        };

        static float s_TriangleNormals[][3] =
        {
            { 0.0f, 0.0f, -1.0f, },             // Normal of vertex 0.
            { 0.0f, 0.0f, -1.0f, },             // Normal of vertex 1.
            { 0.0f, 0.0f, -1.0f, },             // Normal of vertex 2.
        };

        static float s_TriangleColors[][4] =
        {
            { 1.0f, 0.0f, 0.0f, 1.0f, },        // Color of vertex 0.
            { 0.0f, 1.0f, 0.0f, 1.0f, },        // Color of vertex 1.
            { 0.0f, 0.0f, 1.0f, 1.0f, },        // Color of vertex 2.
            { 1.0f, 1.0f, 0.0f, 1.0f, },
        };

        static int s_TriangleIndices[][3] =
        {
            { 2, 1, 0, },
            { 0, 3, 2, },
            { 3, 1, 2, },
            { 0, 1, 3, },
        };

        static float s_FloorVertices[][3] =
        {
            { -2.0f,  0.0f, -2.0f, },
            { 2.0f,  -0.0f, -2.0f, },
            { 2.0f,  0.0f, 2.0f, },
            { -2.0f,  -0.0f, 2.0f, },
        };

        static float s_FloorNormals[][3] =
        {
            { 0.0f, 1.0f, 0.0f, },             // Normal of vertex 0.
            { 0.0f, 1.0f, 0.0f, },             // Normal of vertex 1.
            { 0.0f, 1.0f, 0.0f, },             // Normal of vertex 2.
            { 0.0f, 1.0f, 0.0f, },             // Normal of vertex 2.
        };

        static float s_FloorColors[][4] =
        {
            { 0.8f, 0.521f, 0.072f, 1.0f, },        // Color of vertex 0.
            { 0.8f, 0.521f, 0.072f, 1.0f, },        // Color of vertex 1.
            { 0.8f, 0.521f, 0.072f, 1.0f, },        // Color of vertex 2.
            { 0.8f, 0.521f, 0.072f, 1.0f, },
        };

        static int s_FloorIndices[][3] =
        {
           { 0, 2, 3, },
           { 0, 1, 2, },
        };


        // -----------------------------------------------------------------------------
        // Define the mesh and its material. The material defines the look of the
        // surface covering the mesh. If the material should contain normals, colors, or
        // texture coordinates then their number has to match the number of vertices.
        // If you do not support normals in a mesh, YoshiX will not apply lighting to
        // this mesh. A textured mesh always has to contain texture coordinates and a
        // handle to a texture. A mesh always has to contain vertices and indices.
        // -----------------------------------------------------------------------------
        SMeshInfo MeshInfo;

        MeshInfo.m_pVertices = &s_TriangleVertices[0][0];
        MeshInfo.m_pNormals = &s_TriangleNormals[0][0];
        MeshInfo.m_pColors = nullptr;
        MeshInfo.m_pTexCoords = nullptr;
        MeshInfo.m_NumberOfVertices = 4;
        MeshInfo.m_NumberOfIndices = 12;
        MeshInfo.m_pIndices = &s_TriangleIndices[0][0];
        MeshInfo.m_pTexture = nullptr;

        CreateMesh(MeshInfo, &m_pTriangleMesh);

        MeshInfo.m_pVertices = &s_FloorVertices[0][0];
        MeshInfo.m_pNormals = &s_FloorNormals[0][0];
        MeshInfo.m_pColors = &s_FloorColors[0][0];
        MeshInfo.m_pTexCoords = nullptr;
        MeshInfo.m_NumberOfVertices = 4;
        MeshInfo.m_NumberOfIndices = 12;
        MeshInfo.m_pIndices = &s_FloorIndices[0][0];
        MeshInfo.m_pTexture = nullptr;

        CreateMesh(MeshInfo, &m_pFloorleMesh);



        static const float s_HalfEdgeLength = 1.0f;

        static float s_CubeVertices[][3] =
        {
            { -s_HalfEdgeLength, -s_HalfEdgeLength, -s_HalfEdgeLength, },
            {  s_HalfEdgeLength, -s_HalfEdgeLength, -s_HalfEdgeLength, },
            {  s_HalfEdgeLength,  s_HalfEdgeLength, -s_HalfEdgeLength, },
            { -s_HalfEdgeLength,  s_HalfEdgeLength, -s_HalfEdgeLength, },

            { -s_HalfEdgeLength, -s_HalfEdgeLength,  s_HalfEdgeLength, },
            {  s_HalfEdgeLength, -s_HalfEdgeLength,  s_HalfEdgeLength, },
            {  s_HalfEdgeLength,  s_HalfEdgeLength,  s_HalfEdgeLength, },
            { -s_HalfEdgeLength,  s_HalfEdgeLength,  s_HalfEdgeLength, },
        };

        static float s_CubeNormals[][3] =
        {
            { -1.0f, -1.0f, -1.0f, },
            {  1.0f, -1.0f, -1.0f, },
            {  1.0f,  1.0f, -1.0f, },
            { -1.0f,  1.0f, -1.0f, },
            { -1.0f, -1.0f,  1.0f, },
            {  1.0f, -1.0f,  1.0f, },
            {  1.0f,  1.0f,  1.0f, },
            { -1.0f,  1.0f,  1.0f, },
        };

        static float s_CubeColors[][4] =
        {
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
        };

        static int s_CubeIndices[][3] =
        {
            {  0,  1,  2, },
            {  0,  2,  3, },
            {  1,  6,  2, },
            {  1,  5,  6, },
            {  5,  7,  6, },
            {  5,  4,  7, },
            {  4,  3,  7, },
            {  4,  0,  3, },
            {  3,  6,  7, },
            {  3,  2,  6, },
            {  1,  0,  4, },
            {  1,  4,  5, },
        };

        MeshInfo.m_pVertices = &s_CubeVertices[0][0];
        MeshInfo.m_pNormals = &s_CubeNormals[0][0];
        MeshInfo.m_pColors = nullptr;
        MeshInfo.m_pTexCoords = nullptr;
        MeshInfo.m_NumberOfVertices = 8;
        MeshInfo.m_NumberOfIndices = 36;
        MeshInfo.m_pIndices = &s_CubeIndices[0][0];
        MeshInfo.m_pTexture = nullptr;

        CreateMesh(MeshInfo, &m_pCubeMesh);

        return true;
    }

    // -----------------------------------------------------------------------------

    bool CApplication::InternOnReleaseMeshes()
    {
        std::cout << "--------------------------release meshes" << std::endl;
        // -----------------------------------------------------------------------------
        // Important to release the mesh again when the application is shut down.
        // -----------------------------------------------------------------------------
        ReleaseMesh(m_pTriangleMesh);
        ReleaseMesh(m_pFloorleMesh);
        ReleaseMesh(m_pCubeMesh);

        if (fileMeshCreated) {
            ReleaseMesh(m_pTestImportMesh);
        }

        return true;
    }

    bool CApplication::InternOnReleaseTextures()
    {

        //ReleaseTexture(m_testTexture);
        return true;
    }

    // -----------------------------------------------------------------------------

    bool CApplication::InternOnResize(int _Width, int _Height)
    {
        float ProjectionMatrix[16];

        // -----------------------------------------------------------------------------
        // The projection matrix defines the size of the camera frustum. The YoshiX
        // camera has the shape of a pyramid with the eye position at the top of the
        // pyramid. The horizontal view angle is defined by the vertical view angle
        // and the ratio between window width and window height.
        // -----------------------------------------------------------------------------
        GetProjectionMatrix(m_FieldOfViewY, static_cast<float>(_Width) / static_cast<float>(_Height), 0.1f, 100.0f, ProjectionMatrix);

        SetProjectionMatrix(ProjectionMatrix);



        return true;
    }

    // -----------------------------------------------------------------------------

    

    //HWND window = FindWindow(L"TaskManagerWindow", L"Task-Manager");

    bool cursorMovement() {

        if (GetCursorPos(&p))
        {
            //std::cout << "DivX: " << GET_X_LPARAM << "\n";
            if (!firstRunDone) {
                prevP = p;
                window = FindWindowW(0, L"YoshiX-Example");

                if (window) {
                    GetWindowRect(window, &rect);
                }
            }


            if (delay % 1 == 0) {
                deltaP.x = prevP.x - p.x;
                deltaP.y = prevP.y - p.y;

                prevP = p;
            }
            firstRunDone = true;

            delay++;

            posX += deltaP.x * sensitivity;
            posY += deltaP.y * sensitivity;

            if (!mouseIsFree) {
                SetCursorPos(rect.left + windowXDisplacement + 400, rect.top + windowYDisplaycement + 300);
                prevP = { rect.left + windowXDisplacement + 400, rect.top + windowYDisplaycement + 300 };
            }
        }

        return true;
    }

    bool CApplication::InternOnMouseEvent(int _X, int _Y, int _Button, bool _IsButtonDown, bool _IsDoubleClick, int _WheelDelta) {
        //std::cout << "x: " << _X << "; y: " << _Y << "\n";

        if (_IsButtonDown) {
            //std::cout << "Button Down: " << _Button << " \n";
        }

        if (_WheelDelta) {
            //std::cout << "Weeeeeee....." << _WheelDelta << " \n";

            scrollOffset += (_WheelDelta / 120) * 1.0f;
        }

        //mouseControlls(_X, _Y);

        return true;
    }

    bool CApplication::InternOnKeyEvent(unsigned int _Key, bool _IsKeyDown, bool _IsAltDown) {

        // -----------------------------------------------------------------------------
        // Pressing the 'Space' key implies the condition to become true.
        // -----------------------------------------------------------------------------
        if (_Key == ' ' && _IsKeyDown)
        {
            std::cout << "klick" << std::endl;
            mouseIsFree = !mouseIsFree;
        }

        return true;
    }

    bool CApplication::InternOnUpdate()
    {
        cursorMovement(); // IMPORTANT

        float LightPosition[3] = { scrollOffset, 5.0f, scrollOffset, };

        SetLightPosition(LightPosition);

        float Eye[3];
        float At[3];
        float Up[3];

        float ViewMatrix[16];
        float TranslationMatrix[16];
        float RotationXMatrix[16];
        float RotationYMatrix[16];
        float RotationZMatrix[16];

        float RotationMatrix[16];

        // -----------------------------------------------------------------------------
        // Define position and orientation of the camera in the world.
        // -----------------------------------------------------------------------------
        Eye[0] = 0.0f; At[0] = 0.0f; Up[0] = 0.0f;
        Eye[1] = 7.0f; At[1] = 0.0f; Up[1] = 1.0f;
        Eye[2] = -10.0f; At[2] = 0.0f; Up[2] = 0.0f;

        GetViewMatrix(Eye, At, Up, ViewMatrix);


    

        GetTranslationMatrix(0.0f, 0.0f, 0.0f, TranslationMatrix);
        GetRotationXMatrix(posY, RotationXMatrix);
        GetRotationYMatrix(posX, RotationYMatrix);
        GetRotationZMatrix(0.0f, RotationZMatrix);

        MulMatrix(RotationYMatrix, RotationXMatrix, RotationMatrix);
        MulMatrix(RotationMatrix, ViewMatrix, ViewMatrix);


        SetViewMatrix(ViewMatrix);

        return true;
    }


    double calcAngle(double hypotenuse, double opositeSide, double adjacentSide) {
        //std::cout << "/*hypotenuse: " << hypotenuse << ", opositeSide: " << opositeSide << " adjacentSide: " << adjacentSide << "\n";
        //std::cout << "*/Wert: " << ((pow(hypotenuse, 2) - pow(opositeSide, 2) - pow(adjacentSide, 2)) / (-2 * opositeSide * adjacentSide)) << "\n";
        return acos((pow(hypotenuse, 2) - pow(opositeSide, 2) - pow(adjacentSide, 2)) / (-2 * opositeSide * adjacentSide)) * 180 / PI;
    }


    // -----------------------------------------------------------------------------

    bool CApplication::InternOnFrame()
    {

        float WorldMatrix[16];
        float ScaleMatrix[16];
        float TranslationMatrix[16];
        float RotationMatrix[16];

        GetTranslationMatrix(0.0f, 0.0f, 0.0f, WorldMatrix);

        SetWorldMatrix(WorldMatrix);

        DrawMesh(m_pTriangleMesh);


        GetScaleMatrix(5.0f, 1.0f, 5.0f, WorldMatrix);

        SetWorldMatrix(WorldMatrix);

        DrawMesh(m_pFloorleMesh);


        if (fileLoaded) {
            GetTranslationMatrix(0.0f, 2.0f, -2.0f, WorldMatrix);

            SetWorldMatrix(WorldMatrix);

            DrawMesh(m_pTestImportMesh);
        }


        GetTranslationMatrix(-3.0f, 2.0f, -2.0f, WorldMatrix);

        SetWorldMatrix(WorldMatrix);

        DrawMesh(m_pCubeMesh);


        return true;
    }


} // namespace

void main()
{
    CApplication Application;

    RunApplication(800, 600, "YoshiX-Example", &Application);
}