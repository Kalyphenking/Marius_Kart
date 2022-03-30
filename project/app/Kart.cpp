#include "Kart.h"
#include "MeshLoader.h"


using namespace gfx;
#define PI 3.14159265

enum DrivingDirection { forward, backward, none };
enum StearingDirection { left, right, straight };

BHandle m_pKart_Rahmen = nullptr;
BHandle m_pKart_Schweller = nullptr;
BHandle m_pKart_Stoﬂstangen = nullptr;
BHandle m_pKart_Sitz = nullptr;
BHandle m_pKart_Stearing_Wheel = nullptr;
BHandle m_pKart_Fahrer = nullptr;

BHandle m_pKart_Motor = nullptr;
BHandle m_pKart_Kette_Antrieb = nullptr;
BHandle m_pKart_Zahnrad_Achse = nullptr;


BHandle m_pKart_Reifen_Vorne_Rechts = nullptr;
BHandle m_pKart_Reifen_Vorne_Links = nullptr;
BHandle m_pKart_Reifen_Hinten = nullptr;

BHandle m_pKart_Felgen_Vorne_Rechts = nullptr;
BHandle m_pKart_Felgen_Vorne_Links = nullptr;
BHandle m_pKart_Felgen_Hinten = nullptr;

Kart::vec3d offsetPosition = { 0.0f, 0.69, 0.0f };
Kart::vec3d position;

DrivingDirection drivingDirection;
StearingDirection stearingDirection;

int speedClass = 0;
int priviousSpeedClass = -1;


int engineCounter = 0;
int engineDelay = 60;

float stearingAngle;
float driftAngle = 0.0f; // DRIFT COMMING SOON

float maxStearingAngle = 30.0f;
float speed = 0.0f;
float maxSpeed = 0.4f;



float acceleration = 0.003f;
float deceleration = 0.002f;
float breakDeceleration = 0.004f;
float stearingAcceleration = 10.0f;
float wheelbase = 2.217f;

double yRotationAngle;


bool drift = false;
bool* gameIsPaused;
bool* raceStarted;

bool engineRuning = false;

Kart::Kart(bool* gameIsPaused, bool* raceStarted, float* cameraZValue)
: position{ 0.0f, 0.0f, 0.0f }
, yRotationAngle(0.0f)
, gameIsPaused(gameIsPaused)
, raceStarted(raceStarted)
, cameraZValue(cameraZValue)
{
}

//Kart::~Kart();

// comming soon
bool engineSound() {

    if (engineRuning) {
        engineDelay = 15 / (1 + (abs(speed) * 2));
        std::cout << "engineDelay: " << engineDelay << "; engineCounter: " << engineCounter << std::endl;

        
        // Elektro Sound

        /*if (engineCounter == engineDelay) {
            PlaySound(L"../project/src/sounds/EnergySound", NULL, SND_ASYNC);
        }*/

        // Combustion Sound

        if (engineCounter == engineDelay / 2 * 1) {
            PlaySound(L"../project/src/sounds/SingleExplosion", NULL, SND_ASYNC);
        }
        if (engineCounter == engineDelay / 2 * 2) {
            PlaySound(L"../project/src/sounds/SmallExplosion", NULL, SND_ASYNC);
        }

        if (engineCounter >= engineDelay) {
            engineCounter = 0;
        }

        engineCounter++;
    }

/*if (engineCounter == engineDelay / 3 * 2) {
    PlaySound(L"../project/src/sounds/SingleExplosion", NULL, SND_ASYNC);
    std::cout << "Bumm2\n";
}*/



return true;
}


bool Kart::ResetPosition() {
    position = { 0.0f, 0.0f, 0.0f };
    yRotationAngle = 0.0f;

    return true;
}


