import sys, os, json, glob, re
from itertools import chain

config_mk_str = ""

def process_config_mk(config_mk_file, filename):
    global config_mk_str
        
    config_mk_replace = False
    config_mk_lines = config_mk_str.splitlines()

    for i, line in enumerate(config_mk_lines):
        if line.find("auto_component_SOURCES") != -1: #auto component line
            if line.find(filename) == -1: #add source file to config.mk
                replace_str = line + " " + filename
                config_mk_lines[i] = replace_str                
                config_mk_replace = True
    
    if config_mk_replace:
        #auto component change config.mk.   
        buf = ""
        for line in config_mk_lines:
            buf += line + "\n"
        config_mk_str = buf

        with open(config_mk_file, "w") as config_mk:
            config_mk.write(config_mk_str)

def process_component_test(source_directory):    
    global config_mk_str

    test_register_src = "testcase_register.c"

    code_list = []

    source_codes = "/*\n * warning: testcase collection code is auto generate, please don't change!!!\n */\n\n"
    source_codes += "#include \"aos/kernel.h\"\n\n"

    components = re.findall(r"COMPONENTS\s+\:\=\s+.+\n", config_mk_str)[0]
    for name in components.split(" "):
        if name.endswith("_test"):
            location = name + "_LOCATION\s+\:\=\s+.+"
            # find all source code files related to test components
            for root, dirs, files in chain.from_iterable(os.walk(path.strip()) for path in \
                    re.findall(location, config_mk_str)[0].split(":= ")[1:]):
                for source in [source for source in files if source.endswith(".c")]:
                    with open(os.path.join(root, source), "r") as head:
                        codes = head.read()
                        # find AOS_TESTCASE macro function
                        for code in re.findall("AOS_TESTCASE\s*\((.+\)\s*;)", codes):
                            code_list.append(code[:len(code)-2])

    
    for code in code_list:
        source_codes +=  "extern void %s(void);\n"%(code)

    source_codes += "\nvoid add_test(void) {\n\n"

    # temporary work around for the process sequence of mesh & netmgr testcase
    mesh_test_code = ""
    netmgr_test_code = ""

    for code in code_list:
        if code.find("mesh") == -1 and code.find("netmgr") == -1 :
            source_codes += "    %s();\n\n"%(code)
        else:
            if code.find("mesh") == -1:
                netmgr_test_code = code
            else:
                mesh_test_code = code

    if mesh_test_code:
        source_codes += "    %s();\n\n"%(mesh_test_code)

    if netmgr_test_code:
        source_codes += "    %s();\n\n"%(netmgr_test_code)


    source_codes += "}"

    with open(os.path.join(source_directory, test_register_src), "w") as f:
        f.write(source_codes)

    process_config_mk(sys.argv[1], test_register_src)

def process_component_init(init_dir):
    global config_mk_str

    component_init_src = "component_init.c"

    init_codes = "/*\n * warning: component init code, don't change!!!\n */\n\n#include \"aos/kernel.h\"\n\nvoid aos_components_gen_init(void) {\n"

    # find all head file through config.mk 
    for root, dirs, files in  chain.from_iterable(os.walk(path.strip()) for path in \
            re.findall("AOS_SDK_INCLUDES\s+\+\=\s+(.+\n)", config_mk_str)[0].split("-I")[1:]):
        for include in [include for include in files if include.endswith(".h")]:
            with open(os.path.join(root, include), "r") as head:
                code = head.read()
                # find AOS_COMPONENT_INIT macro function
                for init in re.findall("AOS_COMPONENT_INIT\s*\((.+\)\s*;)", code):
                    init_code = init.replace(',', '(', 1)

                    if init == init_code:#init function no argument
                        init_code = init[:len(init)-2] +'(' + init[len(init)-2:]

                    init_codes += '    ' + init_code + '\n'  
    
    init_codes += "}"
    # write component init code.
    with open(os.path.join(init_dir, component_init_src), "w") as f:
        f.write(init_codes)

    process_config_mk(sys.argv[1], component_init_src)

def main():
    global config_mk_str

    with open(sys.argv[1], "r") as f:
        config_mk_str = f.read()

        #process_component_init(sys.argv[2])
        process_component_test(sys.argv[2])

if __name__ == "__main__":
    main()



