#include<bits/stdc++.h>
using namespace std;

// Ondal�k say�y� ikilik say� sistemine �evirme fonksiyonu
string dec_to_bin(int decimal)
{
	string binary = "";
	for(int i = decimal; i > 0; i /= 2)
		binary = to_string(i % 2) + binary;
	if(binary.length() < 8)
		binary = string(8 - binary.length(), '0').append(binary);
	return binary;
}

// Mnemonics yap�s�: isim, ikilik g�sterim, ve boyut
struct mnemonics{
	string name;
	string binary;
	int size;
}MOT[15]; // Makine Operasyon Tablosu (MOT)

// Sembol yap�s�: isim, t�r, konum, boyut, b�l�m kimli�i, ve global olup olmad���
struct symbol{ 
	string name;
	string type;
	int location;
	int size;
	int section_id;
	string is_global;
};

// B�l�m yap�s�: kimlik, isim, ve boyut
struct section{ 
	int id;
	string name;
	int size;
};

// Global de�i�kenler ve vekt�rler
vector<symbol> symbol_table; 
vector<section> section_table; 
int lc = 0; // Lokasyon sayac�
int sec_id = 0; // B�l�m kimli�i
int var_lc; // De�i�ken lokasyonu
ifstream in_f; // Giri� dosya ak���
ofstream out_f; // ��k�� dosya ak���
string word; // Kelime de�i�keni
string str; // Ge�ici string
int pointer, size; // ��aret�i ve boyut de�i�kenleri

// MOT'da opcode arama fonksiyonu
int search_MOT(string opcode) 
{
	int index = -1;
	for(int i = 0; i < 15; i++)
	{
		if(MOT[i].name == opcode)
		{
			index = i;
			break;
		}
	}
	return index;
}

// Sembol tablosunda de�i�ken arama fonksiyonu
int search_symbol_table(string variable) 
{
	int location = -1;
	for(vector<symbol>::const_iterator i = symbol_table.begin(); i != symbol_table.end(); ++i)
	{
		if(i->name == variable)
		{
			location = i->location;
			break;
		}
	}
	return location;
}

// Veri boyutunu hesaplama fonksiyonu
int get_size(string data) 
{
	int size = 0;
	for(int i = 0; i < data.length(); i++)
	{
		if(data[i] == ',')
			size += 4;
	}
	size += 4;
	return size;
}

// Veriyi ikilik formata �evirme fonksiyonu
string get_data(string data) 
{
	string final;
	string temp_str = "";
	for(int i = 0; i < data.length(); i++)
	{
		if(data[i] == ',')
		{
			final += dec_to_bin(stoi(temp_str.c_str())) + ",";
			temp_str = "";
		}
		else 
			temp_str += data[i];
	}
	final += dec_to_bin(stoi(temp_str.c_str())) + ",";
	temp_str = "";
	final.erase(final.length() - 1, 1);
	return final;
}

// Sembol tablosunu dosyaya yazma fonksiyonu
void store_symbol_table() 
{
	out_f.open("symbol.csv");
	out_f << "Name,Type,Location,Size,SectionID,IsGlobal\n";
	for(vector<symbol>::const_iterator i = symbol_table.begin(); i != symbol_table.end(); ++i)
	{
		out_f << i->name << ",";
		out_f << i->type << ",";
		out_f << i->location << ",";
		out_f << i->size << ",";
		out_f << i->section_id << ",";
		out_f << i->is_global << "\n";
	}	
	out_f.close();
}

// B�l�m tablosunu dosyaya yazma fonksiyonu
void store_sec() 
{
	out_f.open("section.csv");
	out_f << "ID,Name,Size\n";
	for(vector<section>::const_iterator i = section_table.begin(); i != section_table.end(); ++i)
	{
		out_f << i->id << ",";
		out_f << i->name << ",";
		out_f << i->size << "\n";
	}
	out_f.close();
}

