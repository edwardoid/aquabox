#!/usr/bin/python3

import requests
import re
import tarfile
import traceback
import json
import shutil
import os
import subprocess

base_url = ""

env = {}

def set_env(prop, value):
    global env
    if prop[0] != '$':
        prop = '$' + prop
    if value is not None:
        env[prop] = value

def get_env(prop, default = None):
    global env
    if prop[0] != '$':
        prop = '$' + prop
    v = env.get(prop)
    if v is None:
        return default
    return v

def get_globals():
    global env
    try:
        if os.path.isfile("current_manifest.json"):
            with open("current_manifest.json", 'r') as file:
                manifest = json.loads(file.read())
                if "env" in manifest:
                    env = manifest["env"]
    except:
        pass

def merge_env(new_env):
    global env
    for v in new_env.keys():
        env[v] = new_env[v]

    for v in env.keys():
        if v[0] is '-':
            del env[v]


def apply_env(string):
    global env
    try:
        for v in env.keys():
            if env[v] is not None:
                upd = string.replace(v, env[v])
            if upd is not string:
                string = upd
    except:
        return ""
    return string

def get_packages():
    global base_url
    global env
    try:
        resp = requests.get(base_url)
        packes_regexp = re.compile("aquabox-" + get_env("$APP") + get_env("$APP_POSTFIX", "") + "-\w+.tar.gz")
        packages = packes_regexp.findall(resp.text)
        packages = list(dict.fromkeys(packages))
        if len(packages) is 0:
            return None
        build_num = re.compile("\d+")
        return sorted(packages, key = lambda pkg: int(build_num.findall(pkg)[0]))
    except BaseException as error:
        print('An exception occurred: {}'.format(error))
    return None

def pick_update(available_packages):
    global env
    if len(available_packages) is 0:
        return None
    try:
        upd = available_packages.index(get_env("$UPDATE_PACKAGE", "")) + 1
        if upd is len(available_packages):
            upd = None
    except:
	    upd = available_packages[-1]
    return upd

def downlad_package(pkg):
    try:
        destination = pkg
        print("Downloading " + pkg + " as " + destination + " from " + base_url + pkg)
        r = requests.get(base_url + pkg, allow_redirects=True)
        f= open(destination, 'wb')
        print("Saving as " + destination)
        f.write(r.content)
        f.close()
        return destination
    except BaseException as error:
        print('An exception occurred: {}'.format(error))
    return None

def do_exec(action):
    if "data" in action:
        os.system(apply_env(action["data"]))
    else:
        print ("Action does not have command attribute!")
        return False
    return True

def do_extract(action, arch):
    source = action.get("source")
    source = apply_env(source)
    destination = action.get("destination")
    destination = apply_env(destination)
    files_in_arch = arch.getmembers()
    if ([x for x in files_in_arch if x.name is source]) != 0:
        stream = arch.extractfile(source)
        open(destination, "wb").write(stream.read())
    else:
        print ("File " + source + " was not found in update package")
        return False
    return True

def do_copy(action):
    src = action.get("source")
    dst = action.get("destination")
    try:
        if dst is None:
            return False
        src = apply_env(src)    
        dst = apply_env(dst)
        if os.path.isfile(src):
            shutil.copyfile(src, dst, follow_symlinks=True)
        elif os.path.isdir(src):
            shutil.copytree(src, dst)
        return True
    except:
        return False
        

def do_remove(action):
    src = action.get("source")
    src = apply_env(src)
    try:
        if os.path.isfile(src):
            os.remove()
        elif os.path.isdir(src):
            shutil.rmtree(src)
        return True
    except:
        return False

def do_move(action):
    src = action.get("source")
    dst = action.get("destination")
    try:
        if dst is None:
            return False
        src = apply_env(src)
        dst = apply_env(dst)
        if os.path.isfile(src):
            shutil.move(src, dst, follow_symlinks=True)
        elif os.path.isdir(src):
            shutil.move(src, dst)
        return True
    except:
        return False