// called on InternOnUpdate
bool Kart::update() {

    // engineSound();
    if (GetKeyState('E') & 0x8000)
    {
        //drift = true;

        //engineRuning = !engineRuning;
    }


    if (!*gameIsPaused && *raceStarted) {

        // gas & break

        if (GetKeyState(VK_SPACE) & 0x8000) {
            if (speed > 0.0f) {
                speed -= breakDeceleration;
            }
            if (speed < 0.0f) {

                speed += breakDeceleration;
            }
        }
        else {
            if (GetKeyState('W') & 0x8000)
            {
                drivingDirection = forward;
                if (speed < 0) {

                    speed += breakDeceleration;
                }
                else if (speed <= maxSpeed) {
                    speed += acceleration;
                }
            }
            else if (GetKeyState('S') & 0x8000)
            {
                drivingDirection = backward;

                if (speed > 0) {
                    speed -= breakDeceleration;
                }
                else if (speed >= -maxSpeed) {
                    speed -= acceleration;
                }
            }
            else if (speed > 0.001) {
                speed -= deceleration;
            }
            else if (speed < -0.001) {
                speed += deceleration;
            }
        }

        if (-0.001 <= abs(speed) && abs(speed) <= 0.001) {
            drivingDirection = none;
            speed = 0;
        }



        // DRIFT COMMING SOON
        
        //if (GetKeyState(VK_SHIFT) & 0x8000)
        //{
        //    

        //    if (stearingDirection == right && !drift) {
        //        driftAngle = -5;
        //    }
        //    else if (stearingDirection == left && !drift) {
        //        driftAngle = +5;
        //    }
        //    //bool played = PlaySound(L"../project/src/sounds/EngineStart", NULL, SND_ASYNC);
        //    drift = true;

        //    //engineRuning = !engineRuning;
        //}
        //else {
        //    driftAngle = 0;
        //    drift = false;
        //}

        /*if (!drift) {
            driftAngle = 0;
        }*/
        /*if (drift) {
            maxStearingAngle = 30;
        }*/




        // Stearing
        maxStearingAngle = -160 * pow(speed, 2) + 30;

        for (int index = 0; index < stearingAcceleration; index++) {
            if (abs(stearingAngle) <= maxStearingAngle) {
                if (GetKeyState('D') & 0x8000 && GetKeyState('A') & 0x8000) {
                    stearingAngle = 0;
                }
                else if (GetKeyState('D') & 0x8000)
                {
                    stearingDirection = left;

                    // DRIFT COMMING SOON
                    
                    //for (int index = 0; index < stearingAcceleration; index++) {

                    //}


                    /*if (stearingAngle > maxStearingAngle) {
                        stearingAngle = maxStearingAngle;
                    }*/
                    stearingAngle += 5 / stearingAcceleration - (drift ? 0.5 : 0);
                }
                else if (GetKeyState('A') & 0x8000)
                {
                    stearingDirection = right;

                    // DRIFT COMMING SOON

                    //for (int index = 0; index < stearingAcceleration; index++) {

                    //if (stearingAngle > -maxStearingAngle) {
                    //    
                    //    //stearingAngle -= stearingAcceleration;
                    //}
                    //}


                    /*if (stearingAngle > maxStearingAngle) {
                        stearingAngle = -maxStearingAngle;
                    }*/
                    stearingAngle -= 5 / stearingAcceleration + (drift ? 0.5 : 0);
                }
                else {
                    stearingDirection = straight;
                    if (stearingAngle >= -2.0f && stearingAngle <= 2.0f) {

                        stearingAngle = 0;
                    }
                    else if (stearingAngle > 1) {
                        stearingAngle -= 1;
                    }
                    else if (stearingAngle < -1) {
                        stearingAngle += 1;
                    }
                }
            }
            else {
                if (stearingDirection == right) {
                    stearingAngle = -maxStearingAngle;
                }
                if (stearingDirection == left) {
                    stearingAngle = maxStearingAngle;
                }

            }

        }


        position.z += speed * cos((yRotationAngle) / 180 * PI);
        position.x += speed * sin((yRotationAngle) / 180 * PI);
    }

    return true;
}

