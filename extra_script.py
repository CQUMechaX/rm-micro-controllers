Import("env")

# Add semihosting feature
env.Append(
    LINKFLAGS=["--specs=rdimon.specs"],
    LIBS=["rdimon"]
)
env.ProcessUnFlags(["--specs=nosys.specs", "--specs=nano.specs", "-lnosys"])
# print(env)
# env.LINKFLAGS.erase("--specs=nosys.specs")
# env.LIBS.erase("nosys")