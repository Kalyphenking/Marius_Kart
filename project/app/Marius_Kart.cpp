#include "yoshix_fix_function.h"
#include "MeshLoader.h"
#include "CameraController.h"
#include "Kart.h"
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
#include <string> 

using namespace gfx;

#define PI 3.14159265

namespace {

	class CApplication : public IApplication
	{
	public:

		CApplication();
		virtual ~CApplication();

	private:

		float   m_FieldOfViewY;     // Vertical view angle of the camera
        double cameraFollowYRotation;
        Kart::vec3d cameraFollowPosition;

        BHandle m_pStrecke;
        BHandle m_pBorder_Red;
        BHandle m_pBorder_White;

        BHandle m_pUmgebung;
        BHandle m_pSaeulen;
        BHandle m_pBanner;

        BHandle m_pButton_Start_Game_Yellow;
        BHandle m_pButton_Start_Game_Grey;
        BHandle m_pButton_Continue_Yellow;
        BHandle m_pButton_Continue_Grey;
        BHandle m_pButton_Controlls_Yellow;
        BHandle m_pButton_Controlls_Grey;
        BHandle m_pButton_Quit_Game_Yellow;
        BHandle m_pButton_Quit_Game_Grey;
        BHandle m_pControlls;
        BHandle m_pZiellinie;

        BHandle m_pCountDown_One;
        BHandle m_pCountDown_Two;
        BHandle m_pCountDown_Three;
        BHandle m_pCountDown_Gooooo;
        BHandle m_pCountDown_Finish;

        Kart* kart;

        LPCTSTR WindowName = L"Marius Kart";

        CameraController* camController;

        std::chrono::steady_clock::time_point cameraNotMovingTimerBegin;
        std::chrono::steady_clock::time_point cameraNotMovingTimerEnd;

        std::chrono::steady_clock::time_point lapTimeBegin;
        std::chrono::steady_clock::time_point lapTimeEnd;

        std::chrono::steady_clock::time_point countDownBegin;
        std::chrono::steady_clock::time_point countDownEnd;

        std::chrono::steady_clock::time_point gamePausedBegin;
        std::chrono::steady_clock::time_point gamePausedEnd;


        bool cameraNotMovingTimerStarted;
        bool cameraResetTime;
        bool gameIsPaused;
        bool gameStarted;

        bool buttonStartContinueGameHighlighted;
        bool buttonControllsHighlighted;
        bool buttonQuittGameHighlighted;
        bool controllsMenuOpen;

        bool cameraIsMoving;

        bool newLapStarted;
        bool raceStarted;
        bool raceFinished;

        int windowWidth;
        int windowHeight;

        int lapCounter;
        int maxLapCount;

        float cameraYAngle;
        float cameraXAngle;
        float maxCameraPlayDistance;
        float cameraZValue;
        float previousCameraZValue;
        float CameraViewMatrix[16];
        float minCameraXAngle;
        float maxCameraXAngle;

        std::chrono::steady_clock::duration gamePausedTime;

        float** objectCoords;
        std::string* lapTimes;
       

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
    };
}


