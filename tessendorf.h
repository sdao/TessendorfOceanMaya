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

/**
 * A class that simulates ocean waves at a given time using Tessendorf's wave equations and the FFT method.
 *
 * The equations referenced by the documentation comments are those in
 * "Simulating Ocean Waves", (c) 1999-2001 Jerry Tessendorf (SIGGRAPH Course Notes 2002).
 */
class tessendorf {
    double              T = 240.;                   /* Time of one phase of simulation (currently 4'0"). */
    double              omega_0 = 2. * M_PI / T;    /* Dispersion-sub-naught; calculated using Tessendorf's equation (17). */
    int                 M;                          /* Resolution of grid along X-axis (16 <= M <= 2048; where M = 2^x for integer x). */
    int                 N;                          /* Resolution of grid along Z-axis (16 <= N <= 2048; where N = 2^z for integer z). */
    double              Lx;                         /* Length of plane along X-axis (in m). */
    double              Lz;                         /* Length of plane along Z-axis (in m). */
    double              l;                          /* Size limit that waves must surpass to be rendered. */
    double              A;                          /* Controls height of Phillips spectrum. */
    double              V;                          /* Wind speed (in m/s). */
    MVector             w_hat;                      /* Direction of wind. */
    double              lambda;                     /* Choppiness factor. */
    double              t;                          /* Time (in s). */
    int                 seed;                       /* Seed for the pseudorandom number generator. */
    MFloatPointArray    vertices;
    
    // Values precached on initialization.
    double              P_h__L;                     /* Precached for tessendorf::P_h. Largest possible waves arising from a continuous wind of speed V. */
    double              P_h__l_2;                   /* Precached for tessendorf::P_h. Square of l (l being the wave size limit). */
    
public:
    /**
     * Creates a new Tessendorf wave simulation at a specified time, given the specified parameters.
     * \param amplitude controls height of Phillips spectrum
     * \param speed wind speed (in m/s)
     * \param direction direction of wind
     * \param choppiness choppiness factor; greater is choppier
     * \param time time (in s)
     * \param resX resolution of grid along X-axis (16 <= M <= 2048; where M = 2^x for integer x)
     * \param resZ resolution of grid along Z-axis (16 <= N <= 2048; where N = 2^z for integer z)
     * \param scaleX length of plane along X-axis (in m)
     * \param scaleZ length of plane along Z-axis (in m)
     * \param waveSizeLimit size limit that waves must surpass to be rendered
     * \param rngSeed seed for the pseudorandom number generator
     */
    tessendorf(double amplitude, double speed, MVector direction, double choppiness, double time, int resX, int resZ, double scaleX, double scaleZ, double waveSizeLimit, int rngSeed);
    
    /**
     * Generates the initial wave surface and performs Fast Fourier Transforms (FFTs) to calculate the displacement.
     * The main height displacement is based on the Fourier series in Tessendorf's equation (19).
     * The horizontal displacement is based on the Fourier series in equation (29).
     */
    MFloatPointArray    simulate();
    
private:
    /**
     * Gets the wave dispersion factor for a given vector k.
     * Calculated using Tessendorf's equations (14) and (18) combined.
     */
    double              omega(MVector k);
    
    /**
     * Gets the value of the Phillips spectrum, which models wind-driven waves, for a given vector k.
     * Calculated using Tessendorf's equations (23) and (24) combined.
     */
    double              P_h(MVector k);
    
    /**
     * Gets the value of h~-sub-naught for a given vector k at the current simulation time.
     * Calculated using Tessendorf's equation (25).
     */
    complex             h_tilde_0(MVector k);
    
    /**
     * Gets the value of h~ for a given vector k at the current simulation time.
     * Calculated using Tessendorf's equation (26).
     */
    complex             h_tilde(MVector k);
};

#endif /* defined(__TessendorfOceanNode__tessendorf__) */
