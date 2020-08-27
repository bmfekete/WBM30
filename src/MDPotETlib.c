/******************************************************************************

GHAAS Water Balance/Transport Model V2.0
Global Hydrologic Archive and Analysis System
Copyright 1994-2020, UNH - ASRC/CUNY

MDPotETlib.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

float MDPETlibLeafAreaIndex (float laiFac,float lpMax) {
///* projected leaf area index (lai) pulled out from cover dependent PET functions 
// * laiFac    - lai Factor (originally fl) seasonal reduction factor for maximum LAI, 0 to 1
// * lpMax     - maximum projected leaf area index */
	float lai;
   lai = laiFac * lpMax; 
   return (0.001 > lai ? 0.001 : lai);
}

float MDPETlibSteamAreaIndex (float lpMax,float canopyH) {
/* Projected Stem area index (sai) pulled out from McNaugthon and Black PET function 
 * lpMax     - maximum projected leaf area index
 * canopyH   - canopy height, m */
	return (lpMax > MDConstLPC ? MDConstCS * canopyH : (lpMax / MDConstLPC) * MDConstCS * canopyH);
}

float MDPETlibRoughnessClosed (float height,float z0g) {
/* roughness parameter clossed canopy
 * height    - canopy height [m]
 * z0g       - ground surface roughness [m] */
	float z0c;

	if (height > MDConstHR) z0c = MDConstCZR * height;
	else if (height < MDConstHS) z0c = MDConstCZS * height;
	else z0c = MDConstCZS * MDConstHS + (MDConstCZR * MDConstHR - MDConstCZS * MDConstHS) *
													(height - MDConstHS) / (MDConstHR - MDConstHS);
	/* Limit Z0C to Z0G 2001-05-23 Tony and FBM */
	return (z0c < z0g ? z0g : z0c);
}

float MDPETlibRoughness (float disp,float height,float lai,float sai,float z0g) {
/* roughness parameter
 * disp      - zero-plane displacement
 * height    - canopy height [m]
 * lai       - projected leaf area index
 * sai       - projected stem area index
 * z0g       - ground surface roughness [m] */
	float z0c; /* roughness length for closed canopy [m] */
	float xx;
	/* sparse canopy from Shuttleworth and Gurney (1990) */

	z0c = MDPETlibRoughnessClosed (height,z0g);
	if (lai + sai > MDConstLPC + MDConstCS * height) return (z0c); /* closed canopy */

	xx = pow (-1.0 + exp(0.909 - 3.03 * z0c / height),4.0) / (MDConstLPC + MDConstCS * height) * (lai + sai);
	return (MDMinimum (0.3 * (height - disp), z0g + sqrt (0.3 * height * xx)));
}

float MDPETlibZPDisplacement (float height,float lai,float sai,float z0g) {
/* roughness parameter
 * height    - canopy height [m]
 * lai       - projected leaf area index
 * sai       - projected stem area index */
	float z0c;   /* roughness parameter (closed canopy) */
	float dispc; /* zero-plane displacement (closed canopy) */
	float xx;
	
	z0c = MDPETlibRoughnessClosed (height,z0g);
	dispc = height - z0c / 0.3;
	/* Changed the if statement IF (LAI .GE. LPC) 2001-05-23 Tony and FBM */
	if (lai + sai > MDConstLPC + MDConstCS * height) return (dispc); /* closed canopy */

	xx = pow (-1.0 + exp(0.909 - 3.03 * z0c / height),4.0) / (MDConstLPC + MDConstCS * height) * (lai + sai);
	//printf("Height %f Discpl %f \n",height,dispc);
	return (1.1 * height * log (1.0 + pow (xx,0.25)));
 
}

float MDPETlibCanopySurfResistance (float airTmin,float solRad,float dd,
											float lai,float sai,
											float r5,float cd,float cr,float glMax) {
/* canopy surface resistance, RC, s/m after Shuttleworth and Gurney (1990) and Stewart (1988)
 * solRad    - solar radiation on canopy [W/m2]
 * airTmin   - minimum air temperature for the day [degree C]
 * dd        - vapor pressure deficit [kPa]
 * lai       - projected leaf area index
 * sai       - projected stem area index
 * r5        - solar radiation at which conductance is halved [W/m2]
 * cd        - vpd at which conductance is halved [kPa]
 * cr        - light extinction coefficient for projected LAI
 * glMax     - maximum leaf surface conductance for all sides of leaf [m/s] */
	float fs;    /* correction for stem area */
	float r0;    /* a light response parameter */
   float frInt; /* integral of fR dL over Lp */
	float fd;    /* dependence of leaf conductance on vpd, 0 to 1 */
	float ft;    /* dependence of leaf conductance on temperature, 0 to 1 */

   if (r5 > MDConstRM / 2) { CMmsgPrint (CMmsgUsrError,"R5 must be < RM/2\n"); return (0.0); }

	solRad = solRad / MDConstIGRATE;
/* solar radiation limitation integrated down through canopy
   Stewart (1988) and Saugier and Katerji (1991) */
   fs = (lai + sai) / lai; 
	if (solRad < 1e-10) frInt = 0;
	else {
      r0 = MDConstRM * r5 / (MDConstRM - 2 * r5);
      frInt = ((MDConstRM + r0) / (MDConstRM * cr * fs)) *
				  log((r0 + cr * solRad) / (r0 + cr * solRad * exp (-cr * fs * lai)));
	}

/* vapor deficit limitation Lohammar et al. (1980) and Stannard (1993) */
 	fd = 1.0 / (1.0 + dd / cd);

/* temperature limitation limitation only by low minimum daily temperature */
   ft = airTmin > 0.0 ? 1.0 : (airTmin < -5.0 ? 0.0 : 1.0 + airTmin / 5);

   return (1.0 / (fd * ft * frInt * (glMax - MDConstGLMIN) + lai * MDConstGLMIN)); 
}

