


void randomObstacles(void) {
  obstNum=0;
  for(int i=0; i<MAX_OBSTACLES; i++) {  // creo gli ostacoli a caso
      for(int j=0; j<4; j++)
        obstacles[i][j] = random(-1.0,1.0,0.01);
      obstNum++;
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
