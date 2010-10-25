
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

#include "birdslib.h"
#include "birdsmouse.h"
#include "birdskeys.h"
#include "birdsdisplay.h"
#include "birdsmenu.h"




using namespace std;


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
