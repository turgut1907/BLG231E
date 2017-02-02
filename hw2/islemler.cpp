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
#include <time.h>
using namespace std;

void list::readFromFile() {
	dosya = fopen(dosyaadi, "r");
	if (dosya == NULL) {
		cerr << "File can not be found!" << endl;
		return;
	}
	playlist *newlist = new playlist;
	newlist->name = "Original";
	head = newlist;

	playlist *rand = new playlist;
	rand->name = "Random";
	playlist *sorted = new playlist;
	sorted->name = "Sorted";
	head->next = sorted;
	sorted->prev = head;
	sorted->next = rand;
	rand->prev = sorted;
	rand->next = head;
	head->prev = rand;

	song_node *traverse,*tail,*traverse2,*traverse3,*tail3;
	song temp;
	newlist->songnumber = 0;
	sorted->songnumber = 0;
	rand->songnumber = 0;
	newlist->head = NULL;
	sorted->head = NULL;
	rand->head = NULL;

	while (!feof(dosya)) {

		fscanf(dosya, "\n%[^\t]\t%[^\t]\t%d\t%s", temp.name, temp.singer, &temp.year, temp.style);
		
		song *sarki = new song;
		song_node *snode = new song_node;
		song_node *sorted_node = new song_node;
		song_node *rand_node = new song_node;

		strcpy(sarki->name, temp.name);
		strcpy(sarki->singer, temp.singer);
		strcpy(sarki->style, temp.style);
		sarki->year = temp.year;
		snode->data = sarki;
		sorted_node->data = sarki;
		rand_node->data = sarki;

		newlist->songnumber++;
		sorted->songnumber++;
		rand->songnumber++;
		if (sorted->head == NULL) {
			sorted->head = sorted_node;
		}

		else {
			traverse2 = sorted->head;
			bool soneleman = false;
			while (strcmp(sorted_node->data->singer, traverse2->data->singer) > 0) {

				if (sorted->songnumber == 2) {
					sorted->head->next = sorted_node;
					sorted_node->prev = sorted->head;
					sorted->head->prev = sorted_node;
					sorted_node->next = sorted->head;
				}

				traverse2 = traverse2->next;

				if (traverse2 == sorted->head) {
					traverse2 = traverse2->prev;
					soneleman = true;
					break;
				}
			}
			while (strcmp(sorted_node->data->singer, traverse2->data->singer) == 0 && strcmp(sorted_node->data->name, traverse2->data->name) > 0) {
				traverse2 = traverse2->next;

				if (traverse2 == sorted->head) {
					traverse2 = traverse2->prev;
					soneleman = true;
					break;
				}
			}
			if (traverse2 == sorted->head) {

				if (sorted->songnumber == 2) {
					sorted->head->next = sorted_node;
					sorted_node->prev = sorted->head;
					sorted->head->prev = sorted_node;
					sorted_node->next = sorted->head;
				}
				else {
					sorted->head->prev->next = sorted_node;
					sorted_node->prev = sorted->head->prev;
					sorted_node->next = sorted->head;
					sorted->head->prev = sorted_node;
				}
				sorted->head = sorted_node;
			}
			else {
				if (soneleman) {
					sorted->head->prev->next = sorted_node;
					sorted_node->prev = sorted->head->prev;
					sorted_node->next = sorted->head;
					sorted->head->prev = sorted_node;
				}

				else {
					sorted_node->next = traverse2;
					sorted_node->prev = traverse2->prev;
					traverse2->prev->next = sorted_node;
					traverse2->prev = sorted_node;
				}
			}
		}


		if (newlist->head == NULL) {
			newlist->head = snode;
			traverse = snode;
		}

		else {
			tail = traverse;
			traverse = snode;
			tail->next = traverse;
			traverse->prev = tail;
			traverse->next = newlist->head;
			newlist->head->prev = traverse;
		} 

		if (rand->head == NULL) {
			rand->head = rand_node;
			traverse3 = rand_node;
		}

		else {
			tail3 = traverse3;
			traverse3 = rand_node;
			tail3->next = traverse3;
			traverse3->prev = tail3;
			traverse3->next = rand->head;
			rand->head->prev = traverse3;
		}
	}
	karistir(); // karistir() her cagrildiginda karisir random listesi
	playlist_count = 3;

};

