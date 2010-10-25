
extern "C" {
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <time.h>
#include <math.h>
}

#include <sstream>
#include <iostream>

#include "pointLineSegmentDistance.h"

using namespace std;

#ifdef __APPLE__
    #include <GLUT/glut.h>
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
		#include <gl.h>
		#include <glu.h>
#endif



//#define DEBUG 0

// compilare con:    g++ -framework GLUT -framework OpenGL birds.c -o birds
// eseguire con:		 ./birds [n]   (con n = num. di uccelli)

/* cose da fare:
 * v- OSTACOLI: Segmenti che gli uccelli devono evitare (tramite un'apposito calcolo) per non incontrare la morte.
 * v- STEPS PER TUTTI: Inserire nella classe bird un vettore che mantenga lo storico delle posizioni (quindi un percorso).
 *                    Con possibilità di visualizzare tutti i percorsi insieme.
 * - ANALISI PERCORSI PREFERITI: Mappare la presenza degli uccelli durante il tempo suddividendo l'area in tanti quadrati
 *															 e per ogni quadrato tenere un contatore. Incrementare il contatore aggiungendo il numero
 *															 di uccelli interni al quadrato relativo ogni istante di tempo (o anche più raramente se
 *															 i quadrati sono abbastanza grandi).
 * - MIGLIORIE GRAFICHE: Spostare le statistiche dal terminale a un'area della finestra.
 *											 Inserire pulsanti nella toolbar che emulino le più importanti voci del menu popup.
 * - RIPRODUZIONE: Permettere l'evoluzione dei caratteri degli uccelli tramite la riproduzione e la "selezione naturale".
 * - PREDATORI: Altre creature che inseguono e mangiano gli uccelli (l'evoluzione cambia?).
 */


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

//stats
float topAccel = 0.0;
float topSpeed = 0.0;
float topSight = 0.0;
float averageSpeed = 0.0;
long int unsigned cicli = 0;

float xUserAccel = 0.0;
float yUserAccel = 0.0;

//ostacoli
int obstNum = 0;
float obstacles [MAX_OBSTACLES][4];


struct step {
   float x,y, speed;
   struct step * next;
};

/*
step * steplist = NULL;
int stepNum = 0;


void pushStep(float x, float y, float speed)
{
	step* first = steplist;
	
	steplist = (step*)malloc(sizeof(step));
	
	steplist->x = x;
	steplist->y = y;
	steplist->speed = speed;
	steplist->next = first;
	
	stepNum++;
}

void clearStepList()
{
	stepNum = 0;
	step* erase;
	
	while (steplist != NULL)
	{
		erase = steplist;
		steplist = steplist->next;
		free(erase);
	}
	
}*/




void reduceVectComponentsToMax (float * vx, float * vy, float max) //modificata il 13.9.10
{
	/*if((*vx) >= 0)
		(*vx) = ((*vx) * max) / ( sqrt( pow( (*vx) , 2) + pow( (*vy) , 2) ) );
	else
		(*vx) = ((*vx) * max) / ( sqrt( pow( (*vx) , 2) + pow( (*vy) , 2) ) );
		
	if((*vy) >= 0)
		(*vy) = sqrt( pow( max , 2) - pow( (*vx) , 2) );
	else
		(*vy) = - sqrt( pow( max , 2) - pow( (*vx) , 2) );*/


  if((*vx) != 0.0 && (*vy) != 0.0) {
    float c = sqrt((*vx)*(*vx)+(*vy)*(*vy));

    if(DEBUG) {printf("\nDEBUG reduceVectComponentsToMax C=%f!!!\n",c); fflush(stdout);}

    (*vx) = (*vx)*max/c;
    (*vy) = (*vy)*max/c;
  }
}







 /*returns a float between m and M     ( with (|M-m|)/step +1 different chanches.
  *										 Ex: m=2.0, M=1.0, step=0.2  means
  *									     (|1.0-2.0|)/0.2 +1 = 6 chanches (1.0, 1.2, 1.4, 1.6, 1.8, 2.0) )
  */
float random(float m, float M, float step) { 
		if(m>M) {
			float temp = M;
			M = m;
			m = temp;
		}
		
		float randomN;
		
		int NoS;
		step = fabs(step);
		
		if (step != 0.0)
			NoS = (int) ((M-m)/step) +1; // Number of Steps
		else
			return 0.0;
		
		randomN = (rand()%(NoS))*step + m;
		
		if (randomN > M)
			randomN = M;
		
		if(m+step <= M)
			return randomN;
		else
			return 0.0;
	}


void randomVector(float * vx, float * vy, float max) {
	(*vx) = random(-0.1,0.1,0.00001);
	(*vy) = random(-0.1,0.1,0.00001);

    if(DEBUG) {printf("\nDEBUG randomVector vx=%f vy=%f\n",(*vx),(*vy)); fflush(stdout);}
	reduceVectComponentsToMax(vx,vy,max);
}


void randomObstacles(void) {
  obstNum=0;
  for(int i=0; i<MAX_OBSTACLES; i++) {  // creo gli ostacoli a caso
      for(int j=0; j<4; j++)
        obstacles[i][j] = random(-1.0,1.0,0.01);
      obstNum++;
    }
  
}



float segmentToPointDistance(float x1, float y1, float x2, float y2, float X, float Y, float  &length, float &det) { //scritta il 13.9.10
	float vx = x1-X, vy = y1-Y, ux = x2-x1, uy = y2-y1;
	length = ux*ux+uy*uy;

	det = (-vx*ux)+(-vy*uy); //if this is < 0 or > length then its outside the line segment
  if(det<0 || det>length)
  {
    ux=x2-X;
    uy=y2-Y;
    return std::min<float>(vx*vx+vy*vy, ux*ux+uy*uy);
  }

  det = ux*vy-uy*vx;
  return (det*det)/length;
}


float pointToPointDistance(float x1, float y1, float x2, float y2) { //scritta il 13.9.10
  return sqrt( pow(x2-x1,2) + pow(y2-y1,2));
}


// salva in Vx e Vy le componenti del vettore diretto lungo la retta AB  (A(x1,y1), B(x2,y2)) con verso A -> B
void getVectComponentsWith2P(float x1, float y1, float x2, float y2, float Vlength, float * Vx, float * Vy) { //scritta il 13.9.10
	float coeffAng_num = y2-y1;
	float coeffAng_den = x2-x1;
	
	if (coeffAng_den == 0.0) {
		if(DEBUG) { printf("\nDEBUG getVectComponentsWith2P. coeffAng_den = 0"); fflush(stdout);};
		(*Vx)=0.0;
		
		if(y2>y1)
			(*Vy)=Vlength;
		else 
			(*Vy)=-Vlength;
		
		return;
	} else if(coeffAng_num == 0.0) {
		if(DEBUG) { printf("\nDEBUG getVectComponentsWith2P. coeffAng_num = 0"); fflush(stdout);};
		(*Vy)=0.0;
		
		if(x2>x1)
			(*Vx)=Vlength;
		else 
			(*Vx)=-Vlength;
		
		return;
	}
	
	
	
	float coeffAng = coeffAng_num/coeffAng_den;
	if(DEBUG) { printf("\nDEBUG2 getVectComponentsWith2P. coeffAng = %.4f", coeffAng); fflush(stdout);};
	
	
	float alpha = atan(coeffAng);
	
	if(x2<x1 ) {
	(*Vx) = -cos(alpha) * Vlength;
	(*Vy) = -sin(alpha) * Vlength;
	} else {
	(*Vx) = cos(alpha) * Vlength;
	(*Vy) = sin(alpha) * Vlength;
	}
	
}


// restituisce le coordinate del punto P, proiezione di A sul segmento (x1,y1)(x2,y2)
void getProjectionOfPointAOnSegment(float x1, float y1, float x2, float y2, float Ax, float Ay, float * Px, float * Py) {  //scritta il 13.9.10
  float r_numerator = (Ax-x1)*(x2-x1) + (Ay-y1)*(y2-y1);
  float r_denomenator = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
  float r = r_numerator / r_denomenator;

  (*Px) = x1 + r*(x2-x1);
  (*Py) = y1 + r*(y2-y1);
}


class bird
{
	
	public:
	
	
	bool live;
	
	bool hawk;
	
	int id;
	long int unsigned birthday;

    int stomach;

	float posX;
	float posY;
	
	float xAccel;
	float yAccel;
	
	float maxAccel;
	
	float maxSpeed;
	
	float minDist;
	float maxDist;
	
	float obstDist; //la distanza a cui l'uccello tiene gli ostacoli
	
	float speed;
	
	float xSpeed;
	float ySpeed;
	
	float sight;
	
	float independence;
	
	float Rcolor;
	float Gcolor;
	float Bcolor;
	
	float nearBirdsX;
	float nearBirdsY;
	
	float nearBirdsXspeed;
	float nearBirdsYspeed;
	
	float nearBirdsXYDist;
	int nearBirdsNum;
	float distFromNearBirds[MAX_BIRDS_TO_FOLLOW];
	int nearBirds[MAX_BIRDS_TO_FOLLOW];


    step * steplist;
    int stepNum;


    void pushStep(float x, float y, float speed)
    {


      step* first = steplist;


      steplist = (step*)malloc(sizeof(step));

      steplist->x = x;
      steplist->y = y;
      steplist->speed = speed;
      steplist->next = first;

      stepNum++;


      if(stepNum > MAX_STEPS_BUFFER) { // se sono troppi cancella lo step più vecchio
        step* temp;
        temp = steplist;
        while (temp->next->next != NULL) // finchè steplist non punta il penultimo elemento
        {
          temp = temp->next;
        }

        free(temp->next);
        temp->next = NULL;

        stepNum--;

      }

    }

    void clearStepList()
    {
      stepNum = 0;
      step* erase;

      while (steplist != NULL)
      {
        erase = steplist;
		steplist = steplist->next;
		free(erase);
      }

    }

	
	
