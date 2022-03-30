#include "CameraController.h"



    bool firstRunDone = false;
    bool* camIsMoving;

    POINT p = { 0,0 };
    POINT prevP = { 0,0 };
    POINT deltaP = { 0,0 };
    float sensitivity;
    float minCameraXAngle;
    float maxCameraXAngle;
    int windowXDisplacement;
    int windowYDisplaycement;
    int windowHeight;
    int windowWidth;

    LPCTSTR WindowName;

    HWND window;
    RECT rect = { 0 };

    CameraController::CameraController() {
    };

    CameraController::CameraController(LPCTSTR WindowName, bool* camIsMoving, float minCameraXAngle, float maxCameraXAngle, float sensitivity, int xOffset, int yOffset) 
        :WindowName(WindowName)
        , sensitivity(sensitivity)
        , windowXDisplacement(xOffset)
        , windowYDisplaycement(yOffset)
        , minCameraXAngle(minCameraXAngle)
        , maxCameraXAngle(maxCameraXAngle)
        , camIsMoving(camIsMoving)
    {
    };


    CameraController::~CameraController() {

    };


    bool CameraController::cursorMovement(float* camYAngleValue, float* camXAngleValue, bool gameIsPaused) {
        if (GetCursorPos(&p))
        {
            // sets first prevP
            if (!firstRunDone) {
                prevP = p;
            }

            // find yoshiX window by name
            window = FindWindowW(0, WindowName);

            // stores window dimensions
            if (window) {
                GetWindowRect(window, &rect);
                windowHeight = rect.bottom - rect.top;
                windowWidth = rect.right - rect.left;

                /*DefWindowProc;
                InvalidateRect(window, &rect, false);
                Invalidate*/
            }

            firstRunDone = true;

            // gets delta position
            deltaP.x = prevP.x - p.x;
            deltaP.y = prevP.y - p.y;

            // sets camIsMoving pointer of its unsing class
            if (prevP.x != p.x && prevP.y != p.y) {
                *camIsMoving = true;
            }
            else {
                *camIsMoving = false;
            }


            prevP = p;

            // sets cam value pointer of its unsing class
            *camYAngleValue += !gameIsPaused ? deltaP.x * sensitivity : 0;
            *camXAngleValue += !gameIsPaused ? deltaP.y * sensitivity : 0;

            if (*camXAngleValue > -minCameraXAngle) {
                *camXAngleValue = -minCameraXAngle;
            }
            else if (*camXAngleValue < -maxCameraXAngle) {
                *camXAngleValue = -maxCameraXAngle;
            }

            if (!gameIsPaused) {
                SetCursorPos(rect.left + windowXDisplacement + windowWidth / 2, rect.top + windowYDisplaycement + windowHeight / 2);
                prevP = { rect.left + windowXDisplacement + windowWidth / 2, rect.top + windowYDisplaycement + windowHeight / 2 };
            }
        }
        return true;
    }

    // tried to display text on existing window
    
    //LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

    //    std::cout << "WindowProcedure: " << message << std::endl;
    //    switch (message) {
    //    case WM_PAINT: 

    //        std::cout << "message: " << message << std::endl;

    //       /* PAINTSTRUCT ps;
    //        HDC hdc;
    //        hdc = BeginPaint(window, &ps);

    //        LPCWSTR str = L"HalloHallo";

    //        TextOut(hdc, 50, 50, TEXT("Michael Morrison"), 10);*/
    //        break; 
    //    }

    //    return 0;
    //}


