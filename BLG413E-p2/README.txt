SYSTEM PROGRAMMING - PROJECT 2 - TURGUT CAN AYDINALEV / BESTE BURCU BAYHAN
----------------------------------------------------------------------------------------------------------------------------------------------------
For running the module;
in the messagebox folder with "sudo su"
% make
% insmod ./messagebox.ko // to set the box limit, use "% insmod ./messagebox.ko boxlimit=NUMBER"
% mknod /dev/messagebox c 247 0  // care the major number. check with "% grep messagebox /proc/devices"
% chmod 666 /dev/messagebox // for 'permission denied' error while using the echo command, changes running mode of the device.

After using make, you can use start.sh for the other commands

Syntax;
Sending a Message:  % echo "@nameoftheuser message" > /dev/messagebox
Reading Messages: % cat /dev/messagebox

IOCTL Functions;
Changing the reading mode: SET_READ with INCLUDE_READ and EXCLUDE_READ arguments
Setting the limit: SET_LIMIT with an integer argument
Delete the messages of a user: DELETE_MSG with a char array argument
For testing the functions, "test.c" is provided 