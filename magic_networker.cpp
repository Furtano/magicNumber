//   g++ -o beleg_prog2_chris -W beleg_prog2_chris.cpp
/*
	Belegarbeit Programmieren II
	C. Schade 2009
*/

#include <cstdio>
#include <iostream>
#include <sstream>
#include <cstring>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h> // fuer aufruf von externen Programmen z.b. system()
#include <iomanip>

#include "tcp_read.cc"

	#define debugX

// Zeilen der CSV-Datei
#define CSV_ROWS 12
// Maximale Bytes der Magic Number
#define MAXBYTES 12 

 
using namespace std;

namespace magics{
  struct magic_csv{	
	/* Die CSV-Datei wird mit readCSV() eingelesen und geparst, kommt  dann in dieses Struct
	*/
        // CSV_ROWS+1 (wegen Titelzeile) fuer den ersten relevanten Datensatz
        string csv_buffer2[CSV_ROWS];// Dateityp
        string csv_buffer3[CSV_ROWS];// Magic Number
        string csv_buffer4[CSV_ROWS];// Kommando
        long number[CSV_ROWS]; // ungenutzt :)
	} magie;
}

// Temporaere Dateiname
char temp_filename[] = "temp";

// Prototypen   
int readSocket();
int readCSV(); // parst die CSV und fuegt sie in die Struktur magic_csv ein
int compare_mnumbers(); // Rueckgabe = Dateiformat als Zeile-1 (Element in CSV_Array) in der CSV  // Vergleicht Den Byteanfang in der temporaeren Datei mit MaggicNumbers
int error(int);
int int_to_hex(int *dez, string & hexfile_12, int *gelesene_bytes); // wird in compare_numbers gebraucht, wandelt die gelsenen Bytes in einen Hexadezimalen String um.

int execute_program(int programformat); // programformat = Dateiformat als Zeile-1 (Element in CSV_Array) in der CSV
					//               => Uebergabe von compare_numbers()
					// aufgabe: setzt den kommandostring zusammen und fuehrt datei aus

magics::magic_csv magie;


int main (int argc, char* argv[]){
	int dateiformat = 0;
	cout << "Belegarbeit C. Schade PII-09\n";
	
	if(!readCSV()){ // wenn CSV erfolgreich gelesen wurde
		cout << " ...war erfolgreich!\n";
		cout << "\n\tBitte mit dem beleg_tester eine Datei senden.\n";
		
		if(!readSocket()){
			cout << " Datei erfolgreich empfangen!";		

			dateiformat = compare_mnumbers();
			if(dateiformat){ 
				cout << "\nNummern erfolgreich verglichen!";
				// cout << "\n" << dateiformat;
				// cout << endl << magie.csv_buffer2[dateiformat];

				execute_program(dateiformat); // gebe Dateiformat weiter und lass temp Datei ausfuehren
	

			}
			else{// unbekannted Dateiformat
				error(5);			
			}		
		}		
	}
   	
	else{
        	exit(4);
    	}
	
return 0;
}



