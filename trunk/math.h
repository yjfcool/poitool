#ifndef  _MATH_H_
#define _MATH_H_
#include <math.h>
#define DELTA_LAT_4_RECALC  60.0
/* PtInPolygon.cpp中的函数声明 */
static DOUBLE   M2SecRefLat = 35.0;		 // 米到秒的参考经纬度
static FLOAT    M2SecLon = 0.039435559F; //１米的秒数
static FLOAT    M2SecLat = 0.032665153F; //１米的秒数

void CalOnlyPrecDist(POINT* lonlat1, POINT* lonlat2, float* distance)
{
	DOUBLE  ref_lat;    
    FLOAT   real_delta_lat; 
    FLOAT   real_delta_lon; 
    FLOAT   real_delta_d;   
    LONG    delta_lat;  
    LONG    delta_lon;  
	
	if( lonlat1->x == lonlat2->x && lonlat1->y == lonlat2->y ){
		*distance = 0;
		return;
	}
    delta_lon = lonlat2->x  - lonlat1->x;
    delta_lat = lonlat2->y  - lonlat1->y;
	
    ref_lat = (lonlat1->y + delta_lat / 2.0) / 256.0;
	
    if (fabs(ref_lat - M2SecRefLat) >= DELTA_LAT_4_RECALC) 
    {
        ComputeS2M(ref_lat);
    }
	
    real_delta_lat = delta_lat / M2SecLat / 256.0F;
    real_delta_lon = delta_lon / M2SecLon / 256.0F;
	
    real_delta_d = CalcDeltaD(real_delta_lat, real_delta_lon);
	*distance = real_delta_d;
}

#endif
