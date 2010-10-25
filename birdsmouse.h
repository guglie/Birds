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
