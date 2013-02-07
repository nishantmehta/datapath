#!/bin/bash
#
#  Copyright 2013 Christopher Dudley
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

# A simple script to generate a profile.d script for this DataPath installation
# and install it in /etc/profile.d

SCRIPT_FILE=/etc/profile.d/datapath.sh

# Remove any temporary files
[ -e temp.sh ] && rm temp.sh

echo '#!/bin/sh' >> temp.sh
echo "export DATAPATH_INSTALL_PATH=$(readlink -f ..)" >> temp.sh
echo 'export DATAPATH_LOCK_FILE=$DATAPATH_INSTALL_PATH/lock' >> temp.sh
echo 'export DATAPATH_EXEC_PATH=$DATAPATH_INSTALL_PATH/src/Tool_DataPath/executable' >> temp.sh
echo 'export DATAPATH_HEADER_PATH=$DATAPATH_INSTALL_PATH/src/Headersdp' >> temp.sh
echo 'export DATAPATH_LIBRARY_PATH=$DATAPATH_INSTALL_PATH/Libs' >> temp.sh
echo 'export DATAPATH_PKGCONFIG_PATH=$DATAPATH_INSTALL_PATH/pkgconfig' >> temp.sh
echo 'export DATAPATH_M4_PATH=$DATAPATH_INSTALL_PATH/src/M4/m4:$DATAPATH_INSTALL_PATH/src/WPFunctionModules' >> temp.sh

cp temp.sh $SCRIPT_FILE

rm temp.sh

echo "DataPath profile.d script installed."
echo "Note: the script will not take affect until your next login."