// �lk ge�i� fonksiyonu: sembolleri ve b�l�mleri tan�mlar, boyutlar�n� ve konumlar�n� belirler
void pass1()
{
	in_f.open("input.txt");
	while(in_f >> word)
	{
		pointer = search_MOT(word);
		if(pointer == -1)
		{
			str = word;
			if(word.find(":") != -1) // Etiket (label) tan�m�
			{
				symbol_table.push_back({str.erase(word.length() - 1, 1), "label", lc, -1, sec_id, "false"});
			}
			else if(word == "section") // Yeni b�l�m tan�m�
			{
				in_f >> word;
				sec_id++;
				section_table.push_back({sec_id, word, 0}); 
				if(sec_id != 1) // �nceki b�l�m�n boyutunu g�ncelle
				{
					section_table[sec_id - 2].size = lc;
					lc = 0;
				}
			}
			else if(word == "global") // Global de�i�ken tan�m�
			{
				in_f >> word;
				symbol_table.push_back({word, "label", -1, -1, -1, "true"}); 
			}
			else if(word == "extern") // Harici de�i�ken tan�m�
			{
				in_f >> word;
				symbol_table.push_back({word, "external", -1, -1, -1, "false"}); 
			}
			else // De�i�ken tan�m�
			{
				in_f >> word;
				in_f >> word;
				size = get_size(word);
				symbol_table.push_back({str, "var", lc, size, sec_id, "false"}); 
				lc += size;
			}
		}
		else // Opcode i�lenmesi
		{
			if(!(pointer == 9 || pointer == 14)) 
				in_f >> word;
			if(pointer == 2 || pointer == 10 || pointer == 11)
				in_f >> word;
			lc += MOT[pointer].size;
		}
	}
	section_table[sec_id - 1].size = lc; // Son b�l�m�n boyutunu g�ncelle
	store_symbol_table(); // Sembol tablosunu kaydet
	store_sec(); // B�l�m tablosunu kaydet
	in_f.close();
}

// �kinci ge�i� fonksiyonu: assembly kodunu makine koduna �evirir ve dosyaya yazar
void pass2()
{
	in_f.open("input.txt");
	out_f.open("output.txt");
	while(in_f >> word)
	{
		pointer = search_MOT(word);
		if(pointer == -1)
		{
			str = word;
			if(word.find(":") != -1) // Etiket sat�rlar�n� bo� ge�
 			{
 				out_f << "";
			}
			else if(word == "global") // Global de�i�ken tan�m�
			{
				in_f >> word;
				out_f << "global " << word << endl;
			}
			else if(word == "extern") // Harici de�i�ken tan�m�
			{
				in_f >> word;
				out_f << "extern " << word << endl;
			}
			else if(word == "section") // Yeni b�l�m tan�m�
			{
				in_f >> word;
				out_f << "section ." << word << endl;
				lc = 0;
			}
			else // De�i�ken tan�m� ve ikilik veriye �evirme
			{
				in_f >> word;
				in_f >> word;
				out_f << dec_to_bin(lc) << " " << get_data(word) << endl;
				size = get_size(word);
				lc += size;
			}
		}
		else // Opcode i�lenmesi
		{
			out_f << dec_to_bin(lc) << " " << MOT[pointer].binary;
			if(pointer == 0 || pointer == 1) // Tek operandl� talimatlar
			{
				in_f >> word;
				out_f << " " << word;
			}
			else if(pointer == 5 || pointer == 6 || pointer == 7 || pointer == 8 || pointer == 13) // Bellek adresli talimatlar
			{
				in_f >> word;
				var_lc = search_symbol_table(word);
				if(var_lc == -1)
					out_f << " " << dec_to_bin(stoi(word.c_str()));
				else
					out_f << " " << dec_to_bin(var_lc);
			}
			else if(pointer == 2 || pointer == 10) // �ki operandl� talimatlar
			{
				in_f >> word;
				out_f << " " << word;
				in_f >> word;
				var_lc = search_symbol_table(word);
				if(var_lc == -1)
					out_f << " " << dec_to_bin(stoi(word.c_str()));
				else
					out_f << " " << dec_to_bin(var_lc);
			}
			else if(pointer == 11) // Veri ta��ma talimatlar�
			{
				in_f >> word;
				out_f << " " << word;
				in_f >> word;
				out_f << " " << word;
			}
			lc += MOT[pointer].size;
			out_f << "\n";
		}	
	}
	out_f.close();
	in_f.close();
}

// Ana fonksiyon: MOT tan�mlamas�, pass1 ve pass2 �a�r�s�
int main()
{
	// MOT (Makine Operasyon Tablosu) tan�mlamalar�
	MOT[0] = {"ADD", "00000001", 1};
	MOT[1] = {"INC", "00000010", 1};
	MOT[2] = {"CMP", "00000011", 5};
	MOT[3] = {"JNC", "00000100", 1};
	MOT[4] = {"JNZ", "00000101", 1};
	MOT[5] = {"ADDI", "00000110", 5};
	MOT[6] = {"JE", "00000111", 5};
	MOT[7] = {"JMP", "00001000", 5};
	MOT[8] = {"LOAD", "00001001", 5};
	MOT[9] = {"LOADI", "00001010", 1};
	MOT[10] = {"MVI", "00001011", 5};
	MOT[11] = {"MOV", "00001100", 1};
	MOT[12] = {"STOP", "00001101", 1};
	MOT[13] = {"STORE", "00001110", 5};
	MOT[14] = {"STORI", "00001111", 1};
	pass1(); // �lk ge�i�
	lc = 0;
	pass2(); // �kinci ge�i�
	
	return 1;
}

