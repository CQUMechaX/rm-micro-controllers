// #define rad_format(Ang) loop_fp32_constrain((Ang), -PI, PI)
#pragma once

#ifndef __cplusplus

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#endif /* ! __cplusplus */

#define rad_format(ang) (ang)
#define float_constrain(val, min, max) ((val < max) ? ((val < min) ? min : val) : max)
#define CONSTRAIN(val, min, max) ((val < max) ? ((val < min) ? min : val) : max)
#define CONSTRAIN_ABS(val, max) (max ? ((val < max) ? ((val < -max) ? -max : val) : max) : val)
#define CONSTRAIN_ARR(val, minmax) ((val < minmax[1]) ? ((val < minmax[0]) ? minmax[0] : val) : minmax[1])
#define DEGREE_CONSTRAIN()