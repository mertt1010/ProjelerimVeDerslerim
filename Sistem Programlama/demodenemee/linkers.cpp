#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <iomanip>

using namespace std;

/*Mnemonic: Bir mnemonic'in adýný, opcode'unu ve boyutunu tutar. 
 "MOV" mnemonic'i birçok assembly dilinde bir veri kopyalama komutunu temsil eder. Ancak bu komutun bilgisayar
 tarafýndan iþlenmesi için bir opcode'a ihtiyaç vardýr. Örneðin, x86 mimarisinde "MOV" mnemonic'i için bir opcode,
  0x8B sayýsýna karþýlýk gelir. Yani, bilgisayar "MOV" komutunu iþlerken bu sayýsal deðeri kullanýr. Dolayýsýyla mnemonic,
  insanlar için, opcode ise bilgisayarlar için önemlidir.*/
struct Mnemonic {
    string name;
    string opcode;
    int size;
};

// Symbol: Bir sembolün adýný ve adresini tutar.
struct Symbol {
    string name;
    int address;
};

// 8085 mikroiþlemci için mnemonic'leri, opcode'larý ve boyutlarý içeren  tablo (MOT)
//Opcode bir bilgisayarýn anlayabileceði makine dilindeki bir komutun sayýsal temsilidir.bellekte "78" opcode'unu gördüðünde, bu "MOV"
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

// Sembol tablosunu tutmak için bir vektör tanýmladýk bu vektör etiketlerin ve adreslerinin listesini tutar.
vector<Symbol> symbolTable;

// string'in tamamýnýn rakamlardan oluþup oluþmadýðýný kontrol sembol mü sayý mý ona göre veri iþleme yapmalýyýz
bool isNumber(const string& str) {
    for (char const &c : str) {//her karakteri teker teker tarýyoruz
        if (isdigit(c) == 0) return false; // karakterin rakam olup olmadýgýný kontrol ediyoruz
    }
    return true;
}

//  pass1 Fonksiyonu Sembol Tablosunu Oluþturmak için 
void pass1(const string& filename) {
    ifstream inputFile(filename); //input dosyasýný açar.
    string line;
    int locationCounter = 0; //Konum sayacýný baþlatýr. taramaya baþlama yerimiz 

    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    while (getline(inputFile, line)) { // Dosyayý satýr satýr oku
        
        istringstream iss(line);// Satýrý boþluklara ve tablara göre ayýr
        string label, mnemonic, operand;
        if (!(iss >> label)) { // Satýr boþsa veya yorum satýrýysa, atlar
            
            continue;
        }

        
        if (label.back() == ':') { // Satýrdaki ilk kelimenin sonunda ':' karakteri varsa, bu bir etiket tanýmýdýr
                                  // etiket tanýmý var, sembol tablosuna ekle
            
            label.pop_back(); // Etiket iþaretini (:) kaldýr
            symbolTable.push_back({label, locationCounter});// Etiketi ve mevcut adresi sembol tablosuna ekler
    // Etiket iþareti sonrasý kýsmý okumak için bir boþluk býrak
            
            if (!(iss >> mnemonic)) {
                // Etiketten sonra baþka bir þey yoksa, bir sonraki satýra geç
                continue;
            }
        } else {
            mnemonic = label;// Etiket tanýmý yoksa, satýrýn baþýndaki deðer bir mnemonik olmalýdýr
    // Bu durumda da sembol tablosuna eklenir
            
            symbolTable.push_back({mnemonic, locationCounter});// Mnemonik ve mevcut adresi sembol tablosuna ekler
        }

        // Ýþlem kodunu geçerliyse konum sayacýný güncelle
        if (MOT.find(mnemonic) != MOT.end()) {
            Mnemonic m = MOT[mnemonic];// Mnemonik bilgilerini al
            locationCounter += m.size;// Konum sayacýný mnemonik boyutuna göre güncelle
        }
    }

    inputFile.close();
}





