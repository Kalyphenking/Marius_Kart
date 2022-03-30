#pragma once
#include <windows.h>
#include <iostream>
#include <stdio.h>

class CameraController {

public:
    bool firstRunDone = false;
    bool* camIsMoving;

    POINT p = { 0,0 };
    POINT prevP = { 0,0 };
    POINT deltaP = { 0,0 };
    float sensitivity;
    float minCameraXAngle;
    float maxCameraXAngle;
    bool mouseFixed;
    int windowXDisplacement;
    int windowYDisplaycement;
    int windowHeight;
    int windowWidth;

    LPCTSTR WindowName;

    HWND window;
    RECT rect = { 0 };

    CameraController();
    CameraController(LPCTSTR WindowName, bool* camIsMoving, float minCameraXAngle, float maxCameraXAngle, float sensitivity = 0.05f, int xOffset = 0, int yOffset = 0);
    ~CameraController();

    bool cursorMovement(float* camYAngleValue, float* camXAngleValue, bool mouseFixed);

};