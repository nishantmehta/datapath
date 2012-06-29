#!/usr/bin/env python2
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

import os
from os.path import realpath
import argparse
import shutil
import sys
import subprocess
import glob
import tempfile

"""A script to load the TPC-H dataset into the database."""

# Set up the argument parser
parser = argparse.ArgumentParser(
        description='Load the TPC-H dataset into the database.')

parser.add_argument('-d', '--data',
        action='store',
        default='./data',
        metavar='path',
        help='the directory in which to store the table data. [./data]')

parser.add_argument('-g', '--gen-version',
        action='store',
        default='2.14.3',
        metavar='x.x.x',
        help='the version of the TPC-H generator to use. [2.14.3]')

parser.add_argument('-f', '--scale-factor',
        action='store',
        default=1,
        type=int,
        metavar='scale',
        help='the scale factor to use for the TPC-H data. [1]')

parser.add_argument('-s', '--source-dir',
        action='store',
        default='../../',
        metavar='path',
        help='the path to the datapath source directory. [../../]')

parser.add_argument('-k', '--keep-files',
        action='store_true',
        default=False,
        help='do not delete temporary files.')

parser.add_argument('-r', '--reload',
        action='store_true',
        default=False,
        help='redownload the tpch data generator and regenerate the data, even if it already exists.')

parser.add_argument('-q', '--query-dir',
        action='store',
        default='./LOAD_TPCH',
        metavar='path',
        help='the directory containing the query files used to bulkload the data. [./LOAD_TPCH]')

parser.add_argument('-n', '--num-stripes',
        action='store',
        default=1,
        type=int,
        metavar='stripes',
        help='the number of stripes in which to generate the TPC-H data. [1]')

parser.add_argument('-i', '--initialize',
        action='store_true',
        default=False,
        help='initialize DataPath (i.e. answer the questions it asked upon first run).')

parser.add_argument('-p', '--disk-pattern',
        action='store',
        default='./disks/disks%d',
        help='the pattern to use for the disks when initializing DataPath. [./disks/disk%d]')

parser.add_argument('-e', '--exponent',
        action='store',
        default=0,
        type=int,
        help='the page multiplier exponent to use when initializing DataPath. [0]')

parser.add_argument('-m', '--num-disks',
        action='store',
        type=int,
        default=1,
        help='the number of disks to specify when initializing DataPath. [1]')

parser.add_argument('--ignore-dp',
        action='store_true',
        default=False,
        help='ignore errors caused by non-zero return codes from the DataPath executable.')

# Parse the arguments
args = parser.parse_args()

# Set up constants we'll use
dataDir = realpath(args.data)
srcDir = realpath(args.source_dir)
dpDir = os.path.join( srcDir, 'Tool_DataPath/executable')
dpExec = os.path.join( dpDir, 'dp' )
dtmiExec = realpath( os.path.join(srcDir, 'Tool_DataTypeManagerInitializer/executable/dtm_init'))

tpchVersion = args.gen_version.replace('.', '_')
tpchDir = os.path.join( dataDir, 'tpch_' + tpchVersion)
tpchArchiveName = 'tpch_' + tpchVersion + '.tgz'
tpchFile = os.path.join( dataDir, tpchArchiveName )
tpchURL = 'http://www.tpc.org/tpch/spec/'
tpchArchiveWeb = tpchURL + tpchArchiveName

dbgenDir = os.path.join( tpchDir, 'dbgen' )

scaleFactor = str(args.scale_factor)
numStripes = str(args.num_stripes)

queryDir = realpath( args.query_dir )

sedCommand = "s/CC[[:space:]]*=.*$/CC = gcc/ ; " \
        "s/DATABASE[[:space:]]*=.*$/DATABASE = SQLSERVER/ ; " \
        "s/MACHINE[[:space:]]*=.*$/MACHINE = LINUX/ ; " \
        "s/WORKLOAD[[:space:]]*=.*$/WORKLOAD = TPCH/"

queryFiles = { 'tpch.pgy', 'bulkload_lineitem.pgy', 'bulkload_customer.pgy', 'bulkload_region.pgy', 'bulkload_nation.pgy',
        'bulkload_orders.pgy', 'bulkload_part.pgy', 'bulkload_partsupp.pgy', 'bulkload_supplier.pgy'}

# Used when asking the user a yes or no question
yes = {'Y', 'y', 'yes', 'YES', 'Yes'}
no = {'N', 'n', 'no', 'NO', 'No'}

