import sys, os, json

if len(sys.argv) == 3:
    file_dir = os.path.join(os.getcwd(), sys.argv[1])
    if not os.path.exists(file_dir):
        os.makedirs(file_dir)

    dependency = eval(sys.argv[2])
    for component in  dependency:
        dependency[component] = dependency[component].split()
        
    with open(os.path.join(file_dir, "dependency.json"), "w") as f:
       f.write(json.dumps(dependency, sort_keys=True, indent=4) +"\n")