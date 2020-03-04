import re
import os
import sys
import click

if sys.version_info[0] < 3:
    from imp import reload
    try:
        reload(sys)
        sys.setdefaultencoding('UTF8')
    except:
        pass


def get_source_files(sourcelist, appdir):
    """ Get sources from *.c files """
    src = []
    for item in sourcelist:
        if item.endswith(".c"):
            item = os.path.abspath(item).replace("\\", "/")
            tmp = item.replace(appdir + "/", "")
            if tmp:
                src.append(tmp)

    src = list(set(src))
    return src


def get_values(input_line, keyword):
    """ Get the values from makefile's line which matched keyword """
    values = []
    patten = re.compile("\$\(NAME\)_%s.*=\s*(.*)" % keyword)
    input_line = input_line.strip()
    match = patten.match(input_line)
    if match:
        tmp = match.group(1)
        for item in tmp.split(" "):
            if os.path.exists(item):
                values.append(item)

    return values


def update_aosmk(sourcelist, aosmk):
    """ Update aos.mk with source list """
    appdir = os.path.dirname(aosmk)
    sources = get_source_files(sourcelist, appdir)

    if not sources:
        return

    content = []
    found_src_changes = False
    with open(aosmk, "r") as f:
        for line in f.readlines():
            if line.startswith("$(NAME)_SOURCES_CUSTOMIZED"):
                found_src_changes = True
                tmp = get_values(line, "SOURCES_CUSTOMIZED")
                if sources:
                    tmp += sources
                    tmp = list(set(tmp))
                    line = "$(NAME)_SOURCES_CUSTOMIZED := %s\n" % " ".join(tmp)

            content.append(line)

    if sources and not found_src_changes:
        line = "$(NAME)_SOURCES_CUSTOMIZED := %s\n" % " ".join(sources)
        content.append(line)
        content.append("$(NAME)_SOURCES += $($(NAME)_SOURCES_CUSTOMIZED)\n")

    with open(aosmk, "w") as f:
        for line in content:
            f.write(line)


@click.command()
@click.argument("sourcelist", nargs=-1, metavar="<SOURCELIST>")
@click.option("-m", "--makefile", help="Target makefile to update")
def cli(sourcelist, makefile):
    """ Add component sources to aos.mk """
    if not sourcelist:
        return

    aosmk = os.path.join(os.getcwd(), "aos.mk")
    if makefile:
        aosmk = makefile

    if os.path.isfile(aosmk):
        aosmk = os.path.abspath(aosmk).replace("\\", "/")
    else:
        click.echo("[Error] No such file: %s" % aosmk)
        return 1

    for item in sourcelist:
        if not os.path.exists(item):
            click.echo("[Error] No such file: %s" % item)
            return 1

    update_aosmk(sourcelist, aosmk)


if __name__ == "__main__":
    cli()
