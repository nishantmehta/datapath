#!/usr/bin/env python3
#
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
#
# add_license.py

"""
Add the Apache 2.0 license to files.

This script can be used to quickly and easily add the Apache 2.0 license
to multiple files or entire folders at once.

It takes as arguments a year, a name, and a list of files or folders to
which to add the license. For each folder listed, the folder will be
recursively searched for files. For each file found, a temporary file
will be created, the customized Apache 2.0 license will be added to the
top, the contents of the original file appended to the temporary file,
and then the original file will be replaced with the new file.
"""

from os.path import realpath
import os
import fnmatch
import argparse
import tempfile
import shutil

# Set up the argument parser
parser = argparse.ArgumentParser(
        description='Add the Apache 2.0 license to files customized with the name of the author and year of copyright.',
        epilog='Note: folders will be searched recursively for files.')

parser.add_argument('year',
        help='the year that should be displayed for the copyright notice')

parser.add_argument('author',
        help='the author or authors that should be displayed in the copyright notice.')

parser.add_argument('file',
        nargs='+',
        help='a file or folder to which to add the license.')

parser.add_argument('-b', '--backup',
        action='store_true',
        default=False,
        help='make a backup of each file before adding the license.')

parser.add_argument('-v', '--version',
        action='version',
        version='%(prog)s 1.0')

# Actually parse the arguments
args = parser.parse_args()

# Set up the visited set so we can ensure we never visit a file twice
visitedPaths = set()

# Set up the known file type patterns to their comment string.
# We will only add the license to file types we know the comment string for.
commentString = {
        '*.c'   : '//',
        '*.cc'  : '//',
        '*.cpp' : '//',
        '*.h'   : '//',
        '*.m4'  : 'dnl #',
        '*.hpp' : '//',
        '*.sh'  : '#',
        '*.py'  : '#',
        '*.awk' : '#'
        }

# Set up the license text
licenseLines = [
        '',
        '  Copyright ' + args.year + ' ' + args.author,
        '',
        '  Licensed under the Apache License, Version 2.0 (the "License");',
        '  you may not use this file except in compliance with the License.',
        '  You may obtain a copy of the License at',
        '',
        '      http://www.apache.org/licenses/LICENSE-2.0',
        '',
        '  Unless required by applicable law or agreed to in writing, software',
        '  distributed under the License is distributed on an "AS IS" BASIS,',
        '  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.',
        '  See the License for the specific language governing permissions and',
        '  limitations under the License.',
        '']

### BEGIN FUNCTION DEFINITIONS ###

def ValidFile( filePath ):
    """Determines if a file or folder should be checked."""
    fileName = os.path.basename( filePath )

    for pattern in commentString.keys():
        if fnmatch.fnmatch( fileName, pattern ) :
            return True

    return False

def GetCommentString( filePath ):
    """Returns the comment string for the given file"""
    fileName = os.path.basename( filePath )

    for pattern in commentString.keys():
        if fnmatch.fnmatch( fileName, pattern ):
            return commentString[pattern]

    return ''

def HandleFile( filePath ):
    """Adds the Apache 2.0 license to a file."""
    if filePath in visitedPaths:
        return

    visitedPaths.add( filePath )

    if not ValidFile( filePath ):
        return

    cmtStr = GetCommentString( filePath )

    tmp = tempfile.TemporaryFile()

    origFile = open( filePath, 'rb' )

    # See if the file starts with a hashbang (#!)
    # If so, copy that line first so that the file is still properly
    # executable
    firstLine = origFile.readline()

    if firstLine[0:2] == b'#!':
        tmp.write( firstLine )
    else:
        origFile.seek(0)

    # Write the license to the top
    for line in licenseLines:
        toWrite = cmtStr + line + '\n'
        tmp.write( toWrite.encode() )


    # Write the contents of the original file to the temporary file
    for line in origFile:
        tmp.write( line )

    origFile.close()

    if args.backup:
        shutil.copy(filePath, filePath + '.bak')

    newFile = open( filePath, mode='wb' )

    # Return the current position in tmp to the beginning
    tmp.seek(0, 0)

    # Copy the contents of the temporary file to the new one
    for line in tmp:
        newFile.write( line )

    tmp.close()
    newFile.close()

    print( filePath )

    return

def RecurseDirs( dirPath ):
    """Recursively visits all files and subdirectories in a directory."""
    if dirPath in visitedPaths:
        return

    visitedPaths.add( dirPath )

    if os.path.islink( dirPath ):
        return

    for fileName in os.listdir( dirPath ):
        # Don't visit hidden files or folders
        if( fileName[0] == '.' ) :
            continue;

        absFileName = realpath( os.path.join( dirPath, fileName  ) )

        if os.path.isdir( absFileName ):
            RecurseDirs( absFileName )
        else:
            HandleFile( absFileName )

    return

### END FUNCTION DEFINITIONS ###

# For all files or folders given, visit them
for fileName in args.file:
    absFileName = realpath( fileName )

    if os.path.isdir( absFileName ):
        RecurseDirs( absFileName )
    else:
        HandleFile( absFileName )
