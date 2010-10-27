

void pauseBirds (void) {
	if(paused) {
			millsec_per_frame = old_millsec;
		} else {
			old_millsec = millsec_per_frame;
			millsec_per_frame = 1000 / FRAMES_PER_SEC * 10;
		}
	paused = !paused;
}


void reduceVectComponentsToMax (float * vx, float * vy, float max) //modificata il 13.9.10
{
	/*if((*vx) >= 0)
		(*vx) = ((*vx) * max) / ( sqrt( pow( (*vx) , 2) + pow( (*vy) , 2) ) );
	else
		(*vx) = ((*vx) * max) / ( sqrt( pow( (*vx) , 2) + pow( (*vy) , 2) ) );
		
	if((*vy) >= 0)
		(*vy) = sqrt( pow( max , 2) - pow( (*vx) , 2) );
	else
		(*vy) = - sqrt( pow( max , 2) - pow( (*vx) , 2) );*/


  if((*vx) != 0.0 && (*vy) != 0.0) {
    float c = sqrt((*vx)*(*vx)+(*vy)*(*vy));

    if(DEBUG) {printf("\nDEBUG reduceVectComponentsToMax C=%f!!!\n",c); fflush(stdout);}

    (*vx) = (*vx)*max/c;
    (*vy) = (*vy)*max/c;
  }
}




 /*returns a float between m and M     ( with (|M-m|)/step +1 different chanches.
  *										 Ex: m=2.0, M=1.0, step=0.2  means
  *									     (|1.0-2.0|)/0.2 +1 = 6 chanches (1.0, 1.2, 1.4, 1.6, 1.8, 2.0) )
  */
float random(float m, float M, float step) { 
		if(m>M) {
			float temp = M;
			M = m;
			m = temp;
		}
		
		float randomN;
		
		int NoS;
		step = fabs(step);
		
		if (step != 0.0)
			NoS = (int) ((M-m)/step) +1; // Number of Steps
		else
			return 0.0;
		
		randomN = (rand()%(NoS))*step + m;
		
		if (randomN > M)
			randomN = M;
		
		if(m+step <= M)
			return randomN;
		else
			return 0.0;
	}


void randomVector(float * vx, float * vy, float max) {
	(*vx) = random(-0.1,0.1,0.00001);
	(*vy) = random(-0.1,0.1,0.00001);

    if(DEBUG) {printf("\nDEBUG randomVector vx=%f vy=%f\n",(*vx),(*vy)); fflush(stdout);}
	reduceVectComponentsToMax(vx,vy,max);
}

void limitToBounds (float* x, float top, float bottom) {
	if((*x) < bottom)
		(*x) = bottom;
	else if ((*x) > top)
		(*x) = top;
}


float segmentToPointDistance(float x1, float y1, float x2, float y2, float X, float Y, float  &length, float &det) { //scritta il 13.9.10
	float vx = x1-X, vy = y1-Y, ux = x2-x1, uy = y2-y1;
	length = ux*ux+uy*uy;

	det = (-vx*ux)+(-vy*uy); //if this is < 0 or > length then its outside the line segment
  if(det<0 || det>length)
  {
    ux=x2-X;
    uy=y2-Y;
    return std::min<float>(vx*vx+vy*vy, ux*ux+uy*uy);
  }

  det = ux*vy-uy*vx;
  return (det*det)/length;
}


float pointToPointDistance(float x1, float y1, float x2, float y2) { //scritta il 13.9.10
  return sqrt( pow(x2-x1,2) + pow(y2-y1,2));
}


// salva in Vx e Vy le componenti del vettore diretto lungo la retta AB  (A(x1,y1), B(x2,y2)) con verso A -> B
void getVectComponentsWith2P(float x1, float y1, float x2, float y2, float Vlength, float * Vx, float * Vy) { //scritta il 13.9.10
	float coeffAng_num = y2-y1;
	float coeffAng_den = x2-x1;
	
	if (coeffAng_den == 0.0) {
		if(DEBUG) { printf("\nDEBUG getVectComponentsWith2P. coeffAng_den = 0"); fflush(stdout);};
		(*Vx)=0.0;
		
		if(y2>y1)
			(*Vy)=Vlength;
		else 
			(*Vy)=-Vlength;
		
		return;
	} else if(coeffAng_num == 0.0) {
		if(DEBUG) { printf("\nDEBUG getVectComponentsWith2P. coeffAng_num = 0"); fflush(stdout);};
		(*Vy)=0.0;
		
		if(x2>x1)
			(*Vx)=Vlength;
		else 
			(*Vx)=-Vlength;
		
		return;
	}
	
	
	
	float coeffAng = coeffAng_num/coeffAng_den;
	if(DEBUG) { printf("\nDEBUG2 getVectComponentsWith2P. coeffAng = %.4f", coeffAng); fflush(stdout);};
	
	
	float alpha = atan(coeffAng);
	
	if(x2<x1 ) {
	(*Vx) = -cos(alpha) * Vlength;
	(*Vy) = -sin(alpha) * Vlength;
	} else {
	(*Vx) = cos(alpha) * Vlength;
	(*Vy) = sin(alpha) * Vlength;
	}
	
}


// restituisce le coordinate del punto P, proiezione di A sul segmento (x1,y1)(x2,y2)
void getProjectionOfPointAOnSegment(float x1, float y1, float x2, float y2, float Ax, float Ay, float * Px, float * Py) {  //scritta il 13.9.10
  float r_numerator = (Ax-x1)*(x2-x1) + (Ay-y1)*(y2-y1);
  float r_denomenator = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);
  float r = r_numerator / r_denomenator;

  (*Px) = x1 + r*(x2-x1);
  (*Py) = y1 + r*(y2-y1);
}