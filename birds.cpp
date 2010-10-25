
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
#include "options.h"
#include "utils.h"
#include "bird.h"
#include "stats.h"



using namespace std;





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














void randomObstacles(void) {
  obstNum=0;
  for(int i=0; i<MAX_OBSTACLES; i++) {  // creo gli ostacoli a caso
      for(int j=0; j<4; j++)
        obstacles[i][j] = random(-1.0,1.0,0.01);
      obstNum++;
    }
  
}











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
    printf("Usage: ./birds n [-d] [-ot] [-ot2] [-wb] \n");
		printf("   ( where \'n\' is the starting number of birds and must be smaller than %d \n", MAXBIRDS);
    printf("     -d (optional) starts birds in debug mode\n");
    printf("     -ot (optional) tests if birds can pass through obstacles\n");
    printf("     -ot2 (optional) tests the vector resultant from an obstacle\n");
		printf("     -ot2c (optional) tests the vector resultant from an obstacle in a campo (field) mode\n");
		printf("     -wb (optional) sets a white background (better bird-look) )\n");
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
