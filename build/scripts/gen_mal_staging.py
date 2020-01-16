import os
import sys
import click
import re

reload(sys)
sys.setdefaultencoding('UTF8')

scriptdir = os.path.dirname(os.path.abspath(__file__))
templates = {
    "nbiot": ["templates/mal_nbiot_template", "components/peripherals/mal/nbiot"],
    "gprs": ["templates/mal_gprs_template", "components/peripherals/mal/gprs"],
    "wifi": ["templates/mal_wifi_template", "components/peripherals/mal/wifi"],
    "lte": ["templates/mal_lte_template", "components/peripherals/mal/lte"],
    "eth": ["templates/mal_eth_template", "components/peripherals/mal/eth"],
    "other": ["templates/mal_other_template", "components/peripherals/mal/other"],
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

        # add Config.in source
        if tempfile == "Config.in":
            father_config = '%s/../Config.in'  % destdir
            destdir_relative = re.findall(r'.*(components\/peripherals\/mal.*)', destdir)
            fatherdir_relative = re.findall(r'.*(components\/peripherals\/mal/[a-zA-Z0-9_\-]+)/.*', destdir)

            newf = []
            newline = []
            exist = False
            if not os.path.exists(father_config):
                grandfather_config = '%s/../../Config.in'  % destdir
                with open(grandfather_config, 'r') as f:
                    for l in f.readlines():
                        newf.extend([l])
                        if re.findall(r'.*bool \"Null\".*', l):
                            newline = "    source \"%s/Config.in\"\n" % fatherdir_relative[0]
                            newf.extend([newline])
                        if fatherdir_relative[0] in l:
                            exist = True

            if exist:
                newf.remove(newline)

            if newf:
                with open(grandfather_config, 'w') as f:
                    for l in newf:
                        f.write(l)

            with open(father_config, "a") as f:
                f.write("source \"%s/Config.in\"\n" % destdir_relative[0])

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
        click.echo("[Error] The driver templete is not evailable yet!\n")
        return 1

    sources = os.listdir(templatedir)
    for tempfile in sources:
        copy_template(tempfile, templatedir, destdir, drivername)

    click.echo("[Info] Drivers Initialized at: %s" % destdir)


if __name__ == "__main__":
    cli()
