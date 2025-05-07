#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
// verificare OS (win/linux-macos)
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif
#define LEN_CUV 100 //lungime maxima cuvant
#define Lista "Lista_Cuvinte.txt"
#define ROUNDS 5 //numar runde
#define LDB "Leaderboard.txt"
#define MAXplayers 1000

//culori
#define RESET       "\033[0m"
#define BOLD        "\033[1m"
#define BLACK       "\033[0;30m"
#define RED         "\033[0;31m"
#define GREEN       "\033[0;32m"
#define YELLOW      "\033[0;33m"
#define BLUE        "\033[0;34m"
#define MAGENTA     "\033[0;35m"
#define CYAN        "\033[0;36m"
#define WHITE       "\033[0;37m"
#define BRED        "\033[1;31m"
#define BGREEN      "\033[1;32m"
#define BYELLOW     "\033[1;33m"
#define BBLUE       "\033[1;34m"
#define BMAGENTA    "\033[1;35m"
#define BCYAN       "\033[1;36m"
#define BWHITE      "\033[1;37m"

//sleep function
void pause(int time) {
#ifdef _WIN64
    Sleep(time * 1000);
#else

    usleep(time * 1000000);
#endif
}

//asteapta enter pentru revenire la meniu
void user_input() {
    printf("\nApasa ");
    printf(BRED"ENTER"RESET);
    printf(" pentru a reveni la meniu...");
    fflush(stdout);

    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}


//afisare titlu
void first_open() {
    int val = 50;
    for (int i = 0; i < val; ++i)
        printf(" ");
    printf(BRED "SPANZURATOAREA" RESET);
    for (int i = 0; i < val; ++i)
        printf(" ");
    printf("\n");
}

typedef struct word{
    char cuv[LEN_CUV];
    int len;
}word;

typedef struct player {
    char *nume;
    int scor;
}player;

typedef struct game {
    player *jucator;
    char **cuvinte;
    int wordCount;
}game;

player *createPlayer() {
    player *p = (player*) malloc(sizeof(player));
    if (p) {
        p->scor = -1;
    }
    return p;
}
word **createNewWordList() {
    word **lista_cuv = (word **) malloc(sizeof(word*) * ROUNDS);
    return lista_cuv;
}

game *createNewGame() {
    game *new = (game*) malloc(sizeof(game));

    new->jucator = createPlayer();
    new->cuvinte = (char**)malloc(sizeof(char*) * ROUNDS);
    new->wordCount = 0;
    return new;
}


//numara cuv lista
int number_of_words() {
    FILE *list = fopen(Lista, "r");
    if (list == NULL) return 1;
    char cuv[LEN_CUV];
    int rank = 0;
    while (fscanf(list, "%s", cuv  ) != EOF) {
        rank++;
    }
    fclose(list);
    return rank;
}

//verificare indice pentru cuvinte din load_rounds()
int exists(int v[], int n, int val, int *pos) {
    for (int i = 0; i < n; i++)
        if (val == v[i]) {
            *pos = i;
            return 1;
        }
    return 0;
}

void swap(int *a, int *b) {
    int aux = *a;
    *a = *b;
    *b = aux;
}

//amestecam indicii intr un vector
void chooseRandomWords(int *a) {

    srand(time(NULL));
    int nr_cuvinte = number_of_words();
    for (int i = 0; i < nr_cuvinte; i++)
        a[i] = i;
    for (int i = nr_cuvinte - 1; i >= 0; i--) {
        int random_pos = rand() % (i + 1);
        swap(a+i, a+random_pos);
    }
}

word *addToWordList( char *cuv) {

        word *cuvant = (word*) malloc(sizeof(word));
        strcpy(cuvant->cuv, cuv);
        cuvant->len = strlen(cuv);
        return cuvant;

}
//alegere cuvinte din lista pt newgame
void load_rounds(word **list){

    FILE  *f = fopen(Lista,"r");
    if (!f) {
        perror(RED "Eroare!"RESET);
        perror(" Nu se poate deschide fisierul!");
        return;
    }

    int len = number_of_words();
    if (len < ROUNDS) {
        printf(RED"Eroare! "RESET);
        printf("Fisierul nu contine destule cuvinte!\n");
        fclose(f);
        return;
    }
    int *indiciCuvinte = (int*)malloc(sizeof(int) * len);
    chooseRandomWords(indiciCuvinte);


    char cuv_fisier[LEN_CUV]; //cuvant din fisier care va fi citit
    int cnt = 0, selected = 0;
    while (fscanf(f, "%s", cuv_fisier) != EOF) {
        for (int i = 0; i < ROUNDS; i++) {
            if (indiciCuvinte[i] == cnt)//verif daca pos cuv din fisier = indice cuv ales random
            {
                list[selected] = addToWordList(cuv_fisier);
                selected++;
                break;
            }
        }
        cnt++; //trecem la urmatorul cuvant
    }
    fclose(f);
    free(indiciCuvinte);
}

