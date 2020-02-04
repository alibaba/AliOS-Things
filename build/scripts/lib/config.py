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

def merge_config(config_file, new_config):
    """ merge config_file(.config) with new_config(app.config or board.config)
    1. read new config file line by line, ignore line without pattern xxx=yy
    2. if the config in this line is existed in .config, replace the old config with it
    3. if the config in this line is not existed, just append it """
    print("[INFO]: merge %s with %s" % (config_file, new_config))

    with open (config_file, "r") as f:
        text_config = f.read()

    patten = re.compile(r'([A-Z0-9_]*)=(.*)')
    with open (new_config, "r") as f:
        for line in f.readlines():
            match = patten.match(line)
            if match:
                p = "(# )?"+match.group(1)+"( is not set|=(.*))\n"
                text_config, cnt = re.subn(p, line, text_config, 1)
                if cnt == 0:
                    text_config += line

    with open(config_file, 'w') as result_file:
        result_file.write(text_config)

