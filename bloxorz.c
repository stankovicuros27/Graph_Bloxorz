#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#define MAX 30

//Strukture

typedef struct objekat {
    int n1, m1;
    int n2, m2;
    enum stanje { ver = 1, hld = 2, hgd = 3 } stanje;
}objekat;

typedef struct polje {
    int n, m, ciljN, ciljM;
    char **mat;
}polje;

typedef struct cvor {
    int n, m, index;
}cvor;

typedef struct poseta {
    int n1, n2, m1, m2;
    int flag;
}poseta;

typedef struct graf {
    polje polje;
    objekat obj;
    int brEl;
    cvor *cvorovi;
    int **matsus;
}graf;

//Funkcije

void konzola() {
    puts("\n1. Pravljenje grafa");
    puts("2. Dodavanje cvorova");
    puts("3. Brisanje cvorova");
    puts("4. Ispisivanje terena igrice");
    puts("5. Ispisivanje matrice susednosti");
    puts("6. Pocetak igranja igrice");
    puts("7. Brisanje celog grafa");
    puts("8. Nadji resenje igrice");
    puts("0. Izlazak iz igrice");
    printf("\nUnesite komandu kako bi izvrsili zeljenu akciju: ");
}

FILE *ucitajFajl() {
    char ime[MAX];
    printf("Unesite ime fajla u kome se nalazi teren: ");
    scanf("%s", ime);
    FILE *file = fopen(ime, "r");
    if (!file) {
        perror(NULL);
        exit(1);
    }
    return file;
}

char** praviPolje(FILE* file, int *n, int *m, int *ciljN, int *ciljM) {
    
    char **mapa = NULL;
    (*n) = 0;
    (*m) = 0;
    char c;
    while (1) {
        
        mapa = realloc(mapa, ((*n) + 2) * sizeof(char*));
        mapa[(*n)] = NULL;
        mapa[(*n) + 1] = NULL;
        (*m) = 0;
        while ((c = fgetc(file)) != '\n') {
            if (c == EOF) return mapa;
            if (c == 'T') {
                (*ciljN) = (*n);
                (*ciljM) = (*m);
            }
            mapa[(*n)] = realloc(mapa[(*n)], ((*m) + 2) * sizeof(char));
            mapa[(*n)][(*m)] = c;
            mapa[(*n)][(*m) + 1] = '\0';
            (*m)++;
        }
        (*n)++;
    }
}

void ispisiPolje(char **mapa) {
    int i = 0;
    while (mapa[i]) {
        puts(mapa[i]);
        i++;
    }
}

void staviO(polje *p, objekat *obj) {
    
    if ((*obj).n2 != -1) {
        if ((*obj).m2 != -1) {
            (*p).mat[(*obj).n2][(*obj).m2] = 'o';
        }
    }
    (*p).mat[(*obj).n1][(*obj).m1] = 'o';
    
}

void staviX(polje *p, objekat *obj) {
    if ((*obj).n2 != -1) {
        if ((*obj).m2 != -1) {
            (*p).mat[(*obj).n2][(*obj).m2] = 'x';
        }
    }
    (*p).mat[(*obj).n1][(*obj).m1] = 'x';
}

void staviT(polje *p, objekat *obj) {
    
    if ((*p).mat[(*p).ciljN][(*p).ciljM] == 'o') {
        (*p).mat[(*p).ciljN][(*p).ciljM] = 'T';
    }
}


void pronadjiCvorove(graf *g) {
    
    (*g).cvorovi = NULL;
    int ind = 0;
    (*g).brEl = 0;
    
    int i = 0;
    while ((*g).polje.mat[i]) {
        int j = 0;
        while ((*g).polje.mat[i][j]) {
            if ((*g).polje.mat[i][j] != '=') {
                (*g).brEl++;
                (*g).cvorovi = realloc((*g).cvorovi, (ind + 2) * sizeof(cvor));
                (*g).cvorovi[ind].index = ind;
                (*g).cvorovi[ind].n = i;
                (*g).cvorovi[ind].m = j;
                ind++;
            }
            j++;
        }
        i++;
    }
    
}