void list::createList(char *isim) {
	karistir(); // yeniden karistiriliyor random list
	playlist *yeni = new playlist;
	playlist_count++;
	strcpy(yeni->name,isim);
	yeni->songnumber = 0;
	head->prev->next = yeni;
	yeni->prev = head->prev;
	yeni->next = head;
	head->prev = yeni;
	int secenek;
	cout << "Please select a option(1 for adding song by song, 2 for create a PL of songs of a singer, 3 for create a PL of songs of a style: ";
	cin >> secenek;

	if (secenek == 1) {
		char secim;
		bool flag = true;
		while (flag) {
			addSong(yeni);
			cout << "Do you want to add more(Y/N)?: ";
			cin >> secim;
			while (secim != 'y' && secim != 'Y' && secim != 'n' && secim != 'N') {
				cout << "Just (Y/N)?: ";
				cin >> secim;
			}
			if (secim == 'N' || secim == 'n') {
				flag = false;
			}
		}
	}

	else if (secenek == 2) {
		cout << "Please select a singer:" << endl;
		int j = 1;
		int i = 0;
		song_node *cursor = head->next->head;
		while (i < head->next->songnumber) {

			if (strcmp(cursor->next->data->singer, cursor->data->singer) == 0) {
				cursor = cursor->next;
				i++;
				continue;
			}
			cout << j << ". " << cursor->data->singer << endl;
			i++;
			j++;
			cursor = cursor->next;
		}
		cout << "Choice: ";
		int sarkici;
		cin >> sarkici;
		int a = 0;
		cursor = head->next->head;
		while (a < sarkici - 1) {

			if (strcmp(cursor->next->data->singer, cursor->data->singer) == 0) {
				cursor = cursor->next;
				continue;
			}
			a++;
			cursor = cursor->next;
		}
		song_node *gecici = head->next->head; // sorted in basina
		for (int i = 0; i < head->next->songnumber; i++) {
			if (strcmp(cursor->data->singer, gecici->data->singer) == 0) {
				teksarkiekle(yeni, gecici);
			}
			gecici = gecici->next;
		}
		cout << "Success!" << endl;
	}

	else if(secenek == 3){
		cout << "Please select a style: " << endl;
		cout << "1-Classical" << endl << "2-Blues" << endl << "3-Flamenco" << endl << "4-Rock" << endl << "5-Slow" << endl << "6-Soundtrack" << endl;

		
		song_node *cursor;
		cout << "Choice: ";
		int stil;
		cin >> stil;
		char *tut;
		switch (stil) {
		case 1:
			tut = "CLASSICAL";
			break;
		case 2:
			tut = "BLUES";
			break;
		case 3:
			tut = "FLAMENCO";
			break;
		case 4:
			tut = "ROCK";
			break;
		case 5:
			tut = "SLOW";
			break;
		case 6:
			tut = "SOUNDTRACK";
			break;
		}
		int a = 0;
		cursor = head->next->head;


		song_node *gecici = head->next->head; // sorted in basina
		for (int i = 0; i < head->next->songnumber; i++) {
			if (strcmp(tut, gecici->data->style) == 0) {
				teksarkiekle(yeni, gecici);
			}
			gecici = gecici->next;
		}
		cout << "Success!" << endl;
	}

};