	bird (int ID) {
        stepNum = 0;
        steplist = NULL;

        stomach = 0;

		live = true;
		
		id = ID;
		birthday = cicli;
		
		if(obstacleTest) {
			posX = random(-0.9, 0.9, 0.001);
			posY = random(posX, 0.91, 0.001)+0.005;
		} else {
			posX = random(-0.9, 0.9, 0.001);
			posY = random(-0.9, 0.9, 0.001);
		}
		
		maxAccel = random(0.001, 0.003, 0.00005);
		
		maxSpeed = random(0.0015, 0.0075, 0.000025);
		
		minDist = random(0.03, 0.07, 0.001);
		maxDist = random(0.1, 0.15, 0.001);
		
		obstDist = random(0.08, 0.15, 0.001);
		
		
		xAccel = yAccel = 0.0;
		
		//xSpeed = random(-maxSpeed, maxSpeed, 0.00001);
		//ySpeed = random(-sqrt(pow(maxSpeed,2)-pow(xSpeed,2)), sqrt(pow(maxSpeed,2)-pow(xSpeed,2)), 0.00001);
		randomVector(&xSpeed,&ySpeed,maxSpeed);
		//xSpeed = 0.0;
		//ySpeed = 0.0;
		
		
		speed = sqrt(pow(xSpeed,2) + pow(ySpeed,2));
		
		sight = random(0.2, 0.4, 0.01);
		
		independence = random(0.8,1.0,0.01);
		
		if (obstDist > sight) obstDist = sight;
		
		Rcolor = random(0.4,1.0,0.001);
		Gcolor = random(0.4,1.0,0.001);
		Bcolor = random(0.4,1.0,0.001);
		
	}
	
	void accelForEdges (float * xAcc, float * yAcc)
	{
		float xDist;
		float yDist;
		
		
		if(posX > 0.0) xDist = 1.0 - posX;
		else xDist = 1.0 + posX;
		
		if(posY > 0.0) yDist = 1.0 - posY;
		else yDist = 1.0 + posY;
		
		if(xDist < obstDist)
		{
			if(yDist < obstDist)
			{
				if(posX>0) (*xAcc) = (-maxAccel);
			  else (*xAcc) = maxAccel;
			
				if(posY>0) (*yAcc) = (-maxAccel);
				else (*yAcc) = maxAccel;
			}
			else
			{
				if(posX>0) (*xAcc) = (-maxAccel);
			  else (*xAcc) = maxAccel;
			
				(*yAcc) = random(-maxAccel/10,maxAccel/10, maxAccel/1000);
			}
		}
		else if(yDist < obstDist)
		{
			(*xAcc) = random(-maxAccel/10,maxAccel/10, maxAccel/1000);
			
			if(posY>0) (*yAcc) = (-maxAccel);
			else (*yAcc) = maxAccel;
		}
		
	}
	
	

  //calcolare l'accelerazione per tutti gli ostacoli e "sommarla" a quella ottenuta dai bordi
  void accelForObst(float * xAcc, float * yAcc) { //scritta il 13.9.10

    (*xAcc) = 0.0;
    (*yAcc) = 0.0;

    if (!noBounds) accelForEdges(xAcc, yAcc);


    float xTemp, yTemp;

    for (int i = 0; i < obstNum; i++) {
      float Px, Py;
      float dist;

      if (DistanceFromLine(posX, posY, obstacles[i][0], obstacles[i][1], obstacles[i][2], obstacles[i][3], dist)) {
        // il punto più vicino è una delle estremità del segmento
        if (pointToPointDistance(obstacles[i][0], obstacles[i][1], posX, posY) < pointToPointDistance(obstacles[i][2], obstacles[i][3], posX, posY)) {
          Px = obstacles[i][0];
          Py = obstacles[i][1];
        } else {
          Px = obstacles[i][2];
          Py = obstacles[i][3];
        }
      } else { // bisogna calcolare P
        getProjectionOfPointAOnSegment(obstacles[i][0], obstacles[i][1], obstacles[i][2], obstacles[i][3], posX, posY, &Px, &Py);
      }

      if (dist < LETHAL_DISTANCE) {
        kill();
      } else if (dist < obstDist) {
        getVectComponentsWith2P(Px, Py, posX, posY, maxAccel, &xTemp, &yTemp);
        (*xAcc) += xTemp;
        (*yAcc) += yTemp;
      }
    }

    if ((*xAcc) != (*xAcc) || (*yAcc) != (*yAcc)) {printf("\n NAN problem ID %d, xAcc= %f , yAcc= %f\n", id, (*xAcc), (*yAcc));fflush(stdout);}

    if (DEBUG) {printf("\nDEBUG accelForObst xAcc=%f yAcc=%f\n", (*xAcc), (*yAcc));fflush(stdout);}

    // il vettore risultante dalla somma ha la giusta inclinazione ma non la giusta lunghezza quindi:
    reduceVectComponentsToMax(xAcc, yAcc, maxAccel);


  }
	



void accelForHawks(float * xAcc, float * yAcc) {


    if (pointToPointDistance(birds[0]->posX, birds[0]->posY, posX, posY) < sight) {
      (*xAcc) = -birds[0]->posX + posX;
      (*yAcc) = -birds[0]->posY + posY;

      reduceVectComponentsToMax(xAcc, yAcc, maxAccel / FOLLOW_FRIENDS_FACTOR);
    }
  }











  void accelForFriends (float * xAcc, float * yAcc)
	{
		/*if(hawk)
		{
			if(nearBirdsNum > 0)
			{
				if(nearBirdsXYDist > maxDist) //bisogna avvicinarsi a tutti i costi
				{
					(*xAcc) = nearBirdsX-posX;
					(*yAcc) = nearBirdsY-posY;
				
					reduceVectComponentsToMax(xAcc, yAcc, maxAccel);
					reduceVectComponentsToMax(xAcc, yAcc, maxAccel);
				}
			}
			
			return;
		}*/
		
		
		if(nearBirdsNum > 0)
		{
			if(nearBirdsXYDist > maxDist || hawk) //bisogna avvicinarsi
			{
				(*xAcc) = nearBirdsX-posX;
				(*yAcc) = nearBirdsY-posY;


                if(DEBUG) {printf("\nDEBUG accelForFriends xAcc=%f yAcc=%f\n",(*xAcc),(*yAcc)); fflush(stdout);}
				reduceVectComponentsToMax(xAcc, yAcc, maxAccel/FOLLOW_FRIENDS_FACTOR);
			}
			else if(nearBirdsXYDist < minDist) //bisogna allontanarsi
			{
				(*xAcc) = -nearBirdsX+posX;
				(*yAcc) = -nearBirdsY+posY;
				
				if(DEBUG) {printf("\nDEBUG4 accelForFriends xAcc=%f yAcc=%f\n",(*xAcc),(*yAcc)); fflush(stdout);}
				reduceVectComponentsToMax(xAcc, yAcc, maxAccel/FOLLOW_FRIENDS_FACTOR);
			}
		}
	}
	
	void accelAsFriends(float * xAcc, float * yAcc)
	{
		if(nearBirdsNum > 0)
		{
			float temp;
			
			(*xAcc) = nearBirdsXspeed;
			(*yAcc) = nearBirdsYspeed;
			
			if(DEBUG) {printf("\nDEBUG accelAsFriends xAcc=%f yAcc=%f\n",(*xAcc),(*yAcc)); fflush(stdout);}
			reduceVectComponentsToMax(xAcc, yAcc, maxAccel/FOLLOW_FRIENDS_FACTOR);
			if(DEBUG) {printf("\nDEBUG2 accelAsFriends xAcc=%f yAcc=%f\n",(*xAcc),(*yAcc)); fflush(stdout);}

			if(rand()%2)
			{
				temp = sqrt(pow(maxAccel,2)-pow((*yAcc),2));
				(*xAcc) = xAccel/2 + random(-temp, temp, 0.000001);
			}
			else
			{
				temp = sqrt(pow(maxAccel,2)-pow((*xAcc),2));
				(*yAcc) = yAccel/2 + random(-temp, temp, 0.000001);
			}
			
			if(DEBUG) {printf("\nDEBUG3 accelAsFriends xAcc=%f yAcc=%f\n",(*xAcc),(*yAcc)); fflush(stdout);}
			reduceVectComponentsToMax(xAcc, yAcc, maxAccel/FOLLOW_FRIENDS_FACTOR);
            if(DEBUG) {printf("\nDEBUG4 accelAsFriends xAcc=%f yAcc=%f\n",(*xAcc),(*yAcc)); fflush(stdout);}
		}
	}

  void calcNearBirds(void) {
    nearBirdsNum = 0;

    float temp;
    distFromNearBirds[0] = 1.0; //utile se è un falco

    for (int i = 0; i < birdNum; i++) {
      if (nearBirdsNum >= MAX_BIRDS_TO_FOLLOW)
        break;

      temp = sqrt(pow(birds[i]->posX - posX, 2) + pow(birds[i]->posY - posY, 2));

      if (birds[i]->live && temp < sight && i != id) {
        if (!hawk) {
          distFromNearBirds[nearBirdsNum] = temp;
          nearBirds[nearBirdsNum] = i;
          nearBirdsNum++;
        } else if (temp < distFromNearBirds[0]){ //se è un falco cerco solo il più vicino
          distFromNearBirds[nearBirdsNum] = temp;
          nearBirds[nearBirdsNum] = i;
        }
      }
    }


    nearBirdsX = 0.0;
    nearBirdsY = 0.0;
    nearBirdsXspeed = 0.0;
    nearBirdsYspeed = 0.0;


    if(!hawk){

    

		
		if(nearBirdsNum == 0)
		{
			nearBirdsX = -1.0;
			nearBirdsY = -1.0;
		}
		else
		{
			for(int i=0; i<nearBirdsNum; i++)
			{
				nearBirdsX += birds[(nearBirds[i])]->posX;
				nearBirdsY += birds[(nearBirds[i])]->posY;
				
				nearBirdsXspeed += birds[(nearBirds[i])]->xSpeed;
				nearBirdsYspeed += birds[(nearBirds[i])]->ySpeed;
			}
			
			nearBirdsX /= nearBirdsNum;
			nearBirdsY /= nearBirdsNum;
			
			nearBirdsXspeed /= nearBirdsNum;
			nearBirdsYspeed /= nearBirdsNum;
			
			nearBirdsXYDist = sqrt( pow( nearBirdsX-posX ,2) + pow( nearBirdsY-posY ,2) );
		}
		
	} else { // se è un falco
      nearBirdsNum=1;
      nearBirdsX = birds[nearBirds[0]]->posX;
      nearBirdsY = birds[nearBirds[0]]->posY;
      nearBirdsXspeed += birds[nearBirds[0]]->xSpeed;
      nearBirdsYspeed += birds[nearBirds[0]]->ySpeed;
    }
  }