int susedi(cvor a, cvor b) {
    if ((abs(a.n - b.n) == 1 && a.m == b.m) || (abs(a.m - b.m) == 1 && a.n == b.n)) {
        return 1;
    }
    return 0;
}


//PAZI OVDE!!!
void praviMatSus(graf *g) {
    
    int **mat = NULL;
    mat = malloc((*g).brEl * sizeof(int*));
    
    for (int i = 0; i < (*g).brEl; i++) {
        mat[i] = calloc((*g).brEl, sizeof(int));
        for (int j = 0; j < (*g).brEl; j++) {
            if (susedi((*g).cvorovi[i], (*g).cvorovi[j])) {
                mat[i][j] = 1;
            }
        }
    }
    (*g).matsus = mat;
}


//ogranici samo na krajeve polja i ubaci u niz cvorova!!!
void dodajCvor(graf *g) {
    
    int n, m;
    
    printf("Unesite n koordinatu cvora: ");
    scanf("%d", &n);
    printf("Unesite m koordinatu cvora: ");
    scanf("%d", &m);
    
    if (m > (*g).polje.m || m < 0 || n >(*g).polje.n || n < 0) {
        puts("Nekorektna vrednost koordinata!");
        return;
    }
    
    if ((*g).polje.mat[n][m] != '=') {
        puts("Cvor vec postoji!");
        return;
    }
    
    if ((*g).polje.mat[n][m] == '=') {
        (*g).polje.mat[n][m] = 'o';
        (*g).cvorovi = realloc((*g).cvorovi, ((*g).brEl + 2) * sizeof(cvor));
        (*g).cvorovi[(*g).brEl].index = (*g).brEl;
        (*g).cvorovi[(*g).brEl].n = n;
        (*g).cvorovi[(*g).brEl].m = m;
        (*g).brEl++;
        praviMatSus(g);
        pronadjiCvorove(g);
        puts("Dodali ste cvor!");
    }
}

void obrisiCvor(graf *g) {
    
    puts("Unesite n koordinatu cvora koji se brise: ");
    int n;
    scanf("%d", &n);
    puts("Unesite m koordinatu cvora koji se brise: ");
    int m;
    scanf("%d", &m);
    
    if ((*g).polje.mat[n][m] == '=') {
        puts("Cvor ne postoji!");
        return;
    }
    
    if ((*g).polje.mat[n][m] == 'x' || (*g).polje.mat[n][m] == 'T') {
        puts("Ne mozete obrisati ovaj cvor! (cilj ili pozicija objekta!)");
        return;
    }
    
    if ((*g).polje.mat[n][m] == 'o') {
        (*g).polje.mat[n][m] = '=';
        int i = 0;
        while ((*g).cvorovi[i].n != n || (*g).cvorovi[i].m != m) {
            i++;
        }
        for (int j = i; j < (*g).brEl - 2; j++) {
            (*g).cvorovi[j] = (*g).cvorovi[j + 1];
        }
        (*g).brEl--;
        (*g).cvorovi = realloc((*g).cvorovi, ((*g).brEl + 2) * sizeof(cvor));
        praviMatSus(g);
        pronadjiCvorove(g);
    }
    puts("Obrisali ste cvor!");
}

void ispisiMatSus(graf *g) {
    printf("\n    ");
    for (int i = 0; i < (*g).brEl; i++) {
        printf("%2d ", i);
    }
    printf("\n    ");
    
    for (int i = 0; i < (*g).brEl; i++) {
        printf("---");
    }
    
    putchar('\n');
    for (int i = 0; i < (*g).brEl; i++) {
        printf("%2d |", i);
        for (int j = 0; j < (*g).brEl; j++) {
            printf("%2d ", (*g).matsus[i][j]);
        }
        putchar('\n');
    }
}

