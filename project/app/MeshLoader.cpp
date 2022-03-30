#include "MeshLoader.h"


using namespace gfx;

// runs trought .x file created by Cinema4D and is optimized for Cinema4D
bool LoadFromObjectFile(std::string sFilename, BHandle* _ppMesh) {

    struct vec4d
    {
        float x, y, z, a;
    };

    std::fstream modelFile(sFilename);

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

    bool hasNormals = false;

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

    const int maxNumberOfVertices = 1024000;
    const int maxNumberOfNormals = 1024000;
    const int maxNumberOfColors = 1024000;
    const int maxNumberOfindices = 1024000;
    const int maxNumberOfTexCoords = 1024000;

    auto s_Vertices = new float[maxNumberOfVertices][3];
    auto s_Normals = new float[maxNumberOfNormals][3];
    auto s_Colors = new float[maxNumberOfColors][4];
    auto s_Indices = new int[maxNumberOfindices][3];
    auto s_TextureCoords = new float[maxNumberOfTexCoords][2];


    int arrayIndex = 0;

    if (!modelFile.is_open()) {
        std::cout << "File " << sFilename << "          does not exist\n";
        return false;
    }
    else {
        std::cout << "File: " << sFilename << "         is loading....." "\n";
        fileExists = true;
    }


    while (!modelFile.eof() && fileExists) {
        char line[256];
        modelFile.getline(line, 256);

        std::strstream s;
        s << line;

        char junk;

        std::string string;

        string = line;


        // searches line for startIndex of values

        if (string.find("\n") != std::string::npos) {
        }

        if (string.find("Mesh CINEMA4D_Mesh {") != std::string::npos) {
            verticesStartIndex = count + 3;
        }

        if (string.find("MeshNormals {") != std::string::npos) {
            normalsStartIndex = count + 3;
            hasNormals = true; 
        }

        if (string.find("MeshTextureCoords {") != std::string::npos) {
            textureCoordsStartIndex = count + 3;
        }

        if (string.find("Mat {") != std::string::npos) {
            colorStartIndex = count + 1;
            colorsDefined = true;
        }

        if (string.find("TextureFilename {") != std::string::npos) {
            textureNameStartIndex = count + 1;
            textureDefined = true;
        }


        // gets values from startIndex and line number

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

        if (count == normalsStartIndex - 2 && hasNormals) {
            s >> numberOfNormals;

            if (numberOfNormals > maxNumberOfNormals) {
                std::cout << "numberOfNormals > maxNumberOfNormals: " << numberOfIndices << std::endl;
                return false;
            }
        }


        // saves values in arrays

        if (strlen(line) == 0 && verticesStartIndex != 0 && indicesStartIndex == 0 && normalsStartIndex == 0 && count > (verticesStartIndex + numberOfVertices - 1)) {
            indicesStartIndex = count + 3;
        }


        if (strlen(line) != 0 && verticesStartIndex != 0 && count >= verticesStartIndex && count <= (verticesStartIndex + numberOfVertices - 1)) {
            vec4d v;
            s >> v.x >> junk >> v.y >> junk >> v.z;

            arrayIndex = count - verticesStartIndex;

            s_Vertices[arrayIndex][0] = v.x;
            s_Vertices[arrayIndex][1] = v.y;
            s_Vertices[arrayIndex][2] = v.z;
        }


        if (hasNormals && strlen(line) != 0 && normalsStartIndex != 0 && count >= normalsStartIndex && count <= (normalsStartIndex + numberOfNormals - 1)) {
            vec4d v;
            s >> v.x >> junk >> v.y >> junk >> v.z;

            arrayIndex = count - normalsStartIndex;

            s_Normals[arrayIndex][0] = v.x;
            s_Normals[arrayIndex][1] = v.y;
            s_Normals[arrayIndex][2] = v.z;
        }


        if (strlen(line) != 0 && indicesStartIndex != 0 && count >= indicesStartIndex && count <= (indicesStartIndex + (numberOfIndices / 3) - 1)) {
            vec4d v;
            s >> junk >> junk >> v.x >> junk >> v.y >> junk >> v.z;

            arrayIndex = count - indicesStartIndex;

            s_Indices[arrayIndex][0] = v.x;
            s_Indices[arrayIndex][1] = v.z;
            s_Indices[arrayIndex][2] = v.y;
        }

        if (strlen(line) != 0 && textureCoordsStartIndex != 0 && count >= textureCoordsStartIndex && count <= (textureCoordsStartIndex + numberOfTextureCoords - 1)) {
            vec4d v;
            s >> v.x >> junk >> v.y;

            arrayIndex = count - textureCoordsStartIndex;

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


    // creates texture
    if (fileExists) {
        if (textureDefined) {
            std::string textureRootDirectory = "../project/src/textures/";

            textureFileName = textureFileName.substr(1, (textureFileName.length() - 3));

            std::string texturePath = textureRootDirectory + textureFileName;

            std::fstream textureFile(texturePath);


            if (textureFile.is_open()) {
                textureExists = true;

                textureFile.close();
                CreateTexture(texturePath.c_str(), &s_texture);
            }
            else {
                std::cout << "texture not exists: " << std::endl;
            }
        }

        // creates mesh
        SMeshInfo MeshInfo;

        MeshInfo.m_pVertices = &s_Vertices[0][0];
        MeshInfo.m_pNormals = hasNormals ? &s_Normals[0][0] : nullptr;
        MeshInfo.m_pColors = colorsDefined && !textureExists ? &s_Colors[0][0] : nullptr;
        MeshInfo.m_pTexCoords = textureExists ? &s_TextureCoords[0][0] : nullptr;
        MeshInfo.m_NumberOfVertices = numberOfVertices;
        MeshInfo.m_NumberOfIndices = numberOfIndices;
        MeshInfo.m_pIndices = &s_Indices[0][0];
        MeshInfo.m_pTexture = textureExists ? s_texture : nullptr;

        CreateMesh(MeshInfo, _ppMesh);

        // releases texture
        if (textureExists) {
            ReleaseTexture(s_texture);
        }
    }

    delete[] s_Vertices;
    delete[] s_Normals;
    delete[] s_Colors;
    delete[] s_Indices;
    delete[] s_TextureCoords;

    return true;
}