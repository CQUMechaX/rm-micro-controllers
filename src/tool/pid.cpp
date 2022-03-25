#include "tool/pid.hpp"

#include <freertos_os2.h>
#include <task.h>

#include "marco.hpp"

#define ISNAN_OR_ZERO(val) (isnan(val) ? 0 : val)  // isnormal