int readCSV(){ // Oeffnet die magic-number csv Datei, trennt die Spalten auf und speichert diese in Struktur magie
	
	
	char csv_filename[] = "magic_5378859.txt"; 
	ifstream csv_file(csv_filename); // fuer die magicnr. datei
	
	int operation = 0;
	int string_erasepoint = 0;
    	string magic_number; // nicht verwndet
	
                
	if (!csv_file){
		cout << "\n\t Magic-Number-Datei konnte nicht geoeffnet werden!";
		return 1;
	}
		
	cout << "\n Parse die CSV-Datei mit den Magic Numbers...\n";	
	
	// Jeder Zeile in der CSV Datei wird in csv_buffer2 geladen
	while(getline(csv_file,magie.csv_buffer2[operation])){// Jede Zeile in der CSV kommt in eigenes String-Element
			
		if(magie.csv_buffer2[operation].empty()){// Bei Leeren Zeilen in der CSV -> Abbruch
			break;
		}
			
			
		/* 
		 * Nachdem jede Zeile in der CSV einen eigenden string hat, 
		 * wird in jeder String-Zeile nach einem Semikolon gesucht.
		 * Dann wird der String hinter dem Simikolon gelöscht.
		 * Danach wird die erhaltene Spalte mit dem alten String verglichen, 
		 * und die neu gefundene Spalte aus dem "alten" entfernt :).
		*/
		
		/*
		 * Erste Spalte wird zerlegt  (Dateityp)
		*/
		// Backup von csv_buffer2 in csv_buffer3
	   	magie.csv_buffer3[operation].insert(0, magie.csv_buffer2[operation]);
		
		// Aus jeder Zeile das Symikolon finden und von da an den String löschen
		string_erasepoint = magie.csv_buffer2[operation].find(';');
		
		// csv_buffer2(Dateityp) vorher komplette Zeile, wird zur "nur" Dateityp
		// indem ab ';' gelöscht wird 
		magie.csv_buffer2[operation].erase(string_erasepoint);

		/*
	         * Zweite Spalte wird zerlegt (MagicNumber)
		*/
		// Lösche Substring csv_buffer2 aus csv_buffer3
		string::size_type i = magie.csv_buffer3[operation].find(magie.csv_buffer2[operation]); 
			
			if (i != string::npos){ // wenn suche nicht fehlschlägt lösche den Substring
			   magie.csv_buffer3[operation].erase(i, magie.csv_buffer2[operation].length()+1);
			}
		
			
		/* 
		 * Dritte Spalte wird zerlegt (Kommando)
		*/
		// Backup von csv_buffer4 zu csv_buffer3
		magie.csv_buffer4[operation].insert(0, magie.csv_buffer3[operation]);
		
		// Aus jeder Zeile das Symikolon finden und von da an den String löschen
		string_erasepoint = magie.csv_buffer3[operation].find(';');
		magie.csv_buffer3[operation].erase(string_erasepoint);
				
		// Lösche Substring csv_buffer2 aus csv_buffer3
		i = magie.csv_buffer4[operation].find(magie.csv_buffer3[operation]); 
		
		if (i != string::npos){ // wenn suche nicht fehlschlägt lösche den Substring
		   magie.csv_buffer4[operation].erase(i, magie.csv_buffer3[operation].length()+1);
		}
						
		operation+=1;
	}
		
	#ifdef debug
	cout << "__" << "Dateityp" <<"__" << endl;
	for(int i = 0; i < CSV_ROWS; i++)
	cout << magie.csv_buffer2[i] << endl;
	
	cout << "__" << "MN" <<"__" << endl;
	for(int i = 0; i < CSV_ROWS; i++)
	cout << magie.csv_buffer3[i] << endl;
		
	cout << "__" << "Kommando" <<"__" << endl;
	for(int i = 0; i < CSV_ROWS; i++)
	cout << magie.csv_buffer4[i] << endl;
	#endif
		
	csv_file.close();      
		  
	return 0;	
}

int readSocket(){ // emfpaengt die Datei ueber das Netzwerk und speichert sie als temp ab
	int n_packets; // Anzahl der empfangenden Packete
	char *msg; // Referenz auf Netzwerkdatei
		//char filename[] = "temp"; // temporaerer Name der Netzwerkdatei
	
	
	
	n_packets = Beleg::SS09::readMessageFromNet(msg); 
	
	if (n_packets >= 0){ // wenn eine Datei Empfangen wurde
		cout << endl << " Bytes via Netz empfangen: " << n_packets << " (" << n_packets/1000 << "KB)" << endl;

		FILE* file = fopen(temp_filename,"wb"); // Temporaere Datei oeffnen
	   
		if (file!=NULL){ // Wenn Datei erfolgreich geoeffnet schreibe die Netzwerkdateireferenz in dei temporaere Datei
		       fwrite(msg, sizeof(char),n_packets,file);
		}
		   
		else{ // oeffnen der temporaeren Datei fehlgeschlagen
   		       error(3); 
		       delete[] msg;
		       msg=NULL; 
		       fclose(file);
		       return(1);  
		}
	
		fclose(file);
	
	
		// von readMessageFromNett allokierter Speicherplatz wieder freigeben
		delete[] msg;
		msg=NULL;
	   
		
	}
	
	else{// Keine Datei empfangen
		error(2);
		delete[] msg;
   		msg=NULL;
		return 1;	
	}	
	
	
	
	
	return 0;
}


