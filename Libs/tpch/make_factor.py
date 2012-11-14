#!/usr/bin/env python2.7

import subprocess
import argparse
import os

parser = argparse.ArgumentParser(description='Creates a set of Factor types using a config file.')

parser.add_argument('file',
    action='store',
    type=argparse.FileType('r'),
    help='the configuration file to use.')

args = parser.parse_args()

for line in args.file:
    line = line.rstrip('\n')
    parts = line.split('|')
    storage = parts[0]
    name = parts[1]

    defineStorage = '--define=STORAGE_TYPE={0}'.format(storage)
    defineName = '--define=DICT_NAME={0}'.format(name)

    inFileName = 'm4/Factor.h.m4'
    outFileName = 'Types/{0}.h'.format(name)
    outFile = open(outFileName, 'w')

    try:
        subprocess.check_call(['m4', '-P', defineStorage, defineName, inFileName],
            stdout=outFile)
    except:
        print 'Failed to generate Factor {0} with storage type {1}'.format(name, storage) 

myDir = os.path.realpath('.')
myLib = os.path.basename(myDir)
libDir = os.path.realpath('..')
makeDesc = os.path.join(libDir, 'make_desc.py')

try:
    subprocess.check_call([makeDesc, myLib], cwd=libDir)
except:
    print 'Failed to create library description files.'
