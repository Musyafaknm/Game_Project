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
    string isim;
    int boyut, vuruslar;
    vector<pair<int, int>> konumlar;

    Gemi(string i, int b) : isim(i), boyut(b), vuruslar(0) {}
    bool battiMi() { return vuruslar >= boyut; }
};

// Harita sınıfı: Bu sınıf, oyun alanını ve gemilerin yerleşimini yönetir.
class Harita
{
private:
    vector<vector<char>> harita;
    vector<Gemi> gemiler;

public:
    Harita() { harita.resize(5, vector<char>(5, '~')); } // 5x5 boyutlu harita oluşturulur.

    // Gemiyi haritaya yerleştirir.
    void gemiYerlestir(Gemi gemi)
    {
        while (true)
        {
            int satir = rand() % 5, sutun = rand() % 5;
            bool yatay = rand() % 2;
            if (yerlestirilebilirMi(gemi.boyut, satir, sutun, yatay))
            {
                for (int i = 0; i < gemi.boyut; i++)
                {
                    int s = satir + (yatay ? 0 : i), k = sutun + (yatay ? i : 0);
                    harita[s][k] = 'G';
                    gemi.konumlar.push_back({s, k});
                }
                gemiler.push_back(gemi);
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
        if (harita[satir][sutun] == 'G')
        {
            harita[satir][sutun] = 'X';
            for (auto &gemi : gemiler)
                for (auto &konum : gemi.konumlar)
                    if (konum.first == satir && konum.second == sutun)
                        if (++gemi.vuruslar == gemi.boyut)
                            cout << gemi.isim << " batti!\n";
            return true;
        }
        harita[satir][sutun] = 'O';
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
    Harita oyuncuHarita, bilgisayarHarita;
    set<pair<int, int>> bilgisayarHamleleri;
    vector<pair<int, int>> hedefler;

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
            oyuncuHarita.goster();
            cout << "\nDusman Haritasi:\n";
            bilgisayarHarita.goster(false);

            // Oyuncunun saldırısı
            int satir, sutun;
            while (true)
            {
                cout << "Saldiri koordinatlarini gir (Orn: A2): ";
                char sutunHarf;
                cin >> sutunHarf >> satir;
                sutun = sutunHarf - 'A';
                if (sutun >= 0 && sutun < 5 && satir >= 0 && satir < 5)
                    break;
                cout << "Gecersiz giris Tekrar deneyin.\n";
            }
            if (bilgisayarHarita.saldir(satir, sutun))
                cout << "Vuruldu!\n";
            else
                cout << "Iska gecti!\n";

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