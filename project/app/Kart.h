#pragma once
#include "yoshix_fix_function.h"
#include <iostream>
#include <windows.h>
#include <string>
#include <future>
#include <vector>

class Kart {
   


public:
    struct vec3d
    {
        float x, y, z;

        /*vec3d operator+(vec3d a) const {
            return vec3d{ a.x + x, a.y + y, a.z + z };
        }
        vec3d operator*(vec3d a) const {
            return vec3d{ a.x * x, a.y * y, a.z * z };
        }

        vec3d operator+(float a) const {
            return vec3d{ a + x, a + y, a + z };
        }
        vec3d operator*(float a) const {
            return vec3d{ a * x, a * y, a * z };
        }

        vec3d operator-(vec3d a) const {
            return vec3d{ a.x - x, a.y - y, a.z - z };
        }
        
        vec3d operator/(vec3d a) const {
            return vec3d{ a.x / x, a.y / y, a.z / z };
        }

        
        vec3d operator-(float a) const {
            return vec3d{ x - a, y - a, z - a };
        }
       
        vec3d operator/(float a) const {
            return vec3d{ x / a, y / a, z / a };
        }*/
    };

    vec3d position;

    float* cameraZValue;
    float yRotationAngle;
    bool* gameIsPaused;
    bool* raceStarted;

    Kart(bool* gameIsPaused, bool* raceStarted, float* cameraZValue);
    //~Kart();

    bool ResetPosition();

    bool update();

    bool CreateMeshes();

    bool DrawMeshes();

    bool ReleaseMeshes();

};