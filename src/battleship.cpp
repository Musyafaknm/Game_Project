#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <set>
using namespace std;

// Gemi sınıfı: Bu sınıf, oyun içindeki gemilerin özelliklerini ve davranışlarını temsil eder.
class Gemi
{
public:
    string isim;                     // Geminin adı
    int boyut, vuruslar;             // Geminin boyutu ve aldığı vuruş sayısı
    vector<pair<int, int>> konumlar; // Geminin bulunduğu konumlar

    Gemi(string i, int b) : isim(i), boyut(b), vuruslar(0) {} // Gemi özellikleri başlatılır.
    bool battiMi() { return vuruslar >= boyut; }              // Gemi tamamen battı mı?
};

// Harita sınıfı: Bu sınıf, oyun alanını ve gemilerin yerleşimini yönetir.
class Harita
{
private:
    vector<vector<char>> harita; // Oyun haritası ('~': boş, 'G': gemi, 'X': vurulan, 'O': ıskalanan)
    vector<Gemi> gemiler;        // Haritadaki gemiler

public:
    Harita() { harita.resize(5, vector<char>(5, '~')); } // 5x5 boyutlu harita oluşturulur.

    // Gemiyi haritaya yerleştirir.
    void gemiYerlestir(Gemi gemi)
    {
        while (true)
        {
            int satir = rand() % 5, sutun = rand() % 5; // Rastgele bir başlangıç noktası seçilir.
            bool yatay = rand() % 2;                    // Gemi yatay mı dikey mi yerleşecek?
            if (yerlestirilebilirMi(gemi.boyut, satir, sutun, yatay))
            {
                for (int i = 0; i < gemi.boyut; i++)
                {
                    int s = satir + (yatay ? 0 : i), k = sutun + (yatay ? i : 0);
                    harita[s][k] = 'G'; // Gemi konumlandırılır.
                    gemi.konumlar.push_back({s, k});
                }
                gemiler.push_back(gemi); // Gemi haritaya eklenir.
                break;
            }
        }
    }

    // Geminin belirli bir konuma yerleşip yerleşemeyeceğini kontrol eder.
    bool yerlestirilebilirMi(int boyut, int satir, int sutun, bool yatay)
    {
        for (int i = 0; i < boyut; i++)
        {
            int s = satir + (yatay ? 0 : i), k = sutun + (yatay ? i : 0);
            if (s >= 5 || k >= 5 || harita[s][k] != '~') // Harita sınırlarının dışına çıkmamalı ve dolu olmamalı.
                return false;
        }
        return true;
    }

    // Belirtilen koordinata saldırır ve sonucu döndürür.
    bool saldir(int satir, int sutun)
    {
        if (harita[satir][sutun] == 'G') // Eğer gemi vurulduysa
        {
            harita[satir][sutun] = 'X'; // Vurulan nokta işaretlenir.
            for (auto &gemi : gemiler)
                for (auto &konum : gemi.konumlar)
                    if (konum.first == satir && konum.second == sutun)
                        if (++gemi.vuruslar == gemi.boyut) // Eğer gemi tamamen batmışsa
                            cout << gemi.isim << " batti!\n";
            return true;
        }
        harita[satir][sutun] = 'O'; // Eğer ıskalanmışsa
        return false;
    }

    // Haritayı gösterir. Gemiler gizlenebilir.
    void goster(bool gemilerGorunsun = false)
    {
        cout << "  A B C D E\n"; // Harita sütunları harflerle temsil edilir.
        for (int i = 0; i < 5; i++)
        {
            cout << i << " "; // Harita satırları numaralarla temsil edilir.
            for (int j = 0; j < 5; j++)
            {
                cout << (harita[i][j] == 'G' && !gemilerGorunsun ? '~' : harita[i][j]) << " ";
            }
            cout << endl;
        }
    }

    // Tüm gemilerin batıp batmadığını kontrol eder.
    bool tumGemilerBattiMi()
    {
        for (auto &gemi : gemiler)
            if (!gemi.battiMi()) // Eğer bir gemi hala batmadıysa
                return false;
        return true;
    }
};

