#ifndef _tensor_h_
#define _tensor_h_
/* tensor.h
 *
 * Copyright (C) 2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "melder.h"
#include <math.h>

inline static double sqrt_scalar (double x) {
	#if defined (_WIN32)
		if (x < 0.0) return undefined;
	#endif
	return sqrt (x);
}

inline static double sum_scalar (numvec x) {
	double sum = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		sum += x [i];
	}
	return sum;
}

inline static double mean_scalar (numvec x) {
	if (x.size == 0) return undefined;
	double sum = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		sum += x [i];
	}
	return sum / x.size;
}

double stdev_scalar (numvec x) noexcept;
double center_scalar (numvec x) noexcept;

inline static double inner_scalar (numvec x, numvec y) {
	if (x.size != y.size) return undefined;
	double result = 0.0;
	for (long i = 1; i <= x.size; i ++) {
		result += x [i] * y [i];
	}
	return result;
}

autonumvec copy_numvec (numvec x);

inline static autonumvec add_numvec (numvec x, numvec y) {
	//if (x.size != y.size) return autonumvec { nullptr, 0, 0 };
	autonumvec result (x.size, false);
	for (long i = 1; i <= x.size; i ++) {
		result [i] = x [i] + y [i];
	}
	return result;
}

autonummat copy_nummat (nummat x);
autonummat outer_nummat (numvec x, numvec y);

/* End of file tensor.h */
#endif