//verifica daca s-au ales cuvintele bine => folosit pt test
void new_game(word **list) {
    system("cls");
    for (int i = 0; i < ROUNDS; i++)
            printf("%s %d\n", list[i]->cuv, list[i]->len);
}

//pune '_' in cuvant
char *transformare_cuvant_runda(char word[]) {

    char *aux = (char *)malloc(sizeof(char) * (strlen(word) + 1));
    strcpy(aux, word);
    char first = aux[0], last = aux[strlen(aux) - 1];
    for (int i = 0; aux[i]; i++)
        if (aux[i] != first && aux[i] != last)
            aux[i] = '_';
   return aux;
}

//adauga litera la incomplete_word
void adaugare_litera_corecta(char word[], char l, char *aux) {

    for (int i = 0; word[i]; i++)
        if (word[i] == l)
            aux[i] = l;
}

//verifica daca cuvantul a fost ghicit
int guessed(char aux[]) {
    int cnt = 0;
    for (int i = 0; aux[i]; i++)
        if (aux[i] == '_')
            cnt++;
    return (cnt == 0) ? 1:0;
}
char *adaugaCuvant(char *cuv) {
    char *s = (char*) malloc(sizeof(char) * (strlen(cuv) + 1));
    strcpy(s, cuv);
    return s;
}
void writeToHistory(game *game) {
    FILE *f = fopen("history.txt", "a+");
    if (!f) {
        perror("Eroare la deschiderea fisierului history.txt");
        return;
    }

    fprintf(f, "%s,%d", game->jucator->nume, game->jucator->scor);
    for (int i = 0; i < game->wordCount; i++) {
        fprintf(f, ",%s", game->cuvinte[i]);
    }
    fprintf(f, "\n");

    fclose(f);
}

void storeData(game *game, int score) {
    printf("Introduceti");
    printf(BGREEN" numele: \n"RESET);


    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);

    char aux[1000];
    fgets(aux, 1000, stdin);

    aux[strcspn(aux, "\n")] = '\0'; // eliminare '\n' din nume
    game->jucator->nume = adaugaCuvant(aux);
    game->jucator->scor = score;
}


//verifica daca exista litera l in cuvant
int checkLetterInWord(char cuvant[], char l){
    char word[100];
    strcpy(word, cuvant);
    strcpy(word, word + 1);
    word[strlen(word) - 1] = '\0';

    return (strchr(word, l) != NULL)? 1:0;
}


int verificare_reintroducere_litera(char hidden_word[], char c) {
    for (int i = 0; hidden_word[i]; i++)
        if (c == hidden_word[i])
            return 1;
    return 0;
}
int countLDB() {
    FILE  *f = fopen(LDB,"r");
    if (!f) {
        perror(RED "Eroare!"RESET);
        perror(" Nu se poate deschide fisierul!");
        return -1;
    }
    char cuv[LEN_CUV];
    int cnt = 0;
    while (fgets(cuv, LEN_CUV, f)) {
        cnt++;
    }
    fclose(f);
    return cnt;
}
int comparaScoruriDesc(const void *a, const void *b) {
    player *p1 = *(player **)a;
    player *p2 = *(player **)b;
    return p2->scor - p1->scor;
}

void sortLDB() {
    int length = countLDB();
    FILE  *f = fopen(LDB,"r");
    if (!f) {
        perror(RED "Eroare!"RESET);
        perror(" Nu se poate deschide fisierul!");
        return ;
    }


    player **jucatori = (player **) malloc(sizeof(player*) * length);
    int cnt = 0;
    char linie[1000];
    while (fgets(linie, 1000, f)) {
        char *tok = strtok(linie, ",");
        jucatori[cnt] = (player*)malloc(sizeof(player));
        jucatori[cnt]->nume = (char *) malloc(sizeof(char) * (strlen(tok) + 1));
        strcpy(jucatori[cnt]->nume, tok);
        tok = strtok(NULL, ",");
        jucatori[cnt]->scor = atoi(tok);
        cnt++;
    }
    fclose(f);

    qsort(jucatori, length, sizeof(player*), comparaScoruriDesc);

      f = fopen(LDB,"w");
    if (!f) {
        perror(RED "Eroare!"RESET);
        perror(" Nu se poate deschide fisierul!");
        return ;
    }
    for (int i = 0; i < length; i++) {
        fprintf(f, "%s,%d\n", jucatori[i]->nume, jucatori[i]->scor);
        free(jucatori[i]->nume);
        free(jucatori[i]);
    }
    free(jucatori);
    fclose(f);
}
void writeToLDB(player *p) {
    FILE  *f = fopen(LDB,"a+");
    if (!f) {
        perror(RED "Eroare!"RESET);
        perror(" Nu se poate deschide fisierul!");
        return ;
    }
    fprintf(f, "%s,%d\n", p->nume, p->scor);
    fclose(f);
    sortLDB();
}

