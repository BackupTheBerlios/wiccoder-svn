/*****************************************************************************
* A floating-point lifting implementation of Daubechies 9/7 biorthogonal wavelet filter.
* Does 58.6 dB PSNR on a Lena test image.
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
* TODO: The code performance is very slow, there's no use of it in
*    realtime applications.
*****************************************************************************/

#include <stdlib.h>
#include "cdf97.h"

#define MAX(_x, _y) (_x > _y ? _x : _y)
#define ROUND(_x) ((float)(((_x) < 0) ? (int) ((_x) - 0.5f) : (int) ((_x) + 0.5f)))
#define FIX(_x) ((_x) < 0 ? 0 : ((_x) > 255 ? 255 : (_x)))

/* Cohen - Daubechies - Feauveau 9/7 (CDF 9/7) biorthogonal wavelet
// Coefficients for the lifting scheme
*/
#define ALPHA     -1.58615986717275f
#define BETA      -0.05297864003258f
#define GAMMA      0.88293362717904f
#define DELTA      0.44350482244527f
#define EPSILON    1.14960430535816f

/*****************************************************************************
* wt1d_cdf97_fwd()
* One-dimensional forward CDF 9/7 DWT transform by a lifting.
* IN  :  - *signal_in,signal_length
* OUT :  - *signal_out
* RET :	 void
* NOTE:  
*****************************************************************************/
static __inline void wt1d_cdf97_fwd(float *signal_in, float *signal_out, int signal_length)
{
  float *even, *odd;
  int i, half;

  for (i = 1; i < signal_length - 2; i += 2)
		signal_in[i] += ALPHA * (signal_in[i - 1] + signal_in[i + 1]);
  signal_in[signal_length - 1] += 2 * ALPHA * signal_in[signal_length - 2];

  signal_in[0] += 2 * BETA * signal_in[1];
  for (i = 2; i < signal_length; i += 2)
		signal_in[i] += BETA * (signal_in[i + 1] + signal_in[i - 1]);

  for (i = 1; i < signal_length - 2; i += 2)
		signal_in[i] += GAMMA * (signal_in[i - 1] + signal_in[i + 1]);
  signal_in[signal_length - 1] += 2 * GAMMA * signal_in[signal_length - 2];

  signal_in[0] = EPSILON * (signal_in[0] + 2 * DELTA * signal_in[1]);
  for (i = 2; i < signal_length; i += 2)
		signal_in[i] = EPSILON * (signal_in[i] + DELTA * (signal_in[i + 1] + signal_in[i - 1]));

  for (i = 1; i < signal_length; i += 2)
		signal_in[i] /= (-EPSILON);

  half = signal_length >> 1;

  even = signal_out;
  odd = signal_out + half;

  for (i = 0; i < half; i++) {
    even[i] = signal_in[i << 1];
    odd[i] = signal_in[(i << 1) + 1];
  }
}

/*****************************************************************************
* wt1d_cdf97_inv()
* One-dimensional inverse CDF 9/7 DWT transform by a lifting.
* IN  :  - *signal_in,signal_length
* OUT :  - *signal_out
* RET :	 void
* NOTE:  
*****************************************************************************/
static __inline void wt1d_cdf97_inv(float *signal_in, float *signal_out, int signal_length)
{
  float *even, *odd;
  int i, half;

  half = signal_length >> 1;

  even = signal_in;
  odd = signal_in + half;

  for (i = 0; i < half; i++) {
    signal_out[i << 1] = even[i];
    signal_out[(i << 1) + 1] = odd[i];
  }
 
  for (i = 1; i < signal_length; i += 2)
		signal_out[i] *= (-EPSILON);

  signal_out[0] = signal_out[0] / EPSILON - 2 * DELTA * signal_out[1];
  for (i = 2; i < signal_length; i += 2)
		signal_out[i] = signal_out[i] / EPSILON - DELTA * (signal_out[i + 1] + signal_out[i - 1]);

  for (i = 1; i < signal_length - 2; i += 2)
		signal_out[i] -= GAMMA * (signal_out[i - 1] + signal_out[i + 1]);
  signal_out[signal_length - 1] -= 2 * GAMMA * signal_out[signal_length - 2];

  signal_out[0] -= 2 * BETA * signal_out[1];
  for (i = 2; i < signal_length; i += 2)
		signal_out[i] -= BETA * (signal_out[i + 1] + signal_out[i - 1]);

  for (i = 1; i < signal_length - 2; i += 2)
		signal_out[i] -= ALPHA * (signal_out[i - 1] + signal_out[i + 1]);
  signal_out[signal_length - 1] -= 2 * ALPHA * signal_out[signal_length - 2];
}

