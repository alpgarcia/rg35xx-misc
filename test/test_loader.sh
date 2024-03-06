#!/bin/bash

LOGO="boot_logo.jpg"
STATUS_FILE="init_status.log"
N=31

# Create file or remove its contents
echo "" > ${STATUS_FILE}

./loading_screen ${LOGO} ${STATUS_FILE} 127 &

# wait before start sending messages
#sleep 5

echo "SCRIPTS ${N}" >> ${STATUS_FILE} 

for ((i=1; i<=${N}; i++)); do

	echo "START Process ${i} started" >> ${STATUS_FILE}
	
	sleep .$((0 + RANDOM % 999))
	
	echo "END Process ${i} finished" >> ${STATUS_FILE}
	
	sleep .3

done

echo "PRINT Everything done, exiting in 5 seconds" >> ${STATUS_FILE}

sleep 5

echo "FINISH" >> ${STATUS_FILE}