// Oyun sınıfı: Oyun akışını yönetir.
class Oyun
{
private:
    Harita oyuncuHarita, bilgisayarHarita;   // Oyuncu ve bilgisayarın haritaları
    set<pair<int, int>> bilgisayarHamleleri; // Bilgisayarın yaptığı hamleler
    vector<pair<int, int>> hedefler;         // Bilgisayarın yeni hedefleri

public:
    // Oyunu başlatmadan önce gemileri yerleştirir.
    void kurulum()
    {
        vector<Gemi> gemiler = {Gemi("Savas Gemisi", 3), Gemi("Denizalti", 2)}; // Oyundaki gemiler tanımlanır.
        for (auto &gemi : gemiler)
        {
            oyuncuHarita.gemiYerlestir(gemi);     // Gemiler oyuncu haritasına yerleştirilir.
            bilgisayarHarita.gemiYerlestir(gemi); // Gemiler bilgisayar haritasına yerleştirilir.
        }
    }

    // Oyunun ana döngüsü.
    bool oyna()
    {
        while (true)
        {
            cout << "\nSenin Haritan:\n";
            oyuncuHarita.goster(); // Oyuncunun haritası gösterilir.
            cout << "\nDusman Haritasi:\n";
            bilgisayarHarita.goster(false); // Düşmanın haritası gösterilir (gizli).

            // Oyuncunun saldırısı
            int satir, sutun;
            while (true)
            {
                cout << "Saldiri koordinatlarini gir (Orn: A2): ";
                char sutunHarf;
                cin >> sutunHarf >> satir; // Kullanıcıdan koordinat alınır.
                sutun = sutunHarf - 'A';
                if (sutun >= 0 && sutun < 5 && satir >= 0 && satir < 5) // Geçerli bir koordinat mı?
                    break;
                cout << "Gecersiz giris Tekrar deneyin.\n";
            }
            if (bilgisayarHarita.saldir(satir, sutun))
                cout << "Vuruldu!\n"; // Eğer gemi vurulduysa
            else
                cout << "Iska gecti!\n"; // Eğer ıskalandıysa

            if (bilgisayarHarita.tumGemilerBattiMi())
            {
                cout << "Tebrikler Tum dusman gemilerini batirdiniz Kazandiniz!\n";
                return true;
            }

            // Bilgisayarın saldırısı
            int bs, bk;
            if (!hedefler.empty())
            {
                bs = hedefler.back().first;
                bk = hedefler.back().second;
                hedefler.pop_back();
            }
            else
            {
                do
                {
                    bs = rand() % 5;
                    bk = rand() % 5;
                } while (bilgisayarHamleleri.count({bs, bk}));
            }
            bilgisayarHamleleri.insert({bs, bk});
            cout << "Bilgisayar saldiriyor: " << char('A' + bk) << bs << endl;
            if (oyuncuHarita.saldir(bs, bk))
            {
                cout << "Bilgisayar geminizi vurdu!\n";
                if (bs > 0)
                    hedefler.push_back({bs - 1, bk});
                if (bs < 4)
                    hedefler.push_back({bs + 1, bk});
                if (bk > 0)
                    hedefler.push_back({bs, bk - 1});
                if (bk < 4)
                    hedefler.push_back({bs, bk + 1});
            }
            else
            {
                cout << "Bilgisayar iskaladi!\n";
            }

            if (oyuncuHarita.tumGemilerBattiMi())
            {
                cout << "Kaybettiniz Bilgisayar tum gemilerinizi batirdi.\n";
                return false;
            }
        }
    }
};

// Ana program: Oyunu başlatır.
int main()
{
    srand(time(0));
    string oyuncuIsim;
    cout << "Oyuncu adini gir: ";
    cin >> oyuncuIsim;

    while (true)
    {
        Oyun oyun;
        oyun.kurulum();
        oyun.oyna();

        char tekrar;
        cout << "\nTekrar oynamak ister misiniz? (E/H): ";
        cin >> tekrar;
        if (tekrar == 'H' || tekrar == 'h')
        {
            cout << "Oyun sona erdi Hosca kalin!\n";
            break;
        }
    }
    return 0;
}