  // calcola la distanza da ogni ostacolo e se è minore di obstDist allora salva lunghezza, determinante e distanza in un vettore
 /*
    void calcNearObst(void) { //scritta il 13.9.10
    if (id < 5) {
      if(DEBUG) {printf("\nDEBUG calcNearObst. bird ID %d \n", id); fflush(stdout);}
      fflush(stdout);
    }
    nearObstNum = 0;
    float obstLength;
    float obstDet;
    float obstDistNow;

    for (int i = 0; i < obstNum; i++) {
      //obstDistNow = segmentToPointDistance(obstacles[i][0],obstacles[i][1],obstacles[i][2],obstacles[i][3], posX, posY, obstLength, obstDet);
      nearObstDet[i] = DistanceFromLine(posX,posY,obstacles[i][0],obstacles[i][1],obstacles[i][2],obstacles[i][3],obstDistNow);
      if (obstDistNow < obstDist) {
        if (id < 5) {
          if(DEBUG) {printf("\nDEBUG2 calcNearObst. bird ID %d \n", id); fflush(stdout);}
          fflush(stdout);
        }
        nearObstDist[nearObstNum] = obstDistNow;
        nearObstNum++;
      }
    }
  }*/

	void kill (void) {
		live = false;
		liveBirdNum--;
	}
	
	
	bool amIDead (void) {  //modificata il 13.9.10
      if(DEBUG) {printf("\nDEBUG amIDead. bird ID %d \n", id); fflush(stdout);}
		if(!noBounds) { // se si contano i bordi uccidi l'uccello se è uscito
			if(posX >= 1.0 || posX <= -1.0) {
				kill();
				return true;
			}
			if(posY >= 1.0 || posY <= -1.0) {
				kill();
				return true;
			}
		} else { // teletrasporta l'ucccelo
			if(posX >= 1.0) posX -= 2.0;
			if(posX <= -1.0) posX += 2.0;
			if(posY >= 1.0) posY -= 2.0;
			if(posY <= -1.0) posY += 2.0;
		}
		
		
		return false;
	}
	
	
	void doStep (void) {  //modificata il 13.9.10
		if(live)
		{
          if(hawk){
            stomach--;
            if (pointToPointDistance(nearBirdsX,nearBirdsY,posX,posY) < EAT_DIST) { //eat
              birds[nearBirds[0]]->live = false;
              stomach += 1000;
            }
          }



          pushStep(posX,posY,speed);
          if(DEBUG) {printf("\nDEBUG step. bird ID %d \n", id); fflush(stdout);}
			// per tutti gli ostacoli calcola la distanza
			//calcNearObst();
            if(DEBUG) {printf("\nDEBUG2 step. bird ID %d \n", id); fflush(stdout);}
			if(amIDead()) return;
			if(posY<posX && obstacleTest) { //only for test if birds can pass trough obstacles
				selectedBird = id;
				cout << endl << endl << endl << "BIRD PASSED LIVE THROUGH OBSTACLE AT TEST N. " << numberOfObstaclesTests << "!" << endl << endl;
				cout << "LETHAL DISTANCE = " << LETHAL_DISTANCE << endl;
				cout << "ID " << id << endl;
				cout << "posX " << posX << "  posY " << posY << endl;
				cout << "xAccel " << xAccel << "  yAccel " << yAccel << "  accel "<< sqrt(pow(xAccel,2) + pow(yAccel,2)) << "   maxAccel " << maxAccel << endl;
				cout << "xSpeed " << xSpeed << "  ySpeed " << ySpeed << "  speed "<< speed << "   (maxSpeed " << maxSpeed << ")" << endl;
				stop = true;
			}
			
			float xAcc = 0.0, yAcc = 0.0;
			float xAcc2 = 0.0, yAcc2 = 0.0;
            float xAcc3 = 0.0, yAcc3 = 0.0;
            float xAcc4 = 0.0, yAcc4 = 0.0;
			
			calcNearBirds();
			if(DEBUG) {printf("\nDEBUG3 step. bird ID %d \n", id); fflush(stdout);}
			
			if(!hawk || stomach<0)accelForFriends(&xAcc, &yAcc);
            if(!hawk) accelForHawks(&xAcc4,&yAcc4);
			accelAsFriends(&xAcc3, &yAcc3);
			if(DEBUG) {printf("\nDEBUG4 step. bird ID %d \n", id); fflush(stdout);}
			
			//xAcc += independence * random(-maxAccel,maxAccel, maxAccel/100);
			//yAcc += independence * random(-maxAccel,maxAccel, maxAccel/100);
			//xAcc2 += independence * random(-maxAccel,maxAccel, maxAccel/100);
			//yAcc2 += independence * random(-maxAccel,maxAccel, maxAccel/100);
			
			accelForObst(&xAcc2, &yAcc2);
            if(DEBUG) {printf("\nDEBUG5 step. bird ID %d \n", id); fflush(stdout);}
            //fare il calcolo per gli ostacoli !!!

			if(xSpeed != xSpeed || ySpeed != ySpeed) {printf("\n NAN problem ID %d, xSpeed= %f , ySpeed= %f, ciclo %ld\n",id,xSpeed,ySpeed, cicli); fflush(stdout);}
			
			if(xUserAccel != 0.0 || yUserAccel != 0.0) {
				xSpeed += xUserAccel;
				ySpeed += yUserAccel;
				
				xUserAccel = 0.0;
				yUserAccel = 0.0;
			} else if(xAcc != 0.0 || yAcc != 0.0 || xAcc2 != 0.0 || yAcc2 != 0.0 || xAcc3 != 0.0 || yAcc3 != 0.0 || xAcc4 != 0.0 || yAcc4 != 0.0) {
				xSpeed += (xAccel = xAcc/3.0 + xAcc2*2.0/3.0 + xAcc3 + xAcc4 + random(-maxAccel/100, maxAccel/100, 0.00001));
				ySpeed += (yAccel = yAcc/3.0 + yAcc2*2.0/3.0 + yAcc3 + yAcc4 + random(-maxAccel/100, maxAccel/100, 0.00001));
				//if(xSpeed != xSpeed || ySpeed != ySpeed) {printf("\n NAN problem ID %d, xSpeed= %f , ySpeed= %f, ciclo %d\n",id,xSpeed,ySpeed, cicli); fflush(stdout);}

			}
			else
			{
				randomVector(&xAccel,&yAccel,maxAccel);
				xSpeed += xAccel;
				ySpeed += yAccel;
			}
			
			//if(fabs(xSpeed) > maxSpeed) xSpeed = xSpeed>0?maxSpeed:-maxSpeed;
			//if(fabs(ySpeed) > maxSpeed) ySpeed = ySpeed>0?maxSpeed:-maxSpeed;
			
			//if(posX >= 1.0 || posX <= -1.0) xSpeed = -xSpeed;
			//if(posY >= 1.0 || posY <= -1.0) ySpeed = -ySpeed;
			if(DEBUG) {printf("\nDEBUG6 step xSpeed=%f ySpeed=%f\n",xSpeed,ySpeed); fflush(stdout);}
			reduceVectComponentsToMax(&xSpeed,&ySpeed,maxSpeed);
            if(DEBUG) {printf("\nDEBUG7 step xSpeed=%f ySpeed=%f\n",xSpeed,ySpeed); fflush(stdout);}
			
			posX += xSpeed;
			posY += ySpeed;
			
			speed = sqrt(pow(xSpeed,2) + pow(ySpeed,2));
			
			/*if(posX > 1.0) posX = -1.0;
			else if(posX < -1.0) posX = 1.0;
			
			if(posY > 1.0) posY = -1.0;
			else if(posY < -1.0) posY = 1.0;
			*/
			
			if(nativeColor && dinamicColor)
			{
			
				Rcolor += random(-0.05,0.05,0.001);
				Gcolor += random(-0.05,0.05,0.001);
				Bcolor += random(-0.05,0.05,0.001);
				
				if (Rcolor > 1.0) Rcolor = 1.0;
				else if (Rcolor < 0.2) Rcolor = 0.2;
				if (Gcolor > 1.0) Gcolor = 1.0;
				else if (Gcolor < 0.2) Gcolor = 0.2;
				if (Bcolor > 1.0) Bcolor = 1.0;
				else if (Bcolor < 0.2) Bcolor = 0.2;
			}
		}
	}
	
};




















static void timerCallback (int value) {
	
	if((cicli%7500 == 0 || liveBirdNum<50) && obstacleTest) {  //only for test if a bird can pass trough an obstacle
		numberOfObstaclesTests++;
		cout << "BEGINNING OBSTACLE TEST N. " << numberOfObstaclesTests << "  LETHAL DISTANCE = "<<LETHAL_DISTANCE<<endl << endl;
		if(numberOfObstaclesTests%100 == 0)
			LETHAL_DISTANCE -= 0.00005;
		
		cicli = 0;
		for(int i=0; i<MAXBIRDS; i++)
		{
			birds[i] = new bird(i);
		}
		liveBirdNum = MAXBIRDS;
	}
	
	
	for (int i=0; i<birdNum; i++)
		birds[i]->doStep();
	
	cicli++;
	
	if(liveBirdNum > 600) pointSize = lineWidth = 2.0;
	else if(liveBirdNum > 200) pointSize = lineWidth = 3.0;
	else pointSize = 4.0;
	
	glutPostRedisplay();
	/* call back again after elapsedUSecs have passed */ 
	if(!stop) glutTimerFunc (millsec_per_frame, timerCallback, value);
}


