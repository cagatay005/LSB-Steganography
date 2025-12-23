#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

class Color {
public:
    unsigned char r, g, b;
    Color(unsigned char rr = 0, unsigned char gg = 0, unsigned char bb = 0) {
        r = rr; g = gg; b = bb;
    }
};

// Rengi tek bir 32-bit tamsayıda paketler (A yapar)
unsigned int Birlestir(unsigned char r, unsigned char g, unsigned char b) {
    return (r << 16) | (g << 8) | b;
}

// A sayısını alıp R, G, B parçalarına (RGB) ayırır
// Bu fonksiyonu Image sınıfının dışına aldık ki her yerden erişilebilsin.
void ParcalariAyir(unsigned int A, unsigned char& R, unsigned char& G, unsigned char& B) {
    R = (A >> 16) & 0xFF; // İlk 8 bit (En sol - Kırmızı Kanalı)
    G = (A >> 8) & 0xFF; // İkinci 8 bit (Orta - Yeşil Kanalı)
    B = A & 0xFF;  // Üçüncü 8 bit (En sağ - Mavi Kanalı)
}

// --- IMAGE SINIFI ---
class Image {
private:
    int width, height;
    Color** matrix; // Pikselleri tutan 2 boyutlu dizi

public:
    Image(int w, int h) {
        width = w;
        height = h;
        matrix = new Color*[height];
        for (int i = 0; i < height; i++)
            matrix[i] = new Color[width];
    }

    // Dosyadan resmi okuyup matrix'e doldurur
    void yukleBMP(const char* filename) {
        ifstream file(filename, ios::binary);
        if (!file) {
            cout << "HATA: BMP dosyasi acilamadi!" << endl;
            return;
        }

        unsigned char header[54];
        file.read((char*)header, 54);
        
        // BMP dosyalarında satırlar genelde tersten kaydedilir (height-1'den başlar) bu yüzden ters döngü kullanıyoruz.
        for (int i = height - 1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                unsigned char pixel[3];
                file.read((char*)pixel, 3);
                matrix[i][j].b = pixel[0]; // Mavi
                matrix[i][j].g = pixel[1]; // Yeşil
                matrix[i][j].r = pixel[2]; // Kırmızı


            }
        }
        file.close();
        cout << "Resim basariyla yuklendi." << endl;
    }

    void TumResmiAnalizEt() {
        cout << "Analiz basliyor..." << endl;
        
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
            
            // 1. Mevcut pikseli al
            Color p = matrix[i][j];

            // 2. Paketle (A yap) - Sayısal olarak tamamını A ya yazar
            unsigned int A = Birlestir(p.r, p.g, p.b);

            // 3. Parçala (R, G, B yap - Sayısal olarak)
            unsigned char R, G, B;
            ParcalariAyir(A, R, G, B);

            // 4. --- BINARY GÖRÜNTÜLEME KISMI ---
            // Her bir parçayı 8 bitlik binary formatına çeviriyoruz.Bu sayede her rengin bit düzeyindeki temsilini görebiliriz.
            bitset<8> binR(R);
            bitset<8> binG(G);
            bitset<8> binB(B);

            

            // Örnek olarak sadece ilk pikselin değerlerini yazdıralım Denemek için burayı yorum satırından çıkarabilirsin.
            /*if (i == 0 && j == 0) {
                cout << "Piksel (0,0) Degerleri:" << endl;
                
                cout << "R (Kirmizi - Binary): " << binR << endl; 
                cout << "G (Yesil   - Binary): " << binG << endl;
                cout << "B (Mavi    - Binary): " << binB << endl;
                
                // Ayrıca A'nın tamamını (32 bit veya 24 bit) görmek istersen:
                cout << "A (Tum Paket Binary): " << bitset<24>(A) << endl;
            }*/

            // Eğer tüm pikselleri analiz etmek istersen, aşağıdaki kodu kullanabilirsin (Tüm pikselleri yazdırır ve çok fazla çıktı olur. Dikkatli kullan):
            /*for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    cout << "Piksel (0,0) Degerleri:" << endl;
                
                    cout << "R (Kirmizi - Binary): " << binR << endl; 
                    cout << "G (Yesil   - Binary): " << binG << endl;
                    cout << "B (Mavi    - Binary): " << binB << endl;
                
                    // Ayrıca A'nın tamamını (32 bit veya 24 bit) görmek istersen:
                    cout << "A (Tum Paket Binary): " << bitset<24>(A) << endl;
                    }
                }*/
            }
        }
        cout << "Analiz tamamlandi." << endl;
    }
    
    
    void Sifrecozme() {
        cout << "Sifre cozme islemi basladi: ";
        
        char Mesaj[1024]; 
        int Indeks = 0;
        unsigned char mesajBitleri = 0;
        int bitSayaci = 0;

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                
                unsigned int A = Birlestir(matrix[i][j].r, matrix[i][j].g, matrix[i][j].b);
                unsigned char R, G, B;
                ParcalariAyir(A, R, G, B);

                unsigned char Renkler[3] = {R, G, B};
                
                for (int k = 0; k < 3; k++) {
                    unsigned char sonBitler = Renkler[k] & 1;
                    mesajBitleri = (mesajBitleri << 1) | sonBitler;
                    bitSayaci++;

                    // 8 bit (1 Harf) oluştuğunda:
                    if (bitSayaci == 8) {
                        char bulunanKarakter = (char)mesajBitleri;
                        Mesaj[Indeks] = bulunanKarakter;
                        Indeks++;
                        
                        // Sadece karakteri değil, sayısal kodunu da (ASCII) yazdırıyoruz.
                        // Böylece karakter görünmez olsa bile (boşluk, null vs.) sayıyı görürsün.
                        if (bulunanKarakter >= 32 && bulunanKarakter < 127) {
                            // Okunabilir harf ise harfi göster
                            cout << "[" << bulunanKarakter << "] "; 
                        } else {
                            // Okunamaz ise sayısal değerini göster (Örn: [0], [13] vb.)
                            cout << "[" << (int)(unsigned char)bulunanKarakter << "] ";
                        }

                        // Çıkış Koşulu: Null karakter
                        if (bulunanKarakter == '\0') {
                            cout << "\n[Bitti] Gizli Mesaj Tamamlandi." << endl;
                            return;
                        }

                        // Çıkış Koşulu: Dizi taşması
                        if (Indeks >= 1000) {
                            cout << "\n\n[SINIR] 1000 karakter okundu." << endl;
                            return;
                        }

                        mesajBitleri = 0;
                        bitSayaci = 0;
                    }
                }
            }
        }
    }
    
    // Yıkıcı fonksiyon (Bellek temizliği)
    ~Image() {
        for (int i = 0; i < height; i++)
            delete[] matrix[i];
        delete[] matrix;
    }
};


int main() {
    // (Gerçek bir BMP yükleyeceksen boyutları o resme göre ayarlamayı veya constructor'ı değiştirmeyi unutma) Benim yüklediğim output.bmp 300x300 boyutunda.
    Image resim(300, 300); 

    resim.yukleBMP("output1.bmp");
    
    // 3. Yüklenen resmin her pikselini binary mantığıyla ayırıp analiz et
    resim.TumResmiAnalizEt();

    // 4. Gizli mesajı çöz ve ekrana yazdır
    resim.Sifrecozme();
    
    return 0;
}