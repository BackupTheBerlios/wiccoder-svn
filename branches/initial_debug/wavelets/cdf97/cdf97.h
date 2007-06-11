/*
 * QuikInfo:
 *
 * A lifting implementation of Daubechies 9/7 biorthogonal wavelet filter.
 *
 * References:
 *
 * A. Cohen, I. Daubechies, J. C. Feauveau, "Biorthogonal Bases of
 * Compactly Supported Wavelets," Communications on Pure and
 * Applied Mathematics, vol. 45, no. 5, pp. 485-560, May 1992.
 *
 * I. Daubechies and W. Sweldens, "Factoring Wavelet Transforms Into
 * Lifting Steps," J. Fourier Anal. Appl., vol. 4, no. 3, 
 * pp. 245-267, 1998.
 *
 * M. Antonini, M. Barlaud, P. Mathieu, I. Daubechies, "Image Coding Using
 * Wavelet Transform," IEEE Transactions on Image Processing, vol. 1,
 * no. 2, pp. 205-220, April 1992.
 * 
 */

#ifndef CDF97_H
#define CDF97_H

#ifdef __cplusplus
extern "C" {
#endif

int wt2d_cdf97_fwd(float *subbands, int rows, int cols, int levels);
int wt2d_cdf97_inv(float *subbands, int rows, int cols, int levels);

#ifdef __cplusplus
}
#endif

#endif /* CDF97_H */