/*
static void stepCallback (int v) {
	
	if(showSteps && selectedBird != -1 && birds[selectedBird]->live)
	{
		pushStep(birds[selectedBird]->posX,birds[selectedBird]->posY, birds[selectedBird]->speed);
	}
	
	//call back again after elapsedUSecs have passed
	if(!stop) glutTimerFunc (millsec_per_frame*4>1000?1000:millsec_per_frame*4, stepCallback, v);
}
*/



static void statsUpdate (int v) {  //modificata il 13.9.10
	
	topAccel = topSpeed = topSight = averageSpeed = 0.0;
	
	for (int i=0; i<birdNum; i++)
	{
		if(birds[i]->live)
		{
			if(birds[i]->maxAccel > topAccel)
				topAccel = birds[i]->maxAccel;
			if(birds[i]->maxSpeed > topSpeed)
				topSpeed = birds[i]->maxSpeed;
			if(birds[i]->sight > topSight)
				topSight = birds[i]->sight;
			
			averageSpeed += birds[i]->speed;
		}
	}
	
	if(liveBirdNum>0)
	  averageSpeed = averageSpeed/liveBirdNum;
	
	if(!obstacleTest) {
		printf("\n\nCiclo %lu", cicli);
		printf("\ncostruiti %d uccelli (%d vivi)\n", birdNum, liveBirdNum);
		if(followFriends) printf("\n Fly mode: follow friends.");
		else printf("\n Fly mode: go toward friends.");
		printf("\n\n Top Accel: %.8f", topAccel);
		printf("\n Top Speed: %.8f", topSpeed);
		printf("\n Top Sight: %.8f\n", topSight);
		
		printf("\n Average Speed: %.8f", averageSpeed);
		
		printf("\n Live birds: %d", liveBirdNum);
		printf("\n Dead birds: %d\n", birdNum - liveBirdNum);
		
		if (selectedBird != -1)
		{
			if(birds[selectedBird]->live)
			{
				printf("\n** Selected bird (ID %d - Birthday %lu)", selectedBird, birds[selectedBird]->birthday);
				printf("\n* Position: (%.2f, %.2f)", birds[selectedBird]->posX, birds[selectedBird]->posY);
				printf("\n* Speed:  %.8f  (%.8f, %.8f)\n*", birds[selectedBird]->speed,  birds[selectedBird]->xSpeed, birds[selectedBird]->ySpeed);
				printf("\n* Max accel:  %.8f", birds[selectedBird]->maxAccel);
				printf("\n* Max speed:  %.8f", birds[selectedBird]->maxSpeed);
				printf("\n* Sight:  %.8f", birds[selectedBird]->sight);
				printf("\n* NearBirds(%d):  average (%.3f, %.3f)", birds[selectedBird]->nearBirdsNum,  birds[selectedBird]->nearBirdsX, birds[selectedBird]->nearBirdsY);
				printf("\n*\n* Available commands:\n* k -kill the selected bird\n* b -create a new bird\n* p -\"pause\", slow the simulation\n");
			}
			else {
				printf("\n\n** The selected bird is dead.");
				printf("\n*\n* Available commands:\n* b -create a new bird\n* p -\"pause\", slow the simulation\n");
			}
		}
		else {
			printf("\n\n** No bird Selected.");
			printf("\n*\n* Available commands:\n* b -create a new bird\n* p -\"pause\", slow the simulation\n");
		}
		printf("\n");
	
	}
	if(!stop) glutTimerFunc (1000, statsUpdate, 0);
}



void newBird(void) {
	if(liveBirdNum < MAXBIRDS) {
		if(birdNum < MAXBIRDS) { // se ci sono posti liberi
			birds[birdNum] = new bird(birdNum);
			liveBirdNum++;
			birdNum++;
		}
		else { // se non ci sono posti liberi ma ci sono uccelli morti
			for(int i=0; i<birdNum; i++) {
				if(!birds[i]->live) {
					birds[i] = new bird(i);
					liveBirdNum++;
					break;
				}
			}
		}
	}
}




void myKeyboard(unsigned char ch, int x, int y)  //modificata il 13.9.10
{
	if(DEBUG) { printf("\nDEBUG myKeyboard: tasto premuto \'%c\'.\n", ch); fflush(stdout); }

    ch = tolower(ch);


  if(ch=='q' || ch=='Q' || ch==27)
	{
		exit(0);
	}
	else if(ch=='b')
	{
		newBird();
	}
	else if(ch=='k' && selectedBird != -1 && birds[selectedBird]->live) {
		birds[selectedBird]->kill();
	}
	else if(ch=='p') {
		if(paused) {
			millsec_per_frame = old_millsec;
		} else {
			old_millsec = millsec_per_frame;
			millsec_per_frame = 1000 / FRAMES_PER_SEC * 100;
		}
		paused = !paused;
	}
	else if(ch=='w' && selectedBird != -1) {
		//printf("\nDEBUG2 myKeyboard: tasto premuto \'%c\'. yaccel = %.3f\n", ch, birds[selectedBird]->maxAccel); fflush(stdout);
		yUserAccel = birds[selectedBird]->maxAccel;
		xUserAccel = 0.0;
	}
	else if(ch=='a' && selectedBird != -1) {
		yUserAccel = 0.0;
		xUserAccel = -birds[selectedBird]->maxAccel;
	}
	else if(ch=='s' && selectedBird != -1) {
		yUserAccel = -birds[selectedBird]->maxAccel;
		xUserAccel = 0.0;
	}
	else if(ch=='d' && selectedBird != -1) {
		yUserAccel = 0.0;
		xUserAccel = birds[selectedBird]->maxAccel;
	}
    else if(ch=='o') {
      randomObstacles();
      glutPostRedisplay();
    }
    else if(ch=='c' && obstacleTest2) {
      useCampo = !useCampo;
      glutPostRedisplay();
    }
}






void myMouse(int tasto, int stato, int x, int y)
{
	if(DEBUG) { printf("\nDEBUG myMouse: tasto premuto.\n"); fflush(stdout); }


  if(tasto == GLUT_LEFT_BUTTON && stato == GLUT_DOWN)
	{
		for (int i=0; i<birdNum; i++)
			if(birds[i]->posX < mouseX + 0.05 && birds[i]->posX > mouseX - 0.05 && birds[i]->posY < mouseY + 0.05 && birds[i]->posY > mouseY - 0.05) {
				selectedBird = i;
				//clearStepList();
				break;
			}
			else {
				selectedBird = -1;
				//clearStepList();
				//showSteps = false;
			}
	}
	else if(tasto==GLUT_RIGHT_BUTTON && stato == GLUT_DOWN)
	{
		
	}
}







/* funzione ausiliaria che disegna una stringa di caratteri 
   ad una certa posizione */
void displayString(float x, float y, string s)
{
  int i;
  glRasterPos2f(x,y);
  for (i=0; s[i]!='\0'; i++)
  {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, s[i]);
  }
}







