//
//  tessendorf.cpp
//  TessendorfOceanNode
//
//  Created by Steven Dao on 7/25/13.
//
//

#include "tessendorf.h"
#include "helpers.h"
#include "kissfft.hh"
#include <maya/MGlobal.h>
#include <sstream>

tessendorf::tessendorf(double amplitude, double speed, MVector direction, double choppiness, double time, int resX, int resZ, double scaleX, double scaleZ, double waveSizeLimit, int rngSeed)
{
    A = amplitude;
    V = speed;
    w_hat = direction.normal();
    t = time;
    lambda = choppiness;
    M = resX;
    N = resZ;
    Lx = scaleX;
    Lz = scaleZ;
    l = waveSizeLimit;
    seed = rngSeed;
}

double tessendorf::omega(MVector k)
{
    return floor(sqrt(GRAVITY * k.length()) / omega_0) * omega_0;
}

double tessendorf::P_h(MVector k)
{
    double k_length = k.length();
    
    if (k_length < DBL_EPSILON) {
        return 0.; // Avoid divison by zero error;
    }
    
    double L = pow(V, 2) / GRAVITY;
    MVector k_hat = k.normal();
    
    double nomin = exp(-1. / pow(k_length * L, 2));
    double denom = pow(k_length, 4);
    double scale = exp(-pow(k_length, 2) * pow(l, 2));
    
    return A * nomin / denom * pow(k_hat * w_hat, 2) * scale;
}

complex tessendorf::h_tilde_0(MVector k)
{
    complex xi = random_gaussian_complex();
    
    return xi * (double)sqrt(P_h(k) / 2.);
}

complex tessendorf::h_tilde(MVector k)
{
    complex h_tilde_0_k = h_tilde_0(k);
    complex h_tilde_0_k_star = h_tilde_0(-k);
    
    double omega_k_t = omega(k) * t;
    
    double cos_omega_k_t = cos(omega_k_t);
    double sin_omega_k_t = sin(omega_k_t);
    
    complex c0(cos_omega_k_t, sin_omega_k_t);
    complex c1(cos_omega_k_t, -sin_omega_k_t);
    
    return h_tilde_0_k * c0 + h_tilde_0_k_star * c1;
}

MFloatPointArray tessendorf::simulate()
{
    srand(seed);
    vertices.clear();
    
    complex* h_tildes_in = new complex[M*N];
    complex* disp_x_in = new complex[M*N];
    complex* disp_z_in = new complex[M*N];
    
    complex* h_tildes_out = new complex[M*N];
    complex* disp_x_out = new complex[M*N];
    complex* disp_z_out = new complex[M*N];
    
    for (int m = 0; m < M; m++) {
        for (int n = 0; n < N; n++) {
            int index = m * N + n;
            
            int m_ = m - M / 2;  // m coord offsetted.
            int n_ = n - N / 2; // n coord offsetted.
            
            MVector k(2. * M_PI * n_ / Lx, 0., 2. * M_PI * m_ / Lz);
            
            complex h_tilde_k = h_tilde(k);
            h_tildes_in[index] = h_tilde_k;
            
            MVector k_hat = k.normal();
            disp_x_in[index] = complex(0., -k_hat.x) * h_tilde_k; // Displacement by equation (29).
            disp_z_in[index] = complex(0., -k_hat.z) * h_tilde_k;
        }
    }
    
    kissfft<double> fft(M*N, false);
    fft.transform(h_tildes_in, h_tildes_out);
    fft.transform(disp_x_in, disp_x_out);
    fft.transform(disp_z_in, disp_z_out);
    
    double signs[2] = { -1., 1. };
    
    for (int m = 0; m < M; m++) {
        for (int n = 0; n < N; n++) {
            int index = m * N + n;
            int sign = signs[(m + n) & 1]; // Sign-flip all of the odd coefficients.
            
            int m_ = m - M / 2;  // m coord offsetted.
            int n_ = n - N / 2;  // n coord offsetted.
            
            MFloatVector x(n_ * Lx / N + real(disp_x_out[index]) * lambda * sign,
                           real(h_tildes_out[index]) * sign,
                           m_ * Lz / M + real(disp_z_out[index]) * lambda * sign);
            vertices.append(x);
        }
    }
    
    return vertices;
}

/*
 {
 return MVector(2. * M_PI * n / Lx, 0., 2. * M_PI * m / Lz);
 }
 */