// input dosyasýný okuyarak makine kodunu üretir ve çýktý dosyasýna yazmak geçiþ 2
void pass2(const string& filename) {
    ifstream inputFile(filename);
    ofstream outputFile("output.txt");//Çýktý dosyasýný açar.
    string line;
    int address = 0; // Adres sayacýný baþlatýr

    // AÞAGIDAKÝ ÝKÝ ÝF DOSYALARIN AÇILIP AÇILMADIGINI KONTROL EDER 
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }

    if (!outputFile.is_open()) {
        cerr << "Error opening output file" << endl;
        exit(1);
    }

    while (getline(inputFile, line)) {// Giriþ dosyasýný satýr satýr okur
        istringstream iss(line);// Satýrý boþluklara göre ayrýþtýrýr
        string label, mnemonic, operand;
        if (!(iss >> label)) {// Satýrdan ilk kelimeyi alýr
            continue;// Satýr boþsa veya yorum satýrýysa, bir sonraki satýra geçer
        }

          // Eðer kelime(label), etiket veya iþlem tablosunda bulunamazsa satýrý atlar
        if (MOT.find(label) == MOT.end()) {// Ýlk kelime (label) MOT'da bulunamazsa satýrý atlar 
        
            continue;
        }

        mnemonic = label;// Ýlk kelimeyi mnemonik olarak kabul eder
         // Eðer mnemonik geçerli bir iþlem koduysa
        if (MOT.find(mnemonic) != MOT.end()) {
            Mnemonic m = MOT[mnemonic];// Mnemonic bilgilerini alýr
            // Ýþlem kodunu ve adresini çýktý dosyasýna yaz
            outputFile << m.opcode << " " << setw(4) << setfill('0') << hex << address;
            
            // Eðer mnemonic boyutu 1'den büyükse, bir operand olabilir
            if (m.size > 1) {
                iss >> operand; // Operandý alýr
                if (!operand.empty()) {
                	  // Eðer operand bir sayý ise, hexadecimal olarak yazar
                    if (isNumber(operand)) {
                        outputFile << " " << operand;
                    } else {
                        // Operand sembol ise, sembolün adresini bul ve hexadecimal olarak yaz
                        for (const auto& sym : symbolTable) {
                            if (sym.name == operand) {
                                outputFile << " " << setw(4) << setfill('0') << hex << sym.address;
                                break; 
                            }
                        }
                    }
                }
            }
            outputFile << endl; // Yeni satýr ekler
            address += m.size;  // Adres sayacýný mnemonik boyutuna göre günceller
        } 
    }

    inputFile.close();
    outputFile.close();
}



//sembol tablasý oluþturmak için gerekli fonksiyon 
void printSymbolTable() {
	    // Sembol tablosunu yazmak için bir dosya açar
    ofstream symTableFile("symbol_table.txt");
    
    // Dosyanýn doðru açýlýp açýlmadýðýný kontrol eder
    if (!symTableFile.is_open()) {
        cerr << "Error opening symbol table file" << endl;
        exit(1);// Dosya açma hatasý durumunda programý sonlandýrýr
    }
     //dosyanýn içine baþlýklarý attýk 
    symTableFile << "Symbol Table:\n";
    symTableFile << "Name\tAddress\n";
    
    // Sembol tablosundaki her bir sembolü dosyaya yazar
    for (const auto& sym : symbolTable) {
        // Sembol ismi ve adresini hexadecimal formatta dosyaya yazar
        symTableFile << sym.name << "\t" << setw(4) << setfill('0') << hex << sym.address << endl;
    }

    symTableFile.close();
}

int main() {
    string inputFileName = "input.asm"; // demo kodumuzun olduðu input dosyasý
    pass1(inputFileName);//ilk geçiþ sembol tablosunu oluþturuyoruz
    cout << "Symbol Table Boyutu: " << symbolTable.size() << endl; // Kontrol için sembol tablosunun boyutunu yazdýk
    printSymbolTable();//sembol tablosunu dosyaya yazdýk 
    pass2(inputFileName); // Ýkinci geçiþ  makine kodunu oluþturur
    return 0;
}

/*BÝZ SEMBOL TABLOSUNDA MNEMONÝCLERÝ YAZDIRDIK SEMBOLLER ORG LOOP GÝBÝ YAZDIRMAMIZ GEREKÝYO OLABÝLÝR.
MOV A, 10   ; MOV mnemonic, A ve 10 operandlar
ADD A, B    ; ADD mnemonic, A ve B operandlar
JMP START   ; JMP mnemonic, START operand*/