void displayObstacleTest2() {

  float posX = obstacleTest2PosX;
  float posY = obstacleTest2PosY;
  
  float passo = 0.02;
  float passiFatti = 0.0;
  float totPassi = 4.0/(passo*passo);

  if(useCampo) {
  for (float campoX = -1.0+passo; campoX < 1.0; campoX += passo) {
    for (float campoY = -1.0+passo; campoY < 1.0; campoY += passo) {

      passiFatti += 1.0;

      printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n  Calcolo: %.1f%%\n", passiFatti/totPassi*100);
      fflush(stdout);

      //coordinate del punto da cui si calcola l'accelerazione (ipotetico uccello)
      
      posX = campoX;
      posY = campoY;
      float maxAccel=0.025;

      float obstDistanceLook = 0.15; // massima distanza entro la quale gli uccelli tengono conto degli ostacoli


      //------CODICE UGUALE AD accelForObst()------------------------

      float xAcc = 0.0;
      float yAcc = 0.0;


      float xTemp, yTemp;

      for (int i = 0; i < obstNum; i++) {
        float Px, Py;
        float dist;
        if (DistanceFromLine(posX, posY, obstacles[i][0], obstacles[i][1], obstacles[i][2], obstacles[i][3], dist)) {
          // il punto più vicino è una delle estremità del segmento
          if (pointToPointDistance(obstacles[i][0], obstacles[i][1], posX, posY) < pointToPointDistance(obstacles[i][2], obstacles[i][3], posX, posY)) {
            Px = obstacles[i][0];
            Py = obstacles[i][1];
          } else {
            Px = obstacles[i][2];
            Py = obstacles[i][3];
          }
        } else { // bisogna calcolare P
          getProjectionOfPointAOnSegment(obstacles[i][0], obstacles[i][1], obstacles[i][2], obstacles[i][3], posX, posY, &Px, &Py);
        }

        if (dist < obstDistanceLook && dist>0.0) { // se è troppo vicino lo conto
          getVectComponentsWith2P(Px, Py, posX, posY, maxAccel, &xTemp, &yTemp);
          xAcc += xTemp / dist;
          yAcc += yTemp / dist;
        }

      }

      // il vettore risultante dalla somma ha la giusta inclinazione ma non la giusta lunghezza quindi:
      reduceVectComponentsToMax(&xAcc, &yAcc, maxAccel);


      //-------FINE DEL CODICE DI accelForObst()----------------------

      obstacleTest2VectX = xAcc;
      obstacleTest2VectY = yAcc;


      //getVectComponentsWith2P(obstacles[0][0], obstacles[0][1], obstacles[0][2], obstacles[0][3], 0.5, &obstacleTest2VectX, &obstacleTest2VectY);
      glColor3f(1.0, 1.0, 1.0);
      std::stringstream out;

      out.str("");
      out << " Press \'o\' to change obstacles";
      displayString(-0.9, 0.85, out.str());
      out.str("");
      out << " Press \'c\' to switch off campo mode";
      displayString(-0.9, 0.8, out.str());


      glLineWidth(1.0);
      glBegin(GL_LINE_STRIP);  //disegno il vettore
      glColor3f(0.0, 0.0, 1.0);
      glVertex2f(posX, posY);
      glColor3f(1.0, 0.0, 0.0);
      glVertex2f(posX + obstacleTest2VectX, posY + obstacleTest2VectY);
      glEnd();

    }}
  }
  else {

    float maxAccel = 0.3;

      float obstDistanceLook = 0.15;
      //if(useCampo) obstDistanceLook=0.4;

      float birdDim = 0.0015;

      //------CODICE UGUALE AD accelForObst()------------------------

      float xAcc = 0.0;
      float yAcc = 0.0;

      //if(!noBounds) accelForEdges (xAcc,yAcc);


      float xTemp, yTemp;

      for (int i = 0; i < obstNum; i++) {
        float Px, Py;
        float dist;
        if (DistanceFromLine(posX, posY, obstacles[i][0], obstacles[i][1], obstacles[i][2], obstacles[i][3], dist)) {
          // il punto più vicino è una delle estremità del segmento
          if (pointToPointDistance(obstacles[i][0], obstacles[i][1], posX, posY) < pointToPointDistance(obstacles[i][2], obstacles[i][3], posX, posY)) {
            Px = obstacles[i][0];
            Py = obstacles[i][1];
          } else {
            Px = obstacles[i][2];
            Py = obstacles[i][3];
          }
        } else { // bisogna calcolare P
          getProjectionOfPointAOnSegment(obstacles[i][0], obstacles[i][1], obstacles[i][2], obstacles[i][3], posX, posY, &Px, &Py);
        }

        if (dist < obstDistanceLook) { // se è troppo vicino lo conto

          glBegin(GL_POINTS); //disegno P
            glColor3f(0.0, 1.0, 0.0);
            glPointSize(5.0);
            glVertex2f(Px, Py);
            glEnd();


            glColor3f(1.0, 1.0, 1.0);
            glLineWidth(1.0);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(1, 127);
            glBegin(GL_LINE_STRIP); //disegno (Px,Py)(posX,posY)
            glVertex2f(Px, Py);
            glVertex2f(posX, posY);
            glEnd();
            glDisable(GL_LINE_STIPPLE);




          getVectComponentsWith2P(Px, Py, posX, posY, maxAccel, &xTemp, &yTemp);
          xAcc += xTemp / dist;
          yAcc += yTemp / dist;
        }

      }

      // il vettore risultante dalla somma ha la giusta inclinazione ma non la giusta lunghezza quindi:
      reduceVectComponentsToMax(&xAcc, &yAcc, maxAccel);


      //-------FINE DEL CODICE DI accelForObst()----------------------


      glBegin(GL_POLYGON); // disegno il cerchio che rappresenta l'uccello
      glColor3f(1.0, 1.0, 1.0);
      for (float j = 0; j < 2 * 3.1415; j += (2 * 3.1415) / CIRCLE_STEPS) {
        float x = posX + cos(j)*birdDim;
        float y = posY + sin(j)*birdDim;
        glVertex2f(x, y);
      }
      glEnd();


        glBegin(GL_LINE_STRIP); // disegno il cerchio che rappresenta la distanza entro la quale si contano gli ostacoli
        glColor3f(0.0, 1.0, 0.0);
        for (float j = 0; j < 2 * 3.1415; j += (2 * 3.1415) / CIRCLE_STEPS) {
          float x = posX + cos(j) * obstDistanceLook;
          float y = posY + sin(j) * obstDistanceLook;
          glVertex2f(x, y);
        }
        glEnd();


      obstacleTest2VectX = xAcc;
      obstacleTest2VectY = yAcc;


      //getVectComponentsWith2P(obstacles[0][0], obstacles[0][1], obstacles[0][2], obstacles[0][3], 0.5, &obstacleTest2VectX, &obstacleTest2VectY);
      glColor3f(1.0, 1.0, 1.0);
      std::stringstream out;

        out.precision(3);
        out.setf(ios::fixed, ios::floatfield);
        out << (obstacleTest2VectX >= 0 ? " Vx=  " : " Vx= ") << obstacleTest2VectX << (obstacleTest2VectY >= 0 ? " Vy=  " : " Vy= ") << obstacleTest2VectY << "    move the mouse over the window";
        displayString(-0.9, 0.9, out.str());


      out.str("");
      out << " Press \'o\' to change obstacles";
      displayString(-0.9, 0.85, out.str());
      out.str("");
      out << " Press \'c\' to switch on campo mode";
      displayString(-0.9, 0.8, out.str());

      glBegin(GL_LINE_STRIP);  //disegno il vettore
      glColor3f(0.0, 0.0, 1.0);
      glVertex2f(posX, posY);
      glColor3f(1.0, 0.0, 0.0);
      glVertex2f(posX + obstacleTest2VectX, posY + obstacleTest2VectY);
      glEnd();

      float x, y;

        glBegin(GL_LINE_LOOP); // disegno il cerchio che rappresenta la lunghezza del vettore
        glColor3f(1.0, 0.0, 0.0);
        for (float j = 0; j < 2 * 3.1415; j += (2 * 3.1415) / CIRCLE_STEPS) {
          x = posX + cos(j) * maxAccel;
          y = posY + sin(j) * maxAccel;
          glVertex2f(x, y);
        }
        glEnd();
      
    
  }

  
  glPointSize(pointSize);
}








