
/* fare la funzione ausiliaria che disegna la passMap */

void displayPassMap() {
	
	for(int i=0; i<QUADS_FOR_EDGE_OF_MAP; i++)
		for(int j=0; j<QUADS_FOR_EDGE_OF_MAP; j++) { //disegna il quadrato con colore pesato su passMap[i][j] / maxvalPass
			
			//if(passMap[i][j] > 0) {
				glColor3f(0.0,0.0,passMap[i][j]*1.0 / maxvalPass);
			
				float x1, y1, x2, y2;
				x1=i*quadMapLength-1.0;
				y1=j*quadMapLength-1.0;
				
				x2=(i+1)*quadMapLength;
				y2=(j+1)*quadMapLength;
			
				glRectf(x1,y1,x2,y2);
			//}
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
  
  if(showPassMap)
  	displayPassMap();
  


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
	if(showBirds)
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
