


void randomObstacles2(float maxLenght) {
  obstNum=0;
  float x,y;
  
  for(int i=0; i<MAX_OBSTACLES; i++) {  // creo gli ostacoli a caso
      for(int j=0; j<2; j++) {
        obstacles[i][j] = random(-1.0,1.0,0.01);
      }
      
      printf("\n obst1(%.2f,%.2f)\n",obstacles[i][0], obstacles[i][1]);
      fflush(stdout);
      
      //randomVector(&x, &y, maxLenght);
      obstacles[i][2] = obstacles[i][0] + random(-maxLenght,maxLenght,0.00001);
      obstacles[i][3] = obstacles[i][1] + random(-maxLenght,maxLenght,0.00001);
      
      printf("\n obst2(%.2f,%.2f)\n",obstacles[i][2], obstacles[i][3]);
      fflush(stdout);
      
	  limitToBounds(&(obstacles[i][2]), 1.0, -1.0);
	  limitToBounds(&(obstacles[i][3]), 1.0, -1.0);
      
      
      obstNum++;
    }
  
}




void randomObstacles(void) {
  if(OBST_MAX_LENGHT == 0.0) {
    obstNum=0;
    for(int i=0; i<MAX_OBSTACLES; i++) {  // creo gli ostacoli a caso
        for(int j=0; j<4; j++)
          obstacles[i][j] = random(-1.0,1.0,0.01);
        obstNum++;
      }
  } else {
  	randomObstacles2(OBST_MAX_LENGHT);
  }
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