float MDPETlibBoundaryResistance (float windSpeed,float height,float z0g,float z0c,float dispc,float z0,float disp) {
/* boundary layer resistance [RAA] [s/m]
 * windSpeed - average wind speed for the day [m/s] 
 * height    - canopy height [m]
 * z0g       - ground surface roughness [m] 
 * z0c       - roughness parameter (closed canopy)
 * dispc     - zero-plane displacement (closed canopy)
 * z0        - roughness parameter [m]
 * disp      - height of zero-plane [m] */
	float za;		/* reference height [m] */
	float uStar, kh;

	// Sufrace 
	
	
	za    = height + MDConstZMINH;
	uStar = MDConstK * windSpeed / log ((za - disp) / z0);
	kh    = MDConstK * uStar * (height - disp);
	return (log ((za - disp) / (height - disp)) / (MDConstK * uStar) +
				(height / (MDConstN * kh)) * (-1.0 + exp (MDConstN * (1.0 - (z0c + dispc) / height))));


}

float MDPETlibLeafResistance (float windSpeed,float height, float lWidth,float z0g,float lai, float sai,float z0, float disp) {
/* Leaf aerodynamic resistance (RAC) [s/m]
 * height    - canopy height [m]
 * lWidth    - average leaf width [m]
 * z0g       - ground surface roughness [m] 
 * lai       - projected leaf area index
 * sai       - projected stem area index
 * z0c       - roughness parameter (closed canopy)
 * dispc     - zero-plane displacement (closed canopy) */
	float za;      /* reference height [m] */
	float uStar, uh, rb;
	
	za    = height + MDConstZMINH;
	uStar = MDConstK * windSpeed / log ((za - disp) / z0);
	uh    = (uStar / MDConstK) * log ((height - disp) / z0);
	rb    = (100.0 * MDConstN) * sqrt (lWidth / uh) / (1.0 - exp (-MDConstN / 2.0));
   return (rb / (MDConstRHOTP * lai + M_PI * sai));
}

float MDPETlibGroundResistance (float windSpeed,float height, float z0g,float z0c,float dispc,float z0,float disp) {
/* Ground aerodynamic resistance (RAS) [s/m]
 * height    - canopy height [m]
 * z0g       - ground surface roughness [m] 
 * z0c       - roughness parameter (closed canopy)
 * dispc     - zero-plane displacement (closed canopy) */
	float za;      /* reference height [m] */
	float uStar, kh;
    float output;
	za    = height + MDConstZMINH;
	uStar = MDConstK * windSpeed / log ((za - disp) / z0);
	if (uStar==0)
	{
//	printf ("uStar =0! MDConstK %f windSpeed %f za %f disp %f z0 %f \n",MDConstK , windSpeed ,za ,disp, z0);

	}
	kh    = MDConstK * uStar * (height - disp);
 
	output=((height * exp (MDConstN) / (MDConstN * kh)) *
		 	(exp (-MDConstN * z0g / height) - exp (-MDConstN * (z0c + dispc) / height)));
   // output=70;
	if (isinf(output))printf ("ras inf! height %f z0%f, dispc %f kh %f uStar %f za %f\n",height,z0c,dispc,kh,uStar,za);
	//return ((height * exp (MDConstN) / (MDConstN * kh)) *
		//	 	(exp (-MDConstN * z0g / height) - exp (-MDConstN * (z0c + dispc) / height)));
	return output;
}

float MDPETlibVPressSat (float airT) {
/* calculates saturated vp from airt temperature Murray (1967)
 * airT      - air temperature [degree C] */
	return ((airT >= 0.0) ? 0.61078 * exp (17.26939 * airT / (airT + 237.3)) :
   							   0.61078 * exp (21.87456 * airT / (airT + 265.5)));
}

