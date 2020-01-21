import os
import sys
import click
import json
import shutil
from lib.code import compute_header_md5sum, get_depends_from_source

"""
1. Create new project via:
* Predefined template: scripts/templates/new_project_template
  - Copy new_project_template/* and replace appname ...
  - Copy new_project_template/.vscode/* and replace appname ...

* Builtin demo apps:   app/example/*
  - Copy app/example/*/* and replace app name ...
  - Copy new_project_template/.vscode/* and replace appname ...
  - Update Config.in with build required configs

2. Copy board to project directory
3. Source Config.in files from depend
4. Write initial headers md5sum.

"""

from imp import reload
try:
    reload(sys)
    sys.setdefaultencoding('UTF8')
except:
    pass

scriptdir = os.path.dirname(os.path.abspath(__file__))
template = "templates/new_project_template"
COMP_INDEX = "aos_comp_index.json"


def get_comp_depends(comp_info, comps):
    """ Get comp depends from comp index """
    depends = []
    for comp in comps:
        if comp in comp_info:
            depends += comp_info[comp]["dependencies"]

    if depends:
        depends += get_comp_depends(comp_info, depends)

    return list(set(depends))


def write_depends_config(config_file, board, app=None):
    """ Source Config.in of depends """
    configs = []
    comp_info = {}
    aos_sdk = os.environ.get("AOS_SDK_PATH")
    with open(os.path.join(aos_sdk, COMP_INDEX), "r") as f:
        comp_info = json.load(f)

    comps = [board]
    if app:
        comps.append(app)

    if comp_info:
        depends = get_comp_depends(comp_info, comps)
        depends.append(board)
        for comp in depends:
            if comp in comp_info:
                config = comp_info[comp]["config_file"]
                if config:
                    configs.append(config)

    if configs:
        configs = sorted(list(set(configs)))
        with open(config_file, "a") as f:
            f.write("\n")
            for config in configs:
                if board in config:
                    # USER_APP_PATH (APPDIR) is exported by "aos make"
                    line = 'source "$USER_APP_PATH/board/%s/Config.in"\n' % board
                else:
                    line = 'source "$AOS_SDK_PATH/%s"\n' % config
                f.write(line)


def write_file(contents, destfile):
    """ Write contents to destfile """
    subdir = os.path.dirname(destfile)

    if not os.path.isdir(subdir):
        os.makedirs(subdir)

    with open(destfile, "w") as f:
        for line in contents:
            f.write(line)


def copy_template_file(tempfile, templatedir, destdir, projectname, board):
    """ Copy template file to destdir """
    contents = []

    # Replace projectname from file contents
    with open(os.path.join(templatedir, tempfile), "r") as f:
        for line in f.readlines():
            if "@projectname@" in line:
                line = line.replace("@projectname@", projectname)
            elif "@PROJECTNAME@" in line:
                line = line.replace("@PROJECTNAME@", projectname.upper().replace("-", "_"))
            else:
                pass

            if "@boardname@" in line:
                line = line.replace("@boardname@", board)

            contents += [line]

    # Replace projectname from filename
    if tempfile == "aos.mk.temp":
        destfile = "aos.mk"
    elif "projectname" in tempfile:
        destfile = tempfile.replace("projectname", projectname)
    else:
        destfile = tempfile

    # Write to destfile
    if contents:
        destfile = os.path.join(destdir, destfile)
        write_file(contents, destfile)


def update_demo_app_config(config_file, projectname, board):
    """ Write build required configs to dest Config.in """
    contents = """
config AOS_BUILD_BOARD
    string
    default "%s"

config AOS_BUILD_APP
    string
    default "%s"

config AOS_SDK_PATH
    string
    option env="AOS_SDK_PATH"

config USER_APP_PATH
    string
    option env="APPDIR"
""" % (board, projectname)

    with open(config_file, "a") as f:
        f.write(contents)


def copy_demo_app_file(appfile, appdir, destdir, projectname, board, appname):
    """ Copy demo app source file to destdir """
    contents = []
    u_appname = appname.upper()
    u_projectname = projectname.upper()
    with open(os.path.join(appdir, appfile), "r") as f:
        for line in f.readlines():
            if "AOS_APP_%s" % u_appname in line:
                line = line.replace(u_appname, u_projectname)
            if line.startswith("NAME") and appname in line:
                line = line.replace(appname, projectname)
            if line.startswith("$(NAME)_SUMMARY") and appname in line:
                line = line.replace(appname, projectname)

            contents += [line]

    if contents:
        destfile = os.path.join(destdir, appfile)
        write_file(contents, destfile)


def get_sources(templatedir):
    """ Get sources files from templatedir and subdir """
    sources = []
    for root, dirs, files in os.walk(templatedir):
        for filename in files:
            if filename == "ucube.py":
                continue

            tempfile = "%s/%s" % (root, filename)
            sources.append(tempfile.replace("\\", "/"))

    templatedir = templatedir.replace("\\", "/")
    sources = [item.replace(templatedir + "/", "") for item in sources]

    return sources


