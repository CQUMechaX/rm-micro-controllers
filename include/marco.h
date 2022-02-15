/** @file marco.h/hpp
 * @dir 
 * @brief Provide marcos related to math caculating, and check math header.
 * Include "marco.h/hpp" after include system header.
 * @author
 * @date
 */
#pragma once

#if defined(_MATH_H_) || defined(_GLIBCXX_CMATH)
# if ! defined(_ARM_MATH_H)
#  error "FPU is enabled. You should use <arm_math.h> instead"
# endif /* ! defined(_ARM_MATH_H) */
#endif /* defined(_MATH_H_) || defined(_GLIBCXX_CMATH) */

#ifndef __FPU_PRESENT
# define __FPU_PRESENT 1U // defined in hal.h | In case
#endif /* __FPU_PRESENT */
#ifndef ARM_MATH_CM4
# define ARM_MATH_CM4 // defined in extra_script.py | For intlligence.
#endif /* ARM_MATH_CM4 */

#ifndef __cplusplus

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <arm_math.h>

#endif /* ! __cplusplus */

#define rad_format(ang) (ang)
#define RAD_FORMAT(ang) CONSTRAIN_ABS(ang, PI)
#define float_constrain(val, min, max) ((val < max) ? ((val < min) ? min : val) : max)
#define CONSTRAIN(val, min, max) ((val < max) ? ((val < min) ? min : val) : max)
#define CONSTRAIN_ABS(val, max) (max ? ((val < max) ? ((val < -max) ? -max : val) : max) : val)
#define CONSTRAIN_ARR(val, minmax) ((val < minmax[1]) ? ((val < minmax[0]) ? minmax[0] : val) : minmax[1])
#define FORMAT_ANGLE(val, max) ((val < max) ? val : val - max )
#define NORMAL_OR_ZERO(val) (isnormal(val) ? 1 : val == 0.0)
