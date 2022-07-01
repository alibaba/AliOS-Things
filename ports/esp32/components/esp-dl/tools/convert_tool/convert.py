import os
import sys
import argparse
import platform

system_type = platform.system()
python_version = platform.python_version()
m, s, _ = python_version.split('.')
path = f'{os.path.dirname(os.path.abspath(__file__))}/{system_type.lower()}/{m}{s}'

if os.path.exists(path):
    sys.path.append(path)
    from utils import Convert

    parser = argparse.ArgumentParser(description='Model generator tool')
    parser.add_argument('-t', '--target_chip', help='esp32, esp32s2, esp32s3, esp32c3')
    parser.add_argument('-i', '--input_root', help="npy files root")
    parser.add_argument('-j', '--json_file_name', help="json file", default='config.json')
    parser.add_argument('-n', '--name', help="name of generated files")
    parser.add_argument('-o', '--output_root', help="generated source files root")
    parser.add_argument('-q', '--quant', help="quantization granularity: 0(default) for per-tensor, 1 for per-channel",
                        type=int, default=0)
    args = parser.parse_args()

    if args.input_root is None or args.name is None or args.output_root is None:
        parser.print_help()
        quit()

    print(f'\nGenerating {args.output_root}/{args.name} on {args.target_chip}...', end='')
    convert = Convert(target_chip=args.target_chip,
                      input_root=args.input_root,
                      json_file_name=args.json_file_name,
                      output_root=args.output_root,
                      name=args.name,
                      quant=args.quant)
    convert()
    print(' Finish\n')

else:
    print(f'Not supported python == {python_version} on {system_type}')
