




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