void praviObjekat(graf *g) {
    
    int i = 0;
    while ((*g).polje.mat[i]) {
        int j = 0;
        while ((*g).polje.mat[i][j]) {
            if ((*g).polje.mat[i][j] == 'x') {
                (*g).obj.n1 = i;
                (*g).obj.m1 = j;
                (*g).obj.n2 = -1;
                (*g).obj.m2 = -1;
                (*g).obj.stanje = ver;
                return;
            }
            j++;
        }
        i++;
    }
}

void praviGraf(graf *g) {
    
    FILE *file = ucitajFajl();
    (*g).polje.mat = praviPolje(file, &(*g).polje.n, &(*g).polje.m, &(*g).polje.ciljN, &(*g).polje.ciljM);
    praviObjekat(g);
    pronadjiCvorove(g);
    praviMatSus(g);
    puts("Napravili ste graf!");
}

void obrisiGraf(graf *g) {
    for (int i = 0; i < (*g).polje.n; i++) {
        free((*g).polje.mat[i]);
    }
    free((*g).polje.mat);
    
    for (int i = 0; i < (g->brEl); i++) {
        free((*g).matsus[i]);
    }
    free((*g).matsus);
    free((*g).cvorovi);
    puts("\nObrisali ste graf!");
}

int idiGore(polje *p, objekat *obj, int *ziv) {
    if ((*obj).stanje == ver) {
        if ((*obj).n1 - 1 >= 0 && (*obj).n1 - 2 >= 0 && (*p).mat[(*obj).n1 - 1][(*obj).m1] != '=' && (*p).mat[(*obj).n1 - 2][(*obj).m1] != '=') {
            staviO(p, obj);
            (*obj).stanje = hgd;
            (*obj).n2 = (*obj).n1 - 1;
            (*obj).n1 = (*obj).n1 - 2;
            (*obj).m2 = (*obj).m1;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    
    
    if ((*obj).stanje == hgd) {
        if ((*obj).n1 - 1 >= 0 && (*p).mat[(*obj).n1 - 1][(*obj).m1] != '=') {
            staviO(p, obj);
            (*obj).stanje = ver;
            (*obj).n1 = (*obj).n1 - 1;
            (*obj).n2 = -1;
            (*obj).m2 = -1;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    
    if ((*obj).stanje == hld) {
        if ((*obj).n1 - 1 >= 0 && (*p).mat[(*obj).n1 - 1][(*obj).m1] != '=' && (*p).mat[(*obj).n2 - 1][(*obj).m2] != '=') {
            staviO(p, obj);
            (*obj).n1 = (*obj).n1 - 1;
            (*obj).n2 = (*obj).n2 - 1;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    return 1;
}

int idiDole(polje *p, objekat *obj, int *ziv) {
    if ((*obj).stanje == ver) {
        if ((*obj).n1 + 2 < (*p).n && (*obj).n1 + 1 < (*p).n && (*p).mat[(*obj).n1 + 1][(*obj).m1] != '=' && (*p).mat[(*obj).n1 + 2][(*obj).m1] != '=') {
            staviO(p, obj);
            (*obj).stanje = hgd;
            (*obj).n2 = (*obj).n1 + 2;
            (*obj).n1 = (*obj).n1 + 1;
            (*obj).m2 = (*obj).m1;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    
    if ((*obj).stanje == hgd) {
        if ((*obj).n2 + 1 < (*p).n && (*p).mat[(*obj).n2 + 1][(*obj).m1] != '=') {
            staviO(p, obj);
            (*obj).stanje = ver;
            (*obj).n1 = (*obj).n2 + 1;
            (*obj).n2 = -1;
            (*obj).m2 = -1;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    
    if ((*obj).stanje == hld) {
        if ((*obj).n1 + 1 < (*p).n && (*p).mat[(*obj).n1 + 1][(*obj).m1] != '=' && (*p).mat[(*obj).n2 + 1][(*obj).m2] != '=') {
            staviO(p, obj);
            (*obj).n1 = (*obj).n1 + 1;
            (*obj).n2 = (*obj).n2 + 1;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    return 1;
}

int idiLevo(polje *p, objekat *obj, int *ziv) {
    
    if ((*obj).stanje == ver) {
        if ((*obj).m1 - 2 >= 0 && (*obj).m1 - 1 >= 0 && (*p).mat[(*obj).n1][(*obj).m1 - 1] != '=' && (*p).mat[(*obj).n1][(*obj).m1 - 2] != '=') {
            staviO(p, obj);
            (*obj).stanje = hld;
            (*obj).m2 = (*obj).m1 - 1;
            (*obj).m1 = (*obj).m1 - 2;
            (*obj).n2 = (*obj).n1;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
        
    }
    
    if ((*obj).stanje == hgd) {
        if ((*obj).m1 - 1 >= 0 && (*p).mat[(*obj).n1][(*obj).m1 - 1] != '=' && (*p).mat[(*obj).n2][(*obj).m2 - 1] != '=') {
            staviO(p, obj);
            (*obj).m2--;
            (*obj).m1--;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    
    if ((*obj).stanje == hld) {
        if ((*obj).m1 - 1 >= 0 && (*p).mat[(*obj).n1][(*obj).m1 - 1] != '=') {
            staviO(p, obj);
            (*obj).stanje = ver;
            (*obj).m2 = -1;
            (*obj).n2 = -1;
            (*obj).m1--;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    return 1;
}

int idiDesno(polje *p, objekat *obj, int *ziv) {
    
    if ((*obj).stanje == ver) {
        if ((*obj).m1 + 1 < (*p).m && (*obj).m1 + 2 < (*p).m && ((*p).mat[(*obj).n1][(*obj).m1 + 1] != '=') && ((*p).mat[(*obj).n1][(*obj).m1 + 2] != '=')) {
            staviO(p, obj);
            (*obj).stanje = hld;
            (*obj).m2 = (*obj).m1 + 2;
            (*obj).m1 = (*obj).m1 + 1;
            (*obj).n2 = (*obj).n1;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    
    if ((*obj).stanje == hgd) {
        if ((*obj).m1 + 1 < (*p).m && (*p).mat[(*obj).n1][(*obj).m1 + 1] != '=' && (*p).mat[(*obj).n2][(*obj).m2 + 1] != '=') {
            staviO(p, obj);
            (*obj).m2++;
            (*obj).m1++;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    
    if ((*obj).stanje == hld) {
        if ((*obj).m1 + 1 < (*p).m && (*p).mat[(*obj).n1][(*obj).m2 + 1] != '=') {
            staviO(p, obj);
            (*obj).stanje = ver;
            (*obj).m1 = (*obj).m2 + 1;
            (*obj).m2 = -1;
            (*obj).n2 = -1;
            staviX(p, obj);
            return 1;
        }
        else {
            (*ziv) = 0;
            return 0;
        }
    }
    return 1;
}

int igrajIgricu(polje *p, objekat *obj) {
    int ziv = 1;
    puts("Krecite se po mapi w a s d komandama\n");
    char c;
    
    while (ziv) {
        
        getchar();
        ispisiPolje((*p).mat);
        
        puts("\nUnesite komandu: ");
        c = getchar();
        
        if (c == 'w') {
            idiGore(p, obj, &ziv);
        }
        if (c == 'a') {
            idiLevo(p, obj, &ziv);
        }
        if (c == 's') {
            idiDole(p, obj, &ziv);
        }
        if (c == 'd') {
            idiDesno(p, obj, &ziv);
        }
        
        if ((*obj).stanje == ver && (*obj).n1 == (*p).ciljN && (*obj).m1 == (*p).ciljM) {
            puts("PRESLI STE IGRICU! : ) : ) : )");
            return 1;
        }
    }
    puts("Izgubili ste : ( : ( : (");
    return 0;
}

int getIndex(graf *g, int n, int m) { // vraca indeks cvora prema n i m koordinati
    int i = 0;
    if (n == -1 && m == -1) {
        return (*g).brEl;
    }
    while ((*g).cvorovi[i].n != n || (*g).cvorovi[i].m != m) {
        i++;
    }
    return i;
}

int **napraviPosete(graf *g) {
    
    int **poseta;
    
    poseta = malloc(((*g).brEl + 1) * sizeof(int*));
    
    //poseta[brEl][index] - posecen samo jedan cvor!!! to u poslednjoj vrsti
    
    int j = 1;
    for (int i = 0; i <= (*g).brEl; i++) {
        poseta[i] = calloc(j, sizeof(int));
        j++;
    }
    return poseta;
    
}

int minx(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

int maxx(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int indexCilja(graf *g) {
    int cilj = getIndex(g, (*g).polje.ciljN, (*g).polje.ciljM);
    return cilj;
}

///RESENJE!!!!!!!


graf *klonirajGraf(graf *g) {
    
    graf *g2 = malloc(sizeof(graf));
    (*g2) = (*g);
    
    (*g2).polje.mat = malloc(((*g2).polje.n + 2) * sizeof(char*));
    
//    int i = 0;
//    while((*g).polje.mat[i]){
//        (*g2).polje.mat[i] = malloc((*g).polje.m * sizeof(char));
//        strcpy((*g2).polje.mat[i], (*g).polje.mat[i]);
//        i++;
//    }
    
    for (int i = 0; i <= (*g2).polje.n; i++) {
        (*g2).polje.mat[i] = malloc(((*g2).polje.m + 1) * sizeof(char));
        for (int j = 0; j <= (*g2).polje.m; j++) {
            (*g2).polje.mat[i][j] = (*g).polje.mat[i][j];
        }
    }
    (*g2).polje.mat[(*g2).polje.n + 1] = NULL;

    return g2;
    
}


int nadjiResenjeRek(graf *g2, int **poseta, int *nadjeno, char *potezi, int* brPoteza) {
    
    if ((*nadjeno) == 1) return 1;//mozda posle svakog poziva? pre?
    
    graf *g = klonirajGraf(g2);
    
    int ind1 = maxx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
    int ind2 = minx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
    poseta[ind1][ind2] = 1;
    
    char potezi2[120];
    strcpy(potezi2, potezi);
    
    int brPoteza2 = (*brPoteza);
    
    if ((*nadjeno) == 0 && (*g).obj.stanje == ver && (*g).obj.n1 == (*g).polje.ciljN && (*g).obj.m1 == (*g).polje.ciljM) {
        puts("Nadjeno resenje:");
        for (int i = 0; i < (*brPoteza); i++) {
            putchar(potezi[i]);
        }
        (*nadjeno) = 1;
        return 1;
    }
    
    int ziv = 1;
    
    if (idiDole(&(*g).polje, &(*g).obj, &ziv)) {
        
        (*brPoteza)++;
        //potezi = realloc(potezi, ((*brPoteza) + 1) * sizeof(char));
        potezi[(*brPoteza) - 1] = 's';
        
        int ind1 = maxx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
        int ind2 = minx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
        if (!poseta[ind1][ind2]) {
            nadjiResenjeRek(g, poseta, nadjeno, potezi, brPoteza);
        }
        
    }
    
    g = klonirajGraf(g2);
    (*brPoteza) = brPoteza2;
    potezi = strcpy(potezi, potezi2);
    
    if (idiLevo(&(*g).polje, &(*g).obj, &ziv)) {
        
        (*brPoteza)++;
        //potezi = realloc(potezi, ((*brPoteza) + 1) * sizeof(char));
        potezi[(*brPoteza) - 1] = 'a';
        
        int ind1 = maxx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
        int ind2 = minx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
        if (!poseta[ind1][ind2]) {
            nadjiResenjeRek(g, poseta, nadjeno, potezi, brPoteza);
        }
        
    }
    
    g = klonirajGraf(g2);
    (*brPoteza) = brPoteza2;
    potezi = strcpy(potezi, potezi2);
    
    if (idiDesno(&(*g).polje, &(*g).obj, &ziv)) {
        
        (*brPoteza)++;
        //potezi = realloc(potezi, ((*brPoteza) + 1) * sizeof(char));
        potezi[(*brPoteza) - 1] = 'd';
        
        int ind1 = maxx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
        int ind2 = minx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
        if (!poseta[ind1][ind2]) {
            nadjiResenjeRek(g, poseta, nadjeno, potezi, brPoteza);
        }
        
    }
    
    g = klonirajGraf(g2);
    (*brPoteza) = brPoteza2;
    potezi = strcpy(potezi, potezi2);
    
    if (idiGore(&(*g).polje, &(*g).obj, &ziv)) {
        
        (*brPoteza)++;
        //potezi = realloc(potezi, ((*brPoteza) + 1) * sizeof(char));
        potezi[(*brPoteza) - 1] = 'w';
        
        int ind1 = maxx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
        int ind2 = minx(getIndex(g, (*g).obj.n1, (*g).obj.m1), getIndex(g, (*g).obj.n2, (*g).obj.m2));
        if (!poseta[ind1][ind2]) {
            nadjiResenjeRek(g, poseta, nadjeno, potezi, brPoteza);
        }
    }
    
    g = klonirajGraf(g2);
    (*brPoteza) = brPoteza2;
    potezi = strcpy(potezi, potezi2);
    
    if((*nadjeno)) return 1;
    return 0;
    
}

void nadjiResenje(graf *g) {
    
    graf *g2 = klonirajGraf(g);
    int **poseta;
    poseta = napraviPosete(g2);
    int nadjeno = 0;
    char potezi[120];
    int brPoteza = 0;
    int r = nadjiResenjeRek(g2, poseta, &nadjeno, potezi, &brPoteza);
    //(*g) = (*g2);
    if (!r) puts("Nema resenja!");
}

//RESENJE

int main(void) {
    
    graf *g, *g2;
    g = malloc(sizeof(graf));
    int init = 0;
    
    while (1) {
        
        konzola();
        int q;
        scanf("%d", &q);
        
        switch (q) {
            case 1:
                if (init) {
                    puts("\nGraf je vec inicijalizovan!");
                    break;
                }
                praviGraf(g);
                init = 1;
                break;
            case 2:
                if (!init) {
                    puts("\nGraf nije inicijalizovan!");
                    break;
                }
                dodajCvor(g);
                break;
            case 3:
                if (!init) {
                    puts("\nGraf nije inicijalizovan!");
                    break;
                }
                obrisiCvor(g);
                break;
            case 4:
                if (!init) {
                    puts("\nGraf nije inicijalizovan!");
                    break;
                }
                ispisiPolje((*g).polje.mat);
                break;
            case 5:
                if (!init) {
                    puts("\nGraf nije inicijalizovan!");
                    break;
                }
                ispisiMatSus(g);
                break;
            case 6:
                if (!init) {
                    puts("\nGraf nije inicijalizovan!");
                    break;
                }
                g2 = klonirajGraf(g);
                igrajIgricu(&(*g2).polje, &(*g2).obj);
                free(g2);
                break;
            case 7:
                if (!init) {
                    puts("\nGraf nije inicijalizovan!");
                    break;
                }
                obrisiGraf(g);
                init = 0;
                break;
            case 8:
                if (!init) {
                    puts("\nGraf nije inicijalizovan!");
                    break;
                }
                g2 = klonirajGraf(g);
                nadjiResenje(g2);
                free(g2);
                break;
            case 0:
                free(g);
                puts("\n\nIZASLI STE CAO CAO : ) : ) : )");
                exit(1);
                break;
                
            default:
                puts("Nevazeca komanda!");
                break;
        }
    }
}
