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
		pauseBirds();
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
    else if(ch=='m') {
      showPassMap = !showPassMap;
      pauseBirds();
      glutPostRedisplay();
    }
}


