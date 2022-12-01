Import("env") # post: env + projenv
import os
env.Append(
    # Add semihosting feature
    # LINKFLAGS=["--specs=rdimon.specs"],
    # LIBS=["rdimon"]
    LINKFLAGS=[
        # Identical to pre-compiled microROS library.
	    "-mfloat-abi=hard",
        "-mfpu=fpv4-sp-d16",
    ],
    CCFLAGS=[
        # "-larm_cortexM4lf_math",
        "-Wall",
        "-D__DSP_PRESENT", # avoid cmsis_gcc.h
        "-DARM_MATH_CM4",
        "-D__GIT_COMMIT_ID__=\\\"{}\\\"".format(os.popen("git rev-parse HEAD").read().split()[0]),
        # Identical to pre-compiled microROS library.
	    "-mfloat-abi=hard",
        "-mfpu=fpv4-sp-d16",
    ],
    CFLAGS=[
        "-std=c11"
    ],
    CXXFLAGS=[
        "-std=c++14"
    ]
    # for arm_math.h, see https://github.com/platformio/platform-ststm32/issues/591
)
# env.ProcessUnFlags(["--specs=nosys.specs", "--specs=nano.specs", "-lnosys"])
# print(env)
# env.LINKFLAGS.erase("--specs=nosys.specs")
# env.LIBS.erase("nosys")