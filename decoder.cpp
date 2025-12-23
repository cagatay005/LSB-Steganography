#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t file_type;
    uint32_t file_size;
    uint16_t reserved1, reserved2;
    uint32_t offset_data;
    uint32_t size;
    int32_t  width, height;
    uint16_t planes, bit_count;
    uint32_t compression, size_image;
    int32_t  x_p_m, y_p_m;
    uint32_t c_used, c_imp;
};
#pragma pack(pop)

class BMPDecoder {
private:
    BMPHeader header;
    vector<uint8_t> pixelData;

public:
    bool load(const string& filename) {
        ifstream in(filename, ios::binary);
        if (!in) return false;

        in.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (header.file_type != 0x4D42) return false;

        pixelData.resize(header.size_image);
        in.seekg(header.offset_data, ios::beg);
        in.read(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
        in.close();
        return true;
    }

    // Mesajı çözerken aynı zamanda kullanılan pikselleri beyaza boyama (multithreading yaptığım kısım.)
    string decodeAndMark() {
        string message = "";
        char currentByte = 0;
        int bitCounter = 0;
        int totalBitsRead = 0;

        for (size_t i = 0; i < pixelData.size(); ++i) {
            int lsb = pixelData[i] & 1;
            currentByte |= (lsb << bitCounter);
            bitCounter++;
            totalBitsRead++;

            if (bitCounter == 8) {
                if (currentByte == '\0') {
                    // Mesaj bittiğinde, mesajın bittiği yere kadar olan tüm pikselleri beyaza boyadığım kısım
                    for (int j = 0; j < totalBitsRead; ++j) {
                        pixelData[j] = 255; 
                    }
                    break;
                }
                message += currentByte;
                currentByte = 0;
                bitCounter = 0;
            }
        }
        return message;
    }

    void saveMarked(const string& filename) {
        ofstream out(filename, ios::binary);
        if (out) {
            out.write(reinterpret_cast<char*>(&header), sizeof(header));
            out.write(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
            out.close();
            cout << "Mesajin konumunu gosteren resim kaydedildi: " << filename << endl;
        }
    }
};

int main() {
    BMPDecoder decoder;
    string dosya_adi;
    cout<<"Dosya adini giriniz(.bmp uzantisini yazmaya dikkat ediniz):";
    cin>>dosya_adi;
    if (decoder.load(dosya_adi)) {
        string result = decoder.decodeAndMark();
        cout << "Cozulen Mesaj: " << result << endl;
        decoder.saveMarked("kanit.bmp");
    } else {
        cout << "Hata: Dosya yuklenemedi!" << endl;
    }
    return 0;
}