float MDPETlibVPressDelta (float airT) {
/* calculates saturated vp delta from airt temperature Murray (1967)
 * airT      - air temperature [degree C] */
	return ((airT >= 0.0) ?
			   4098 * MDPETlibVPressSat (airT) / ((airT + 237.3) * (airT + 237.3)) :
				5808 * MDPETlibVPressSat (airT) / ((airT + 265.5) * (airT + 265.5)));
}

float MDSRadNETLong (float i0hDay,float airT,float solRad,float ea) {
/* net longwave radiation,  W/m2 emissivity of the surface taken as 1.0
 * to also account for reflected all equations and parameters from Brutsaert (1982)
 * airT      - air temperature [degree C]
 * ea        - vapor pressure  [kPa] */
	float effem;  /* effective emissivity from clear sky */
	float novern; /* sunshine duration fraction of daylength */
	float cldcor; /* cloud cover correction to net longwave under clear sky */

/* Brutsaert method */
	effem = 1.24 * pow (ea * 10. / (airT + 273.15),1.0 / 7.0);
/*
	additional methods not used
	Brunt method

	bruntA = .44;    - Brunt intercept
	bruntB = .253;   - Brunt EA coefficient, for kPa (value for mb * �10)
	effem = bruntA + bruntB * sqrt(ea);

	Satterlund method
	effem = 1.08 * (1 - exp (pow (-(10.0 * ea),(airT + 273.15) / 2016.)))

	Swinbank method
	effem = 0.0000092 * (airT + 273.15) * (airT + 273.15);

	Idso-Jackson method
	effem = 1.0 - 0.261 * exp (-.000777 * airT * airT);
*/
   novern = i0hDay > 0.0 ? (solRad / i0hDay - MDConstC1) / MDConstC2 : (1.0 - MDConstC1) / MDConstC2;
   if (novern > 1.0) novern = 1.0;
   if (novern < 0.0) novern = 0.0;
   cldcor = MDConstC3 + (1.0 - MDConstC3) * novern; 
   return ((effem - 1.0) * cldcor * MDConstSIGMA * pow (airT + 273.15,4.0));
}

float MDPETlibPenmanMontieth (float aa,float dd,float delta,float ra,float rc) {
/* Penman-Monteith evapotranspiration [W/m2]
 * aa        - net energy input (Rn - S) [W/m2]
 * dd        - vapor pressure deficit [kPa]
 * delta     - dEsat/dTair [kPa/K]
 * ra        - boundary layer resistance [s/m]
 * rc        - canopy resistance [s/m] */
	return ((delta * aa + MDConstCPRHO * dd / ra) / (delta + MDConstPSGAMMA + MDConstPSGAMMA * rc / ra));
}

float MDPETlibShuttleworthWallace (float rss,float aa,float asubs,float dd,float raa,float rac,float ras,float rsc,float delta) {
/* Shuttleworth and Wallace (1985) evapotranspiration [W/m2]
 * rss       - ground evaporation resistance [s/m]
 * aa        - net radiation at canopy top minus ground flux [W/m2]
 * asubs     - net radiation minus ground flux at ground [W/m2]
 * dd        - vapor pressure deficit [kPa]
 * raa       - boundary layer resistance [s/m]
 * rac       - leaf-air resistance  [s/m]
 * ras       - ground-air resitance [s/m]
 * rsc       - canopy surface resistance [s/m]
 * delta     - dEsat/dTair [kPa/K] */
	float rs, rc, ra, ccs, ccc, pms, pmc;
  float output;  
  //printf ("Hier\n");

   rs = (delta + MDConstPSGAMMA) * ras + MDConstPSGAMMA * rss; 
   rc = (delta + MDConstPSGAMMA) * rac + MDConstPSGAMMA * rsc; 
	ra = (delta + MDConstPSGAMMA) * raa;
	printf ("rs %f rc %f ra %f \n",rs,rc,ra);
	//rs=70;
	 if (isinf(ra)){
//		   printf ("ra is INF!!  delta %f MDConstPSGAMMA %f  raa %f\n",delta ,MDConstPSGAMMA, raa);
		   
	   }
   ccs = 1.0 / (1.0 + rs * ra / (rc * (rs + ra)));
   ccc = 1.0 / (1.0 + rc * ra / (rs * (rc + ra)));
   pms = MDPETlibPenmanMontieth (aa,dd - delta * ras * (aa - asubs) / MDConstCPRHO,delta,raa + ras,rss);
   pmc = MDPETlibPenmanMontieth (aa,dd - delta * rac * asubs / MDConstCPRHO, delta,raa + rac, rsc);
   output=(ccc * pmc + ccs * pms);
 //  printf ("Hier\n");
   if (isnan(output)){
	  // printf ("len is NAN in..LIB ccc %f pmc %f ccs %f pms%f rs %f ra %f rc %f ras %f rss %f rsc %f delta %f\n",ccc , pmc , ccs , pms, rs , ra, rc, ras, rss, rsc,delta);
	   
   }
   return (ccc * pmc + ccs * pms);
}
