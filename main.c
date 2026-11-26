#include <stdio.h>

#define MAX 100

//Global degiskenler: Tum fonksiyonlarin ortak kullandigi sayaclar
int baglam = 0;
int son_surec = -1;
int satir_blok = 0;

//Yazan: [Serhat Deyirmenci] - Yeni algoritma oncesi sayaclari ve bitti dizisini sifirlar
void sifirla(int n, int bitti[]) {
    int i;
    baglam = 0;
    son_surec = -1;
    satir_blok = 0;

    for (i = 0; i < n; i++) {
        bitti[i] = 0;
    }
}

// Yazan: [Mustafa Samet Fistikcioglu] - Gantt semasina blok yazar ve baglam degisimini sayar 
void gantt_ekle(int no, int bas, int son) {
    /* static degisken: Sadece bu fonksiyonda gorunur ama program bitene kadar degerini korur */
    static int toplam_blok = 0;
    toplam_blok++;

    if (no == -1) {
        printf("[BOS %d-%d]", bas, son);
    } else {
        if (son_surec != -1 && son_surec != no) {
            baglam++;
        }
        son_surec = no;
        printf("[P%d %d-%d]", no, bas, son);
    }

    satir_blok++;
    if (satir_blok % 6 == 0) {
        printf("\n");
    } else {
        printf(" ");
    }
}

// Yazan: [Mustafa Samet Fistikcioglu] - Donus ve bekleme surelerini hesaplar, ortalamalari pointer ile dondurur 
void ortalamalar(int n, int gelis[], int cpu[], int bitis[], int donus[], int bekleme[], double *ort_bek, double *ort_don) {
    int i;
    double top_bek = 0;
    double top_don = 0;

    for (i = 0; i < n; i++) {
        donus[i] = bitis[i] - gelis[i];
        bekleme[i] = donus[i] - cpu[i];
        top_don = top_don + donus[i];
        top_bek = top_bek + bekleme[i];
    }

    *ort_bek = top_bek / n;
    *ort_don = top_don / n;
}

// Yazan: [Serhat Deyirmenci] - Sonuc tablosunu ve ozet bilgileri ekrana yazdirir 
void tablo_yazdir(char ad[], int n, int pid[], int gelis[], int cpu[], int bitis[], int donus[], int bekleme[], double ort_bek, double ort_don) {
    int i;

    if (satir_blok % 6 != 0) {
        printf("\n");
    }

    printf("\n--- %s SONUC TABLOSU ---\n", ad);
    printf("Surec\tGelis\tCPU\tBitis\tDonus\tBekleme\n");
    for (i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\n", pid[i], gelis[i], cpu[i], bitis[i], donus[i], bekleme[i]);
    }

    printf("\nOrtalama Bekleme: %.2f\n", ort_bek);
    printf("Ortalama Donus  : %.2f\n", ort_don);
    printf("Baglam Degisimi : %d\n\n", baglam);
}

// Yazan: [Serhat Deyirmenci] - FCFS algoritmasini calistirir 
void fcfs(int n, int pid[], int gelis[], int cpu[], double *ort_bek, double *ort_don) {
    int bitis[MAX], donus[MAX], bekleme[MAX], bitti[MAX];
    int sira[MAX];
    int i, j, gecici, saat = 0;

    sifirla(n, bitti);

    for (i = 0; i < n; i++) {
        sira[i] = i;
    }

    /* Gelis zamanina gore siralama (esitlikte pid kucuk olan once) */
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            int a = sira[j];
            int b = sira[j + 1];
            if (gelis[a] > gelis[b] || (gelis[a] == gelis[b] && pid[a] > pid[b])) {
                gecici = sira[j];
                sira[j] = sira[j + 1];
                sira[j + 1] = gecici;
            }
        }
    }

    printf("\n=== FCFS GANTT SEMASI ===\n");
    for (i = 0; i < n; i++) {
        int idx = sira[i];

        if (saat < gelis[idx]) {
            gantt_ekle(-1, saat, gelis[idx]);
            saat = gelis[idx];
        }

        gantt_ekle(pid[idx], saat, saat + cpu[idx]);
        saat = saat + cpu[idx];
        bitis[idx] = saat;
        bitti[idx] = 1;
    }

    ortalamalar(n, gelis, cpu, bitis, donus, bekleme, ort_bek, ort_don);
    tablo_yazdir("FCFS", n, pid, gelis, cpu, bitis, donus, bekleme, *ort_bek, *ort_don);
}

