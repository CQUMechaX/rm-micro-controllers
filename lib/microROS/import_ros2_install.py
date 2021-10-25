Import("env")
global_env = DefaultEnvironment()

import os 

for package in os.listdir():
    if os.path.isdir(package):
        if os.path.exists(package + "/include"):
            env.Append(CPPPATH=[os.path.abspath(package + "/include")]) 
        # if os.path.exists(package + "/lib"):
        #     for staticLib in os.listdir(package + '/lib'):
        #         if os.path.isfile(package + '/lib/' + staticLib):
        #             env.Append(LINKFLAGS=[os.path.abspath(package + "/lib/" + staticLib)])
env.Append(LINKFLAGS=['-Wl,--start-group'])
env.Append(LINKFLAGS=[os.path.abspath('libmicroros.a')])
# env.Append(LINKFLAGS=['-Wl,--end-group'])
            # print(package)
# env.Append(LINKFLAGS=['-Wl,lib/micro-ros/rclc/lib/librclc.a'])
# env.Append(CPPEFINES=['-DUNREAL'])
# env.Append(CCFLAGS=['-DUNREAL'])

# print(env.Dump())