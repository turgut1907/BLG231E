#include <stdio.h>
#include "messagebox_ioctl.h"
#include <sys/ioctl.h>
#include <fcntl.h>

void flush(void) { // flush stdin for scanf bug with %c
	int c;
	while ( (c = fgetc(stdin)) != EOF && c !='\n' );
}

int main(void) {
	char a;
	int err;
	printf("What do you want to do? (r,l,d): "); //r for change the read mode, l for set the msg box limit, d for delete the messages of given user
	scanf("%c",&a);
	while (a != 'r' && a != 'l' && a != 'd') { 
	printf("\nWhat do you want to do? (r,l,d): ");
	scanf("%c",&a);
	flush();
	}

	if (a == 'r') { //read mode change
	 while (a != 'i' && a != 'e') {
			printf("\nInclude/Exclude? (i,e): ");
			flush();
			scanf("%c",&a);
		};

		if (a == 'i') err = ioctl(open("/dev/messagebox",0),SET_READ,INCLUDE_READ);
		else err = ioctl(open("/dev/messagebox",0),SET_READ,EXCLUDE_READ);
	}

	else if (a == 'l') { // set limit
		int lim;
		printf("\nLimit?: ");
		flush();
		scanf("%d",&lim);
		err = ioctl(open("/dev/messagebox",0),SET_LIMIT,lim);
	}

	else { // delete user messages
		char user[20];
		printf("\nUsername?: ");
		flush();
		scanf("%s",user);
		err = ioctl(open("/dev/messagebox",0),DELETE_MSG,user);
	}

	printf("Err Code: %d\n",err);
	return 0;
}