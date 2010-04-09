#ifndef  _MATH_H_
#define _MATH_H_
#include <math.h>
#define DELTA_LAT_4_RECALC  60.0
/* PtInPolygon.cpp中的函数声明 */
static DOUBLE   M2SecRefLat = 35.0;		 // 米到秒的参考经纬度
static FLOAT    M2SecLon = 0.039435559F; //１米的秒数
static FLOAT    M2SecLat = 0.032665153F; //１米的秒数
#define	PI					(3.14159F)

#define DELTA_LAT_4_RECALC  60.0

#define E2_WGS84    6.69437999013e-3
#define A_WGS84     6378137.0F

 /*-----*-----*-----*-----*-----*-----*-----*-----*-----*-----*-----*-----*
										 原  型：static VOID ComputeS2M(DOUBLE )
										 描  述：计算所给纬度地点的m/s.
										 参  数：DOUBLE latitude  纬度
										 
										   返回值：VOID
-*-----*-----*-----*-----*-----*-----*-----*-----*-----*-----*-----*-----*/
VOID 
ComputeS2M(DOUBLE latitude)
{
    DOUBLE  sinlat;
    DOUBLE  c;
    DOUBLE  lat_radi;
    
    if (latitude > 90*3600*256)
    {
        latitude -= 90*3600*256;
    }
	
    lat_radi = latitude / 3600.0 * PI / 180.0; //转化为弧度
	
    sinlat = sin(lat_radi);
    c = 1.0 - (E2_WGS84 * sinlat * sinlat);
    M2SecLon = (FLOAT)(3600.0F / ((PI/180.0F) * A_WGS84 * cos(lat_radi) / sqrt(c)));
    M2SecLat = (FLOAT)(1.0F / ((PI/648000.0F) * A_WGS84 * (1 - E2_WGS84)) * sqrt(c*c*c));
    M2SecRefLat = latitude;
}
DOUBLE 
CalcDeltaD(DOUBLE delta_lat, DOUBLE delta_lon)
{
    return (FLOAT)(sqrt((delta_lat*delta_lat) + (delta_lon*delta_lon)));
}

void coordinate_dis(POINT* lonlat1, POINT* lonlat2, float* distance)
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