void start_rounds(word **list, game *game) {

    int scor = 0;
    int count = 0;
    for (int i = 0; i < ROUNDS; i++) {


        char *incompl_word = transformare_cuvant_runda(list[i]->cuv); //cuvant cu '_'

        char c;
        int greseli = 0;
        while (greseli < 5) {
            system("cls");
            printf(BOLD"%s\n"RESET, incompl_word);
            printf(BWHITE"LITERA: \n"RESET);
            scanf(" %c", &c);

            if (!isalpha(c)) {
                system("cls");
                printf("NU ATI INTRODUS O");
                printf(BBLUE" LITERA"RESET);
                printf("! Reincercati...\n");
                pause(2);
                system("cls");
            }
            else if (verificare_reintroducere_litera(incompl_word, c)) {
                system("cls");
                printf("LITERA ");
                printf(RED"EXISTA"RESET);
                printf(" DEJA IN CUVANT! Reincercati...\n");
                pause(1);
                system("cls");
            }
            else if (checkLetterInWord(list[i]->cuv, c)) {

               adaugare_litera_corecta(list[i]->cuv, c, incompl_word);

                system("cls");
                printf("%s\n", incompl_word);

                //a ghicit cuvantul
                if (guessed(incompl_word) == 1) {


                    scor += list[i]->len; //creste scor

                    game->cuvinte[count++] = adaugaCuvant(list[i]->cuv);
                    game->wordCount++;
                    system("cls");
                    printf("Cuvant Finalizat cu ");
                    printf(BGREEN"Succes!"RESET);
                    printf("\nAti trecut la nivelul urmator!\n");
                    pause(2);
                    break;
                }
            }
            else
                greseli++;
        }
        system("cls");
        if (greseli == 5) {

            system("cls");
            printf(BRED"ATI PIERDUT!"RESET);
            printf("\nScor acumulat: ");
            printf(BGREEN"%d\n"RESET, scor);
           // printf("Introduceti numele:\n");
            storeData(game, scor);
            writeToLDB(game->jucator);
            writeToHistory(game);
            pause(2);
            return;
        }
    }
    system("cls");
    printf(BGREEN"Felicitari!"RESET);
    printf(" Ati ");
    printf(BGREEN"castigat"RESET);
    printf(" acest joc!\n");

    storeData(game, scor);
    writeToHistory(game);
    writeToLDB(game->jucator);
}



void optiune_1() {
    system("cls");
    printf(BRED"-JOC NOU-\n"RESET);
    pause(1);

    //cream o lista de cuvinte noua
    word **lista_cuv = createNewWordList();

    //cream istoric joc
    game *new_game = createNewGame();

    //initializam cuvintele din jocul curent
    load_rounds(lista_cuv);

    //start joc
    start_rounds(lista_cuv, new_game);

    //exit
    user_input();

}

