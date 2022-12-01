# https://github.com/platformio/platformio-vscode-ide/issues/1940
Import("env")
env.Tool('compilation_db')
env.CompilationDatabase('compile_commands.json')