/* @Author
* Student Name: TURGUT CAN AYDINALEV
* Student ID : 150120021
* Date: 12.10.2016
*/
#include "islemler.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>

using namespace std;
struct islem x;

bool mainloop(char); // secim loopu
void menuyaz(); // menu yazdirma
void list();
void araBr();
void araPt();
void ekle();
void remove();

int main()
{
	bool flag = false;
	char s;

	x.dosyaOkuma();

	while (!flag) {
		menuyaz();
		cin >> s;
		flag = mainloop(s);
	}
	x.dosyaYazma();
}

void menuyaz() {
	cout << endl << endl;
	cout << "Please select the operation to perform and enter the operation code" << endl <<endl;
	cout << "P: Print all of the patient records" << endl;
	cout << "B: Search the data by the branch name" << endl;
	cout << "C: Search the data by the polyclinic number" << endl;
	cout << "I: Insert a new patient record" << endl;
	cout << "R: Remove a patient record" << endl;
	cout << "E: Exit the program" << endl;
	cout << endl;
	cout << "Your selection is: ";
}

bool mainloop(char s) {
	bool flag = false;
	bool defflag = true;

		switch (s) {
		case 'P': case 'p':
			list();
			break;

		case 'B': case 'b':
			araBr();
			break;

		case 'C': case 'c':
			araPt();
			break;

		case 'I': case 'i':
			ekle();
			break;

		case 'R': case 'r':
			remove();
			break;

		case 'E': case 'e':
			flag = true;
			break;

		default:
			cout << "Please enter a valid character!" << endl;
			cout << "Your selection is: ";
			cin >> s;
			defflag = false;
			break;
		}
		if (!defflag) { //hatali giris
			mainloop(s);
		}
	
	return flag;
}

void list() {
	x.listele();
}
void araBr() {
	int a;
	char br[20];
	cout << "Please enter the branch name(Case sensetive!): ";
	cin >> br;
	a = x.araBranch(br);
	if (a == 0) {
		cout << "Record can not be found!" << endl;
	}
}
void araPt() {
	int a, poly;
	cout << "Please enter the polyclinic number: ";
	cin >> poly;
	a = x.araPoly(poly);
	if (a == 0) {
		cout << "Record can not be found!" << endl;
	}
}
void ekle() {
	patient_record a;

	cin.ignore(1000, '\n');
	cout << "Patient Name : ";
	cin >> a.name;

	cin.ignore(1000, '\n');
	cout << "Doctor Name : ";
	cin >> a.doctorName;

	cin.ignore(1000, '\n');
	cout << "Diagnosis : ";
	cin >> a.diagnosis;

	cin.ignore(1000, '\n');
	cout << "Patient Number : ";
	cin >> a.patientNumber;

	while (a.patientNumber < 1000 || a.patientNumber > 9999) { //4 hane kontrolu
		cout << "Patient number should has 4 digits! " << endl;
		cout << "Please reenter the patient number : ";
		cin >> a.patientNumber;
	}

	cout << "Polyclinic Number : ";
	cin >> a.polyclinicNumber;

	cout << "Branch Name : ";
	cin >> a.branchName;

	x.ekle(&a);
}
void remove() {
	int a;
	cout << "Please enter the patient number you want to delete: ";
	cin.ignore(1000, '\n');
	cin >> a;
	x.sil(a);
	cout << "Deleted!" << endl;
}