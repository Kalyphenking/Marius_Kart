
#include "yoshix_fix_function.h"

#include <math.h>
#include <windows.h>

using namespace gfx;

namespace
{
    double    g_Frequency;
    long long g_StartTick;
} // namespace

namespace
{
    void GetFrequency()
    {
        long long Frequency;

        ::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&Frequency));

        g_Frequency = static_cast<double>(Frequency);
    }

    // -----------------------------------------------------------------------------

    void StartTime()
    {
        ::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&g_StartTick));
    }

    // -----------------------------------------------------------------------------

    double GetTimeInSeconds()
    {
        long long CurrentRealTimeTick;

        ::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&CurrentRealTimeTick));

        return static_cast<double>(CurrentRealTimeTick - g_StartTick) / g_Frequency;
    }
} // namespace

namespace
{
    class CApplication : public IApplication
    {
        public:

            CApplication();
            virtual ~CApplication();

        private:

            float   m_FieldOfViewY;     // Vertical view angle of the camera.

            float   m_AngleX;           // Angle of the cube around the x-axis.
            float   m_AngleY;           // Angle of the cube around the y-axis.
            float   m_AngleZ;           // Angle of the cube around the z-axis.

            BHandle m_pCubeMesh;        // A pointer to a YoshiX mesh, which represents a single cube.
            BHandle m_pC4DCubeMesh;

        private:

            virtual bool InternOnStartup();
            virtual bool InternOnShutdown();
            virtual bool InternOnCreateMeshes();
            virtual bool InternOnReleaseMeshes();
            virtual bool InternOnResize(int _Width, int _Height);
            virtual bool InternOnUpdate();
            virtual bool InternOnFrame();
    };
} // namespace

namespace
{
    CApplication::CApplication()
        : m_FieldOfViewY(60.0f)     // Set the vertical view angle of the camera to 60 degrees.
        , m_AngleX      (0.0f)
        , m_AngleY      (0.0f)
        , m_AngleZ      (0.0f)
        , m_pCubeMesh   (nullptr)
        , m_pC4DCubeMesh(nullptr)
    {
    }

    // -----------------------------------------------------------------------------

    CApplication::~CApplication()
    {
    }

    // -----------------------------------------------------------------------------

    bool CApplication::InternOnStartup()
    {
        // -----------------------------------------------------------------------------
        // Define the background color of the window. Colors are always 4D tuples,
        // whereas the components of the tuple represent the red, green, blue, and alpha 
        // channel. The alpha channel indicates the transparency of the color. A value
        // of 1 means the color is completely opaque. A value of 0 means the color is
        // completely transparent. The channels red, green, and blue also are values
        // 0..1 with 0 indicating the minimum intensity and 1 the maximum intensity.
        // -----------------------------------------------------------------------------
        float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f, };

        SetClearColor(ClearColor);

