try:
    import configparser
except ImportError:
    import ConfigParser as configparser

import subprocess
import os
import datetime

Import("env")

def get_git_hash():
    try:
        return subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode('utf-8').strip()
    except:
        return "unknown"

def get_git_branch():
    try:
        return subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD']).decode('utf-8').strip()
    except:
        return "unknown"

def make_build_info(source, target, env):
    git_hash = get_git_hash()
    git_branch = get_git_branch()
    build_date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    
    # Try to get version from platformio.ini (optional)
    # For now hardcoding or leaving empty
    
    header_content = """#pragma once

#define BUILD_TIMESTAMP "{} "
#define BUILD_GIT_HASH "{}"
#define BUILD_GIT_BRANCH "{}"
#define BUILD_BY "PlatformIO"

""".format(build_date, git_hash, git_branch)

    include_dir = os.path.join(env.subst("$PROJECT_DIR"), "include")
    if not os.path.exists(include_dir):
        os.makedirs(include_dir)
        
    output_file = os.path.join(include_dir, "build_info.h")
    
    with open(output_file, 'w') as f:
        f.write(header_content)
    
    print("Generated build_info.h: {} / {}".format(build_date, git_hash))

# Run before build
env.AddPreAction("src/main.cpp", make_build_info)
env.AddPreAction("src/main.o", make_build_info)
