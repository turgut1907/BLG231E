/* @Author
* Student Name: TURGUT CAN AYDINALEV
* Student ID : 150120021
* Date: 12.10.2016
*/
#include "islemler.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <iomanip>
#include <stdio.h>

using namespace std;

//temel olarak sirasiz bir bicimde k da tutuyoruz eklenenleri, gecicide siralayip sorted da bosluklu hale getiriyoruz

void islem::dosyaOkuma() {
	dosya = fopen(dosyaadi, "r");

	if (dosya == NULL) {
		cerr << "File can not be found!" << endl;
		return;
	}
	else {
		int i = 0;

		while (!feof(dosya)) { 
			fscanf(dosya, "\n%[^\t]\t%[^\t]\t%[^\t]\t%d\t%d\t%s",k[i].name,k[i].doctorName,k[i].diagnosis,&k[i].patientNumber,&k[i].polyclinicNumber,k[i].branchName);
			if (feof(dosya)) break;
			i++;
		}

		fclose(dosya);
		ksayim = i;
		islem::sirala();
	}
}
 
void islem::dosyaYazma() {
	dosya = fopen(dosyaadi, "w");
		for (int i = 0; i < ksayim; i++) {
			fprintf(dosya, "%s\t%s\t%s\t%d\t%d\t%s\n", k[i].name, k[i].doctorName, k[i].diagnosis, k[i].patientNumber, k[i].polyclinicNumber, k[i].branchName);
			if (k[i].branchName == NULL) continue;
			} 

	fclose(dosya);
}


void islem::sirala() { 
	for (int i = 0; i < MAXK; i++) {
		sorted[i] = {}; //structlar bosaltiliyor 
		gecici[i] = {};
	}


	for (int i = 0; i <= ksayim; i++) {
		gecici[i] = k[i]; //ilk siralama icin geciciye atiyoruz k yi
	}

	patient_record *kptr, temp;

	kptr = gecici;

	for (int j = 0; j <= ksayim; j++) {

		for (int i = 0; i < (ksayim); i++) { 

			if (strcmp((kptr + i)->branchName, (kptr + i + 1)->branchName) >= 0) {

				if (strcmp((kptr + i)->branchName, (kptr + i + 1)->branchName) == 0) {
					if (((kptr + i)->polyclinicNumber > (kptr + i + 1)->polyclinicNumber)) {
						temp = *(kptr + i + 1);
						*(kptr + i + 1) = *(kptr + i);
						*(kptr + i) = temp;
					}
				}

				else {
					temp = *(kptr + i + 1);
					*(kptr + i + 1) = *(kptr + i);
					*(kptr + i) = temp;
				}
			}
		}
	}
	for (int i = 0; i < ksayim; i++) {
		gecici[i] = gecici[i + 1];
	} 
	gecici[ksayim] = {};
	// geciciyi siraladik fakat yer rezervleri yok. sirali bicimde bosluksuz yazdirmak isteseydik bunu yazdirabilirdik.
	
		kptr = gecici; // yer rezervleri
		int i = 0;
		int sayac = 0;
		

		for (int j = 0; j <= ksayim; j++) {

			if ((kptr + j )->polyclinicNumber == (kptr + j + 1)->polyclinicNumber) {
				sorted[sayac + i] = *(kptr + j);
				i++;
			}

			else {
				sorted[sayac + i] = *(kptr + j);
				i = 0;
				sayac = sayac + 10;
			}
			
		}
		sortsayim = sayac; // sortedin son elemani, 10 ar 10 ar aldigi icin 10 un katlarinda sayida eleman basacak
}