        return true;
    }

    // -----------------------------------------------------------------------------

    bool CApplication::InternOnShutdown()
    {
        return true;
    }

    // -----------------------------------------------------------------------------

    bool CApplication::InternOnCreateMeshes()
    {
        // -----------------------------------------------------------------------------
        // Define the vertices of the mesh and their attributes.
        // -----------------------------------------------------------------------------
        static const float s_HalfEdgeLength = 1.0f;
        
        static float s_CubeVertices[][3] =
        {
            { -s_HalfEdgeLength, -s_HalfEdgeLength, -s_HalfEdgeLength, },
            {  s_HalfEdgeLength, -s_HalfEdgeLength, -s_HalfEdgeLength, },
            {  s_HalfEdgeLength,  s_HalfEdgeLength, -s_HalfEdgeLength, },
            { -s_HalfEdgeLength,  s_HalfEdgeLength, -s_HalfEdgeLength, },

            {  s_HalfEdgeLength, -s_HalfEdgeLength, -s_HalfEdgeLength, },
            {  s_HalfEdgeLength, -s_HalfEdgeLength,  s_HalfEdgeLength, },
            {  s_HalfEdgeLength,  s_HalfEdgeLength,  s_HalfEdgeLength, },
            {  s_HalfEdgeLength,  s_HalfEdgeLength, -s_HalfEdgeLength, },

            {  s_HalfEdgeLength, -s_HalfEdgeLength,  s_HalfEdgeLength, },
            { -s_HalfEdgeLength, -s_HalfEdgeLength,  s_HalfEdgeLength, },
            { -s_HalfEdgeLength,  s_HalfEdgeLength,  s_HalfEdgeLength, },
            {  s_HalfEdgeLength,  s_HalfEdgeLength,  s_HalfEdgeLength, },

            { -s_HalfEdgeLength, -s_HalfEdgeLength,  s_HalfEdgeLength, },
            { -s_HalfEdgeLength, -s_HalfEdgeLength, -s_HalfEdgeLength, },
            { -s_HalfEdgeLength,  s_HalfEdgeLength, -s_HalfEdgeLength, },
            { -s_HalfEdgeLength,  s_HalfEdgeLength,  s_HalfEdgeLength, },

            { -s_HalfEdgeLength,  s_HalfEdgeLength, -s_HalfEdgeLength, },
            {  s_HalfEdgeLength,  s_HalfEdgeLength, -s_HalfEdgeLength, },
            {  s_HalfEdgeLength,  s_HalfEdgeLength,  s_HalfEdgeLength, },
            { -s_HalfEdgeLength,  s_HalfEdgeLength,  s_HalfEdgeLength, },

            { -s_HalfEdgeLength, -s_HalfEdgeLength,  s_HalfEdgeLength, },
            {  s_HalfEdgeLength, -s_HalfEdgeLength,  s_HalfEdgeLength, },
            {  s_HalfEdgeLength, -s_HalfEdgeLength, -s_HalfEdgeLength, },
            { -s_HalfEdgeLength, -s_HalfEdgeLength, -s_HalfEdgeLength, },
        };

        static float s_CubeColors[][4] =
        {
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },

            { 0.0f, 1.0f, 0.0f, 1.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f, },

            { 0.0f, 0.0f, 1.0f, 1.0f, },
            { 0.0f, 0.0f, 1.0f, 1.0f, },
            { 0.0f, 0.0f, 1.0f, 1.0f, },
            { 0.0f, 0.0f, 1.0f, 1.0f, },

            { 1.0f, 1.0f, 0.0f, 1.0f, },
            { 1.0f, 1.0f, 0.0f, 1.0f, },
            { 1.0f, 1.0f, 0.0f, 1.0f, },
            { 1.0f, 1.0f, 0.0f, 1.0f, },

            { 0.0f, 1.0f, 1.0f, 1.0f, },
            { 0.0f, 1.0f, 1.0f, 1.0f, },
            { 0.0f, 1.0f, 1.0f, 1.0f, },
            { 0.0f, 1.0f, 1.0f, 1.0f, },

            { 1.0f, 1.0f, 1.0f, 1.0f, },
            { 1.0f, 1.0f, 1.0f, 1.0f, },
            { 1.0f, 1.0f, 1.0f, 1.0f, },
            { 1.0f, 1.0f, 1.0f, 1.0f, },
        };

        // -----------------------------------------------------------------------------
        // Define the topology of the mesh via indices. An index addresses a vertex from
        // the array above. Three indices represent one triangle. When defining the 
        // triangles of a mesh imagine that you are standing in front of the triangle 
        // and looking to the center of the triangle. If the mesh represents a closed
        // body such as a cube, your view position has to be outside of the body. Now
        // define the indices of the addressed vertices of the triangle in counter-
        // clockwise order.
        // -----------------------------------------------------------------------------
        static int s_CubeIndices[][3] =
        {
            {  0,  1,  2, },
            {  0,  2,  3, },

            {  4,  5,  6, },
            {  4,  6,  7, },

            {  8,  9, 10, },
            {  8, 10, 11, },

            { 12, 13, 14, },
            { 12, 14, 15, },

            { 16, 17, 18, },
            { 16, 18, 19, },

            { 20, 21, 22, },
            { 20, 22, 23, },
        };




        static float s_C4DCubeVertices[][3] =
        {
            {-1.0,-1.0,-1.0},
            {-1.0,1.0,-1.0},
            {1.0,-1.0,-1.0},
            {1.0,1.0,-1.0},
            {1.0,-1.0,1.0},
            {1.0,1.0,1.0},
            {-1.0,-1.0,1.0},
            {-1.0,1.0,1.0},

        };

        static float s_C4DCubeColors[][4] =
        {
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },
            { 1.0f, 0.0f, 0.0f, 1.0f, },

            { 0.0f, 1.0f, 0.0f, 1.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f, },
            { 0.0f, 1.0f, 0.0f, 1.0f, },
        };

        static int s_C4DCubeIndices[][3] =
        {
            {0,1,3},
            {2,3,5},
            {4,5,7},
            {6,7,1},
            {1,7,5},
            {6,0,2},
            {3,2,0},
            {2,5,4},
            {4,7,6},
            {6,1,0},
            {1,5,3},
            {6,2,4},
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

        MeshInfo.m_pVertices        = &s_CubeVertices[0][0];
        MeshInfo.m_pNormals         = nullptr;                          // No normals
        MeshInfo.m_pColors          = &s_CubeColors[0][0];
        MeshInfo.m_pTexCoords       = nullptr;                          // No texture coordinates
        MeshInfo.m_NumberOfVertices = 24;
        MeshInfo.m_NumberOfIndices  = 36;
        MeshInfo.m_pIndices         = &s_CubeIndices[0][0];
        MeshInfo.m_pTexture         = nullptr;                          // No texture

        CreateMesh(MeshInfo, &m_pCubeMesh);


        MeshInfo.m_pVertices = &s_C4DCubeVertices[0][0];
        MeshInfo.m_pNormals = nullptr;                          // No normals
        MeshInfo.m_pColors = &s_C4DCubeColors[0][0];
        MeshInfo.m_pTexCoords = nullptr;                          // No texture coordinates
        MeshInfo.m_NumberOfVertices = 8;
        MeshInfo.m_NumberOfIndices = 36;
        MeshInfo.m_pIndices = &s_C4DCubeIndices[0][0];
        MeshInfo.m_pTexture = nullptr;     
        // No texture
        CreateMesh(MeshInfo, &m_pC4DCubeMesh);
        

        return true;
    }

    // -----------------------------------------------------------------------------

    bool CApplication::InternOnReleaseMeshes()
    {
        // -----------------------------------------------------------------------------
        // Important to release the mesh again when the application is shut down.
        // -----------------------------------------------------------------------------
        ReleaseMesh(m_pCubeMesh);

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

    bool CApplication::InternOnUpdate()
    {
        float Eye[3];
        float At [3];
        float Up [3];

        float ViewMatrix[16];

        // -----------------------------------------------------------------------------
        // Define position and orientation of the camera in the world.
        // -----------------------------------------------------------------------------
        Eye[0] =   0.0f; At[0] = 0.0f; Up[0] = 0.0f;
        Eye[1] =   0.0f; At[1] = 0.0f; Up[1] = 1.0f;
        Eye[2] = -20.0f; At[2] = 0.0f; Up[2] = 0.0f;

        GetViewMatrix(Eye, At, Up, ViewMatrix);

        SetViewMatrix(ViewMatrix);

        return true;
    }

    // -----------------------------------------------------------------------------



    float g_X    = 0.0f;
    float g_Step = 0.5f;



    bool CApplication::InternOnFrame()
    {
        float WorldMatrix[16];
        float translationMatrix[16];
        float rotationMatrix[16];
        float scaleMatrix[16];

        //GetTranslationMatrix(g_X, -g_X, 0.0f, WorldMatrix);
        GetRotationYMatrix(g_X, WorldMatrix);
        SetWorldMatrix(WorldMatrix);
        DrawMesh(m_pCubeMesh);

        g_X += g_Step;

        /*if (g_X < -180.0f || g_X > 180.0f)
        {
            g_Step = -g_Step;
        }*/

        double CurrentRealTime = GetTimeInSeconds();

        for (;;)
        {
            if (GetTimeInSeconds() - CurrentRealTime > 0.012)
            {
                break;
            }
        }


        GetTranslationMatrix(4.0f, -5.0f, 0.0f, WorldMatrix);

        SetWorldMatrix(WorldMatrix);
        DrawMesh(m_pC4DCubeMesh);



        return true;
    }
} // namespace

//void main()
//{
//    GetFrequency();
//    StartTime();
//
//    CApplication Application;
//
//    RunApplication(800, 600, "YoshiX Example", &Application);
//}