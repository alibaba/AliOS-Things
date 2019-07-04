import os
import sys
import click

reload(sys)
sys.setdefaultencoding('UTF8')

scriptdir = os.path.dirname(os.path.abspath(__file__))
template = "templates/new_project_template"

def copy_template(tempfile, templatedir, destdir, projectname, board):
    """ Copy template files to destdir """

    contents = []
    # Replace projectname from file contents

    with open(os.path.join(templatedir, tempfile), "r") as f:
        for line in f.readlines():
            if "@projectname@" in line:
                line = line.replace("@projectname@", projectname)
            elif "@PROJECTNAME@" in line:
                line = line.replace("@PROJECTNAME@", projectname.upper())
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
        with open(os.path.join(destdir, destfile), "w") as f:
            for line in contents:
                f.write(line)


def write_project_config(config_file):
    """ Write projet config file """
    config_data = {"AOS_SDK_PATH": os.environ["AOS_SDK_PATH"]}
    with open(config_file, "w") as f:
        for key in config_data:
            f.write("%s=%s\n" % (key, config_data[key]))


@click.command()
@click.argument("projectname", metavar="[PROJECTNAME]")
@click.option("-b", "--board", required=True, help="Board for creating project")
@click.option("-d", "--projectdir", required=True, help="The project directory")
def cli(projectname, board, projectdir):
    """ Create new project from template """
    templatedir = os.path.join(scriptdir, template)
    if not projectdir:
        projectdir = os.path.join(scriptdir, "../../app")

    destdir = os.path.join(projectdir, projectname)
    destdir = os.path.abspath(destdir)

    if os.path.isdir(destdir):
        click.echo("[Error] The project directory is existing!\n%s" % destdir)
        return 1
    else:
        os.makedirs(destdir)

    sources = os.listdir(templatedir)
    for tempfile in sources:
        copy_template(tempfile, templatedir, destdir, projectname, board)

    write_project_config(os.path.join(destdir, ".aos"))
    click.echo("[Info] Project Initialized at: %s" % destdir)


if __name__ == "__main__":
    cli()
