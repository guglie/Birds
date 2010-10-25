/*
 *  pointLineSegmentDistance.h
 *  
 *
 *  Created by Guglielmo Cassinelli on 13/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


//returns true if the nearest point to C is A or B, false if the nearest point is between A and B
bool DistanceFromLine(float cx, float cy, float ax, float ay , float bx, float by, float &distanceSegment)
{

        //
        // find the distance from the point (cx,cy) to the line
        // determined by the points (ax,ay) and (bx,by)
        //
        // distanceSegment = distance from the point to the line segment
        // distanceLine = distance from the point to the line (assuming
        //                                        infinite extent in both directions
        //

/*

Subject 1.02: How do I find the distance from a point to a line?


    Let the point be C (Cx,Cy) and the line be AB (Ax,Ay) to (Bx,By).
    Let P be the point of perpendicular projection of C on AB.  The parameter
    r, which indicates P's position along AB, is computed by the dot product 
    of AC and AB divided by the square of the length of AB:
    
    (1)    AC dot AB
        r = ---------  
            ||AB||^2
    
    r has the following meaning:
    
        r=0      P = A
        r=1      P = B
        r<0      P is on the backward extension of AB
        r>1      P is on the forward extension of AB
        0<r<1    P is interior to AB
    
    The length of a line segment in d dimensions, AB is computed by:
    
        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 + ... + (Bd-Ad)^2)

    so in 2D:  
    
        L = sqrt( (Bx-Ax)^2 + (By-Ay)^2 )
    
    and the dot product of two vectors in d dimensions, U dot V is computed:
    
        D = (Ux * Vx) + (Uy * Vy) + ... + (Ud * Vd)
    
    so in 2D:  
    
        D = (Ux * Vx) + (Uy * Vy) 
    
    So (1) expands to:
    
            (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
        r = -------------------------------
                          L^2

    The point P can then be found:

        Px = Ax + r(Bx-Ax)
        Py = Ay + r(By-Ay)

    And the distance from A to P = r*L.

    Use another parameter s to indicate the location along PC, with the 
    following meaning:
          s<0      C is left of AB
          s>0      C is right of AB
          s=0      C is on AB

    Compute s as follows:

            (Ay-Cy)(Bx-Ax)-(Ax-Cx)(By-Ay)
        s = -----------------------------
                        L^2


    Then the distance from C to P = |s|*L.

*/
    bool ret;

        float r_numerator = (cx-ax)*(bx-ax) + (cy-ay)*(by-ay);
        float r_denomenator = (bx-ax)*(bx-ax) + (by-ay)*(by-ay);
        float r = r_numerator / r_denomenator;
//
    float px = ax + r*(bx-ax);
    float py = ay + r*(by-ay);
//    
    float s =  ((ay-cy)*(bx-ax)-(ax-cx)*(by-ay) ) / r_denomenator;

        float distanceLine = fabs(s)*sqrt(r_denomenator);

//
// (xx,yy) is the point on the lineSegment closest to (cx,cy)
//
        float xx = px;
        float yy = py;

        if ( (r >= 0) && (r <= 1) )
        {
                distanceSegment = distanceLine;
                ret = false;
        }
        else
        {

                float dist1 = (cx-ax)*(cx-ax) + (cy-ay)*(cy-ay);
                float dist2 = (cx-bx)*(cx-bx) + (cy-by)*(cy-by);
                if (dist1 < dist2)
                {
                        xx = ax;
                        yy = ay;
                        distanceSegment = sqrt(dist1);
                }
                else
                {
                        xx = bx;
                        yy = by;
                        distanceSegment = sqrt(dist2);
                }

                ret = true;
        }

        return ret;
}