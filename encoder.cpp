#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

#pragma pack(push, 1)
struct BMPHeader {
    uint16_t file_type{0x4D42};
    uint32_t file_size{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offset_data{54};
    uint32_t size{40};
    int32_t  width{0};
    int32_t  height{0};
    uint16_t planes{1};
    uint16_t bit_count{24};
    uint32_t compression{0};
    uint32_t size_image{0};
    int32_t  x_pixels_per_meter{0};
    int32_t  y_pixels_per_meter{0};
    uint32_t colors_used{0};
    uint32_t colors_important{0};
};
#pragma pack(pop)

class BMPEncoder {
private:
    BMPHeader header;
    vector<uint8_t> pixelData;
    int rowPadding;

public:
    BMPEncoder(int w, int h) {
        header.width = w;
        header.height = h;
        rowPadding = (4 - (w * 3) % 4) % 4;
        header.size_image = (w * 3 + rowPadding) * h;
        header.file_size = header.offset_data + header.size_image;
        pixelData.assign(header.size_image, 0); // Başlangıçta siyah resim
    }

    bool encodeMessage(string message) {
        message += '\0'; // Bitiş işareti
        if (message.length() * 8 > header.size_image) {
            return false;
        }

        int bitIdx = 0;
        for (char c : message) {
            for (int i = 0; i < 8; ++i) {
                int bit = (c >> i) & 1;
                pixelData[bitIdx] = (pixelData[bitIdx] & 0xFE) | bit; // padding baytlarını atlama kısmı
                bitIdx++;
            }
        }
        return true;
    }

    void save(const string& filename) {
        ofstream out(filename, ios::binary);
        if (out) {
            out.write(reinterpret_cast<char*>(&header), sizeof(header));
            out.write(reinterpret_cast<char*>(pixelData.data()), pixelData.size());
            out.close();
            cout << "Dosya basariyla olusturuldu: " << filename << endl;
        }
    }
};

int main() {
    string msg;
    cout << "Gizlenecek mesaji girin: ";
    getline(cin, msg);

    BMPEncoder encoder(400, 300);
    if (encoder.encodeMessage(msg)) {
        encoder.save("sifreli.bmp");
    } else {
        cout << "Hata: Mesaj resim boyutuna gore cok uzun!" << endl;
    }
    return 0;
}