def cleanup(force = False):
    """Clean up directories used."""

    if not force and args.keep_files:
        return

    # Test for existence of tpch directory
    if os.path.exists( tpchDir ):
        shutil.rmtree( tpchDir )

    # Test for existence of tpch archive
    if os.path.exists( tpchFile ):
        os.remove( tpchFile )

    return

def getArchive():
    """Download the TPC-H data generation tools archive from the TPC website."""

    try:
        subprocess.check_call(['wget', tpchArchiveWeb], cwd=dataDir)
    except:
        print "Error: Failed to download TPC-H generation tools archive."
        sys.exit(2)

    return

def buildGenerator():
    """Build the TPC-H generator, first downloading and extracting the generator if needed."""

    if not os.path.exists(tpchDir):
        print 'No existing TPC-H generator directory found, extracting from archive.'

        # Need to extract the archive
        os.mkdir( tpchDir )

        # Check to see if we already have a local copy of the archive
        if not os.path.exists(tpchFile):
            print 'No existing TPC-H generator archive found, downloading from tpc.org'
            # Download the archive
            getArchive()

        try:
            subprocess.check_call(['tar', '-xzf', tpchFile, '-C', tpchDir], cwd=dataDir)
        except:
            print "Error: unable to extract TPC-H generator archive."
            shutil.rmtree( tpchDir )
            sys.exit(3)

    # We should have the generator extracted into tpchDir at this point
    # Let's set the configuration file properly
    print 'Configuring TPC-H generator makefile.'

    configFile = open( os.path.join( dbgenDir, 'makefile'), 'w')
    sourceConfigFile = os.path.join( dbgenDir, 'makefile.suite')

    try:
        subprocess.check_call(['sed', '-e', sedCommand, sourceConfigFile], stdout=configFile)
    except:
        print "Error: Failed to configure TPC-H generator makefile"
        sys.exit(4)

    # At this point, we should be ready to make the generator.
    print 'Compiling TPC-H generator.'


    try:
        subprocess.check_call('make', cwd=dbgenDir)
    except:
        print "Error: Failed to make TPC-H generator."
        sys.exit(5)

    # Build successful!
    return

def generateTables():
    """Generate the TPC-H data tables using the generator."""

    dbgenExec = os.path.join( dbgenDir, 'dbgen' )
    distsFile = os.path.join( dbgenDir, 'dists.dss')

    if not os.path.exists( dbgenExec ):
        print 'No existing dbgen exectuable found, building it now.'
        buildGenerator()

    # Remove any previous tables.
    for f in glob.glob( os.path.join( tpchDir, '*.tbl*' ) ):
        os.remove(f)

    print 'Generating Tables with scale factor {0} in {1} stripe(s).'.format(scaleFactor, numStripes)

    if args.num_stripes == 1:
        try:
            subprocess.check_call([dbgenExec, '-vf', '-s', scaleFactor, '-b', distsFile], cwd=tpchDir)
        except:
            print 'Error: Failed to generate TPC-H table data files!'
            sys.exit(6)
    else:
        for i in xrange(1, args.num_stripes + 1):
            try:
                subprocess.check_call([dbgenExec, '-vf', '-s', scaleFactor, '-b', distsFile,
                    '-C', numStripes, '-S', str(i)], cwd=tpchDir)
            except:
                print 'Error: Failed to generate TPC-H table data files!'
                sys.exit(6)

    return

def generateQueries():
    """Generate the queries necessary to load the schema and data into the database."""

    if not os.path.exists( queryDir ):
        print 'Error: specified query directory doesn\'t exist!'
        sys.exit(9)
    elif not os.path.isdir( queryDir ):
        print 'Error: specified query directory isn\'t a directory!'
        sys.exit(9)

    for f in queryFiles:
        piggyFile = os.path.join( queryDir, f)
        if os.path.exists( piggyFile ):
            shutil.copy( piggyFile, os.path.join( tpchDir, f) )

        m4File = os.path.join( queryDir, f + '.m4' )
        if os.path.exists( m4File ):
            outFile = open(os.path.join( tpchDir, f), 'w')
            try:
                subprocess.check_call( ['m4', '-DNUM_STRIPES=' + numStripes, '-DTABLE_DIR=' + tpchDir, m4File], stdout=outFile)
            except:
                print 'Error generating query file for {0} using m4!'.format(f)
                sys.exit(10)
    return