void optiune_2() {
    system("cls");
    printf("-VIZUALIZARE ");
    printf(YELLOW"CLASAMENT-\n"RESET);
    char leaderb[LEN_CUV];
    FILE *f = fopen(LDB, "r"); //fisier cuvinte

    if (f == NULL) {
        printf("Nu se poate citi fisierul!\nReincercati!");
        user_input();
    }
    while (fgets(leaderb, sizeof(leaderb), f) != NULL) {
        char *tok = strtok(leaderb, ",");
        printf("Jucator: ");
        printf(BYELLOW"%s"RESET, tok);

        tok = strtok(NULL, ",");
        int scor = atoi(tok);
        printf(" ~ Scor: ");
        printf(BRED"%d\n"RESET, scor);

    }
    fclose(f);
    user_input();
}
int countHIST() {
    FILE  *f = fopen("history.txt","r");
    if (!f) {
        perror(RED "Eroare!"RESET);
        perror(" Nu se poate deschide fisierul!");
        return -1;
    }
    char cuv[LEN_CUV];
    int cnt = 0;
    while (fgets(cuv, LEN_CUV, f)) {
        cnt++;
    }
    fclose(f);
    return cnt;
}
game **loadHistory() {
    int entries = countHIST();
    game **runda = (game**)malloc(sizeof(game*) * entries);
    FILE *f = fopen("history.txt", "r");
    if (!f) {
        perror("Nu se poate deschide history.txt");
        return NULL;
    }
    char linie[1000];
    int cnt = 0;
    while (fgets(linie, 1000, f)) {
        char *tok = strtok(linie, ",");
        runda[cnt] = createNewGame();
        runda[cnt]->jucator->nume = (char*) malloc(sizeof(char) * (strlen(tok) + 1));
        strcpy(runda[cnt]->jucator->nume, tok);

        tok = strtok(NULL, ",");
        runda[cnt]->jucator->scor = atoi(tok);

        int count = 0;

        tok = strtok(NULL, ",");
        while (tok) {
            runda[cnt]->cuvinte[count] = (char*) malloc(sizeof(char) * (strlen(tok) + 1));
            strcpy(runda[cnt]->cuvinte[count++], tok);
            tok = strtok(NULL, ",");
        }
        runda[cnt]->wordCount = count;
        cnt++;
    }
    return runda;
}
void afisareDetalii(game *g) {

    printf(BWHITE"NUME: "RESET);
    printf(BMAGENTA"%s\n"RESET, g->jucator->nume);

    printf(BWHITE"SCOR: "RESET);
    printf(BMAGENTA"%d\n"RESET, g->jucator->scor);

    if (g->wordCount) {
    printf(BWHITE"Cuvintele ghicite: \n"RESET);
    for (int i = 0; i < g->wordCount; i++)
        printf(BBLUE"%s\n"RESET, g->cuvinte[i]);
    }
    else
        printf(BRED"NU"RESET);
        printf(BWHITE" exista cuvinte ghicite!"RESET);

}
//gresit, nu e folosit
void optiune_3()
{
    int entries = countHIST();
    system("cls");
    printf("-CAUTARE ");
    printf(BMAGENTA"RUNDA"RESET);
    printf("-\n");

    printf("Introduceti ");
    printf(BGREEN"numele "RESET);
    printf("pe care il cautati: (<nume_jucator>, doar cu litere, fara alte caractere)\n");


    char name[100];
    fgets(name, 100, stdin);
    name[strlen(name) - 1] = '\0';
    pause(1);

    system("cls");
    game **runda = loadHistory();
    int found = 0;
    for (int i = 0; i < entries; i++)
        if (strcmp(runda[i]->jucator->nume, name) == 0) {
            afisareDetalii(runda[i]);
            found = 1;
            break;
        }
    if (found == 0) {
        printf("Jucatorul cu numele ");
        printf(BRED"%s "RESET, name);
        printf("nu a fost gasit!");
    }

    printf("\n\n\n");

    user_input();
}

void citire_afisare_cuvinte(FILE *fisier) {
    char cuv[LEN_CUV];
    int i = 0;
    while (fscanf(fisier, "%s", cuv) != EOF) {
        printf("%d - %s\n", ++i, cuv);
    }

}
void optiune_4() {
    //vizualizare lista cuvinte
    system("cls");
    printf("-VIZUALIZARE ");
    printf(BBLUE"LISTA CUVINTE"RESET);
    printf("-\n");
    FILE *list = fopen(Lista, "r"); //fisier cuvinte

    if (list == NULL) {
        printf("Nu se poate citi fisierul!\nReincercati!");
        user_input();
    }
    else {
        printf("Lista de ");
        printf(BBLUE"cuvinte"RESET);
        printf(" este: \n"RESET);
        citire_afisare_cuvinte(list);
        user_input();
    }
}


