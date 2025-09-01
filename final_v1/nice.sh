#!/bin/bash 
#       ^  
#       |  
#     Do not forget to check the shebang for your host machine


echo "The script       : $0"
#echo "The question no  : $1"
#echo "Path to dir(abs) : $2"

echo "server files $1"
echo "client files $2"
echo "PROXY files $3"

declare -a CLIENT
declare -a SERVER

if [[ $# == 3 ]];then

	declare -a PROXY
	IFS=' ' read -a PROXY  <<< "$3"
fi


IFS=' ' read -a SERVER <<< "$1"
IFS=' ' read -a CLIENT <<< "$2"


for setOfFiles in "$@";
do
	echo ">>>>$setOfFiles"	
done

> connectionstatus.log
> unitsep.log

FILE_PATH=$2
#TCPDUMP_PID=0

declare -i PROGRAMS_RAN_COUNT=0

declare -A coprocPids
declare -A coprocFDs
declare -A coprocReadFDs
declare -A programPids
declare -A Assigned_Ports

Qi="q1"

source ./fun.sh
source ./clientPorts.sh
source ./serverPorts.sh
source ./student.sh


> ${student_id}_conn.csv
> ${student_id}_status.csv
> ${student_id}_evaluated.csv

# THE STAFF SHOULD WRITE THE CODE HERE TO 
# DO THE EVALUATION IN RESPECT TO THIER NEED.
# THIS SYSTEM IS COMPLETELY RELIABLE AND 
# FLEXIBLE.


START_TCPDUMP "tcp" "${SERVER_PORT[0]}" "transfer.pcap"
sleep 3

WAIT_PORT "tcp" ${SERVER_PORT[0]}
COMPILE_RUN "${SERVER[0]}" "myserver"

CHECK_PORT "127.0.0.1:${SERVER_PORT[0]}" "0.0.0.0:0000" "myserver" "tcp" "LISTEN" 

bash port_set_Advanced.sh ${CLIENT_PORT[0]} ${CLIENT_PORT[0]}
COMPILE_RUN "${CLIENT[0]}" "myclient" 
#COMPILE_RUN "${CLIENT[1]}" "myclient1" ${CLIENT_PORT[1]}
#COMPILE_RUN "${CLIENT[2]}" "myclient2" ${CLIENT_PORT[2]}

ISALIVE myclient
echo "status : $?"

#sleep 1
CHECK_PORT "127.0.0.1:${SERVER_PORT[0]}" "127.0.0.1:${CLIENT_PORT[0]}" "myserver" "tcp" "ESTABLISHED"
INPUT "myclient"  input 3 1
CHECK_PORT "127.0.0.1:${SERVER_PORT[0]}" "127.0.0.1:${CLIENT_PORT[0]}" "myserver" "tcp" "ESTABLISHED" "NO"
#sleep 1

sleep 3
END_TCPDUMP

EVALUATE "tcp" 1

START_TCPDUMP "tcp" "${SERVER_PORT[0]}" "transfer.pcap"
sleep 3


ISALIVE "myclient"
echo "status : $?"

COMPILE_RUN "${CLIENT[0]}" "myclient" ${CLIENT_PORT[0]}
#sleep 1
CHECK_PORT "127.0.0.1:${SERVER_PORT[0]}" "127.0.0.1:${CLIENT_PORT[0]}" "myserver" "tcp" "ESTABLISHED"
INPUT "myclient"  input 3 1
CHECK_PORT "127.0.0.1:${SERVER_PORT[0]}" "127.0.0.1:${CLIENT_PORT[0]}" "myserver" "tcp" "ESTABLISHED" "NO"
#sleep 1


ISALIVE "myclient"
echo "status : $?"


sleep 3
END_TCPDUMP

EVALUATE "tcp" 2

CLEAR_ALL


echo "end of work"