def do_mkdir(action):
    src = action.get("source")
    src = apply_env(src)
    try:
        print("Creating dir " + src)
        os.makedirs(src, exist_ok=True)
        return True
    except FileExistsError as e:
        return true
    except:
        return False

def do_file(action):
    operation = action.get("operation")
    source = action.get("source")
    if operation is None:
        print("No operation specified")
        return False
    if source is None:
        print("Source if not specified")
        return False
    if source is "":
        print("Source if not specified")
        return False
    ret = False
    if operation == "copy":
        ret = do_copy(action)
    elif operation == "remove":
        ret = do_remove(action)
    elif operation == "move":
        ret = do_move(action)
    elif operation == "mkdir":
        ret = do_mkdir(action)
    else:
        print ("Unknown operation " + operation)
    return ret

def do_action(action, arch):
    ret = False
    if action["action"] == "exec":
        ret = do_exec(action)
    elif action["action"] == "extract":
        ret = do_extract(action, arch)
    elif action["action"] == "file":
        ret = do_file(action)
    elif action["action"] == "patch":
        ret = do_file(action)
    else:
        print("Unknown action: " + action["action"])
    return ret

def do_all_actions(actions, arch):
    for action in actions:
        print("Applying: " + json.dumps(action))
        mandatory = action.get("mandatory")
        if mandatory is None:
            mandatory = False
        res = do_action(action, arch)
        if res is False and mandatory is True :
            return False
    return True

def do_prepare(actions, arch):
    return do_all_actions(actions, arch)

def do_update(actions, arch):
    return do_all_actions(actions, arch)

def do_finish(actions, arch):
    return do_all_actions(actions, arch)

def apply_steps(steps, arch):
    if "prepare" in steps:
        if not do_prepare(steps["prepare"], arch):
            return "prepare"
    if "update" in steps:
        if not do_update(steps["update"], arch):
            return "update"
    if "finish" in steps:
        if not do_finish(steps["finish"], arch):
            return "finish"
    return None

def recover(step):
    return True

def update(pkg):
    global env
    try:
        print("Loading " + pkg)
        arch = tarfile.open(pkg)
        files_in_arch = arch.getmembers()
        if ([x for x in files_in_arch if x.name is "manifest.json"]) != 0:
            stream = arch.extractfile("manifest.json")
            open("manifest.json", "wb").write(stream.read())
        else:
            print("Package " + pkg + " does not contain manifest file")
            return False
        with open("manifest.json", 'r') as file:
            manifest = json.loads(file.read())
            if "env" in manifest:
                merge_env(manifest["env"])
            if "steps" in manifest:
                failed_step = apply_steps(manifest["steps"], arch)
                if failed_step is None:
                    print ("Update applied")
                    set_env("$UPDATE_PACKAGE", pkg)
                    manifest["env"] = env
                    with open('current_manifest.json', 'w') as outfile:
                        json.dump(manifest, outfile)
                    return True
                else:
                    return recover(failed_step)
        return False
                    
    except BaseException as error:
        print('An exception occurred: {}'.format(error))
        traceback.print_exc()
        return False

get_globals()

for v in env:
    print (v + " = " + env[v])

set_env("$UPDATE_PACKAGE", "");
set_env("$APP_POSTFIX", "");

base_url += get_env("$UPDATE_SERVER", "http://193.37.152.213:8081") + "/" + get_env("$PLATFORM") + "/"

packages_available = get_packages()
if packages_available is None:
    print("No packages available")
    exit(1)

update_pkg = pick_update(packages_available)
if update_pkg is None:
    print ("Installed version is latest")
    exit(0)

pkg = downlad_package(update_pkg)
if pkg is None:
    print("Can't download package!")
    exit(1)

if update(pkg):
    exit(0)
else:
    print("Can't apply update")
    os.remove(pkg)
    exit(1)
