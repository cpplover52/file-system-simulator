#include "file_system.h"
#include "process_command.h"
#include <vector>
#include <iostream>
#include <conio.h> // _getch() için (Sadece Windows'ta çalýþýr)

// Yardýmcý Fonksiyon: String'in son kelimesini bulur (örn: "cfi dos" -> "dos")
string getLastWord(const string& input) {
    size_t found = input.find_last_of(" ");
    if (found == string::npos) return input; // Hiç boþluk yoksa kelimenin kendisidir
    return input.substr(found + 1);
}

// Özel Input Fonksiyonu (Tab Destekli)
string getLineWithTabCompletion(FileSystemManager& fsm) {
    string input = "";
    char ch;

    while (true) {
        ch = _getch(); // Tuþa basýlmasýný bekle (Enter beklemez)

        // --- ENTER TUÞU (ASCII 13) ---
        if (ch == 13) {
            cout << endl;
            return input;
        }
        // --- BACKSPACE TUÞU (ASCII 8) ---
        else if (ch == 8) {
            if (!input.empty()) {
                input.pop_back();
                // Ekranda silme efekti için: Geri git, boþluk yaz, geri git
                cout << "\b \b";
            }
        }
        // --- TAB TUÞU (ASCII 9) ---
        else if (ch == 9) {
            if (input.empty()) continue;

            // Son kelimeyi al (Prefix)
            string prefix = getLastWord(input);
            if (prefix.empty()) continue;

            vector<string> suggestions = fsm.getSuggestionsPublic(prefix);
            if (suggestions.empty()) {
                
            }
            else if (suggestions.size() == 1) {
                // TEK EÞLEÞME VARSA: Otomatik tamamla
                string completion = suggestions[0].substr(prefix.length());
                input += completion;
                cout << completion; // Ekrana kalaný yazdýr
            }
            else {
                // ÇOKLU EÞLEÞME VARSA: Linux gibi altýna listele
                cout << "\n";
                for (const auto& s : suggestions) {
                    cout << s << "\t";
                }
                cout << "\n";
                // Prompt ve mevcut inputu tekrar yazdýr ki kullanýcý kaldýðý yeri görsün
                cout << fsm.getPath(fsm.getCurrent()) << ":";
                cout << input;
            }
        }
        // --- NORMAL KARAKTERLER ---
        else if (ch >= 32 && ch <= 126) {
            input += ch;
            cout << ch;
        }
    }
}
using namespace std;

/*
Dosya Sistemi Simülatörü (Aðaç + Hash + Kuyruk)
Fikir: Klasör-dosya aðaç yapýsý, arama ve geri dönüþüm kutusu.
Veri yapýlarý: Genel aðaç (N-ary), hash tablo (inode tablosu), queue (silinenleri yaþlandýrma).
Algoritmalar: DFS/BFS tarama, farklý sýralamalarla listeleme.
Ekstra: Kotalar, LRU temizlik.
*/

/*
Binary Search (search file/folder)
*/

int main() {
	setlocale(LC_ALL, "Turkish");
	srand(time(NULL));
	FileSystemManager fsm;
	string input;
	while (true) {
        cout << fsm.getPath(fsm.getCurrent()) << ":";
		/*getline(cin,input);*/
        string input = getLineWithTabCompletion(fsm);
        if (input.empty()) continue; // Boþsa geç
        processCommand(fsm, input);
	}
	return 0;
}