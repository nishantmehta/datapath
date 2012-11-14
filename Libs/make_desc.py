#!/usr/bin/env python2.7

#  Copyright 2012 Christopher Dudley
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

'''
Script to automatically create a library description file from a set of
source files.
The source files are expected to be contained in the following folder
hierarchy:
                       Libs
                        |
                     library
                        |
    ----------------------------------------------------------------
    |           |             |           |        |       |       |
  Types      Functions       GLAs        UDFs     GFs     GTs    GISTs

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
m4Includes = '../src/M4/m4'

directories = ['Types', 'Functions', 'GLAs', 'UDFs', 'GFs', 'GTs', 'GISTs']
descFileName = args.library + '.pgy'

descFile = open(descFileName, 'w')

curDate = time.strftime('%A, %B %d, %Y at %H:%M %Z')

descFile.write('#ifndef _{0}_PGY_\n#define _{0}_PGY_\n'.format(args.library))
descFile.write('// Piggy description file for library {0}\n'.format(args.library))
descFile.write('// generated on {0}\n'.format(curDate))

# Make an overall C++ header for easy inclusion of an entire library
libHeaderFileName = args.library + '.h'
libHeaderFile = open(libHeaderFileName, 'w')

libHeaderFile.write('#ifndef _{0}_H_\n#define _{0}_H_\n'.format(args.library))
libHeaderFile.write('// C/C++ header file for library {0}\n'.format(args.library))
libHeaderFile.write('// generated on {0}\n'.format(curDate))

for directory in directories:
    curDir = os.path.join(args.library, directory)
    
    # Make a new header file for this directory
    dirHeaderFileName = curDir + '.h'
    dirHeaderFile = open( dirHeaderFileName, 'w' )

    dirHeaderFile.write('#ifndef _{0}_{1}_H_\n#define _{0}_{1}_H_\n'.format(args.library, directory))
    dirHeaderFile.write('// C/C++ header file for directory {1} of library {0}\n'.format(args.library, directory))
    dirHeaderFile.write('// generated on {0}\n'.format(curDate))

    libHeaderFile.write('#include "{0}"\n'.format(dirHeaderFileName))

    if os.path.exists(curDir) and os.path.isdir(curDir):
        for f in os.listdir(curDir):
            curFile = os.path.join(curDir, f)
            if not curFile.endswith('.h') and not curFile.endswith('.m4') :
                 continue

            if curFile.endswith('.h'):
                dirHeaderFile.write('#include "{0}"\n'.format(curFile))

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

    dirHeaderFile.write('#endif//_{0}_{1}_H_\n'.format(args.library, directory))
    dirHeaderFile.close()

descFile.write('#endif // _{0}_PGY_\n'.format(args.library))
descFile.close()

libHeaderFile.write('#endif//_{0}_H_\n'.format(args.library))
libHeaderFile.close()
