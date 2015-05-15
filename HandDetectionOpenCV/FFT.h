//
//  FFT.h
//  HandDetectionOpenCV
//
//  Created by Vlad Mihaylenko on 01/04/15.
//  Copyright (c) 2015 Vlad Mihaylenko. All rights reserved.
//

#ifndef __HandDetectionOpenCV__FFT__
#define __HandDetectionOpenCV__FFT__
#include <complex>
#include <vector>
#include <valarray>

namespace FFT {
    
    typedef std::complex<double> Complex;
    typedef std::valarray<Complex> CArray;
    
    void fft(CArray& x);
    // inverse fft (in-place)
    void ifft(CArray& x);
    
}

#endif /* defined(__HandDetectionOpenCV__FFT__) */
