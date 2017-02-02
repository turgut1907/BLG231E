/* @Author
* Student Name: Turgut Can Aydinalev
* Student ID : 150120021
* Date: 23.11.2016 */

#include "islemler.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>

using namespace std;

int main() {
	struct list q;
	q.readFromFile();
	bool flag = true;
	char sec = 'z';
	while (flag) {
		cout << endl << "Choose an operation: " << endl;
		cout << "P: Play" << endl;
		cout << "L: List all playlists" << endl;
		cout << "C: Create a playlist" << endl;
		cout << "A: Add a song" << endl;
		cout << "R: Remove a song" << endl;
		cout << "D: Delete a playlist" << endl;
		cout << "W: Write to file" << endl;
		cout << "E: Exit" << endl << endl;

		cout << "Choice: ";
		cin >> sec;

		switch(sec) {

		case'p':
		case'P':
			q.play(NULL);
			break;

		case'l':
		case'L':
			q.printList();
			break;

		case'c':
		case'C':
			char *isim,temp[50];
			cout << "Enter the name of the PL: ";
			cin >> temp;
			isim = new char[50];
			strcpy(isim, temp);
			q.createList(isim);
			delete isim;
			break;

		case'a':
		case'A':
			q.addSong(NULL);
			break;

		case'r':
		case'R':
			q.removeSong(NULL);
			break;

		case'd':
		case'D':
			q.deleteList(NULL);
			break;

		case'w':
		case'W':
			q.writeToFile();
			break;

		case'e':
		case'E':
			q.exit();
			flag = false;
			break;

		default:
			break;
		}
	}

	return EXIT_SUCCESS;
}