int compare_mnumbers(){ // oeffnet die temporaere datei, liest die ersten 12 Bytes und vergleichst sie mit den magic numbers
			// gibt 0 bei unbekannten Dateiformat, >0 die Spalte in der CSV-Datei zurzeck

	//Stack<char, 24> hex_stack_pop;
		//char filename[] = "temp";  // temporaerer Dateiname
	long int hex;
	int c[MAXBYTES]; // hier werden die ersten 12 Bytes der Temporaeren Datei reinkopiert
	unsigned char hex_char[MAXBYTES];
	string hex_string;
	string hexfile_12;
	string substring; // zum reparieren nach der Kürzung des hexstrings nach Laenge fuer MagicNumber

	int gelesene_bytes = 0;
	
    
	string hierrein;
    
	// Uebermittelte temp datei oeffnen
    	ifstream in;
	in.open(temp_filename, ios::binary);
	stringstream out(stringstream::in | stringstream::out | stringstream::binary);
	stringstream out2;
	size_t size;
    	// Pointer an das Dateiende setzen
	streamoff curspos = in.tellg();
	in.seekg(0, ios::end);
	// Groeße vom Ende der Datei bis zum Anfang  in size ablegen
    	size = in.tellg();
	// Pointer wieder an die Anfangsposition setzen
    	in.seekg(ios::beg);
	

	int ele = 0; // interator fuer die folg. while schleife

	while (in.good()){   // bis MAXBYTES(12) gelesen sind
		c[ele] = in.get();       // get character from file	
		gelesene_bytes +=1;
	
		
		if (ele == MAXBYTES-1){
			#ifdef debug			
			cout << "\n" << MAXBYTES << " Byte-Anfang wurde aus der temporaeren Datei erfolgreich gelesen!.";
			#endif			
			break;
		}
		ele += 1;
		
		
	}
	// temp Datei wieder schliessen
	in.close();

	#ifdef debug
	cout << "\n\tgel. Bytes: " << gelesene_bytes;	
	#endif

	// wandle das Byte-Array in ein hexadezimalen String Array um
	int_to_hex(c, hexfile_12, &gelesene_bytes);
	
	int matched = 0;
	int magicnumber_laenge = 0;
	string backup_hexfile_12; 
	int dateiformat = 0;
	
	
	cout << "\n Vergleiche MN mit HexString...\n";
	for(int i = 1; i < CSV_ROWS-1; i++){  // gehe jede MagicNumber durch  (+1 wegen der Titelspalte)  

		
		if (magie.csv_buffer3[i].length() != 0){
			magicnumber_laenge = magie.csv_buffer3[i].length();

			// Backup vor dem Loeschen
			backup_hexfile_12 = hexfile_12;

			// loesche von hexfile_12 ab der Laenge von der Magic Number die Restlichen Zeichen
			hexfile_12.erase(magicnumber_laenge);
			
			// substring = hexfile_12.substr(magie.csv_buffer3[i].length(), 12);
			#ifdef debug
			cout << "\n\n\tMagic_Number " << i << " " <<  magie.csv_buffer3[i] << "\n";
			cout << "\n\n\tSubstring: " <<  hexfile_12 << "\n";
			cout << "\n\n\tLaenge: " <<  magicnumber_laenge << "\n";
			#endif 

			
			if(magie.csv_buffer3[i] == hexfile_12){ // MagicNumber = Dateianfang => Dateiformat gefunden :)
				dateiformat = i; // wird dann als return zurueckgegeben
				cout << "\n---------------------------------\n";
				cout << "Dateiformat: " << magie.csv_buffer2[i] << endl;				
				cout << "MagicNumber: " << magie.csv_buffer3[i] << endl;
				cout << "Kommando: " << magie.csv_buffer4[i];
				cout << "\n---------------------------------\n";
				matched = 1; 
				break;
			}

			hexfile_12 = backup_hexfile_12;
		}

		else {
			// cout << "\n\nNEWLINE IN DER CSV";
		}

		hexfile_12 += substring; // wieder reparieren	

	}

	

	if (matched == 0){ // => Unbekannted Dateiformat
		return 0;
	}
		  
    		
	return dateiformat;

	
}