namespace
{
    CApplication::CApplication()
        : m_FieldOfViewY(60.0f)    // Set the vertical view angle of the camera to 60 degrees.
        , m_pStrecke(nullptr)
        , m_pBorder_White(nullptr)
        , m_pBorder_Red(nullptr)
        , m_pUmgebung(nullptr)
        , m_pSaeulen(nullptr)
        , m_pBanner(nullptr)
        , m_pCountDown_One(nullptr)
        , m_pCountDown_Two(nullptr)
        , m_pCountDown_Three(nullptr)
        , m_pCountDown_Gooooo(nullptr)
        , m_pCountDown_Finish(nullptr)
        , m_pButton_Start_Game_Yellow(nullptr)
        , m_pButton_Start_Game_Grey(nullptr)
        , m_pButton_Quit_Game_Yellow(nullptr)
        , m_pButton_Quit_Game_Grey(nullptr)
        , m_pControlls(nullptr)
        , m_pZiellinie(nullptr)
        , kart(nullptr)
        , cameraYAngle(0.0f)
        , cameraXAngle(0.0f)
        , cameraZValue(-5.0f)
        , previousCameraZValue(-5.0f)
        , cameraFollowYRotation(0.0f)
        , maxCameraPlayDistance(8.0f)
        , minCameraXAngle(0.5f)
        , maxCameraXAngle(55.0f)
        , cameraResetTime(3.0f)
        , gamePausedTime(0)
        , cameraIsMoving(false)
        , cameraNotMovingTimerStarted(false)
        , gameIsPaused(true)
        , gameStarted(false)
        , buttonStartContinueGameHighlighted(false)
        , buttonControllsHighlighted(false)
        , buttonQuittGameHighlighted(false)
        , controllsMenuOpen(false)
        , newLapStarted(false)
        , raceStarted(false)
        , raceFinished(false)
        , cameraFollowPosition({0.0f, 0.0f, 0.0f })
        , objectCoords(new float* [4])
        , lapTimes(new std::string [4])
        , windowWidth(1280)
        , windowHeight(720)
        , lapCounter(0)
        , maxLapCount(3)
    {
       camController = new CameraController(L"Marius Kart", &cameraIsMoving, minCameraXAngle, maxCameraXAngle);
       kart = new Kart(&gameIsPaused, &raceStarted, &cameraZValue);
    }


    CApplication::~CApplication()
    {
    }

    
    bool CApplication::InternOnStartup()
    {

        for (int row = 0; row < 4; row++) {
            objectCoords[row] = new float[4];
        }


        // finishline coords
        objectCoords[3][0] = -6.833f;
        objectCoords[3][1] = 6.833f;
        objectCoords[3][2] = 1.971f;
        objectCoords[3][3] = 2.804f;


        float ClearColor[4] = { 0.6f, 0.6f, 0.6f, 1.0f, };

        SetClearColor(ClearColor);

        float LightPosition[3] = { 0.0f, 30.0f, 0.0f, };

        SetLightPosition(LightPosition);

        float LightAmbientColor[4] = { 0.7, 0.7, 0.7, 1.0f, };
        float LightDiffuseColor[4] = { 0.6f, 0.6f, 0.6f, 1.0f, };
        float LightSpecularColor[4] = { 0.5, 0.5, 0.5, 1.0f, };


        SetLightColor(LightAmbientColor, LightDiffuseColor, LightSpecularColor, 200);
        std::cout << "Startup \n";

        return true;
    }

