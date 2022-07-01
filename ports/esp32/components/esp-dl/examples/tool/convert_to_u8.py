import cv2
import numpy
import argparse

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Model generator tool')
    parser.add_argument('-i', '--input', help='path to image')
    parser.add_argument('-o', '--output', help='path to image.hpp')
    args = parser.parse_args()

    if args.input is None or args.output is None:
        parser.print_help()
        quit()

    image = cv2.imread(args.input)
    h,w,c=image.shape

    with open(args.output, 'w') as file:
        file.write('#pragma once\n'
                   '#include <stdint.h>\n\n'
                   f'#define IMAGE_HEIGHT {h}\n'
                   f'#define IMAGE_WIDTH {w}\n'
                   f'#define IMAGE_CHANNEL {c}\n\n'
                   'const static uint8_t IMAGE_ELEMENT[] = {\n')

        image = numpy.reshape(image, (-1,))
        for i, element in enumerate(image[:-1], 1):
            if i == 1:
                file.write('    ')

            file.write(f'{element}, ')

            if i % 32 == 0:
                file.write('\n    ')
        file.write(f'{image[-1]}')
        file.write('};\n')
