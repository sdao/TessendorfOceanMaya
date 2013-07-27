//
//  tessendorf.h
//  TessendorfOceanNode
//
//  Created by Steven Dao on 7/25/13.
//
//

#ifndef __TessendorfOceanNode__tessendorf__
#define __TessendorfOceanNode__tessendorf__

#include <complex>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MFloatPointArray.h>

#define GRAVITY 9.8 // Acceleration due to gravity (m/s^2).

typedef std::complex<double> complex;

class tessendorf {
    // Instance variables.
    double T = 200.;                // Time of one phase of simulation.
    double omega_0 = 2. * M_PI / T; // Dispersion 0.
    int M;                          // Resolution of grid along X-axis (16 <= M <= 2048; where M = 2^x for integer x).
    int N;                          // Resolution of grid along Z-axis (16 <= N <= 2048; where N = 2^z for integer z).
    double Lx;                      // Length of plane along X-axis (m).
    double Lz;                      // Length of plane along Z-axis (m).
    double A;                       // Controls height of Phillips spectrum.
    double V;                       // Wind speed (in m/s).
    MVector w_hat;                  // Direction of wind.
    double lambda;                  // Choppiness factor.
    double t;                       // Time (s).
    int seed;                       // Seed for the PRNG.
    
public:
    MFloatPointArray vertices;
    
public:
    tessendorf(double height, double speed, MVector direction, double choppiness, double time, int resX, int resZ, double scaleX, double scaleZ, int rngSeed);
    void                    simulate();
    
private:
    double     omega(MVector k);  // Wave dispersion.
    double     P_h(MVector k);    // Phillips spectrum.
    complex    h_tilde_0(MVector k);
    complex    h_tilde(MVector k);
};

#endif /* defined(__TessendorfOceanNode__tessendorf__) */