def getDPInit():
    """If necessary, create a temporary file containing initialization information for DataPath."""

    if args.initialize:
        tmpFile = tempfile.TemporaryFile();
        tmpFile.write(str(args.exponent) + '\n')
        tmpFile.write(str(args.num_disks) + '\n')
        tmpFile.write(args.disk_pattern + '\n')

        tmpFile.flush()
        tmpFile.seek(0)

        return tmpFile
    else:
        return None

def loadData():
    """Load the generated TPC-H data into the database."""

    if not os.path.exists( dataDir ):
        os.mkdir( dataDir )
    elif not os.path.isdir( dataDir ):
        print 'Error: specified data directory is not actually a directory!'
        sys.exit(7)

    # Check to make sure all of the table files are there.
    tableFiles = { 'lineitem.tbl', 'customer.tbl', 'region.tbl', 'nation.tbl', 'orders.tbl', 'part.tbl', 'partsupp.tbl', 'supplier.tbl'}
    if args.num_stripes > 1:
        tempTableFiles = set()

        for f in tableFiles:
            for i in xrange( 1, args.num_stripes + 1):
                tempTableFiles.add( f + '.{0}'.format(i) )

        tableFiles = tempTableFiles

    foundFiles = set()

    if os.path.exists( tpchDir ):
        for f in os.listdir( tpchDir ):
            foundFiles.add(f)

    if not foundFiles >= tableFiles:
        print "Didn't find all of the TPC-H table data files, generating them."
        generateTables()
    elif args.num_stripes > 1 and 'lineitem.tbl.{0}'.format(args.num_stripes + 1) in foundFiles:
        # The tables that are already generated are in too many pieces, and
        # thus we must regenerate them.
        print "Previously generated table files in too many stripes, regenerating."
        generateTables()

    # Make sure we have all of the query files we need.
    generateQueries()

    foundFiles = set()

    for f in os.listdir( tpchDir ):
        foundFiles.add(f)

    if not foundFiles >= queryFiles:
        print "Error: unable to locate all necessary query files in query directory."
        sys.exit(7)

    print "Loading types into database."

    try:
        subprocess.check_call(dtmiExec, cwd=dpDir)
    except Exception as e:
        print 'Error: unable to load types!'
        print dtmiExec
        print e
        sys.exit(11)

    # TODO: Fix DataPath so that it will exit cleanly with the -q switch.
    # It currently does not save meta-data when bulkloading relations if
    # you have the -q switch.
    print "NOTICE: DataPath currently does not exit cleanly when running a single script."
    print "In order to work around this, DataPath will simply sit idle and do nothing"
    print "once it has finished executing its query. At this point, you will need to"
    print "close DataPath by sending an interrupt (CTRL+C). If everything has gone"
    print "well, it will have already saved all the information that it needs to."
    print

    s = raw_input('Continue? (Y/N) --> ')

    while s not in yes and s not in no:
        print "Please answer yes or no."
        s = raw_input('Continue? (Y/N) --> ')

    if s in no:
        print "Exiting."
        sys.exit(8)

    print "Loading schema into database."
    inFile = getDPInit()

    try:
        subprocess.check_call([dpExec, '-q', '-e', os.path.join(tpchDir, 'tpch.pgy')], stdin=inFile, cwd=dpDir)
    except subprocess.CalledProcessError:
        if not args.ignore_dp:
            print 'Warning: an error occurred while loading the schema. It\'s possible that'
            print 'DataPath exited with a non-zero exit code because it does not yet quit cleanly.'
            print 'If this is the case, it is safe to continue.'
            print

            s = raw_input('Continue? (Y/N) --> ')

            while s not in yes and s not in no:
                print "Please answer yes or no."
                s = raw_input('Continue? (Y/N) --> ')

            if s in no:
                print "Exiting."
                sys.exit(8)

    if inFile != None:
        inFile.close()

    print "Loading data into database."

    queryFiles.remove('tpch.pgy')

    for query in queryFiles:
        try:
            subprocess.check_call([dpExec, '-q', '-e', os.path.join(tpchDir, query)], cwd=dpDir)
        except (subprocess.CalledProcessError, KeyboardInterrupt):
            if not args.ignore_dp:
                print 'Warning: an error occurred while loading the data. It\'s possible that'
                print 'DataPath exited with a non-zero exit code because it does not yet quit cleanly.'
                print 'If this is the case, it is safe to continue.'
                print

                s = raw_input('Continue? (Y/N) --> ')

                while s not in yes and s not in no:
                    print "Please answer yes or no."
                    s = raw_input('Continue? (Y/N) --> ')

                if s in no:
                    print "Exiting."
                    sys.exit(9)

if args.reload:
    cleanup(force = True)

loadData()

cleanup()