void list::addSong(playlist * calmalistesi) {
	if (calmalistesi == NULL) {
		playlist *traverse = head, *curs;
		for (int i = 0; i < playlist_count; i++) {
			cout << i + 1 << ". " << traverse->name << endl;
			traverse = traverse->next;
		}
		cout << endl << "Choice(List No): ";
		int secim;
		cin >> secim;
		curs = head;

		for (int j = 0; j < secim - 1; j++) {
			curs = curs->next;
		}
		calmalistesi = curs;
	}
	if (calmalistesi->name == "Sorted" || calmalistesi->name == "Random" || calmalistesi->name == "Original") {
		cout << "You can not change default playlists!" << endl;
		return;
	}
	playlist *temp = head->next; // sorted
	song_node* traverse = temp->head;


	for (int i=0; i<temp->songnumber;i++) {
		cout << i + 1 << ". " << traverse->data->singer << "\t" << traverse->data->name << "\t" << traverse->data->year << "\t" << traverse->data->style << endl;
		traverse = traverse->next;
	}
	int secim;
	cout << "Please enter the song no that you want to add: ";
	cin >> secim;

	traverse = temp->head;

	for (int i = 0; i<secim-1; i++) {
		traverse = traverse->next;
	}

	song_node *eklenen = new song_node;
	eklenen->data = traverse->data;
	if (calmalistesi->songnumber == 0) { //bossa
		calmalistesi->head = eklenen;
		eklenen->next = NULL;
		calmalistesi->songnumber++;
		return;
	}


	if (calmalistesi->songnumber == 1) { //sadece head varsa
		calmalistesi->head->next = eklenen;
		eklenen->prev = calmalistesi->head;
		calmalistesi->head->prev = eklenen;
		eklenen->next = calmalistesi->head;
		calmalistesi->songnumber++;
		return;
	}
	else {
		song_node *izle = calmalistesi->head; // headin bi oncesine yani sona ekliyoruz
		izle->prev->next = eklenen;
		eklenen->prev = izle->prev;
		eklenen->next = izle;
		izle->prev = eklenen;
		calmalistesi->songnumber++;
	}
	cout<< endl << "Added!" << endl;
};

void list::play(playlist *liste) {
	playlist *temp = head;
	int i = 1;
	if (liste == NULL) { // listesiz opsiyon kullanici seciyo burda
		playlist *traverse = head;
		for (int i = 0; i < playlist_count; i++) {
			cout << i + 1 << ". " << traverse->name << endl;
			traverse = traverse->next;
		}
		cout << endl << "Choice(List No): ";
		int secim;
		cin >> secim;
		temp = head;

		for (int j = 0; j < secim - 1; j++) {
			temp = temp->next;
		}
		liste = temp;
		song_node *gecici = temp->head;

		for (int i = 0; i < temp->songnumber; i++) {
			cout << i + 1 << ". " << gecici->data->singer << "\t" << gecici->data->name << "\t" << gecici->data->year << "\t" << gecici->data->style << endl;
			gecici = gecici->next;
		}


	}
 // listeli versiyon
		cout << "Where do you want to start listening?(For the first song please enter 1): ";
		int kont;
		cin >> kont;
		song_node *traverse = liste->head;

		for (int i = 0; i < kont - 1; i++) {
			traverse = traverse->next;
		}

		char giris = 'q';

		while (giris != 'e' && giris != 'E')  {
			if (giris != 'b') {
				cout << "PLAYING >> " << traverse->data->singer << " - " << traverse->data->name << " (" << traverse->data->year << "-" << traverse->data->style << ") <<" << endl;
			}
			cout << "Choice(P,N,E): ";
			cin >> giris;
			if (giris == 'e' || giris == 'E') {
				return;
			}
			switch (giris)
			{

			case'p':
			case'P':
				traverse = traverse->prev;
				break;

			case'n':
			case'N':
				traverse = traverse->next;
				break;

			case'e':
			case'E':
				break;

			default:
				cout << "Please enter a proper character" << endl;
				giris = 'b';
				break;
			}
		
	}

};

