#ifdef __APPLE__
    #include <GLUT/glut.h>
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
		#include <gl.h>
		#include <glu.h>
#endif


#define MAXBIRDS 800
#define MAX_BIRDS_TO_FOLLOW 20
#define FRAMES_PER_SEC 20
#define CIRCLE_STEPS 36

#define MAX_STEPS_BUFFER 1000

#define FOLLOW_FRIENDS_FACTOR 3

#define EAT_DIST 0.003

#define MAX_OBSTACLES 2
bool obstacleTest = false;  // test if a bird can pass through a wall
float LETHAL_DISTANCE = 0.005;  // 0.00025?

bool obstacleTest2 = false;  // test the calculation of the vector resultant from an obstacle
// components of vector used in obst. test 2
float obstacleTest2VectX;
float obstacleTest2VectY;
float obstacleTest2PosX;
float obstacleTest2PosY;
bool useCampo = true;


bool stop = false;
bool errorStart = false;

bool noBounds = false;    // if false the birds will die exiting the area, teleport elsewhere

int DEBUG = 0;

int startbirds;


int numberOfObstaclesTests = 1;

bool paused = false;
int old_millsec = 1000;
int millsec_per_frame = 1000 / FRAMES_PER_SEC;

float pointSize = 4.0;
float lineWidth = 4.0;

float RedRGB = 1.0;
float GreenRGB = 0.0;
float BlueRGB = 0.0;

GLenum primitiv;

int showHideSubMenu;

bool whiteBackground = false;

bool showSightCircle = true;
bool showObstCircle = false;
bool showDistCircles = false;

bool axes = false;
bool nativeColor = false;
bool dinamicColor = false;
bool showSpeed = true;
bool colorSpeed = true;
bool relativeColorSpeed = false;
bool showNearBirdsAverage = false;
bool showSteps = false;
bool showIDs = false;


bool followFriends = false;

float mouseX = 0.0;
float mouseY = 0.0;

int attribMenu;

int birdNum = 0;
class bird * birds[MAXBIRDS];
int liveBirdNum = 0;
int selectedBird = -1;


//ostacoli
float obstacles [MAX_OBSTACLES][4];
int obstNum = 0;


//stats
float topAccel = 0.0;
float topSpeed = 0.0;
float topSight = 0.0;
float averageSpeed = 0.0;
long int unsigned cicli = 0;

float xUserAccel = 0.0;
float yUserAccel = 0.0;