/* Yazan: [Mustafa Samet Fıstıkcıoglu] - Kesintisiz SJF algoritmasini calistirir */
void sjf(int n, int pid[], int gelis[], int cpu[], double *ort_bek, double *ort_don) {
    int bitis[MAX], donus[MAX], bekleme[MAX], bitti[MAX];
    int biten = 0, saat = 0, i;

    sifirla(n, bitti);

    printf("\n=== SJF GANTT SEMASI ===\n");
    while (biten < n) {
        int secilen = -1;

        for (i = 0; i < n; i++) {
            if (bitti[i] == 0 && gelis[i] <= saat) {
                if (secilen == -1) {
                    secilen = i;
                } else if (cpu[i] < cpu[secilen]) {
                    secilen = i;
                } else if (cpu[i] == cpu[secilen] && gelis[i] < gelis[secilen]) {
                    secilen = i;
                } else if (cpu[i] == cpu[secilen] && gelis[i] == gelis[secilen] && pid[i] < pid[secilen]) {
                    secilen = i;
                }
            }
        }

        if (secilen == -1) {
            int en_erken = -1;
            for (i = 0; i < n; i++) {
                if (bitti[i] == 0) {
                    if (en_erken == -1 || gelis[i] < en_erken) {
                        en_erken = gelis[i];
                    }
                }
            }
            gantt_ekle(-1, saat, en_erken);
            saat = en_erken;
        } else {
            gantt_ekle(pid[secilen], saat, saat + cpu[secilen]);
            saat = saat + cpu[secilen];
            bitis[secilen] = saat;
            bitti[secilen] = 1;
            biten++;
        }
    }

    ortalamalar(n, gelis, cpu, bitis, donus, bekleme, ort_bek, ort_don);
    tablo_yazdir("SJF", n, pid, gelis, cpu, bitis, donus, bekleme, *ort_bek, *ort_don);
}

/* Yazan: [Mustafa Samet Fıstıkcıoglu] - Tur tabanli Round Robin algoritmasini calistirir */
void rr(int n, int pid[], int gelis[], int cpu[], int quantum, double *ort_bek, double *ort_don) {
    int bitis[MAX], donus[MAX], bekleme[MAX], bitti[MAX], kalan[MAX];
    int biten = 0, saat = 0, i;

    sifirla(n, bitti);
    for (i = 0; i < n; i++) {
        kalan[i] = cpu[i];
    }

    printf("\n=== ROUND ROBIN GANTT SEMASI ===\n");
    while (biten < n) {
        int calisti = 0;

        for (i = 0; i < n; i++) {
            if (bitti[i] == 0 && gelis[i] <= saat) {
                int sure;
                if (kalan[i] < quantum) {
                    sure = kalan[i];
                } else {
                    sure = quantum;
                }

                gantt_ekle(pid[i], saat, saat + sure);
                saat = saat + sure;
                kalan[i] = kalan[i] - sure;
                calisti = 1;

                if (kalan[i] == 0) {
                    bitti[i] = 1;
                    bitis[i] = saat;
                    biten++;
                }
            }
        }

        if (calisti == 0 && biten < n) {
            int en_erken = -1;
            for (i = 0; i < n; i++) {
                if (bitti[i] == 0) {
                    if (en_erken == -1 || gelis[i] < en_erken) {
                        en_erken = gelis[i];
                    }
                }
            }
            gantt_ekle(-1, saat, en_erken);
            saat = en_erken;
        }
    }

    ortalamalar(n, gelis, cpu, bitis, donus, bekleme, ort_bek, ort_don);
    tablo_yazdir("ROUND ROBIN", n, pid, gelis, cpu, bitis, donus, bekleme, *ort_bek, *ort_don);
}

/* Yazan: [Ortak] - Uc algoritmanin ortalama bekleme surelerini karsilastirir */
void karsilastir(double fcfs_bek, double sjf_bek, double rr_bek) {
    printf("=== KARSILASTIRMA (ORTALAMA BEKLEME) ===\n");
    printf("FCFS: %.2f | SJF: %.2f | Round Robin: %.2f\n", fcfs_bek, sjf_bek, rr_bek);

    if (fcfs_bek <= sjf_bek && fcfs_bek <= rr_bek) {
        printf("En dusuk ortalama bekleme: FCFS (%.2f)\n", fcfs_bek);
    } else if (sjf_bek <= fcfs_bek && sjf_bek <= rr_bek) {
        printf("En dusuk ortalama bekleme: SJF (%.2f)\n", sjf_bek);
    } else {
        printf("En dusuk ortalama bekleme: Round Robin (%.2f)\n", rr_bek);
    }
}

// Yazan: [Ortak] - Verileri okur ve algoritmalari sirayla cagirir 
int main() {
    int n, quantum, i;
    int pid[MAX], gelis[MAX], cpu[MAX];
    double fcfs_bek, fcfs_don;
    double sjf_bek, sjf_don;
    double rr_bek, rr_don;

    printf("Surec sayisini giriniz: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        pid[i] = i + 1;
        printf("P%d gelis ve CPU suresini giriniz: ", pid[i]);
        scanf("%d %d", &gelis[i], &cpu[i]);
    }

    printf("Zaman dilimini (quantum) giriniz: ");
    scanf("%d", &quantum);

    fcfs(n, pid, gelis, cpu, &fcfs_bek, &fcfs_don);
    sjf(n, pid, gelis, cpu, &sjf_bek, &sjf_don);
    rr(n, pid, gelis, cpu, quantum, &rr_bek, &rr_don);

    karsilastir(fcfs_bek, sjf_bek, rr_bek);

    return 0;
}