// called on InternOnCreateMeshes
bool Kart::CreateMeshes() {
    
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Rahmen.x", &m_pKart_Rahmen);

    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Schweller.x", &m_pKart_Schweller);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Stossstangen.x", &m_pKart_Stoﬂstangen);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Sitz.x", &m_pKart_Sitz);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Fahrer.x", &m_pKart_Fahrer);

    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Stearing_Wheel.x", &m_pKart_Stearing_Wheel);

    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Motor.x", &m_pKart_Motor);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Kette_Antrieb.x", &m_pKart_Kette_Antrieb);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Zahnrad_Achse.x", &m_pKart_Zahnrad_Achse);

    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Reifen_Vorne_Rechts.x", &m_pKart_Reifen_Vorne_Rechts);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Reifen_Vorne_Links.x", &m_pKart_Reifen_Vorne_Links);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Reifen_Hinten.x", &m_pKart_Reifen_Hinten);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Felgen_Vorne_Rechts.x", &m_pKart_Felgen_Vorne_Rechts);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Felgen_Vorne_Links.x", &m_pKart_Felgen_Vorne_Links);
    async(std::launch::async, LoadFromObjectFile, "../project/src/models/Kart_Felgen_Hinten.x", &m_pKart_Felgen_Hinten);

    return true;
}


// called on InternOnFrame
bool Kart::DrawMeshes() {

    float WorldMatrix[16];

    float WorldMatrixTireRight[16];
    float WorldMatrixTireLeft[16];
    float WorldMatrixStearingWheel[16];

    float ScaleMatrix[16];
    float TranslationMatrix[16];

    float TranslationMatrixStearing[16];

    float RotationMatrix[16];

    float RotationMatrixTireRight[16];
    float RotationMatrixTireLeft[16];
    float RotationMatrixStearingWheel[16];

    if (stearingAngle != 0) {
        yRotationAngle += (speed / (wheelbase / sin((stearingAngle) / 180 * PI))) / PI * 180;
        //yRotationAngle += (speed / (wheelbase / sin((stearingAngle + driftAngle) / 180 * PI))) / PI * 180; // drift coming soon
    }
    

    // kart and driver
    GetTranslationMatrix(
        position.x + offsetPosition.x,
        position.y + offsetPosition.y,
        position.z + offsetPosition.z,
        WorldMatrix
    );

   
    GetRotationYMatrix(yRotationAngle, RotationMatrix);

    MulMatrix(RotationMatrix, WorldMatrix, WorldMatrix);

    SetWorldMatrix(WorldMatrix);

    if (m_pKart_Rahmen != nullptr) DrawMesh(m_pKart_Rahmen);
    if (m_pKart_Schweller != nullptr) DrawMesh(m_pKart_Schweller);
    if (m_pKart_Stoﬂstangen != nullptr) DrawMesh(m_pKart_Stoﬂstangen);
    if (m_pKart_Sitz != nullptr) DrawMesh(m_pKart_Sitz);

    // hides Driver when cam is closest (firstPerson)
    if (m_pKart_Fahrer != nullptr && abs(*cameraZValue) > 1) DrawMesh(m_pKart_Fahrer);
    if (m_pKart_Motor != nullptr) DrawMesh(m_pKart_Motor);
    if (m_pKart_Kette_Antrieb != nullptr) DrawMesh(m_pKart_Kette_Antrieb);
    if (m_pKart_Zahnrad_Achse != nullptr) DrawMesh(m_pKart_Zahnrad_Achse);

    if (m_pKart_Reifen_Hinten != nullptr) DrawMesh(m_pKart_Reifen_Hinten);
    if (m_pKart_Felgen_Hinten != nullptr) DrawMesh(m_pKart_Felgen_Hinten);



    // left front tires
    GetTranslationMatrix(
        0.68,
        0,
        1.12,
        TranslationMatrix
    );
    GetRotationYMatrix(stearingAngle, RotationMatrixTireRight);
    MulMatrix(RotationMatrixTireRight, TranslationMatrix, WorldMatrixTireRight);
    MulMatrix(WorldMatrixTireRight, WorldMatrix, WorldMatrixTireRight);

    SetWorldMatrix(WorldMatrixTireRight);

  
    if (m_pKart_Felgen_Vorne_Links != nullptr) DrawMesh(m_pKart_Felgen_Vorne_Links);
    if (m_pKart_Reifen_Vorne_Links != nullptr) DrawMesh(m_pKart_Reifen_Vorne_Links);



    // right front tires
    GetTranslationMatrix(
        -0.68,
        0,
        1.12,
        TranslationMatrix
    );

    GetRotationYMatrix(stearingAngle, RotationMatrixTireLeft);
    MulMatrix(RotationMatrixTireLeft, TranslationMatrix, WorldMatrixTireLeft);
    MulMatrix(WorldMatrixTireLeft, WorldMatrix, WorldMatrixTireLeft);

    SetWorldMatrix(WorldMatrixTireLeft);

    if (m_pKart_Felgen_Vorne_Rechts != nullptr) DrawMesh(m_pKart_Felgen_Vorne_Rechts);
    if (m_pKart_Reifen_Vorne_Rechts != nullptr) DrawMesh(m_pKart_Reifen_Vorne_Rechts);


    // rear tires
    GetTranslationMatrix(
        0,
        0.33f,
        0.23f,
        TranslationMatrix
    );

    GetRotationXMatrix(38, RotationMatrixStearingWheel);
    MulMatrix(RotationMatrixStearingWheel, TranslationMatrix, TranslationMatrix);

    GetRotationZMatrix(-stearingAngle, RotationMatrixStearingWheel);
    MulMatrix(RotationMatrixStearingWheel, TranslationMatrix, WorldMatrixStearingWheel);

    MulMatrix(WorldMatrixStearingWheel, WorldMatrix, WorldMatrixStearingWheel);

    SetWorldMatrix(WorldMatrixStearingWheel);

    if (m_pKart_Stearing_Wheel != nullptr) DrawMesh(m_pKart_Stearing_Wheel);

    return true;
}