/* funzione che esegue disegno della scena nella work area */
void display_work(int x0, int y0, int w, int h) {
  GLint dimSquare;

  glClearColor(0.1, 0.1, 0.1, 0.1);

  dimSquare = w;
  if (h < dimSquare) dimSquare = h;

  glViewport(x0, y0, w, h);

  if (DEBUG) {
    printf("\nDEBUG displayWork.\n");
    fflush(stdout);
  }


  glViewport(w / 2 - dimSquare / 2, h / 2 - dimSquare / 2, dimSquare, dimSquare);

  if (DEBUG) {
    printf("\nDEBUG2 displayWork.\n");
    fflush(stdout);
  }

  /* trasformazione di proiezione (matrice identita') */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (DEBUG) {
    printf("\nDEBUG2.1 displayWork.\n");
    fflush(stdout);
  }

  /* trasformazione dista (matrice identita') */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (DEBUG) {
    printf("\nDEBUG2.2 displayWork.\n");
    fflush(stdout);
  }

  /* attributi */
  glPointSize(pointSize);
  glLineWidth(lineWidth);

  if (DEBUG) {
    printf("\nDEBUG2.3 displayWork.\n");
    fflush(stdout);
  }

  glColor3f(0.0, 0.0, 0.0);
  glRecti(-1, -1, w, h);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  if (DEBUG) {
    printf("\nDEBUG2.4 displayWork.\n");
    fflush(stdout);
  }


  


  

  if (whiteBackground) {
    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex2f(-1.0, -1.0);
    glVertex2f(1.0, -1.0);
    glVertex2f(1.0, 1.0);
    glVertex2f(-1.0, 1.0);
    glEnd();
  }


	glColor3f(0.5, 0.2, 0.5); // disegno gli ostacoli
  glLineWidth(6.0);
  glBegin(GL_LINES);
  for (int i = 0; i < obstNum; i++) {
    glVertex2f(obstacles[i][0], obstacles[i][1]);
    glVertex2f(obstacles[i][2], obstacles[i][3]);
  }
  glEnd();

if (obstacleTest2) {
    displayObstacleTest2();
  }
  glLineWidth(lineWidth);

  if (DEBUG) {
    printf("\nDEBUG2.5 displayWork. selectedBird: %d, steps? %d\n", selectedBird, showSteps);
    fflush(stdout);
  }

  if (showSteps) {
    if (DEBUG) {
      printf("\nDEBUG2.5 displayWork.\n");
      fflush(stdout);
    }

    glColor3f(0.5, 0.5, 0.5);
    glLineWidth(1.0);
    for (int i = 0; i < birdNum; i++) {
      glBegin(GL_LINE_STRIP);
      glVertex2f(birds[i]->posX, birds[i]->posY);

      step* temp = birds[i]->steplist;
      while (temp != NULL) // possibilità di istituire un massimo numero di step in memoria e di dividere i colori per currentStepNum/maxStepNum in modo da sfumare
      {
        if (colorSpeed)
          glColor3f(temp->speed / topSpeed / 2.0, (1.0 - temp->speed / topSpeed) / 2.0, 0.05);
        else
          glColor3f(temp->speed / birds[i]->maxSpeed / 2.0, (1.0 - temp->speed / birds[i]->maxSpeed) / 2.0, 0.05);
        glVertex2f(temp->x, temp->y);
        temp = temp->next;
      }
      glEnd();

    }
    glLineWidth(lineWidth);
  }


  if (DEBUG) {
    printf("\nDEBUG3 displayWork.\n");
    fflush(stdout);
  }

  for (int i = 0; i < birdNum; i++) // disegno gli uccelli
  {
    if (birds[i]->live) {
      if (DEBUG) { // disegna le accelerazioni
        glLineWidth(1.0);
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        glVertex2f(birds[i]->posX, birds[i]->posY);
        glVertex2f(birds[i]->posX + birds[i]->xAccel * 50, birds[i]->posY + birds[i]->yAccel * 50);
        glEnd();
      }
      glPointSize(pointSize);
      glLineWidth(lineWidth);
      if (showSpeed && birds[i]->speed > 0.003) {
        primitiv = GL_LINES;
      } else
        primitiv = GL_POINTS;

      glBegin(primitiv);

      if (showSpeed) {
        if (nativeColor)
          glColor3f(birds[i]->Rcolor / 2, birds[i]->Gcolor / 2, birds[i]->Bcolor / 2);
        else if (colorSpeed)
          glColor3f(birds[i]->speed / topSpeed / 10.0, (1.0 - birds[i]->speed / topSpeed) / 10.0, 0.05);
        else if (relativeColorSpeed)
          glColor3f(birds[i]->speed / birds[i]->maxSpeed / 10.0, (1.0 - birds[i]->speed / birds[i]->maxSpeed) / 10.0, 0.05);
        else
          glColor3f(RedRGB / 2, GreenRGB / 2, BlueRGB / 2);

        glVertex2f(birds[i]->posX - birds[i]->xSpeed * 5, birds[i]->posY - birds[i]->ySpeed * 5);
      }

      if (i == selectedBird) {
        glColor3f(1.0, 1.0, 1.0);

      } else if (nativeColor)
        glColor3f(birds[i]->Rcolor, birds[i]->Gcolor, birds[i]->Bcolor);
      else if (colorSpeed)
        glColor3f(birds[i]->speed / topSpeed, 1.0 - (birds[i]->speed / topSpeed), 0.0);
      else if (relativeColorSpeed)
        glColor3f(birds[i]->speed / birds[i]->maxSpeed, 1.0 - (birds[i]->speed / birds[i]->maxSpeed), 0.0);
      else
        glColor3f(RedRGB, GreenRGB, BlueRGB);

      if (birds[i]->hawk) {
        if(birds[i]->stomach>0)
          glColor3f(0.8, 0.8, 0.8);
        else
          glColor3f(0.1, 0.1, 0.8);
        glPointSize(pointSize + 2.0);
        glLineWidth(lineWidth + 2.0);
      }

      glVertex2f(birds[i]->posX, birds[i]->posY);

      glEnd();

      if (showIDs) { // Disegno l'ID
        std::stringstream out;
        out << birds[i]->id;
        if (DEBUG) {
          cout << "\nDEBUG3.1 displayWork.   x:" << birds[i]->posX << " y:" << birds[i]->posY << " ID:" << out.str() << "\n";
          fflush(stdout);
        }
        displayString(birds[i]->posX + 0.02, birds[i]->posY + 0.02, out.str());
      }

      if (showSpeed) {
        glBegin(GL_POINTS); //disegno un punto in ogni caso
        glVertex2f(birds[i]->posX, birds[i]->posY);
        glEnd();
      }

      if (i == selectedBird) {
        glLineWidth(1);
        float x, y;

        if (showSightCircle) {
          glBegin(GL_LINE_LOOP);
          glColor3f(1.0f, 1.0f, 1.0f);


          for (float j = 0; j < 2 * 3.1415; j += (2 * 3.1415) / CIRCLE_STEPS) //cerchio sight
          {
            x = birds[i]->posX + cos(j) * birds[i]->sight;
            y = birds[i]->posY + sin(j) * birds[i]->sight;
            glVertex2f(x, y);
          }
          glEnd();
        }

        if (showDistCircles) {
          glBegin(GL_LINE_LOOP);
          glColor3f(0.0f, 0.0f, 0.9f);

          for (float j = 0; j < 2 * 3.1415; j += (2 * 3.1415) / CIRCLE_STEPS) //cerchio maxDist
          {
            x = birds[i]->posX + cos(j) * birds[i]->maxDist;
            y = birds[i]->posY + sin(j) * birds[i]->maxDist;
            glVertex2f(x, y);
          }
          glEnd();

          glBegin(GL_LINE_LOOP);
          glColor3f(0.0f, 0.0f, 0.8f);

          for (float j = 0; j < 2 * 3.1415; j += (2 * 3.1415) / CIRCLE_STEPS) //cerchio minDist
          {
            x = birds[i]->posX + cos(j) * birds[i]->minDist;
            y = birds[i]->posY + sin(j) * birds[i]->minDist;
            glVertex2f(x, y);
          }
          glEnd();

        }


        if (showObstCircle) {
          glBegin(GL_LINE_LOOP);
          glColor3f(0.0f, 0.8f, 0.0f);

          for (float j = 0; j < 2 * 3.1415; j += (2 * 3.1415) / CIRCLE_STEPS) //cerchio obstDist
          {
            x = birds[i]->posX + cos(j) * birds[i]->obstDist;
            y = birds[i]->posY + sin(j) * birds[i]->obstDist;
            glVertex2f(x, y);
          }
          glEnd();
        }

        //NEARBIRDS
        if (showNearBirdsAverage) {
          glColor3f(0.6f, 0.6f, 0.6f);
          glBegin(GL_LINES);

          for (int h = 0; h < birds[i]->nearBirdsNum; h++) {
            glVertex2f(birds[(birds[i]->nearBirds[h])]->posX, birds[(birds[i]->nearBirds[h])]->posY);
            glVertex2f(birds[i]->nearBirdsX, birds[i]->nearBirdsY);
          }
          glEnd();

          glColor3f(1.0, 0.0, 0.0);
          glBegin(GL_LINE_LOOP);
          for (float j = 0; j < 2 * 3.1415; j += (2 * 3.1415) / 10) //cerchio averageNearBirds
          {
            x = birds[i]->nearBirdsX + cos(j)*0.007;
            y = birds[i]->nearBirdsY + sin(j)*0.007;
            glVertex2f(x, y);
          }
          glEnd();

          glLineWidth(3);

          glBegin(GL_LINES); //segmento velocità media bird vicini
          glColor3f(0.0, 0.0, 1.0);
          glVertex2f(birds[i]->nearBirdsX, birds[i]->nearBirdsY);
          glColor3f(0.0, 0.0, 0.2);
          printf("\nnearBirdsXYspeed: %.2f,%.2f\n",birds[i]->nearBirdsXspeed,birds[i]->nearBirdsYspeed);fflush(stdout);
          glVertex2f(birds[i]->nearBirdsX - (birds[i]->nearBirdsXspeed * 10), birds[i]->nearBirdsY - (birds[i]->nearBirdsYspeed * 10));
          glEnd();

          //punto media nearBirds
          /*
           glPointSize(2);
           glBegin(GL_POINTS);
           glVertex2f(birds[i]->nearBirdsX, birds[i]->nearBirdsY);
           glEnd();
					 
					 
           glPointSize(pointSize);
					 
           */

        }
        glLineWidth(lineWidth);
      }


    }
  }

  if (DEBUG) printf("\nDEBUG4 displayWork.\n");

  if (axes) {


    glColor3f(1.0, 1.0, 1.0);
    glLineWidth(1.0);

    glEnable(GL_LINE_STIPPLE);

    glLineStipple(1, 127);

    glBegin(GL_LINE_STRIP); //asse X
    glVertex2f(-1.0, 0.0);
    glVertex2f(1.0, 0.0);
    glEnd();


    glBegin(GL_LINE_STRIP); //asse Y
    glVertex2f(0.0, -1.0);
    glVertex2f(0.0, 1.0);
    glEnd();

    glDisable(GL_LINE_STIPPLE);
    /*
    glBegin(GL_LINE_STRIP);  //freccia X
        glVertex2f(0.95, -0.025);
        glVertex2f(1.0,0.0);
        glVertex2f(0.95, 0.025);
    glEnd();
		 
		 
    glBegin(GL_LINE_STRIP);  //freccia Y
        glVertex2f(-0.025, 0.95);
        glVertex2f(0.0,1.0);
        glVertex2f(0.025, 0.95);
    glEnd();
     */
  }

  if (DEBUG) {
    printf("\nDEBUG5 displayWork.\n");
    fflush(stdout);
  }
}














/* funzione che esegue disegno della toolbar area */
void display_toolbar(int x0, int y0, int w, int h)
{
	if(DEBUG) { printf("\nDEBUG displayToolbar.\n"); fflush(stdout); }
	
	glViewport(x0, y0, w, h);
	glColor3f(0.8, 0.8, 0.8);
	glRecti(-w/2, -h/2, w, h);
	
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, w, 0.0, h);
	
	/* trasformazione dista (matrice identita') */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}







void display(void)
{
	if(DEBUG) { printf("\nDEBUG display.\n"); fflush(stdout); }

   GLint wp[4];
   /* per la viewport corrente, cioe' l'intera finestra, conterra'
      nell'ordine (x0,y0,w,h) */

   int y0; /* per l'ordinata minima delle sotto-viewport */
   
   /* abilitazioni (non ce ne sono) */

   /* pulizia del foglio */
   glClear(GL_COLOR_BUFFER_BIT);

   /* DISEGNA LE SOTTO-AREE
      Richiama la funzione di disegno di ciascuna sotto-area passando
      i limiti della sua sotto-viewport. Se la finestra e' troppo
      piccola, comincia a disegnare la toolbar area, poi se ci sta
      disegna la work area */
   glGetIntegerv(GL_VIEWPORT,wp);

   /* toolbar area occupa tutta la larghezza e i primi 40 pixel in
      altezza a partire dall'alto */
   if (wp[3]>=40) y0 = wp[3]-40; else y0 = 0;
   display_toolbar(wp[0], y0, wp[2], 40);

   /* work area occupa tutta la larghezza e l'altezza restante una
      volta sottratti 40 pixel a partire dall'alto, se c'e' posto */
   if (wp[3]>40)
      display_work(wp[0],wp[1],wp[2],wp[3]-40);

   /* RIMETTE LA VIEWPORT A TUTTA FINESTRA
      per la prossima volta */
   glViewport(wp[0], wp[1], wp[2], wp[3]);

   /* scambio dei buffer */
   glFlush();
   glutSwapBuffers();
}





/* funzione di gestione del menu' "stupida", 
   stampa la voce selezionata */
