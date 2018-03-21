/* @Author
* Student Name: TURGUT CAN AYDINALEV
* Student ID : 150120021
* Date: 12.10.2016
*/
#ifndef OPERATIONS_H
#define OPERATIONS_H
#define MAXK 200
#define MAXS 30
#include "kayit.h"
#include <iostream>

struct islem {
	FILE *dosya;
	char *dosyaadi = "database.txt";
	patient_record k[MAXK], sorted[MAXK], gecici[MAXK];

	void dosyaOkuma();
	void dosyaYazma();
	void sirala();
	bool kontrol();
	void listele();
	int araBranch(char [20]);
	int araPoly(int);
	void ekle(patient_record *); 
	void sil(int);
	int ksayim; // normal bitis elemani
	int sortsayim; // sortun bitis elemani
};

#endif