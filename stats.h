
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