def copy_template(templatedir, destdir, projectname, board):
    """ Copy predefined template to destdir """
    sources = get_sources(templatedir)

    for tempfile in sources:
        copy_template_file(tempfile, templatedir, destdir, projectname, board)

    # copy board to project directory
    copy_board_to_project(board, destdir)

    # update dest Config.in
    config_file = os.path.join(destdir, "Config.in")
    write_depends_config(config_file, board)


def copy_demo_app(appdir, destdir, projectname, board, appname):
    """ Copy builtin demo app to destdir """
    sources = get_sources(appdir)
    for appfile in sources:
        if "README.md" in appfile:
            continue

        copy_demo_app_file(appfile, appdir, destdir, projectname, board, appname)

    # copy .vscode from predefined template
    templatedir = os.path.abspath(os.path.join(scriptdir, template)).replace("\\", "/")
    vscode = os.path.join(templatedir, ".vscode")
    for tempfile in os.listdir(vscode):
        copy_template_file(".vscode/" + tempfile, templatedir, destdir, projectname, board)

    # copy README.md from predefined template
    copy_template_file("README.md", templatedir, destdir, projectname, board)

    # copy board to project directory
    copy_board_to_project(board, destdir)

    # update dest Config.in
    config_file = os.path.join(destdir, "Config.in")
    update_demo_app_config(config_file, projectname, board)
    write_depends_config(config_file, board, appname)


def write_project_config(config_file, config_data):
    """ Write projet config file: .aos """
    contents = []
    if os.path.isfile(config_file) and config_data:
        with open(config_file, "r") as f:
            for line in f.readlines():
                line = line.strip()
                tmp = line.split("=")
                key, value = tmp[0], tmp[1]
                if key not in config_data:
                    config_data[key] = value

    for key in config_data:
        contents.append("%s=%s\n" % (key, config_data[key]))

    contents = sorted(contents)
    write_file(contents, config_file)


def check_project_name(projectname):
    """ Generate aos_comp_index and check projectname """
    aos_sdk = os.environ.get("AOS_SDK_PATH")
    if not aos_sdk:
        click.echo("[Error] AliOS Things SDK is not found!")
        sys.exit(1)

    aos_comp_index = os.path.join(aos_sdk, COMP_INDEX)
    if not os.path.isfile(aos_comp_index):
        os.system("python %s/app_gen_comp_index.py %s %s" % (scriptdir, aos_sdk, aos_comp_index))

    comp_info = {}
    with open(aos_comp_index, "r") as f:
        comp_info = json.load(f)
        if projectname in comp_info:
            click.echo("[Error] The project name \"%s\" is reserved!" % projectname)
            sys.exit(1)

    return comp_info


def copy_board_to_project(board, dest_dir):
    """ Copy the board to project directory """
    aos_sdk = os.environ.get("AOS_SDK_PATH")
    board_dir = os.path.join(aos_sdk, "platform/board", board)
    if not os.path.isdir(board_dir):
		board_dir = os.path.join(aos_sdk, "platform/board/board_legacy", board)
		if not os.path.isdir(board_dir):
			click.echo("[Error] No such directory: %s!" % board_dir)
        
    dest_dir = os.path.join(dest_dir, "board", board)
    shutil.copytree(board_dir, dest_dir)


@click.command()
@click.argument("projectname", metavar="[PROJECTNAME]")
@click.option("-b", "--board", required=True, help="Board for creating project")
@click.option("-d", "--projectdir", required=True, help="The project directory")
@click.option("-t", "--templateapp", help="Template application for creating project")
def cli(projectname, board, projectdir, templateapp):
    """ Create new project from template or builtin app """
    comp_info = check_project_name(projectname)

    aos_sdk = os.environ.get("AOS_SDK_PATH")
    if templateapp:
        if templateapp in comp_info:
            templatedir = comp_info[templateapp]["location"]
            templatedir = os.path.join(aos_sdk, templatedir)
        else:
            click.echo("No such application found: \"%s\"" % templateapp)
    else:
        templatedir = os.path.join(scriptdir, template)

    templatedir = os.path.abspath(templatedir).replace("\\", "/")

    if not projectdir:
        projectdir = os.path.join(scriptdir, "../../app")

    destdir = os.path.join(projectdir, projectname)
    destdir = os.path.abspath(destdir)

    if os.path.exists(destdir):
        if os.path.isfile(destdir):
            click.echo("[Error] Can't create project directory, the file is existing!\n%s" % destdir)
        else:
            click.echo("[Error] The project directory is existing!\n%s" % destdir)

        return 1
    else:
        os.makedirs(destdir)

    if templateapp:
        copy_demo_app(templatedir, destdir, projectname, board, templateapp)
    else:
        copy_template(templatedir, destdir, projectname, board)

    # Initial project config
    (md5sum, include_list) = compute_header_md5sum(destdir)
    depends = get_depends_from_source(comp_info, include_list)
    config_data = {
        "DEPENDENCIES": " ".join(depends),
        "MD5SUM_HEADER": md5sum,
    }
    project_config = os.path.join(destdir, ".aos")
    write_project_config(project_config, config_data)

    click.echo("[Info] Project Initialized at: %s" % destdir)


if __name__ == "__main__":
    cli()