// called on InternOnReleaseMeshes
bool Kart::ReleaseMeshes() {

    if (m_pKart_Rahmen != nullptr) ReleaseMesh(m_pKart_Rahmen);
    if (m_pKart_Schweller != nullptr) ReleaseMesh(m_pKart_Schweller);
    if (m_pKart_Stoﬂstangen != nullptr) ReleaseMesh(m_pKart_Stoﬂstangen);
    if (m_pKart_Sitz != nullptr) ReleaseMesh(m_pKart_Sitz);
    if (m_pKart_Sitz != nullptr) ReleaseMesh(m_pKart_Stearing_Wheel);
    if (m_pKart_Sitz != nullptr) ReleaseMesh(m_pKart_Fahrer);

    if (m_pKart_Motor != nullptr) ReleaseMesh(m_pKart_Motor);
    if (m_pKart_Motor != nullptr) ReleaseMesh(m_pKart_Kette_Antrieb);
    if (m_pKart_Motor != nullptr) ReleaseMesh(m_pKart_Zahnrad_Achse);

    if (m_pKart_Reifen_Vorne_Rechts != nullptr) ReleaseMesh(m_pKart_Reifen_Vorne_Rechts);
    if (m_pKart_Reifen_Vorne_Links != nullptr) ReleaseMesh(m_pKart_Reifen_Vorne_Links);
    if (m_pKart_Reifen_Hinten != nullptr) ReleaseMesh(m_pKart_Reifen_Hinten);
    if (m_pKart_Felgen_Vorne_Rechts != nullptr) ReleaseMesh(m_pKart_Felgen_Vorne_Rechts);
    if (m_pKart_Felgen_Vorne_Links != nullptr) ReleaseMesh(m_pKart_Felgen_Vorne_Links);
    if (m_pKart_Felgen_Hinten != nullptr) ReleaseMesh(m_pKart_Felgen_Hinten);

    return true;
}
