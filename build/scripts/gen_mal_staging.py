import os
import sys
import click
import re

reload(sys)
sys.setdefaultencoding('UTF8')

scriptdir = os.path.dirname(os.path.abspath(__file__))
templates = {
    "nbiot": ["templates/mal_nbiot_template", "components/peripherals/module/mal/nbiot"],
    "gprs": ["templates/mal_gprs_template", "components/peripherals/module/mal/gprs"],
    "wifi": ["templates/mal_wifi_template", "components/peripherals/module/mal/wifi"],
    "lte": ["templates/mal_lte_template", "components/peripherals/module/mal/lte"],
    "eth": ["templates/mal_eth_template", "components/peripherals/module/mal/eth"],
    "other": ["templates/mal_other_template", "components/peripherals/module/mal/other"],
}


def copy_template(tempfile, templatedir, destdir, drivername):
    """ Copy template files to destdir """

    contents = []
    # Replace drivername from file contents

    with open(os.path.join(templatedir, tempfile), "r") as f:
        for line in f.readlines():
            if "@drivername@" in line:
                line = line.replace("@drivername@", drivername)
            elif "@DRIVERNAME@" in line:
                line = line.replace("@DRIVERNAME@", drivername.upper())
            else:
                pass

            contents += [line]

    # Replace drivername from filename
    if tempfile == "aos.mk.temp":
        destfile = "aos.mk"
    elif "drivername" in tempfile:
        destfile = tempfile.replace("drivername", drivername)
    else:
        destfile = tempfile

    # Write to destfile
    if contents:
        with open(os.path.join(destdir, destfile), "w") as f:
            for line in contents:
                f.write(line)

def update_config(destdir, devicetype, drivername):
    configfile = os.path.join(destdir, "..", "..", "Config.in")
    if os.path.isfile(configfile):
        newf = []
        exist = False
        newline = "\n    config AOS_MAL_%s_%s\n" % (devicetype.upper(), drivername.upper())
        newline2 = "        bool %s.%s\n" % (devicetype, drivername)

        with open(configfile, 'r') as f:
            for l in f.readlines():
                newf.extend([l])
                if re.findall(r'.*bool\s+\"Null\".*', l):
                    newf.extend([newline])
                    newf.extend([newline2])
                if newline.strip() in l or newline2.strip() in l:
                    exist = True

        if exist:
            newf.remove(newline)
            newf.remove(newline2)

        if newf:
            with open(configfile, 'w') as f:
                for l in newf:
                    f.write(l)

def update_mk(destdir, devicetype, drivername):
    mkfile = os.path.join(destdir, "..", "..", "aos.mk")
    if os.path.isfile(mkfile):
        newline = "$(NAME)_COMPONENTS-$(AOS_MAL_%s_%s) := device_mal_%s" % \
                  (devicetype.upper(), drivername.upper(), drivername)

        with open(mkfile, 'r') as f:
            l = f.readline()
            while l:
                if newline.strip() in l:
                    exist = True
                    return
                l = f.readline()

        with open(mkfile, 'a') as f:
            f.write("\n" + newline)

@click.command()
@click.argument("drivername", metavar="[DRIVERNAME]")
@click.option("-m", "--mfname", help="The manufacturer of device")
@click.option("-t", "--devicetype", required=True, type=click.Choice(["nbiot", "gprs", "wifi", "lte", "eth", "other"]), help="The type of device")
@click.option("-a", "--author", help="The author of driver")
def cli(drivername, mfname, devicetype, author):
    """ Create mal driver staging from template """
    templatedir = os.path.join(scriptdir, templates[devicetype][0])
    destdir = os.path.join(scriptdir, "../../", templates[devicetype][1], drivername)
    destdir = os.path.abspath(destdir)

    if os.path.isdir(destdir):
        click.echo("[Error] The driver directory is existing!\n%s" % destdir)
        return 1
    else:
        os.makedirs(destdir)

    if not os.path.isdir(templatedir):
        click.echo("[Error] The driver templete is not available yet!\n")
        return 1

    sources = os.listdir(templatedir)
    for tempfile in sources:
        copy_template(tempfile, templatedir, destdir, drivername)

    # update Config.in
    update_config(destdir, devicetype, drivername)

    # update aos.mk
    update_mk(destdir, devicetype, drivername)

    click.echo("[Info] Drivers Initialized at: %s" % destdir)


if __name__ == "__main__":
    cli()