bool islem::kontrol() {

	
	int polykont[MAXS];
	for (int i = 0; i < MAXS; i++) {
		polykont[i] = 0;
	}
	int polyk = 0;
	int sayac = 0, polsayar = 0;
	patient_record *kptr;

	kptr = gecici;
	for (int i = 0; i < ksayim; i++) { // sayac yardimi ile 10 u asarsa hata veriyor

		if (strcmp((kptr + i)->branchName, (kptr + i + 1)->branchName) == 0) {


			if ((kptr + i)->polyclinicNumber == (kptr + i + 1)->polyclinicNumber) {
				sayac++;

				if (sayac > 9) {
					cerr << "1 polyclinic has maximum capacity of 10 patients!" << endl;
					return false;
				}
			}

			else {
				sayac = 0;
			}
		}

		else {
			sayac = 0;
		}
	}  // 

	int j = 0;
	char temp[20];
	int sayit;
	strcpy(temp, gecici[0].branchName);
	sayit = gecici[0].polyclinicNumber;

	while (j < ksayim) { // bir dalda 2 den fazla poliklinik var mi onun kontrolu
		if (strcmp(temp,gecici[j].branchName)==0) {

			if (sayit == gecici[j].polyclinicNumber) {
				j++;
				continue;
			}
			else {
				sayit = gecici[j].polyclinicNumber;
				polsayar++;

				if (polsayar > 1) {
					cerr << "1 branch has no more than 2 polyclinics!" << endl;
					return false;
				}
			}
		}
		else {
			polsayar = 0;
			strcpy(temp, gecici[j].branchName);
			sayit = gecici[j].polyclinicNumber;
		}
		j++;
	}

	

	for (int i = 0; i <= ksayim; i++) { // 1 POLY 1 DAL KISMI
		if (gecici[i].polyclinicNumber != gecici[i + 1].polyclinicNumber || strcmp(gecici[i].branchName, gecici[i + 1].branchName) != 0) {
			polykont[polyk] = gecici[i].polyclinicNumber;
			polyk++;
		}
	}

		for (int i = 0; i < polyk; i++)  { 
			for (int j = 0; j < polyk; j++) {
				if ((polykont[i] == polykont[j]) && (polykont[i] != 0) && (polykont[j] != 0) && (i != j)) {
					cerr << "1 polyclinic can be used for only 1 branch!" << endl; //
					return false;
				}
			}
		}

	
	return true;
}


void islem::listele() {
	cout << "Patient List with Empty Lines:" << endl; // eger bosluksuz yazdýrmak istersek gecici arrayini yazdirmamiz yeterli
	for (int i = 0; i < sortsayim; i++)
	{
		cout << i << ". " << sorted[i].name << "\t" << sorted[i].doctorName << "\t" << sorted[i].diagnosis << "\t" << sorted[i].patientNumber << "\t" << sorted[i].polyclinicNumber << "\t" << sorted[i].branchName << endl;
	}
}


int islem::araBranch(char aranan[20]) {
	int f = 0;
	for (int i = 0; i<=sortsayim; i++) {
		if (strncmp(sorted[i].branchName, aranan, strlen(aranan)) != 0) continue;
		cout << i << ". " << sorted[i].name << "\t" << sorted[i].doctorName << "\t" << sorted[i].diagnosis << "\t" << sorted[i].patientNumber << "\t" << sorted[i].polyclinicNumber << "\t" << sorted[i].branchName << endl;
		f++;
	}
	return f;
}


int islem::araPoly(int aranan) {
	int f = 0;
	for (int i = 0; i <= sortsayim; i++) {
		if (aranan != sorted[i].polyclinicNumber) continue;
		cout << i << ". " << sorted[i].name << "\t" << sorted[i].doctorName << "\t" << sorted[i].diagnosis << "\t" << sorted[i].patientNumber << "\t" << sorted[i].polyclinicNumber << "\t" << sorted[i].branchName << endl;
		f++;
	}
	return f;
}


void islem::ekle(patient_record *ptr) {

	k[ksayim] = *ptr;
	ksayim++;
	islem::sirala();
	if (islem::kontrol() == false) { // kontrolden gecmezse
		k[ksayim-1] = {}; //ekledigimizi geri siliyoruz
		ksayim--;
		cout << "Can not be recorded!" << endl;
		islem::sirala();
	}
	else {
		cout << "Recorded!" << endl;
	}
}


void islem::sil(int aranan) {

	for (int i = 0; i <= ksayim; i++) {
		if (aranan == k[i].patientNumber) {
			for (int j = i; j <= ksayim; j++) {
				k[j] = k[j + 1];
			}
			ksayim--;
		}
	islem::sirala();
	}
}