/*****************************************************************************
* wt2d_cdf97_fwd()
* Two-dimensional forward DWT transform.
* IN  :  - *subbands,rows,cols,levels
* OUT :  - *subbands
* RET :	 Zero if successful, nonzero otherwise.
* NOTE:  
*****************************************************************************/
int wt2d_cdf97_fwd(float *subbands, int rows, int cols, int levels)
{
  float *signal_in, *signal_out, *base;
  int cur_level, cur_cols, cur_rows;
  int i, j, max, offs, n_samples;
  int err_code;

  max = MAX(cols, rows);

  signal_in = signal_out = NULL;

  signal_in = (float *) malloc(max * sizeof(float));
  signal_out = (float *) malloc(max * sizeof(float));

  if (signal_in == NULL || signal_out == NULL) {
    err_code = 1;
    goto memory_error;
  }

  n_samples = cols * rows;

  /* DC level shift */
  for (i = 0; i < n_samples; i++)
		subbands[i] -= 128.0f;

  cur_cols = cols;
  cur_rows = rows;

  for (cur_level = 1; cur_level <= levels; cur_level++) {
    offs = cols;

    /* transform all columns */
    for (i = 0; i < cur_cols; i++) {

      base = subbands + i;

      /* load data */
      for (j = 0; j < cur_rows; j++) {
        signal_in[j] = *base;
        base += offs;
      }

      wt1d_cdf97_fwd(signal_in, signal_out, cur_rows);

      base = subbands + i;

      /* save data */
      for (j = 0; j < cur_rows; j++) {
        *base = signal_out[j];
        base += offs;
      }
    }

    offs = 1;

    /* transform all rows */
    for (i = 0; i < cur_rows; i++) {

      base = subbands + i * cols;

      /* load data */
      for (j = 0; j < cur_cols; j++) {
        signal_in[j] = *base;
        base += offs;
      }

      wt1d_cdf97_fwd(signal_in, signal_out, cur_cols);

      base = subbands + i * cols;

      /* save data */
      for (j = 0; j < cur_cols; j++) {
        *base = signal_out[j];
        base += offs;
      }
    }

    /* next scale */
    cur_cols >>= 1;
    cur_rows >>= 1;
  }

  /* uniform scalar quantinization */
  for (i = 0; i < n_samples; i++)
		subbands[i] = ROUND(subbands[i]);

  err_code = 0;

  memory_error:

  free(signal_in);
  free(signal_out);

  return err_code;
}

/*****************************************************************************
* wt2d_cdf97_inv()
* Two-dimensional inverse DWT transform.
* IN  :  - *subbands,rows,cols,levels
* OUT :  - *subbands
* RET :	 Zero if successful, nonzero otherwise.
* NOTE:  
*****************************************************************************/
int wt2d_cdf97_inv(float *subbands, int rows, int cols, int levels)
{
  float *signal_in, *signal_out, *base;
  int cur_level, cur_cols, cur_rows;
  int i, j, max, offs, n_samples;
  int err_code;

  max = MAX(cols, rows);

  signal_in = signal_out = NULL;

  signal_in = (float *) malloc(max * sizeof(float));
  signal_out = (float *) malloc(max * sizeof(float));

  if (signal_in == NULL || signal_out == NULL) {
    err_code = 1;
    goto memory_error;
  }

  cur_cols = cols >> (levels - 1);
  cur_rows = rows >> (levels - 1);

  for (cur_level = 1; cur_level <= levels; cur_level++) {
    offs = 1;

    /* transform all rows */
    for (i = 0; i < cur_rows; i++) {
      base = subbands + i * cols;

      /* load data */
      for (j = 0; j < cur_cols; j++) {
        signal_in[j] = *base;
        base += offs;
      }

      wt1d_cdf97_inv(signal_in, signal_out, cur_cols);

      base = subbands + i * cols;

      /* save data */
      for (j = 0; j < cur_cols; j++) {
        *base = signal_out[j];
        base += offs;
      }
    }

    offs = cols;

    /* transform all columns */
    for (i = 0; i < cur_cols; i++) {
      base = subbands + i;

      /* load data */
      for (j = 0; j < cur_rows; j++) {
        signal_in[j] = *base;
        base += offs;
      }

      wt1d_cdf97_inv(signal_in, signal_out, cur_rows);

      base = subbands + i;

      /* save data */
      for (j = 0; j < cur_rows; j++) {
        *base = signal_out[j];
        base += offs;
      }
    }

    /* next scale */
    cur_cols <<= 1;
    cur_rows <<= 1;
  }

  n_samples = cols * rows;

  /* undo DC level shift */
  for (i = 0; i < n_samples; i++)
		subbands[i] = FIX(ROUND(subbands[i] + 128.0f));

  err_code = 0;

memory_error:
  free(signal_in);
  free(signal_out);

  return err_code;
}