void menuecho(int v)
{
	if(DEBUG) { printf("\nDEBUG menuEcho.  v=%d startbirds=%d\n", v, startbirds); fflush(stdout); }
	
	if(v <= 0) {  // v negativo è il menu birds
		
		if(v>=-startbirds) {
			selectedBird = -v;
			//clearStepList();
		} else if(v>=-2*startbirds) {
			printf("\nDEBUG2 menuEcho.  killing ID %d\n",-v-startbirds); fflush(stdout);
			birds[-v-startbirds]->live = false;
			liveBirdNum--;
		}
		printf("\nDEBUG3 menuEcho.  v=%d startbirds=%d\n", v, startbirds); fflush(stdout);
		return;
	}
	
	
	switch (v)
  {
	case 30:
    pointSize = 1.0;
		lineWidth = 1.0;
    break;
  case 31:
    pointSize = 2.0;
		lineWidth = 2.0;
    break;
  case 32:
    pointSize = 3.0;
		lineWidth = 3.0;
    break;
  case 33:
    pointSize = 4.0;
		lineWidth = 4.0;
    break;
  case 34:
    pointSize = 5.0;
    lineWidth = 5.0;
    break;
  case 35:
    pointSize = 6.0;
    lineWidth = 6.0;
    break;
  case 36:
    pointSize = 7.0;
    lineWidth = 7.0;
    break;
	case 37:
    pointSize = 8.0;
    lineWidth = 8.0;
    break;
  case 38:
    pointSize = 9.0;
    lineWidth = 9.0;
    break;
		
		
	case 60:  //rosso
    RedRGB = 1.0;
    GreenRGB = 0.0;
    BlueRGB = 0.0;
		nativeColor = false;
		colorSpeed = false;
		relativeColorSpeed = false;
		dinamicColor = false;
    break;
  case 61:  //blu
    RedRGB = 0.0;
    GreenRGB = 0.0;
    BlueRGB = 1.0;
		nativeColor = false;
		colorSpeed = false;
		relativeColorSpeed = false;
		dinamicColor = false;
    break;
  case 62:  //giallo
    RedRGB = 1.0;
    GreenRGB = 1.0;
    BlueRGB = 0.0;
		nativeColor = false;
		colorSpeed = false;
		relativeColorSpeed = false;
		dinamicColor = false;
    break;
  case 63:  //viola
    RedRGB = 0.6;
    GreenRGB = 0.0;
    BlueRGB = 1.0;
		nativeColor = false;
		colorSpeed = false;
		relativeColorSpeed = false;
		dinamicColor = false;
    break;
  case 64:  //verde
    RedRGB = 0.0;
    GreenRGB = 0.6;
    BlueRGB = 0.0;
		nativeColor = false;
		colorSpeed = false;
		relativeColorSpeed = false;
		dinamicColor = false;
    break;
  case 65:  //arancione
    RedRGB = 1.0;
    GreenRGB = 0.5;
    BlueRGB = 0.0;
		nativeColor = false;
		colorSpeed = false;
		relativeColorSpeed = false;
		dinamicColor = false;
    break;
	case 66:  //nero
    RedRGB = 0.0;
    GreenRGB = 0.0;
    BlueRGB = 0.0;
		nativeColor = false;
		colorSpeed = false;
		relativeColorSpeed = false;
		dinamicColor = false;
    break;
	case 68:
		nativeColor = true;
		dinamicColor = false;
		break;
	case 69:
		nativeColor = true;
		dinamicColor = true;
		break;
	case 71:
		colorSpeed = true;
		relativeColorSpeed = false;
		nativeColor = false;
		dinamicColor = false;
		break;
	case 72:
		colorSpeed = false;
		relativeColorSpeed = true;
		nativeColor = false;
		dinamicColor = false;
		break;
		
			
	case 120:
    millsec_per_frame = 1000 / FRAMES_PER_SEC * 100;
    break;
  case 121:
    millsec_per_frame = 1000 / FRAMES_PER_SEC * 10;
    break;
  case 122:
		millsec_per_frame = 1000 / FRAMES_PER_SEC * 2;
    break;
  case 123:
    millsec_per_frame = 1000 / FRAMES_PER_SEC;
    break;
  case 124:
    millsec_per_frame = 1000 / FRAMES_PER_SEC / 2;
    break;
  case 125:
    millsec_per_frame = 1000 / FRAMES_PER_SEC / 10;
    break;
  case 126:
    millsec_per_frame = 1000 / FRAMES_PER_SEC / 100;
    break;
	
	
	case 70:
		glutSetMenu(showHideSubMenu);
		axes = !axes;
		if(axes) {
			glutChangeToMenuEntry(1,"Hide Axes",70);
		}else {
			glutChangeToMenuEntry(1,"Show Axes",70);
		}
		if(showIDs && showNearBirdsAverage && showSteps && showDistCircles && showObstCircle && showSightCircle && showSpeed && axes) glutChangeToMenuEntry(9,"Hide All",99);
		else if(!showIDs && !showNearBirdsAverage && !showSteps && !showDistCircles && !showObstCircle && !showSightCircle && !showSpeed && !axes) glutChangeToMenuEntry(9,"Show All",99);
		break;
		
	case 80: 
		glutSetMenu(showHideSubMenu);
		showSpeed = !showSpeed;
		if(showSpeed) {
			glutChangeToMenuEntry(2,"Hide Speed",80);
		}else {
			glutChangeToMenuEntry(2,"Show Speed",80);
		}
		if(showIDs && showNearBirdsAverage && showSteps && showDistCircles && showObstCircle && showSightCircle && showSpeed && axes) glutChangeToMenuEntry(9,"Hide All",99);
		else if(!showIDs && !showNearBirdsAverage && !showSteps && !showDistCircles && !showObstCircle && !showSightCircle && !showSpeed && !axes) glutChangeToMenuEntry(9,"Show All",99);
		break;
	
	case 90:
		glutSetMenu(showHideSubMenu);
		showSightCircle = !showSightCircle;
		if(showSightCircle) {
			glutChangeToMenuEntry(3,"Hide SightCircle",90);
		}else {
			glutChangeToMenuEntry(3,"Show SightCircle",90);
		}
		if(showIDs && showNearBirdsAverage && showSteps && showDistCircles && showObstCircle && showSightCircle && showSpeed && axes) glutChangeToMenuEntry(9,"Hide All",99);
		else if(!showIDs && !showNearBirdsAverage && !showSteps && !showDistCircles && !showObstCircle && !showSightCircle && !showSpeed && !axes) glutChangeToMenuEntry(9,"Show All",99);
		break;
	case 91:
		glutSetMenu(showHideSubMenu);
		showObstCircle = !showObstCircle;
		if(showObstCircle) {
			glutChangeToMenuEntry(4,"Hide ObstCircle",91);
		}else {
			glutChangeToMenuEntry(4,"Show ObstCircle",91);
		}
		if(showIDs && showNearBirdsAverage && showSteps && showDistCircles && showObstCircle && showSightCircle && showSpeed && axes) glutChangeToMenuEntry(9,"Hide All",99);
		else if(!showIDs && !showNearBirdsAverage && !showSteps && !showDistCircles && !showObstCircle && !showSightCircle && !showSpeed && !axes) glutChangeToMenuEntry(9,"Show All",99);
		break;
	case 92:
		glutSetMenu(showHideSubMenu);
		showDistCircles = !showDistCircles;
		if(showDistCircles) {
			glutChangeToMenuEntry(5,"Hide DistCircles",92);
		}else {
			glutChangeToMenuEntry(5,"Show DistCircles",92);
		}
		if(showIDs && showNearBirdsAverage && showSteps && showDistCircles && showObstCircle && showSightCircle && showSpeed && axes) glutChangeToMenuEntry(9,"Hide All",99);
		else if(!showIDs && !showNearBirdsAverage && !showSteps && !showDistCircles && !showObstCircle && !showSightCircle && !showSpeed && !axes) glutChangeToMenuEntry(9,"Show All",99);
		break;
	case 93:
		glutSetMenu(showHideSubMenu);
		if (selectedBird != -1)
		{
			showSteps = !showSteps;
		if(showSteps) {
			glutChangeToMenuEntry(6,"Hide Steps",93);
		}else {
			glutChangeToMenuEntry(6,"Show Steps",93);
		}
			//clearStepList();
		}
		if(showIDs && showNearBirdsAverage && showSteps && showDistCircles && showObstCircle && showSightCircle && showSpeed && axes) glutChangeToMenuEntry(9,"Hide All",99);
		else if(!showIDs && !showNearBirdsAverage && !showSteps && !showDistCircles && !showObstCircle && !showSightCircle && !showSpeed && !axes) glutChangeToMenuEntry(9,"Show All",99);
		break;
	case 94:
		glutSetMenu(showHideSubMenu);
		showNearBirdsAverage = !showNearBirdsAverage;
		if(showNearBirdsAverage) {
			glutChangeToMenuEntry(7,"Hide NearBirdsAverage",94);
		}else {
			glutChangeToMenuEntry(7,"Show NearBirdsAverage",94);
		}
		if(showIDs && showNearBirdsAverage && showSteps && showDistCircles && showObstCircle && showSightCircle && showSpeed && axes) glutChangeToMenuEntry(9,"Hide All",99);
		else if(!showIDs && !showNearBirdsAverage && !showSteps && !showDistCircles && !showObstCircle && !showSightCircle && !showSpeed && !axes) glutChangeToMenuEntry(9,"Show All",99);
		break;
	case 95:
		glutSetMenu(showHideSubMenu);
		showIDs = !showIDs;
		if(showIDs) {
			glutChangeToMenuEntry(8,"Hide IDs",95);
		}else {
			glutChangeToMenuEntry(8,"Show IDs",95);
		}
		if(showIDs && showNearBirdsAverage && showSteps && showDistCircles && showObstCircle && showSightCircle && showSpeed && axes) glutChangeToMenuEntry(9,"Hide All",99);
		else if(!showIDs && !showNearBirdsAverage && !showSteps && !showDistCircles && !showObstCircle && !showSightCircle && !showSpeed && !axes) glutChangeToMenuEntry(9,"Show All",99);
		break;
		
	case 99:
		glutSetMenu(showHideSubMenu);
		if(showNearBirdsAverage && showSteps && showDistCircles && showObstCircle && showSightCircle && showSpeed && axes) {
			showNearBirdsAverage = showSteps = showDistCircles = showObstCircle = showSightCircle = showSpeed = axes = false;
			glutChangeToMenuEntry(1,"Show Axes",70);
			glutChangeToMenuEntry(2,"Show Speed",80);
			glutChangeToMenuEntry(3,"Show SightCircle",90);
			glutChangeToMenuEntry(4,"Show ObstCircle",91);
			glutChangeToMenuEntry(5,"Show DistCircles",92);
			glutChangeToMenuEntry(6,"Show Steps",93);
			glutChangeToMenuEntry(7,"Show NearBirdsAverage",94);
			glutChangeToMenuEntry(8,"Show IDs",95);
			glutChangeToMenuEntry(9,"Show All",99);
		}else {
			showNearBirdsAverage = showSteps = showDistCircles = showObstCircle = showSightCircle = showSpeed = axes = true;
			glutChangeToMenuEntry(1,"Hide Axes",70);
			glutChangeToMenuEntry(2,"Hide Speed",80);
			glutChangeToMenuEntry(3,"Hide Sight",90);
			glutChangeToMenuEntry(4,"Hide Obst",91);
			glutChangeToMenuEntry(5,"Hide Dist",92);
			glutChangeToMenuEntry(6,"Hide Steps",93);
			glutChangeToMenuEntry(7,"Hide NearBirdsAverage",94);
			glutChangeToMenuEntry(8,"Hide IDs",95);
			glutChangeToMenuEntry(9,"Hide All",99);
		}
		break;
		
		
	case 100:
		followFriends = !followFriends;
		break;
	
	}
	glutPostRedisplay();
}






