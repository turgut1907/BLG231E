#!/bin/bash
gcc applicant.c -o applicant
gcc hr.c -o hr -pthread
gcc trial.c -o trial -pthread
 ./applicant $2 & ./hr  $1 $2 & ./trial $1