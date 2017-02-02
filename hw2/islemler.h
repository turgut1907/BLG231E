/* @Author
* Student Name: Turgut Can Aydinalev
* Student ID : 150120021
* Date: 23.11.2016 */

#ifndef OPERATIONS_H
#define OPERATIONS_H
#include "struct.h"
#include <iostream>
//islemler

struct list {
	FILE *dosya;
	char *dosyaadi = "songbook.txt";
	int playlist_count;
	playlist* head;
	void createList(char *);
	void addSong(playlist *);
	void play(playlist *);
	void removeSong(playlist *);
	void deleteList(playlist *);
	void printList();
	void writeToFile();
	void readFromFile();
	void exit();
	void karistir();
	void teksarkiekle(playlist *, song_node *);
};

#endif