void list::removeSong(playlist *liste) {
	if (liste == NULL) {
		playlist *traverse = head, *curs;
		for (int i = 0; i < playlist_count; i++) {
			cout << i + 1 << ". " << traverse->name << endl;
			traverse = traverse->next;
		}
		cout << endl << "Choice(List No): ";
		int secim;
		cin >> secim;
		curs = head;

		for (int j = 0; j < secim - 1; j++) {
			curs = curs->next;
		}
		liste = curs;
	}


	if (liste->name == "Sorted" || liste->name == "Random" || liste->name == "Original") {
		cout << "You can not change default playlists!" << endl;
		return;
	}

	song_node *temp = liste->head;
	cout << liste->name << ":" << endl << endl;
	for (int i = 0; i < liste->songnumber; i++) {
		cout << i + 1 << ". " << temp->data->singer << "\t" << temp->data->name << "\t" << temp->data->year << "\t" << temp->data->style << endl;
		temp = temp->next;
	}
	int secim;
	cout << "Please select a song and enter its initial value: ";
	cin >> secim;
	temp = liste->head; 
	for (int i = 0; i < secim-1; i++) {
		temp = temp->next;
	}
	if (temp == NULL) {
		cerr << "hata" << endl;
	}
	else {
		if (liste->songnumber == 0) {
			cerr << "The list is already empty!" << endl;
			return;
		}
		if (liste->songnumber == 1) {
			delete temp;
			liste->head = NULL;
			liste->songnumber--;
			cout << "Removed!" << endl;
			return;
		}
		if (temp == liste->head) {
			temp->prev->next = temp->next;
			temp->next->prev = temp->prev;
			liste->head = temp->next;
			delete temp;
			liste->songnumber--;
			cout << "Removed!" << endl;
			return;
		}

		if (liste->songnumber == 2) {
			delete temp;
			liste->head->next = NULL;
			liste->head->prev = NULL;
			liste->songnumber--;
			cout << "Removed!" << endl;
		}
		else {
			temp->prev->next = temp->next;
			temp->next->prev = temp->prev;
			delete temp;
			cout << "Removed!" << endl;
			liste->songnumber--;

		}
	}
};

void list::deleteList(playlist *temp) {
	if (temp == NULL) {
		playlist *traverse = head;
		for (int i = 0; i < playlist_count;i++) {
			cout << i+1 << ". " << traverse->name << endl;
			traverse = traverse->next;
		}
		int i;
		cout << "Enter the no of PL you want to delete: ";
		cin >> i;
		temp = head;
		for (int a = 0; a < i - 1; a++) {
			temp = temp->next;
		}
	}

	if (temp->name == "Sorted" || temp->name == "Random" || temp->name == "Original") {
		cout << "You can not delete default playlists!" << endl;
	}
	else {
		temp->prev->next = temp->next;
		temp->next->prev = temp->prev;
		delete temp;
		playlist_count--;
		cout << "Deleted!" << endl;
	}
};

void list::printList() {
	char giris = 'a';
	char giris2 = 'a';

	playlist *traverse = head;
	int i = 1;
	cout << i++ << ". " << traverse->name << endl;
	traverse = traverse->next;
	while (traverse != head) {
		cout << i << ". " << traverse->name << endl;
		i++;
		traverse = traverse->next;
	}

	while (giris != 'e' && giris != 'E' && giris != 's' && giris != 'S') {
		song_node *temp = traverse->head;

		if (giris != 'b') {
			cout << endl << "Current: " << traverse->name << endl;

	}
		cout << endl << "Choice(P,N,S,E): " ;
		cin >> giris;

		switch (giris)
		{
		case'p':
		case'P':
			traverse = traverse->prev;
			break;

		case'n':
		case'N':
			traverse = traverse->next;
			break;

		case'e':
		case'E':
		case's':
		case'S':
			break;

		default:
			cout << "Please enter a proper character(P,N,E,S)" << endl;
			giris = 'b';
			break;
		}
	}
	if (giris == 's' || giris == 'S') {
		song_node *temp = traverse->head;
		if (giris2 != 'b') {
			cout << traverse->name << ":" << endl << endl;
			for (int i = 0; i < traverse->songnumber; i++) {
				cout << i + 1 << ". " << temp->data->singer << "\t" << temp->data->name << "\t" << temp->data->year << "\t" << temp->data->style << endl;
				temp = temp->next;
			}
		}

		while (giris2 != 'e' && giris2 != 'E') {
			cout << endl << "Choice(A,R,D,P,E): ";
			cin >> giris2;
			switch (giris2) {
			case 'd':
			case 'D':
				deleteList(traverse);
				return;

			case'A':
			case'a':
				addSong(traverse);
				return;

			case'r':
			case'R':
				if (traverse->head == NULL)
				{
					cout << "The list is already empty!" << endl;
				}
				else {
					removeSong(traverse);
				}
				return;

			case'p':
			case'P':
				play(traverse);
				return;

			case'e':
			case'E':
				return;

			default:
				cout << "Please enter a proper character" << endl;
				giris2 = 'b';
				break;
			}
		}
	}
};