void majuscule(char word[]) {
    for (int i = 0; word[i]; i++)
        if (word[i] >= 'a' || word[i] <= 'z')
            word[i] -= 32;
}
void adaugare_cuvant() {
    char cuv[LEN_CUV];
    system("cls");

    printf("Introduceti ");
    printf(BLUE"cuvantul"RESET);
    printf(" pe care doriti sa il adaugati: ");
    scanf("%s", cuv);
    majuscule(cuv);

    FILE *list = fopen(Lista, "a+");

    fseek(list, 0, SEEK_END);
    long pozitie_fisier = ftell(list); //pozitia cursorului in fisier

    //verificare daca se scrie cuvantul cu \n
    (pozitie_fisier > 0) ? fprintf(list, "\n%s", cuv): fprintf(list, "%s", cuv);

    fclose(list);

    system("cls");
    printf("Cuvantul \"");
    printf(BMAGENTA"%s"RESET, cuv);
    printf("\" a fost adaugat cu succes!");
    pause(1);
    system("cls");
}
void introducere_cuvant() {

    char cuv[LEN_CUV];
    system("cls");
    printf("-ADAUGARE ");
    printf(BBLUE"CUVANT-\n" RESET);
    printf("Introduceti ");
    printf(BLUE"cuvantul"RESET);
    printf(" pe care doriti sa il adaugati: (cuvantul trebuie sa aiba cel putin 4 litere ale alfabetului englez)\n");
    scanf("%s", cuv);
    getchar();

    FILE *list = fopen(Lista, "a+");

    fseek(list, 0, SEEK_END);//pozitionam cursorul la sf
    long pozitie_fisier = ftell(list); //pozitia cursorului in fisier

    //verificare daca se scrie cuvantul cu \n
    (pozitie_fisier > 0) ? fprintf(list, "\n%s", cuv): fprintf(list, "%s", cuv);

    fclose(list);

    system("cls");
    printf("-ADAUGARE ");
    printf(BBLUE"CUVANT-\n" RESET);
    printf("Cuvantul \"");
    printf(BMAGENTA"%s"RESET, cuv);
    printf("\" a fost adaugat cu succes!");
}
void optiune_5() {
    while (1) {

        introducere_cuvant();
        pause(1);

        while (1) {
            char choice;
            system("cls");
            printf("-ADAUGARE ");
            printf(BBLUE"CUVANT-\n" RESET);
            printf("Doriti sa adaugati inca un cuvant? (");
            printf(GREEN"Y"RESET);
            printf("/");
            printf(RED"N"RESET);
            printf(")\n");


            scanf(" %c", &choice);

            //optiune gresita
            if (choice == 'y' || choice == 'Y')
                break;
            else if (choice == 'n' || choice == 'N') {

                system("cls");
                printf("Revenire catre ");
                printf(RED"meniul principal"RESET);
                printf("...");
                pause(1);
                return;
            }
            else{
                system("cls");
                printf("Optiune");
                printf(BRED" Invalida!"RESET);
                pause(1);
                continue;
            }


        }
        /**
        MENIU ADD+DELETE WORD nu inca!
        int option;
        system("cls");
        while (1)
        {
            printf("-MODIFICARE LISTA CUVINTE-\n");
            printf("1) Adaugare cuvant\n2) Stergere cuvant\n3) Intoarcere in Meniul Principal\nIntroduceti optiunea dorita: ");
            scanf("%d", &option);
            if (option < 1 || option > 3) {
                printf("Optiune invalida! Reincercati...\n");
                sleep(1);
                system("cls");
                continue;
            }
            if (option == 3)
                return;

            optiuni_meniu5(option);

        }
        */
    }
}

void optiune_6() {
    system("cls");
    printf(BGREEN"La revedere!"RESET);
    pause(1);
    exit(0);
}

void choice(int opt) {
    switch (opt) {
        case 1:
            optiune_1();
            break;
        case 2:
            optiune_2();
            break;
        case 3:
            optiune_3();
            break;
        case 4:
            optiune_4();
            break;
        case 5:
            optiune_5();
            break;
        case 6:
            optiune_6();
        break;
    }

}
void meniu_principal() {
    int option;

    while (1) {
        system("cls");
        first_open();
        printf(BOLD"1) Incepe "RESET);
        printf(BMAGENTA"Jocul\n"RESET);

        printf(BOLD"2) Vizualizare "RESET);
        printf(BMAGENTA"Clasament\n"RESET);

        printf(BOLD"3) Cautare dupa "RESET);
        printf(BMAGENTA"Nume\n"RESET);


        printf(BOLD"4) Vizualizare "RESET);
        printf(BMAGENTA"Lista Cuvinte\n"RESET);

        printf(BOLD"5) Modificare "RESET);
        printf(BMAGENTA"Lista Cuvinte\n"RESET);

        printf(BOLD"6) "RESET);
        printf(BRED"EXIT\n"RESET);

        printf("Introduceti ");
        printf(GREEN"optiunea: "RESET);
        printf("\n(1->6)");
        scanf("%d", &option);
        getchar();


        // optiune gresita
        if (option < 1 || option > 6) {
            system("cls");
            printf("EROARE! OPTIUNEA NU EXISTA!");
            pause(1);
        }
        else //optiune valida
            choice(option);
    }
}
int main()
{

    meniu_principal();
    //srand(time(NULL));
    //load_rounds();
    //new_game();
   // store_name_score(10);
    return 0;

}