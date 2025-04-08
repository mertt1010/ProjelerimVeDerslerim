#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <iomanip>

using namespace std;

/*Mnemonic: Bir mnemonic'in ad�n�, opcode'unu ve boyutunu tutar. 
 "MOV" mnemonic'i bir�ok assembly dilinde bir veri kopyalama komutunu temsil eder. Ancak bu komutun bilgisayar
 taraf�ndan i�lenmesi i�in bir opcode'a ihtiya� vard�r. �rne�in, x86 mimarisinde "MOV" mnemonic'i i�in bir opcode,
  0x8B say�s�na kar��l�k gelir. Yani, bilgisayar "MOV" komutunu i�lerken bu say�sal de�eri kullan�r. Dolay�s�yla mnemonic,
  insanlar i�in, opcode ise bilgisayarlar i�in �nemlidir.*/
struct Mnemonic {
    string name;
    string opcode;
    int size;
};

// Symbol: Bir sembol�n ad�n� ve adresini tutar.
struct Symbol {
    string name;
    int address;
};

// 8085 mikroi�lemci i�in mnemonic'leri, opcode'lar� ve boyutlar� i�eren  tablo (MOT)
//Opcode bir bilgisayar�n anlayabilece�i makine dilindeki bir komutun say�sal temsilidir.bellekte "78" opcode'unu g�rd���nde, bu "MOV"
// komutunu anlar ve belirli bir kaynaktan hedefe veri kopyalar.
unordered_map<string, Mnemonic> MOT = {
    {"MOV", {"MOV", "78", 1}},
    {"MVI", {"MVI", "3E", 2}},
    {"LXI", {"LXI", "21", 3}},
    {"LDA", {"LDA", "3A", 3}},
    {"STA", {"STA", "32", 3}},
    {"LHLD", {"LHLD", "2A", 3}},
    {"SHLD", {"SHLD", "22", 3}},
    {"XCHG", {"XCHG", "EB", 1}},
    {"ADD", {"ADD", "80", 1}},
    {"ADI", {"ADI", "C6", 2}},
    {"SUB", {"SUB", "90", 1}},
    {"SUI", {"SUI", "D6", 2}},
    {"INR", {"INR", "3C", 1}},
    {"DCR", {"DCR", "3D", 1}},
    {"INX", {"INX", "23", 1}},
    {"DCX", {"DCX", "2B", 1}},
    {"DAD", {"DAD", "29", 1}},
    {"ANA", {"ANA", "A0", 1}},
    {"XRA", {"XRA", "A8", 1}},
    {"ORA", {"ORA", "B0", 1}},
    {"CMP", {"CMP", "B8", 1}},
    {"CMA", {"CMA", "2F", 1}},
    {"RLC", {"RLC", "07", 1}},
    {"RRC", {"RRC", "0F", 1}},
    {"RAL", {"RAL", "17", 1}},
    {"RAR", {"RAR", "1F", 1}},
    {"JMP", {"JMP", "C3", 3}},
    {"JC", {"JC", "DA", 3}},
    {"JNC", {"JNC", "D2", 3}},
    {"JZ", {"JZ", "CA", 3}},
    {"JNZ", {"JNZ", "C2", 3}},
    {"JP", {"JP", "F2", 3}},
    {"JM", {"JM", "FA", 3}},
    {"JPE", {"JPE", "EA", 3}},
    {"JPO", {"JPO", "E2", 3}},
    {"HLT", {"HLT", "76", 1}},
    {"NOP", {"NOP", "00", 1}}
};

// Sembol tablosunu tutmak i�in bir vekt�r tan�mlad�k bu vekt�r etiketlerin ve adreslerinin listesini tutar.
vector<Symbol> symbolTable;

// string'in tamam�n�n rakamlardan olu�up olu�mad���n� kontrol sembol m� say� m� ona g�re veri i�leme yapmal�y�z
bool isNumber(const string& str) {
    for (char const &c : str) {//her karakteri teker teker tar�yoruz
        if (isdigit(c) == 0) return false; // karakterin rakam olup olmad�g�n� kontrol ediyoruz
    }
    return true;
}

//  pass1 Fonksiyonu Sembol Tablosunu Olu�turmak i�in 
void pass1(const string& filename) {
    ifstream inputFile(filename); //input dosyas�n� a�ar.
    string line;
    int locationCounter = 0; //Konum sayac�n� ba�lat�r. taramaya ba�lama yerimiz 

    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    while (getline(inputFile, line)) { // Dosyay� sat�r sat�r oku
        
        istringstream iss(line);// Sat�r� bo�luklara ve tablara g�re ay�r
        string label, mnemonic, operand;
        if (!(iss >> label)) { // Sat�r bo�sa veya yorum sat�r�ysa, atlar
            
            continue;
        }

        
        if (label.back() == ':') { // Sat�rdaki ilk kelimenin sonunda ':' karakteri varsa, bu bir etiket tan�m�d�r
                                  // etiket tan�m� var, sembol tablosuna ekle
            
            label.pop_back(); // Etiket i�aretini (:) kald�r
            symbolTable.push_back({label, locationCounter});// Etiketi ve mevcut adresi sembol tablosuna ekler
    // Etiket i�areti sonras� k�sm� okumak i�in bir bo�luk b�rak
            
            if (!(iss >> mnemonic)) {
                // Etiketten sonra ba�ka bir �ey yoksa, bir sonraki sat�ra ge�
                continue;
            }
        } else {
            mnemonic = label;// Etiket tan�m� yoksa, sat�r�n ba��ndaki de�er bir mnemonik olmal�d�r
    // Bu durumda da sembol tablosuna eklenir
            
            symbolTable.push_back({mnemonic, locationCounter});// Mnemonik ve mevcut adresi sembol tablosuna ekler
        }

        // ��lem kodunu ge�erliyse konum sayac�n� g�ncelle
        if (MOT.find(mnemonic) != MOT.end()) {
            Mnemonic m = MOT[mnemonic];// Mnemonik bilgilerini al
            locationCounter += m.size;// Konum sayac�n� mnemonik boyutuna g�re g�ncelle
        }
    }

    inputFile.close();
}