void list::writeToFile() {
	dosya = fopen("songbook_edited.txt", "w");
	if (dosya == NULL) {
		cerr << "Error" << endl;
	}
	song_node *temp = head->head;

	for (int i = 0; i < head->songnumber; i++) {
		fprintf(dosya, "%s\t%s\t%d\t%s\n", temp->data->name, temp->data->singer, temp->data->year, temp->data->style);
		temp = temp->next;
	}
	playlist *cursor = head->prev;
	while (cursor != head->next->next) {
		fprintf(dosya,"*****\n");
		temp = cursor->head;
		for (int i = 0; i < cursor->songnumber; i++) {
			fprintf(dosya, "%s\t%s\t%d\t%s\n", temp->data->name, temp->data->singer, temp->data->year, temp->data->style);
			temp = temp->next;
		}
		cursor = cursor->prev;
	}

	fclose(dosya);

};

void list::exit() {
	writeToFile();
	
	//deallocation
	song_node *imlec = head->head;
	song_node *temp;
	while (imlec->data) {
		delete imlec->data;
		imlec->data = NULL;
		imlec = imlec->next;

	}
	playlist *listcursor = head;
	playlist *gecici;
	int i = 0,j, max;
	while (i < playlist_count)  {
		j = 0;
		imlec = listcursor->head;
		max = listcursor->songnumber;
		while (j < max) {
			temp = imlec;
			imlec = imlec->next;
			delete temp;
			j++;
		}
		gecici = listcursor;
		listcursor = listcursor->next;
		delete gecici;
		i++;
	}
};

void list::karistir() { 
	srand(time(NULL));
	playlist *temp = head;
	while (strcmp("Random", temp->name) != 0) {
		temp = temp->next;
	}
	int sarkisayisi = temp->songnumber;


	for (int i = 0; i < 1000; i++) {
		int rastgele = rand() % sarkisayisi;
		int rastgelegidis = rand() % sarkisayisi;
		song_node *traverseGiris = temp->head;

		for (int j = 0; j <= rastgele;j++) {
			traverseGiris = traverseGiris->next;
		}

		if (traverseGiris == temp->head) {
			traverseGiris->prev->next = traverseGiris->next;
			traverseGiris->next->prev = traverseGiris->prev;
			temp->head = traverseGiris->next;
		}
		else {
			traverseGiris->prev->next = traverseGiris->next;
			traverseGiris->next->prev = traverseGiris->prev;
		}

		song_node *traverseCikis = temp->head;

		for (int j = 0; j < rastgelegidis; j++) {
			traverseCikis = traverseCikis->next;
		}
		traverseCikis->next->prev = traverseGiris;
		traverseGiris->next = traverseCikis->next;
		traverseGiris->prev = traverseCikis;
		traverseCikis->next = traverseGiris;
	}
	        
};

void list::teksarkiekle(playlist *calmalistesi, song_node *eklenecek) {
	song_node *eklenen = new song_node;
	eklenen->data = eklenecek->data;
	if (calmalistesi->songnumber == 0) { //bossa
		calmalistesi->head = eklenen;
		eklenen->next = NULL;
		calmalistesi->songnumber++;
		return;
	}


	if (calmalistesi->songnumber == 1) { //sadece head varsa
		calmalistesi->head->next = eklenen;
		eklenen->prev = calmalistesi->head;
		calmalistesi->head->prev = eklenen;
		eklenen->next = calmalistesi->head;
		calmalistesi->songnumber++;
		return;
	}
	else {
		song_node *izle = calmalistesi->head; // headin bi oncesine yani sona ekliyoruz
		izle->prev->next = eklenen;
		eklenen->prev = izle->prev;
		eklenen->next = izle;
		izle->prev = eklenen;
		calmalistesi->songnumber++;
	}
}