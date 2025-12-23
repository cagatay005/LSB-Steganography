#include <iostream>
#include <fstream>
#include <vector>
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

    // İlk 8 bit (En sol - Kırmızı Kanalı)
    R = (A >> 16) & 0xFF;
    // İkinci 8 bit (Orta - Yeşil Kanalı)
    G = (A >> 8) & 0xFF;

    // Üçüncü 8 bit (En sağ - Mavi Kanalı)
    B = A & 0xFF;
}


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
            cout << "BMP dosyasi acilamadi!" << endl;
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

    // Resimdeki her pikseli alır, binary mantığıyla birleştirir ve ayırır.
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
                
                // .to_string() diyerek bunu metin olarak da alabilirsin.
                cout << "B (Kirmizi - Binary): " << binB << endl; 
                cout << "C (Yesil   - Binary): " << binC << endl;
                cout << "D (Mavi    - Binary): " << binD << endl;
                
                // Ayrıca A'nın tamamını (32 bit veya 24 bit) görmek istersen:
                cout << "A (Tum Paket Binary): " << bitset<24>(A) << endl;
            }*/
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

    resim.yukleBMP("output.bmp");

    // 3. Yüklenen resmin her pikselini binary mantığıyla ayır
    resim.TumResmiAnalizEt();

    return 0;
}