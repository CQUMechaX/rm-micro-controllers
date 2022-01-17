// #define rad_format(Ang) loop_fp32_constrain((Ang), -PI, PI)
#define rad_format(ang) (ang)
#define float_constrain(val, min, max) ((val < max) ? ((val < min) ? min : val) : max)