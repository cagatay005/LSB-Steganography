#include <iostream>
#include <fstream>
#include <bitset>
#include <cstring>

using namespace std;

class Color {
public:
    unsigned char r, g, b;
    Color(unsigned char rr = 0, unsigned char gg = 0, unsigned char bb = 0) {
        r = rr; g = gg; b = bb;
    }
};

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
        for (int i = height - 1; i >= 0; i--)
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

    void kaydetBMP(const char* filename) {
        // BMP dosya boyutu hesaplama (Header + Data)
        // Padding (satır sonu boşlukları) ihmal edilmiştir, 
        // standart 4'ün katı genişlik varsayıyoruz.
        int fileSize = 54 + 3 * width * height;

        unsigned char fileHeader[14] = {
            'B','M',
            (unsigned char)(fileSize),
            (unsigned char)(fileSize >> 8),
            (unsigned char)(fileSize >> 16),
            (unsigned char)(fileSize >> 24),
            0,0,0,0,
            54,0,0,0
        };

        unsigned char infoHeader[40] = {
            40,0,0,0,
            (unsigned char)(width),
            (unsigned char)(width >> 8),
            (unsigned char)(width >> 16),
            (unsigned char)(width >> 24),
            (unsigned char)(height),
            (unsigned char)(height >> 8),
            (unsigned char)(height >> 16),
            (unsigned char)(height >> 24),
            1,0,
            24,0,0,0,0,0,0,0,0,0,0,0,0,0
        };

        ofstream file(filename, ios::binary);
        file.write((char*)fileHeader, 14);
        file.write((char*)infoHeader, 40);

        for (int i = height - 1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                unsigned char pixel[3] = {
                    matrix[i][j].b,
                    matrix[i][j].g,
                    matrix[i][j].r
                };
                file.write((char*)pixel, 3);
            }
        }
        file.close();
        cout << "Resim (" << filename << ") olarak kaydedildi." << endl;
    }

    // --- DÜZELTİLMİŞ ŞİFRE YAZMA FONKSİYONU ---
    void Sifreyazma() {
        cout << "Sifre yazma islemi basladi." << endl;
        char Mesaj[1024];
        cout << "Lutfen sifrelenecek mesaji giriniz (max 1023 karakter): ";
        cin.getline(Mesaj, 1024);
        
        int mesajUzunlugu = strlen(Mesaj);
        
        // ÖNEMLİ: Mesajın bittiğini okuyucunun anlaması için NULL karakteri (\0) de yazmalıyız.
        Mesaj[mesajUzunlugu] = '\0'; 
        mesajUzunlugu++; // Uzunluğu 1 artırıyoruz ki \0 da işlensin.

        int charIndex = 0; // Hangi harfdeyiz?
        int bitIndex = 0;  // O harfin kaçıncı bitindeyiz?

        // Okuma kodundaki döngü sırasının aynısı: (height-1 -> 0)
        for (int i = height - 1; i >= 0; i--) {
            for (int j = 0; j < width; j++) {
                
                // Okuma kodun önce B, sonra G, sonra R okuyor.
                // Biz de tam bu sırayla yazmalıyız.
                // Kolaylık olsun diye renklerin adreslerini bir diziye alıyoruz.
                unsigned char* renkKanallari[3] = { 
                    &matrix[i][j].b, 
                    &matrix[i][j].g, 
                    &matrix[i][j].r 
                };

                for (int k = 0; k < 3; k++) {
                    // Eğer mesaj bittiyse işlemden çık
                    if (charIndex >= mesajUzunlugu) {
                        cout << "Sifreleme tamamlandi!" << endl;
                        return;
                    }

                    // Yazılacak harfin ilgili bitini al (0 veya 1)
                    unsigned char bit = (Mesaj[charIndex] >> bitIndex) & 1;

                    // İlgili renk kanalının son bitini temizle ve mesaj bitini koy
                    // & 0xFE -> Son biti 0 yapar (11111110)
                    // | bit  -> Son bite bizim bitimizi koyar
                    *renkKanallari[k] = (*renkKanallari[k] & 0xFE) | bit;

                    bitIndex++; // Bir sonraki bite geç

                    // Eğer 8 bit yazıldıysa (bir harf bittiyse)
                    if (bitIndex == 8) {
                        bitIndex = 0;   // Bit sayacını sıfırla
                        charIndex++;    // Bir sonraki harfe geç
                    }
                }
            }
        }
        cout << "Uyari: Mesaj resmin kapasitesinden uzun olabilir, tamami yazilamadi." << endl;
    }
    
    // Yıkıcı fonksiyon (Bellek temizliği)
    ~Image() {
        for (int i = 0; i < height; i++)
            delete[] matrix[i];
        delete[] matrix;
    }
};

int main() {
    // Okuyacağın kaynak resim (Boyutları resme göre ayarla)
    Image resim(400, 300); 

    resim.yukleBMP("output1.bmp");

    // Gizli mesajı yaz
    resim.Sifreyazma();

    // Şifreli halini yeni dosyaya kaydet
    resim.kaydetBMP("sifreli_yeni.bmp");
    
    return 0;
}