// input dosyas�n� okuyarak makine kodunu �retir ve ��kt� dosyas�na yazmak ge�i� 2
void pass2(const string& filename) {
    ifstream inputFile(filename);
    ofstream outputFile("output.txt");//��kt� dosyas�n� a�ar.
    string line;
    int address = 0; // Adres sayac�n� ba�lat�r

    // A�AGIDAK� �K� �F DOSYALARIN A�ILIP A�ILMADIGINI KONTROL EDER 
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    if (!outputFile.is_open()) {
        cerr << "Error opening output file" << endl;
        exit(1);
    }

    while (getline(inputFile, line)) {// Giri� dosyas�n� sat�r sat�r okur
        istringstream iss(line);// Sat�r� bo�luklara g�re ayr��t�r�r
        string label, mnemonic, operand;
        if (!(iss >> label)) {// Sat�rdan ilk kelimeyi al�r
            continue;// Sat�r bo�sa veya yorum sat�r�ysa, bir sonraki sat�ra ge�er
        }

          // E�er kelime(label), etiket veya i�lem tablosunda bulunamazsa sat�r� atlar
        if (MOT.find(label) == MOT.end()) {// �lk kelime (label) MOT'da bulunamazsa sat�r� atlar 
        
            continue;
        }

        mnemonic = label;// �lk kelimeyi mnemonik olarak kabul eder
         // E�er mnemonik ge�erli bir i�lem koduysa
        if (MOT.find(mnemonic) != MOT.end()) {
            Mnemonic m = MOT[mnemonic];// Mnemonic bilgilerini al�r
            // ��lem kodunu ve adresini ��kt� dosyas�na yaz
            outputFile << m.opcode << " " << setw(4) << setfill('0') << hex << address;
            
            // E�er mnemonic boyutu 1'den b�y�kse, bir operand olabilir
            if (m.size > 1) {
                iss >> operand; // Operand� al�r
                if (!operand.empty()) {
                	  // E�er operand bir say� ise, hexadecimal olarak yazar
                    if (isNumber(operand)) {
                        outputFile << " " << operand;
                    } else {
                        // Operand sembol ise, sembol�n adresini bul ve hexadecimal olarak yaz
                        for (const auto& sym : symbolTable) {
                            if (sym.name == operand) {
                                outputFile << " " << setw(4) << setfill('0') << hex << sym.address;
                                break; 
                            }
                        }
                    }
                }
            }
            outputFile << endl; // Yeni sat�r ekler
            address += m.size;  // Adres sayac�n� mnemonik boyutuna g�re g�nceller
        } 
    }

    inputFile.close();
    outputFile.close();
}



//sembol tablas� olu�turmak i�in gerekli fonksiyon 
void printSymbolTable() {
	    // Sembol tablosunu yazmak i�in bir dosya a�ar
    ofstream symTableFile("symbol_table.txt");
    
    // Dosyan�n do�ru a��l�p a��lmad���n� kontrol eder
    if (!symTableFile.is_open()) {
        cerr << "Error opening symbol table file" << endl;
        exit(1);// Dosya a�ma hatas� durumunda program� sonland�r�r
    }
     //dosyan�n i�ine ba�l�klar� att�k 
    symTableFile << "Symbol Table:\n";
    symTableFile << "Name\tAddress\n";
    
    // Sembol tablosundaki her bir sembol� dosyaya yazar
    for (const auto& sym : symbolTable) {
        // Sembol ismi ve adresini hexadecimal formatta dosyaya yazar
        symTableFile << sym.name << "\t" << setw(4) << setfill('0') << hex << sym.address << endl;
    }

    symTableFile.close();
}

int main() {
    string inputFileName = "input.asm"; // demo kodumuzun oldu�u input dosyas�
    pass1(inputFileName);//ilk ge�i� sembol tablosunu olu�turuyoruz
    cout << "Symbol Table Boyutu: " << symbolTable.size() << endl; // Kontrol i�in sembol tablosunun boyutunu yazd�k
    printSymbolTable();//sembol tablosunu dosyaya yazd�k 
    pass2(inputFileName); // �kinci ge�i�  makine kodunu olu�turur
    return 0;
}

/*B�Z SEMBOL TABLOSUNDA MNEMON�CLER� YAZDIRDIK SEMBOLLER ORG LOOP G�B� YAZDIRMAMIZ GEREK�YO OLAB�L�R.
MOV A, 10   ; MOV mnemonic, A ve 10 operandlar
ADD A, B    ; ADD mnemonic, A ve B operandlar
JMP START   ; JMP mnemonic, START operand*/