    bool CApplication::InternOnCreateMeshes()
    {

        // loads meshes (and textures) from files
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Strecke.x", &m_pStrecke);

        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Border_White.x", &m_pBorder_White);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Border_Red.x", &m_pBorder_Red);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Umgebung.x", &m_pUmgebung);

        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Saeulen.x", &m_pSaeulen);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Banner.x", &m_pBanner);

        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Button_Start_Game_Yellow.x", &m_pButton_Start_Game_Yellow);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Button_Start_Game_Grey.x", &m_pButton_Start_Game_Grey);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Button_Continue_Yellow.x", &m_pButton_Continue_Yellow);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Button_Continue_Grey.x", &m_pButton_Continue_Grey);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Button_Controlls_Yellow.x", &m_pButton_Controlls_Yellow);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Button_Controlls_Grey.x", &m_pButton_Controlls_Grey);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Button_Quit_Game_Yellow.x", &m_pButton_Quit_Game_Yellow);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Button_Quit_Game_Grey.x", &m_pButton_Quit_Game_Grey);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Controlls.x", &m_pControlls);

        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Ziellinie.x", &m_pZiellinie);

        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Countdown_One.x", &m_pCountDown_One);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Countdown_Two.x", &m_pCountDown_Two);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Countdown_Three.x", &m_pCountDown_Three);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Countdown_Gooooo.x", &m_pCountDown_Gooooo);
        async(std::launch::async, LoadFromObjectFile, "../project/src/models/Countdown_Finish.x", &m_pCountDown_Finish);


        kart->CreateMeshes();

        return true;
    }

    bool CApplication::InternOnReleaseMeshes()
    {

        if (m_pStrecke != nullptr) ReleaseMesh(m_pStrecke);
        if (m_pBorder_White != nullptr) ReleaseMesh(m_pBorder_White);
        if (m_pBorder_Red != nullptr) ReleaseMesh(m_pBorder_Red);

        if (m_pUmgebung != nullptr) ReleaseMesh(m_pUmgebung);
        if (m_pSaeulen != nullptr) ReleaseMesh(m_pSaeulen);
        if (m_pBanner != nullptr) ReleaseMesh(m_pBanner);

        if (m_pButton_Start_Game_Yellow != nullptr) ReleaseMesh(m_pButton_Start_Game_Yellow);
        if (m_pButton_Start_Game_Grey != nullptr) ReleaseMesh(m_pButton_Start_Game_Grey);
        if (m_pButton_Continue_Yellow != nullptr) ReleaseMesh(m_pButton_Continue_Yellow);
        if (m_pButton_Continue_Grey != nullptr) ReleaseMesh(m_pButton_Continue_Grey);
        if (m_pButton_Controlls_Yellow != nullptr) ReleaseMesh(m_pButton_Controlls_Yellow);
        if (m_pButton_Controlls_Grey != nullptr) ReleaseMesh(m_pButton_Controlls_Grey);
        if (m_pButton_Quit_Game_Yellow != nullptr) ReleaseMesh(m_pButton_Quit_Game_Yellow);
        if (m_pButton_Quit_Game_Grey != nullptr) ReleaseMesh(m_pButton_Quit_Game_Grey);
        if (m_pControlls != nullptr) ReleaseMesh(m_pControlls);

        if (m_pZiellinie != nullptr) ReleaseMesh(m_pZiellinie);

        if (m_pCountDown_One != nullptr) ReleaseMesh(m_pCountDown_One);
        if (m_pCountDown_Two != nullptr) ReleaseMesh(m_pCountDown_Two);
        if (m_pCountDown_Three != nullptr) ReleaseMesh(m_pCountDown_Three);
        if (m_pCountDown_Gooooo != nullptr) ReleaseMesh(m_pCountDown_Gooooo);
        if (m_pCountDown_Finish != nullptr) ReleaseMesh(m_pCountDown_Finish);

        
        if (kart != nullptr) kart->ReleaseMeshes();


        for (int index = 0; index < 4; index++) {
            delete[] objectCoords[index];
        }
        delete[] objectCoords;
        delete[] lapTimes;

        return true;
    }

    bool CApplication::InternOnCreateTextures()
    {
        // textures in MeshLoader created
        return true;
    }

    bool CApplication::InternOnReleaseTextures()
    {
        // textures in MeshLoader released
        return true;
    }

    bool CApplication::InternOnResize(int _Width, int _Height)
    {
        float ProjectionMatrix[16];

        GetProjectionMatrix(m_FieldOfViewY, static_cast<float>(_Width) / static_cast<float>(_Height), 0.1f, 100.0f, ProjectionMatrix);

        SetProjectionMatrix(ProjectionMatrix);


        // set position of menu button in relation to window height
        for (int row = 0; row < 3; row++) {
            objectCoords[row][0] = (_Width / 2.0f) - (((691.0f / 720.0f) * _Height) / 2.0f);
            objectCoords[row][1] = _Width / 2 + ((691.0f / 720.0f) * _Height) / 2;
            objectCoords[row][2] = (13.0f / 720.0f) * _Height + (70.0f / 720.0f) * _Height + row * ((208.0f / 720.0f) * _Height);
            objectCoords[row][3] = (13.0f / 720.0f) * _Height + (208.0f / 720.0f) * _Height + row * ((208.0f / 720.0f) * _Height);;
        }

        return true;
    }

    bool CApplication::InternOnMouseEvent(int _X, int _Y, int _Button, bool _IsButtonDown, bool _IsDoubleClick, int _WheelDelta) {


        // for zooming in and out
        if (_WheelDelta && !gameIsPaused) {
            cameraZValue += (_WheelDelta / 120) * 1.0f;
            if (abs(cameraZValue) > maxCameraPlayDistance) {
                cameraZValue = -maxCameraPlayDistance;
            }
            if (abs(cameraZValue) < 1) {
                cameraZValue = -1;
            }
        }

        if (gameIsPaused) {
            // when cursor is over "START/CONTINUE" button
            if (objectCoords[0][0] < _X && _X < objectCoords[0][1] && objectCoords[0][2] < _Y && _Y < objectCoords[0][3]) {
                buttonStartContinueGameHighlighted = true; // button gets highlighted
                if (_IsButtonDown) {
                    if (gameStarted) {
                        gameIsPaused = false;
                        cameraZValue = previousCameraZValue;

                        gamePausedEnd = std::chrono::steady_clock::now();

                        gamePausedTime = gamePausedEnd - gamePausedBegin;
                    } 
                    else {

                        // starts race and countdown 

                        countDownBegin = std::chrono::steady_clock::now();
                        
                        gameStarted = true;
                        gameIsPaused = false;

                        raceStarted = false;
                        raceFinished = false;
                        lapCounter = 0;
                        kart->ResetPosition();
                        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                        std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                    }
                }
            }
            else {
                buttonStartContinueGameHighlighted = false; // button gets greyed out
            }

            // when cursor is over "CONTROLLS" button
            if (objectCoords[1][0] < _X && _X < objectCoords[1][1] && objectCoords[1][2] < _Y && _Y < objectCoords[1][3]) {
                buttonControllsHighlighted = true; // button gets highlighted
                if (_IsButtonDown) {
                    controllsMenuOpen = true;
                }
            }
            else {
                buttonControllsHighlighted = false; // button gets greyed out
            }

            // when cursor is over "QUIT" button
            if (objectCoords[2][0] < _X && _X < objectCoords[2][1] && objectCoords[2][2] < _Y && _Y < objectCoords[2][3]) {
                buttonQuittGameHighlighted = true; // button gets highlighted
                if (_IsButtonDown) {
                    exit(EXIT_FAILURE);
                }
            }
            else {
                buttonQuittGameHighlighted = false; // button gets greyed out
            }
        }

        return true;
    }

    bool CApplication::InternOnKeyEvent(unsigned int _Key, bool _IsKeyDown, bool _IsAltDown) {
       
        // handles pause, starts pause timer
        if (GetKeyState(VK_ESCAPE) & 0x8000 && _IsKeyDown) {
            if (controllsMenuOpen) {
                controllsMenuOpen = false;

            } else if (gameStarted) {
                gameIsPaused = !gameIsPaused;
            }

            if (gameIsPaused) {
                // resets camera to display the menu correctly
                previousCameraZValue = cameraZValue;
                cameraZValue = -5;
                gamePausedBegin = std::chrono::steady_clock::now();
            }
            else {
                cameraZValue = previousCameraZValue;
                gamePausedEnd = std::chrono::steady_clock::now();

                gamePausedTime = gamePausedEnd - gamePausedBegin;

            }
        }

        return true;
    }


    
    
    bool CApplication::InternOnUpdate()
    {

        // handles camera movement
        camController->cursorMovement(&cameraYAngle, &cameraXAngle, gameIsPaused);

        // stops countdown for race start
        if (!raceStarted && !gameIsPaused && lapCounter == 0) {
            countDownEnd = std::chrono::steady_clock::now();

            float passedSeconds = std::chrono::duration_cast<std::chrono::seconds>(countDownEnd - countDownBegin).count();

            // race starts after 4 seconds
            if (passedSeconds > 3 && lapCounter == 0) {
                raceStarted = true;
            }

        }

        // action when kart drives over finish line
        if (objectCoords[3][2] < kart->position.z && kart->position.z < objectCoords[3][3] &&
            objectCoords[3][0] < kart->position.x && kart->position.x < objectCoords[3][1]) {

            // calculates lap time each lap 
            if (!newLapStarted) {
                if (lapCounter > 0) {
                    lapTimeEnd = std::chrono::steady_clock::now();


                    int lapTimeMinutes = std::chrono::duration_cast<std::chrono::minutes>(lapTimeEnd - lapTimeBegin - gamePausedTime).count();
                    int lapTimeSeconds = std::chrono::duration_cast<std::chrono::seconds>(lapTimeEnd - lapTimeBegin - gamePausedTime).count() - lapTimeMinutes * 60;
                    int lapTimeMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(lapTimeEnd - lapTimeBegin - gamePausedTime).count() - lapTimeSeconds * 1000;

                    std::string lapTimeString = std::to_string(lapTimeMinutes) + ":" + std::to_string(lapTimeSeconds) + ":" + std::to_string(lapTimeMilliSeconds);

                    // saves laptime in array
                    lapTimes[lapCounter - 1] = lapTimeString;

                    std::cout << "Rundenzeit: " << lapTimeString << std::endl;
                }
                
                newLapStarted = true;
            }

        } 
        else if (newLapStarted) {
            lapTimeBegin = std::chrono::steady_clock::now();
            
            lapCounter ++;

            std::cout << std::endl;
            std::cout << "Runde: " << lapCounter << std::endl;
            newLapStarted = false;

            if (lapCounter > 3) {
                std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                std::cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

                for (int index = 0; index < maxLapCount; index++) {
                    std::cout << "Runde " << index + 1 << ": " << lapTimes[index] << std::endl;
                }
            }
        }

        // starts countdown for displayint finish label 
        if (!raceFinished && lapCounter > maxLapCount) {
            countDownBegin = std::chrono::steady_clock::now();
        }
        if (lapCounter > maxLapCount) {
            countDownEnd = std::chrono::steady_clock::now();

            float passedCountdownSeconds = std::chrono::duration_cast<std::chrono::seconds>(countDownEnd - countDownBegin).count();

            /*std::cout << "passedCountdownSeconds: " << passedCountdownSeconds << std::endl;*/

            if (passedCountdownSeconds > 2) {
                raceStarted = false;
                gameStarted = false;
                gameIsPaused = true;
            }

            
        }

        
        // hides the cursor 
        if (!gameIsPaused) {
            while (ShowCursor(false) >= 0);
        }
        else {
            while (ShowCursor(true) <= 0);
            //ShowCursor(true);
        }


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
        Eye[1] = 0.0f; At[1] = 0.0f; Up[1] = 1.0f;
        Eye[2] = cameraZValue; At[2] = 0.0f; Up[2] = 0.0f;
        //Eye[2] = -5.0f; At[2] = 0.0f; Up[2] = 0.0f;

        GetViewMatrix(Eye, At, Up, ViewMatrix);


        // camera controlls

        if (cameraIsMoving) {
            cameraNotMovingTimerStarted = false;
        }
        else {
            if (!cameraNotMovingTimerStarted) {
                cameraNotMovingTimerBegin = std::chrono::steady_clock::now();
                
                cameraNotMovingTimerStarted = true;
            }
            cameraNotMovingTimerEnd = std::chrono::steady_clock::now();

            if (std::chrono::duration_cast<std::chrono::seconds>(cameraNotMovingTimerEnd - cameraNotMovingTimerBegin).count() > cameraResetTime && !cameraIsMoving) {
                cameraYAngle = cameraYAngle + 0.075f * (0 - cameraYAngle);
            }
            
            
        }

        // smooth camera following
        if (!gameIsPaused && !raceFinished && abs(cameraZValue) > 1 ) {
            cameraFollowYRotation = cameraFollowYRotation + 0.1f * (kart->yRotationAngle - cameraFollowYRotation);
            cameraFollowPosition.x = cameraFollowPosition.x + 0.2f * (kart->position.x - cameraFollowPosition.x);
            cameraFollowPosition.y = cameraFollowPosition.y + 0.2f * (kart->position.y - cameraFollowPosition.y);
            cameraFollowPosition.z = cameraFollowPosition.z + 0.2f * (kart->position.z - cameraFollowPosition.z);
        }

        // hart camera following when fully zoomed in (first person)
        if (abs(cameraZValue) == 1) {
            cameraFollowYRotation = kart->yRotationAngle;
            cameraFollowPosition.x = kart->position.x;
            cameraFollowPosition.y = kart->position.y;
            cameraFollowPosition.z = kart->position.z;
        }
        

        GetTranslationMatrix(-cameraFollowPosition.x, cameraFollowPosition.y - 1.5, -cameraFollowPosition.z, TranslationMatrix);
        GetRotationXMatrix(cameraXAngle - 0, RotationXMatrix);
        GetRotationYMatrix(-cameraFollowYRotation + cameraYAngle, RotationYMatrix);
        GetRotationZMatrix(0.0f, RotationZMatrix);

        MulMatrix(RotationYMatrix, RotationXMatrix, RotationMatrix);

        MulMatrix(RotationMatrix, ViewMatrix, ViewMatrix);

        MulMatrix(TranslationMatrix, ViewMatrix, ViewMatrix);


        SetViewMatrix(ViewMatrix);

        // handles karts movement & behavior
        kart->update();

       
        return true;
    }

    bool CApplication::InternOnFrame()
    {
        float WorldMatrix[16];

        float ButtonStartContinueGameMatrix[16];
        float ButtonQuitGameMatrix[16];
        float ButtonControllsMatrix[16];

        float CountDownDisplayMatrix[16];

        float ScaleMatrix[16];
        float TranslationMatrix[16];
        float RotationMatrix[16];
        float RotationXMatrix[16];
        float RotationYMatrix[16];


        // environment
        GetTranslationMatrix(0.0f, 0.0f, 0.0f, WorldMatrix);
        SetWorldMatrix(WorldMatrix);

        if (m_pStrecke != nullptr) DrawMesh(m_pStrecke);
        if (m_pBorder_White != nullptr) DrawMesh(m_pBorder_White);
        if (m_pBorder_Red != nullptr) DrawMesh(m_pBorder_Red);
        if (m_pSaeulen != nullptr) DrawMesh(m_pSaeulen);
        if (m_pBanner != nullptr) DrawMesh(m_pBanner);



        // Finishline 
        GetTranslationMatrix(0.0f, 0.0f, 2.3877f, WorldMatrix);
        SetWorldMatrix(WorldMatrix);

        if (m_pZiellinie != nullptr) DrawMesh(m_pZiellinie);

       
        // sky
        GetTranslationMatrix(cameraFollowPosition.x, cameraFollowPosition.y, cameraFollowPosition.z, WorldMatrix);
        /*if (!raceFinished) {
            GetTranslationMatrix(kart->position.x, kart->position.y, kart->position.z, WorldMatrix);
        }
        else {
            GetTranslationMatrix(cameraFollowPosition.x, cameraFollowPosition.y, cameraFollowPosition.z, WorldMatrix);
        }*/
       
        SetWorldMatrix(WorldMatrix);

        if (m_pUmgebung != nullptr) DrawMesh(m_pUmgebung);


        // menu buttons and HUD position relativ to camera angle and position
        GetRotationXMatrix(-cameraXAngle - 0, RotationXMatrix);
        GetRotationYMatrix(cameraFollowYRotation - cameraYAngle, RotationYMatrix);
        MulMatrix(RotationXMatrix, RotationYMatrix, RotationMatrix);

        if (gameIsPaused) {
            if (controllsMenuOpen) {

                // "CONTROLLS" view
                GetTranslationMatrix(0.0f + cameraFollowPosition.x, 1.3f, 0.0f + cameraFollowPosition.z , WorldMatrix);
                MulMatrix(RotationMatrix, WorldMatrix, WorldMatrix);

                GetTranslationMatrix(0.0f, 0.0f, -2.0f + cameraZValue + 5, ButtonControllsMatrix);
                MulMatrix(ButtonControllsMatrix, WorldMatrix, ButtonControllsMatrix);
                SetWorldMatrix(ButtonControllsMatrix);

                if (m_pControlls != nullptr) DrawMesh(m_pControlls);
            }
            else {

                // "START/CONTINUE" button
                GetTranslationMatrix(0.0f + cameraFollowPosition.x, 1.5f, 0.0f + cameraFollowPosition.z, WorldMatrix);
                MulMatrix(RotationMatrix, WorldMatrix, WorldMatrix);

                GetTranslationMatrix(0.0f, 1.0f, -2.0f + cameraZValue + 5, ButtonStartContinueGameMatrix);
                MulMatrix(ButtonStartContinueGameMatrix, WorldMatrix, ButtonStartContinueGameMatrix);
                SetWorldMatrix(ButtonStartContinueGameMatrix);

                
                if (buttonStartContinueGameHighlighted) {
                    if (gameStarted) {
                        if (m_pButton_Continue_Yellow != nullptr) DrawMesh(m_pButton_Continue_Yellow);
                    }
                    else {
                        if (m_pButton_Start_Game_Yellow != nullptr) DrawMesh(m_pButton_Start_Game_Yellow);
                    }

                }
                else {
                    if (gameStarted) {
                        if (m_pButton_Continue_Grey != nullptr) DrawMesh(m_pButton_Continue_Grey);
                    }
                    else {
                        if (m_pButton_Start_Game_Grey != nullptr) DrawMesh(m_pButton_Start_Game_Grey);
                    }

                }

                // "CONTROLLS" button
                GetTranslationMatrix(0.0f, 0.0f, -2.0f + cameraZValue + 5, ButtonControllsMatrix);
                MulMatrix(ButtonControllsMatrix, WorldMatrix, ButtonControllsMatrix);
                SetWorldMatrix(ButtonControllsMatrix);

                if (buttonControllsHighlighted) {
                    if (m_pButton_Controlls_Yellow != nullptr) DrawMesh(m_pButton_Controlls_Yellow);
                }
                else {
                    if (m_pButton_Controlls_Grey != nullptr) DrawMesh(m_pButton_Controlls_Grey);
                }

                // "QUIT"
                GetTranslationMatrix(0.0f, -1.0f, -2.0f + cameraZValue + 5, ButtonQuitGameMatrix);
                MulMatrix(ButtonQuitGameMatrix, WorldMatrix, ButtonQuitGameMatrix);
                SetWorldMatrix(ButtonQuitGameMatrix);

                if (buttonQuittGameHighlighted) {
                    if (m_pButton_Quit_Game_Yellow != nullptr) DrawMesh(m_pButton_Quit_Game_Yellow);
                }
                else {
                    if (m_pButton_Quit_Game_Grey != nullptr) DrawMesh(m_pButton_Quit_Game_Grey);
                }
            }
        }
        else {

            // countdown labels
            GetTranslationMatrix(0.0f + cameraFollowPosition.x, 1.7f, 0.0f + cameraFollowPosition.z, WorldMatrix);
            MulMatrix(RotationMatrix, WorldMatrix, WorldMatrix);

            GetTranslationMatrix(0.0f, 0.0f, -2.0f + cameraZValue + 5, CountDownDisplayMatrix);
            MulMatrix(CountDownDisplayMatrix, WorldMatrix, CountDownDisplayMatrix);
            SetWorldMatrix(CountDownDisplayMatrix);

            // displays countdown 
            if (!raceStarted && lapCounter == 0) {
                
                float passedCountdownSeconds = std::chrono::duration_cast<std::chrono::seconds>(countDownEnd - countDownBegin).count();

                if (passedCountdownSeconds < 1) {
                    if (m_pCountDown_Three != nullptr) DrawMesh(m_pCountDown_Three);
                }
                else if (passedCountdownSeconds < 2) {
                    if (m_pCountDown_Two != nullptr) DrawMesh(m_pCountDown_Two);
                }
                else if (passedCountdownSeconds < 3) {
                    if (m_pCountDown_One != nullptr) DrawMesh(m_pCountDown_One);

                }
                else if (passedCountdownSeconds < 4) {
                    if (m_pCountDown_Gooooo != nullptr) DrawMesh(m_pCountDown_Gooooo);
                }
            }


            if (raceStarted && lapCounter > maxLapCount) {
                if (m_pCountDown_Finish != nullptr) DrawMesh(m_pCountDown_Finish);
                raceFinished = true;
            }
        }

        // draws the kart
        if (kart != nullptr) kart->DrawMeshes();
        

        return true;
    }
}

void main()
{
    CApplication Application;

    RunApplication(1920, 1080, "Marius Kart", &Application);

}