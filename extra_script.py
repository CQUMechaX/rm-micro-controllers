Import("env")

# Add semihosting feature
env.Append(
    # LINKFLAGS=["--specs=rdimon.specs"],
    # LIBS=["rdimon"]
    LINKFLAGS=[
        "-mfpu=fpv4-sp-d16",
	    "-mfloat-abi=hard"
    ]
)
# env.ProcessUnFlags(["--specs=nosys.specs", "--specs=nano.specs", "-lnosys"])
# print(env)
# env.LINKFLAGS.erase("--specs=nosys.specs")
# env.LIBS.erase("nosys")