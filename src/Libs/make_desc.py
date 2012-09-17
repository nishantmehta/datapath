#!/usr/bin/env python2

'''
Script to automatically create a library description file from a set of
source files.
The source files are expected to be contained in the following folder
hierarchy:
                       Libs
                        |
                     library
                        |
    ------------------------------------------------
    |           |             |           |        |
  Types      Functions       GLAs        UDFs     GFs

This script will create a file called library.pgy in the Libs folder, where
library is replaced with the name of the library being built.
'''

import os
import subprocess
import argparse
import time

# Set up the argument parser
parser = argparse.ArgumentParser(
        description='Create a description file for a library.')
parser.add_argument('library',
        type=str,
        help='the name of the library.')

# Parse the arguments
args = parser.parse_args()

# Some useful definitions
M4 = 'm4'
m4Includes = '../M4/m4'

directories = ['Types', 'Functions', 'GLAs', 'UDFs']
descFileName = args.library + '.pgy'

descFile = open(descFileName, 'w')

curDate = time.strftime('%A, %B %d, %Y at %H:%M %Z')

descFile.write('#ifndef _{0}_PGY_\n#define _{0}_PGY_\n'.format(args.library))
descFile.write('// Piggy description file for library {0}\n'.format(args.library))
descFile.write('// generated on {0}\n'.format(curDate))

for directory in directories:
    curDir = os.path.join(args.library, directory)
    if os.path.exists(curDir) and os.path.isdir(curDir):
        for f in os.listdir(curDir):
            curFile = os.path.join(curDir, f)
            descFile.write('\n// {0}\n'.format(curFile))
            sourceDefine = '--define=SOURCE_FILE={0}'.format(curFile)
            try:
                output = subprocess.check_output([M4, '-I', m4Includes, sourceDefine, 'descfile.m4'])
                descFile.write(output)
            except subprocess.CalledProcessError as e:
                print "Error running M4 on file", curFile
                print e
    elif os.path.exists(curDir) and not os.path.isdir(curDir):
        print 'Error: {0} is not a directory.'.format(curDir)

descFile.write('#endif // _{0}_PGY_\n'.format(args.library))
