import os
import sys
import click
import re

reload(sys)
sys.setdefaultencoding('UTF8')

scriptdir = os.path.dirname(os.path.abspath(__file__))
templates = {
    "nbiot": ["templates/sal_nbiot_template", "components/peripherals/sal/nbiot"],
    "gprs": ["templates/sal_gprs_template", "components/peripherals/sal/gprs"],
    "wifi": ["templates/sal_wifi_template", "components/peripherals/sal/wifi"],
    "lte": ["templates/sal_lte_template", "components/peripherals/sal/lte"],
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
        if tempfile == "Config.in" and os.path.isfile('%s/../Config.in' % destdir):
            with open('%s/../Config.in'  % destdir, "a") as f:
                destdir_relative = re.findall(r'.*(components\/peripherals\/sal.*)', destdir)
                f.write("\"source %s/Config.in\n\"" % destdir_relative[0])

def update_configin(configin_file, devicetype, drivername):
    """ Update Config.in in paraent directory """
    contents = []
    source_found = "no"
    default_found = "no"
    with open(configin_file, "r") as f:
        for line in f.readlines():
            if "default" in line:
                default_found = "yes"

            if default_found == "yes" and "default" not in line:
                contents += ['    default AOS_COMP_DEVICE_SAL_%s if BSP_EXTERNAL_%s_MODULE = "%s.%s"\n' %
                             (drivername.upper(), devicetype.upper(), devicetype, drivername)]
                default_found = "no"

            if "source" in line:
                source_found = "yes"

            if source_found == "yes" and "source" not in line:
                contents += ['source "drivers/sal/%s/%s/Config.in"\n' % (devicetype, drivername)]
                source_found = "no"

            contents += [line]

    with open(configin_file, "w") as f:
        for line in contents:
            f.write(line)


@click.command()
@click.argument("drivername", metavar="[DRIVERNAME]")
@click.option("-m", "--mfname", help="The manufacturer of device")
@click.option("-t", "--devicetype", required=True, type=click.Choice(["nbiot", "gprs", "wifi", "lte"]), help="The type of device")
@click.option("-a", "--author", help="The author of driver")
def cli(drivername, mfname, devicetype, author):
    """ Create sal driver staging from template """
    templatedir = os.path.join(scriptdir, templates[devicetype][0])
    destdir = os.path.join(scriptdir, "../../", templates[devicetype][1], drivername)
    destdir = os.path.abspath(destdir)

    if os.path.isdir(destdir):
        click.echo("[Error] The driver directory is existing!\n%s" % destdir)
        return 1
    else:
        os.makedirs(destdir)

    sources = os.listdir(templatedir)
    for tempfile in sources:
        copy_template(tempfile, templatedir, destdir, drivername)

    configin_file = os.path.join(scriptdir, "../../drivers/sal/%s/Config.in" % devicetype)
    if os.path.isfile(configin_file):
        update_configin(configin_file, devicetype, drivername)

    click.echo("[Info] Drivers Initialized at: %s" % destdir)


if __name__ == "__main__":
    cli()
