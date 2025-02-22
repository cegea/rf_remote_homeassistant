import os
Import("env")

# include toochain paths
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# override compilation DB path
env.Replace(COMPILATION_PATH=os.path.join("$BUILD_DIR", "compile_commands.json"))
