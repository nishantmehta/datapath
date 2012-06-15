#!/bin/bash

PROJECT_ROOT=".."
BUILD_XML="./build/build.xml"
WAR_NAME="./lib/datapath-console.war"
DEST_WAR_NAME="datapath-console.war"
DEST_APP_NAME="datapath-console"
WANTED_BACKUPS=3
USERNAME="praveen"
HOST="10.5.0.200"
SSHPORT=8022

if [ -f warname ]; then
    DEST_WAR_NAME="$(cat warname).war"
    DEST_APP_NAME="$(cat warname)"
fi

cd ${PROJECT_ROOT}

ant -f ${BUILD_XML} clean
ant -f ${BUILD_XML} all
rm ${DEST_WAR_NAME} > /dev/null
mv ${WAR_NAME} ${DEST_WAR_NAME}

if [ "$1" = "remote" ]; then 
    #backup
    date=`date "+%Y%m%d-%H.%M"`
    echo
    echo "Backing up previous version of ${DEST_WAR_NAME} at kore05:/tmp/${DEST_WAR_NAME}.${date}"
    echo
    ssh -p ${SSHPORT} ${USERNAME}@${HOST} "ssh ${USERNAME}@kore05 'cp /usr/local/tomcat7/webapps/${DEST_WAR_NAME} /tmp/${DEST_WAR_NAME}.${date}'"
    
    #scp to datapath
    echo "SCP ${DEST_WAR_NAME} to datapath at ${HOST}"
    echo
    scp -P ${SSHPORT} ${DEST_WAR_NAME} ${USERNAME}@${HOST}:/home/${USERNAME}/
    echo
    
    #scp to kore05
    echo "SCP ${DEST_WAR_NAME} from datapath to kore05"
    echo
    ssh -p ${SSHPORT} ${USERNAME}@${HOST} "scp /home/${USERNAME}/${DEST_WAR_NAME} ${USERNAME}@kore05:/usr/local/tomcat7/webapps/."
    echo
    echo "*** Destination WAR name: ${DEST_WAR_NAME} ***"
    echo "*** The webapp will be reachable at http://10.5.0.200:8080/${DEST_APP_NAME} ***"
    echo   
else
    echo
    echo "*** Destination WAR name: ${DEST_WAR_NAME} ***"
    echo "*** The webapp will be reachable at http://localhost:8080/${DEST_APP_NAME} ***"
    echo   

    sudo cp ${DEST_WAR_NAME} /usr/local/tomcat7/webapps/.
fi