void motion(int x, int y)
{
	if(DEBUG) { printf("\nDEBUG motion.\n"); fflush(stdout); }

   GLint wp[4];

  glGetIntegerv(GL_VIEWPORT, wp);
	
	int latoQ;
		
	latoQ = wp[3]-40;
	
	if(latoQ > wp[2])
		latoQ = wp[2];
  
	glutSetMenu(attribMenu);
	
	// se il mouse è nell'area di disegno
	if (y >= (wp[3]+40)/2 - latoQ/2   &&   y<= (wp[3]+40)/2 + latoQ/2  && x>= wp[2]/2 - latoQ/2 && x <= wp[2]/2 + latoQ/2) {
			// salva la posizione del mouse in coordinate relative
				mouseY = (latoQ/2 -(y-(wp[3]/2+20-latoQ/2))) / (float)(latoQ/2);
				mouseX = -(latoQ/2 -(x-(wp[2]-latoQ)/2)) / (float)(latoQ/2);
				
				if(obstacleTest2) {
					obstacleTest2PosX = mouseX;
					obstacleTest2PosY = mouseY;
                    if(!useCampo)glutPostRedisplay();
				}
		 }
		 else {
				mouseX = mouseY = -10;
		 }

  if (y>40) {
     glutAttachMenu(GLUT_RIGHT_BUTTON);
	}
  else {
     glutDetachMenu(GLUT_RIGHT_BUTTON);
	}
}





void menustatus(int s, int x, int y)
{
	if(DEBUG) { printf("\nDEBUG menuStatus.\n"); fflush(stdout); }

  if (s==GLUT_MENU_IN_USE)
  {
    glutMotionFunc(NULL);  
    glutPassiveMotionFunc(NULL);
  }
  else
  { 
    glutMotionFunc(motion);  
    glutPassiveMotionFunc(motion);
  }
}

int main(int argc, char *argv[]) //modificata il 13.9.10
{




  if (argc < 2 || argc > 6 || atoi(argv[1]) > MAXBIRDS) {
    errorStart = true;
  } else {
    for (int i = 1; i < argc; i++) {
      if (strcmp(argv[i], "-d") == 0) {
        DEBUG = 1;
      } else if (strcmp(argv[i], "-ot") == 0) { //obstacle test
        if (DEBUG) {
          printf("\nDEBUG main0.0. argv[%d]=%s\n", i, argv[i]);
          fflush(stdout);
        }
        obstacleTest = true;
        obstacles[0][0] = -1.0;
        obstacles[0][1] = -1.0;
        obstacles[0][2] = 1.0;
        obstacles[0][3] = 1.0;
        obstNum++;
        millsec_per_frame = 10 / FRAMES_PER_SEC;
        startbirds = 800;
      } else if (strcmp(argv[i], "-ot2") == 0) { //obstacle test 2
        obstacleTest2 = true;
        axes = true;
        useCampo=false;

        randomObstacles();
        
      } else if (strcmp(argv[i], "-ot2c") == 0) { //obstacle test 2 campo
        obstacleTest2 = true;
        axes = true;
        useCampo=true;

        randomObstacles();

      } else if(strcmp(argv[i], "-wb") == 0) {
				whiteBackground=true;
			} else if (atoi(argv[i]) == 0) errorStart = true;
    }
  }

  if (errorStart) {
    printf("Usage: ./birds n [-d] [-ot] [-ot2] [-wb]    ( where \'n\' is the starting number of birds and must be smaller than %d \n", MAXBIRDS);
    printf("																							-d (optional) starts birds in debug mode\n");
    printf("																							-ot (optional) tests if birds can pass through obstacles\n");
    printf("																							-ot2 (optional) tests the vector resultant from an obstacle\n");
		printf("																							-wb (optional) sets a white background (better bird-look) )\n");
    return 0;
  }


  srand(time(NULL));

  if (DEBUG) {
    printf("\nDEBUG main.\n");
    fflush(stdout);
  }


  glutInit(&argc, argv);


  if (!obstacleTest) startbirds = atoi(argv[1]);


  if (DEBUG) {printf("\nDEBUG2 main.\n"); fflush(stdout); }

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  glutInitWindowSize(700, 740);

  glutCreateWindow("BIRDS");

  if (DEBUG) { printf("\nDEBUG3 main.\n"); fflush(stdout); }


  int killSubMenu = glutCreateMenu(menuecho);
  int selectSubMenu = glutCreateMenu(menuecho);



  birds[0] = new bird(0);
  birds[0]->hawk = true;
  birdNum++;
  liveBirdNum++;

  if(!obstacleTest2)
    for (int i = 1; i < startbirds; i++) {
      birds[i] = new bird(i);
      birdNum++;
      liveBirdNum++;
      std::ostringstream os;
      os << "ID " << i;

      glutSetMenu(killSubMenu);
      glutAddMenuEntry(os.str().c_str(), -i - startbirds);
      glutSetMenu(selectSubMenu);
      glutAddMenuEntry(os.str().c_str(), -i);

      //printf("costruito uccello %d di %d\n", i+1, startbirds);
    }

  printf("costruiti %d uccelli (%d vivi)\n", birdNum, liveBirdNum);


  if (whiteBackground) {
    RedRGB = 0.0;
    GreenRGB = 0.0;
    BlueRGB = 0.0;
    showSpeed = false;
    nativeColor = false;
    colorSpeed = false;
    relativeColorSpeed = false;
    dinamicColor = false;
  }



  int pointSubMenu = glutCreateMenu(menuecho);
  glutAddMenuEntry("1", 30);
  glutAddMenuEntry("2", 31);
  glutAddMenuEntry("3", 32);
  glutAddMenuEntry("4", 33);
  glutAddMenuEntry("5", 34);
  glutAddMenuEntry("6", 35);
  glutAddMenuEntry("7", 36);
  glutAddMenuEntry("8", 37);
  glutAddMenuEntry("9", 38);

  int colorSubMenu = glutCreateMenu(menuecho);
  glutAddMenuEntry("Red", 60);
  glutAddMenuEntry("Blue", 61);
  glutAddMenuEntry("Yellow", 62);
  glutAddMenuEntry("Pink", 63);
  glutAddMenuEntry("Green", 64);
  glutAddMenuEntry("Orange", 65);
  glutAddMenuEntry("Black", 66);
  glutAddMenuEntry("Natural-Static", 68);
  glutAddMenuEntry("Natural-Dinamic", 69);
  glutAddMenuEntry("Speed-Absolute", 71);
  glutAddMenuEntry("Speed-Relative", 72);

  int birdSubMenu = glutCreateMenu(menuecho);
  glutAddSubMenu("Select", selectSubMenu);
  glutAddSubMenu("Kill", killSubMenu);
  glutAddSubMenu("Dimension", pointSubMenu);
  glutAddSubMenu("Color", colorSubMenu);

  int animationSubMenu = glutCreateMenu(menuecho);
  glutAddMenuEntry("0.01x", 120);
  glutAddMenuEntry("0.1x", 121);
  glutAddMenuEntry("0.5x", 122);
  glutAddMenuEntry("1x", 123);
  glutAddMenuEntry("2x", 124);
  glutAddMenuEntry("10x", 125);
  glutAddMenuEntry("100x", 126);

  showHideSubMenu = glutCreateMenu(menuecho);
  glutAddMenuEntry("Show Axes", 70);
  if (showSpeed) glutAddMenuEntry("Hide Speed", 80);
  else glutAddMenuEntry("Show Speed", 80);
  glutAddMenuEntry("Show SightCircle", 90);
  glutAddMenuEntry("Show ObstCircle", 91);
  glutAddMenuEntry("Show DistCircles", 92);
  glutAddMenuEntry("Show Steps", 93);
  glutAddMenuEntry("Show NearBirdsAverage", 94);
  glutAddMenuEntry("Show IDs", 95);
  glutAddMenuEntry("Show All", 99);

  attribMenu = glutCreateMenu(menuecho);
  glutAddSubMenu("Bird", birdSubMenu);
  glutAddSubMenu("Animation", animationSubMenu);
  glutAddSubMenu("Show/Hide", showHideSubMenu);
  glutAddMenuEntry("Go Towards/As friends", 100);


  if(!obstacleTest2)
    randomObstacles();
  /*
  obstacles[0][0] = 0.0;
  obstacles[0][1] = -0.8;
  obstacles[0][2] = -0.5;
  obstacles[0][3] = -0.5;
  obstNum++;
  obstacles[1][0] = 0.0;
  obstacles[1][1] = 0.8;
  obstacles[1][2] = 0.8;
  obstacles[1][3] = 0.5;
  obstNum++;
  obstacles[2][0] = 0.0;
  obstacles[2][1] = 0.8;
  obstacles[2][2] = 0.8;
  obstacles[2][3] = 0.5;
  obstNum++;
  */
  if (!obstacleTest2) glutTimerFunc(millsec_per_frame, timerCallback, 0);

  //if (!obstacleTest2) glutTimerFunc(millsec_per_frame * 4, stepCallback, 0);

  if (!obstacleTest2) glutTimerFunc(10, statsUpdate, 0);



  glutDisplayFunc(display);




  glutMotionFunc(motion);
  glutPassiveMotionFunc(motion);


  glutKeyboardFunc(myKeyboard);
  glutMouseFunc(myMouse);

  glutMenuStatusFunc(menustatus);

  if (DEBUG) {
    printf("\nDEBUG main.\n");
    fflush(stdout);
  }

  glutMainLoop();




  /* Quest'ultima istruzione non sara' mai eseguita. */
  return 1;
}
