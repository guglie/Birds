#include <iostream>

using namespace std;

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