int error(int error_number){
    
    switch (error_number){
        case 2: cerr << "Netzwerkfehler!";
		return 2;
		break;
        
        case 3: cerr << "Temporaere Datei konnte nicht geoffnet werden!";
		return 3;
		break;
        
        case 4: cerr << "Fehler beim Lesen der CSV Datei !";
		return 4;
		break;

	case 5: cerr << "\n Unbekanntes Dateiformat(jedenfalls MN nicht in CSV vorhanden)!\n";
		break;

	case 6: cerr << "\n\nProgrammaufruf fehlgeschlagen";
		break;
	case 7: cerr << "\n\nAnderer Fehler beim Programmaufruf";
    }
    
    return 0;
}

int int_to_hex(int *dez, string & hexfile_12, int *gelesene_bytes){ // Funktion wandelt ein dezimals Zahlenarray in einen HEX-String um und gibt ihn als Referenz zurueck.
	
	stringstream temp (stringstream::in | stringstream::out); // temporaerer Stream fuer die HEX-Zahlen

	
	#ifdef debug
	cout << "\n\n\tgel_Bytes:" << *gelesene_bytes << endl;
	#endif

	if (*gelesene_bytes == MAXBYTES){
		for (int i = 0; i < *gelesene_bytes; i++){ // Gehe das dezimale Array[Bytes] bis zu der max. Anzahl an Bytes durch. 
			temp << hex << setfill('0') << setw(2) << dez[i]; // Wandle die Hexadezimale Zahl in Hex um und gebe es in den Stringstream; 
		
		}
	}

	hexfile_12 = temp.str(); // Wandle den Stringstream in ein String um und weise ihn den finalen  HEXString zu.
	// fuege ein "0x" vorne an	
	hexfile_12.insert(0,"0x");

	#ifdef debug
	cout << "\n\n String nach HEX : " << hexfile_12;
	#endif

return 0;

}


int execute_program(int programformat){ // programformat = Dateiformat als Zeile-1 (Element in CSV_Array) in der CSV
					//               => Uebergabe von compare_numbers()
					// aufgabe: setzt den kommandostring zusammen und fuehrt datei aus

	// Kommando fuer gefundenes Programmformat in String packen	
	string command = magie.csv_buffer4[programformat];
	// den C-String in einen C++ String umwandeln
	string tem_filename = temp_filename; 

	size_t found_position;
	// suche %1 und speichere position in found_position
	found_position = command.find("%1");
	
	// setze Dateinamen bei %1 ein
	if (found_position > 1){ // Programm direkt temporaer ausfuehrbar z.B. jpg
		command.insert(found_position, tem_filename);
	}
	
	else{// ELF executable, es muss also ./ vor den temp. Dateinamen
		command.insert(found_position, "./"+tem_filename);
	}
	// Suche neue Position von %1
	found_position = command.find("%1");
	// und loesche ab da an, damit kommando ausfuehrbar
	command.erase(found_position);

	// fuehre Kommando aus !
	int rueck_system = system(command.c_str());


	if (rueck_system == 127){//Programmaufruf fehlgeschlagen
		error(6);		 
	}
	
	else if(rueck_system == -1){
		error(7);
	}
	
	else{	
		cout << "\n\t Kommando: " << command << endl;
		cout << "\n\t erfolgreich ausgefuehrt.\n\t Programm beendet :)\n\n.";
		#ifdef debug
		cout << "\n\t %1 Position: " << found_position << endl;
		#endif
	}



return rueck_system;
}
