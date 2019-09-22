import re

def get_project_config(config_file, keyword):
    """ Get keyword value from .aos """
    value = ""
    with open(config_file, "r") as f:
        for line in f.readlines():
            line = line.strip()
            if line.startswith("%s=" % keyword):
                value = line.replace("%s=" % keyword, "")

    return value


