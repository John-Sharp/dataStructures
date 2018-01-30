#! /usr/bin/env python3
import argparse
import os
import os.path

script_directory = os.path.dirname(os.path.realpath(__file__))

parser = argparse.ArgumentParser(description='Generate linked list C code for a particular type.')

parser.add_argument('header_dir', metavar='header-dir', type=str,
                    help='dirctory the header files will be saved in')
parser.add_argument('implementation_dir', metavar='impl-dir', type=str,
                    help='dirctory the implementation files will be saved in')
parser.add_argument('prefixType', metavar='prefix:type', type=str, nargs='+',
                    help='prefix for the linked list function names, then a colon, then the C type the linked list will store')
args = parser.parse_args()

print(args.prefixType)

template_locations = [script_directory + t for t in ['/templates/listTemplate.h', '/templates/listTemplate.inc']]


for prefix, typeName in [a.split(':') for a in args.prefixType]:
    template_target = zip(template_locations, [args.header_dir, args.implementation_dir])
    for template, target_dir in template_target:
        s = open(template, 'r').read()
        s = s.replace('<pf>', prefix)
        s = s.replace('<tp>', typeName)

        with open(os.path.join(target_dir, prefix) + "List." + template.split(".")[1], 'w') as out_file:
            out_file.write(s)
