// PROIECT PROGRAMARE C
// Cezar Petreanu, FMI, IA, Anul I licenta, Gr.1, Sgr.1
//
// PROIECT - Rezervare Hotel


#include <stdio.h>
#include <string.h>

typedef struct Camera
{
    int index;
    int rezervat;
    int nr_max_pers;
    int pret;
    int nr_facilitati;
    int nr_pers;
    char checkin[100];
    char checkout[100];
    char facilitati[10][20];
}Camera;

typedef struct Sesiune
{
    char nume[100];
    char prenume[100];
    char user[100];
    char email[100];
    int camere_rezervate;
    char camere[100][200];
}Sesiune;

void verificare_exista(char utilizator_username[100], char utilizator_email[100], int* ok_utilizator, int* ok_emailexista);
void user_add(char nume[100], char prenume[100], char user[100], char email[100], char pass[100]);
int user_find(char user[100], char pass[100], Sesiune* sesiune);

void ecran_golire();

void meniu_cont(int* alegere);
void meniu_cont_autentificare(int* alegere, Sesiune* sesiune);
void meniu_cont_inregistrare(int* alegere);
void meniu_principal(int* alegere, Sesiune* sesiune);
void meniu_rezervare_noua(int* alegere, Camera camere[100], int nr_total_camere, Sesiune* sesiune);
void meniu_rezervarile_mele(int* alegere, Sesiune* sesiune, Camera* camere, int nr_total_camere);

void text_lowercase(char text[100])
{
    int l = strlen(text);
    for (int i = 0; i < l; i++)
        if (text[i] >= 'A' && text[i] <= 'Z')
            text[i] += 32;  // text[i] = text[i] - 'A' + 'a'
}

void text_namecase(char text[100])
{
    if (text[0] >= 'a' && text[0] <= 'z')
        text[0] -= 32;  // text[i] = text[i] - 'a' + 'A'
    int l = strlen(text);
    for (int i = 1; i < l; i++)
        if (text[i] >= 'A' && text[i] <= 'Z')
            text[i] += 32;  // text[i] = text[i] - 'A' + 'a'
}

void camere_init(Camera camere[], int* nr_total_camere)
{
    FILE* f;
    f = fopen("db_camere.dat", "r");

    char c[100];
    int i=0;

    fseek (f, 0, SEEK_END);
    int size = ftell(f);

    if (size == 0)
        return;

    fseek (f, 0, SEEK_SET);

    while(!feof(f))
    {
        fgets(c, 100, f);
        c[strlen(c)-1]='\0';

        if(strlen(c)>1)
        {
            char* t;
            t=strtok(c, " ");       camere[i].index             = atoi(t);
            t=strtok(NULL, " ");    camere[i].rezervat          = atoi(t);
            t=strtok(NULL, " ");    camere[i].nr_max_pers       = atoi(t);
            t=strtok(NULL, " ");    camere[i].pret              = atoi(t);
            t=strtok(NULL, " ");    camere[i].nr_facilitati     = atoi(t);
            t=strtok(NULL, " ");    camere[i].nr_pers           = atoi(t);
            t=strtok(NULL, " ");    strcpy(camere[i].checkin, t);
            t=strtok(NULL, " ");    strcpy(camere[i].checkout, t);

            int j=0;
            t=strtok(NULL, " ");
            while(t)
            {
                strcpy(camere[i].facilitati[j], t);
                j++;
                t=strtok(NULL, " ");
            }
        }

        i++;
    }

    *nr_total_camere = i-1;

    fclose(f);
}

int verificare_facilitati(Camera camere[100], int filtru_facilitati[7], int index)
{
    int gasit=0;
    char facilitati[7][20]={"bucatarie\0", "vedere-la-plaja\0", "pat-etajat\0", "seif\0", "uscator\0", "dus\0", "cada\0"};
    for(int i=0; i<7 && gasit == 0; i++)
    {
        if(filtru_facilitati[i] == 1)
        {
            gasit = 0;
            for(int j=0; gasit == 0 && j<camere[index].nr_facilitati; j++)
                if(!strcmpi(camere[index].facilitati[j], facilitati[i]))
                    gasit = 1;
        }
    }
    return gasit;
}

void camere_afisare(Camera camere[100], int nr_total_camere, int filtru_facilitati[7], int filtru_pret_maxim, int filtru_pret_minim, int filtru_tip, int filtrat)
{
    char facilitati[7][20]={"bucatarie", "vedere-la-plaja", "pat-etajat", "seif", "uscator", "dus", "cada"};
    int ok_afis = 0;
    for(int i=0; i<nr_total_camere; i++)
    {
        if(camere[i].rezervat == 0)
        {
            if(!filtrat || (camere[i].pret >= filtru_pret_minim && camere[i].pret <= filtru_pret_maxim && (camere[i].nr_max_pers == filtru_tip || filtru_tip == 0) && verificare_facilitati(camere, filtru_facilitati, i)))
            {
                ok_afis = 1;
                printf("%2d | %d pers. | %d lei | ", camere[i].index, camere[i].nr_max_pers, camere[i].pret);
                for(int j=0; j<camere[i].nr_facilitati; j++)
                {
                        printf("%s", camere[i].facilitati[j]);
                        if(j < camere[i].nr_facilitati-1)
                            printf(", ");
                }
                printf("\n");
            }
        }
    }
    if(nr_total_camere == 0)
        printf("Momentan nu sunt camere disponobile...\n");
    else if(ok_afis == 0)
        printf("Momentan nu sunt camere care indeplinesc cerintele...\n");
}

void sesiune_init(Sesiune* sesiune)
{
    strcpy(sesiune->nume, "");
    strcpy(sesiune->prenume, "");
    strcpy(sesiune->user, "");
    strcpy(sesiune->email, "");
    sesiune->camere_rezervate = 0;
}

void sesiune_get(char user[100], Sesiune* sesiune)
{
    user[strlen(user)-1] = '\0';

    char file_name[110] = "user_";
    char f_input[100];
    strcat(file_name, user);
    strcat(file_name, ".dat");

    sesiune_init(sesiune);

    FILE *f;

    f = fopen(file_name, "r");

    fseek (f, 0, SEEK_END);
    int size = ftell(f);

    if (size == 0)
        return;

    fseek (f, 0, SEEK_SET);

    if(!feof(f))
    {
        fgets(f_input, 100, f);    f_input[strlen(f_input)-1] = '\0'; strcpy(sesiune->nume, f_input);
        fgets(f_input, 100, f);    f_input[strlen(f_input)-1] = '\0'; strcpy(sesiune->prenume, f_input);
        fgets(f_input, 100, f);    f_input[strlen(f_input)-1] = '\0'; strcpy(sesiune->user, f_input);
        fgets(f_input, 100, f);    f_input[strlen(f_input)-1] = '\0'; strcpy(sesiune->email, f_input);
        fgets(f_input, 100, f);    f_input[strlen(f_input)-1] = '\0'; sesiune->camere_rezervate = atoi(f_input);

        for(int i=0; i<sesiune->camere_rezervate; i++)
        {
            fgets(f_input, 100, f);    f_input[strlen(f_input)-1] = '\0'; strcpy(sesiune->camere[i], f_input);
        }
    }

    fclose(f);
}

void sesiune_create(char nume[100], char prenume[100], char user[100], char email[100])
{
    char file_name[110] = "user_";
    char f_input[100];

    nume[strlen(nume)-1] = '\0';
    prenume[strlen(prenume)-1] = '\0';
    user[strlen(user)-1] = '\0';
    email[strlen(email)-1] = '\0';

    strcat(file_name, user);
    strcat(file_name, ".dat");

    FILE* f;
    f = fopen(file_name, "w");

    fprintf(f, "%s\n%s\n%s\n%s\n%d\n", nume, prenume, user, email, 0);

    fclose(f);
}

void sesiune_print(Sesiune sesiune)
{
    printf("%s \n", sesiune.nume);
    printf("%s \n", sesiune.prenume);
    printf("%s \n", sesiune.user);
    printf("%s \n", sesiune.email);
    printf("%d \n", sesiune.camere_rezervate);
    for(int i=0; i<sesiune.camere_rezervate; i++)
        printf("%d \n", sesiune.camere[i]);
}

int main()
{
    int nr_total_camere = 0;
    Camera camere[100];
    camere_init(camere, &nr_total_camere);

    Sesiune sesiune;
    sesiune_init(&sesiune);

    int rulare  = 1;
    int alegere = 1;

    while (rulare)
    {
        ecran_golire();
        switch (alegere)
        {
        case 0: // iesire din aplicatie
            rulare = 0;
            break;
        case 1: // meniu cont
            meniu_cont(&alegere);
            break;
        case 2: // meniu autentificare
            meniu_cont_autentificare(&alegere, &sesiune);
            break;
        case 3: // meniu inregistrare
            meniu_cont_inregistrare(&alegere);
            break;
        case 4: // meniu principal
            meniu_principal(&alegere, &sesiune);
            break;
        case 5: // meniu rezervare noua
            meniu_rezervare_noua(&alegere, camere, nr_total_camere, &sesiune);
            break;
        case 6: // meniu rezervarile mele
            meniu_rezervarile_mele(&alegere, &sesiune, &camere, nr_total_camere);
            break;
        }
    }

    printf("La revedere!");
    return 0;
}




/*
void users_ready(List *l);
{
    FILE *fileName;

    char file_user[100], file_email[100], file_password[100];

    fileName = fopen("fileName.txt", "r");
    if()
    while(fgets(file_user, 100, fileName))
    {
        fgets(file_email, 100, fileName);
        fgets(file_password, 100, fileName);
        printf("%s", ch);
    }

    fclose(fileName);
}
*/

void ecran_golire()
{
    // O metoda ineficienta...
    //
    // "Clear the screen"
    // http://www.cplusplus.com/forum/articles/10515/

    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void meniu_cont(int* alegere)
{
    char alegere_text[100];

    printf("[ Hotel Prima ]");
    printf("\n\n\n");
    printf("1 - Autentificare\n");
    printf("2 - Inregistrare\n");
    printf("\n0 - Iesire\n");

    do
    {
        printf(">> ");
        fgets(alegere_text, 100, stdin);

        if (!strcmpi(alegere_text, "0\n") || !strcmpi(alegere_text, "iesire\n"))
            *alegere = 0;    // Iesire din program
        else if (!strcmpi(alegere_text, "1\n") || !strcmpi(alegere_text, "autentificare\n"))
            *alegere = 2;    // Te duce la meniul de autentificare
        else if (!strcmpi(alegere_text, "2\n") || !strcmpi(alegere_text, "inregistrare\n"))
            *alegere = 3;    // Te duce la meniul de inregistrare
        else
            *alegere = -1;   // Optiune invalida...

    } while (*alegere == -1);
}

int user_find(char user[100], char pass[100], Sesiune* sesiune)
{
    char f_nume[100]="";
    char f_prenume[100]="";
    char f_username[100]="";
    char f_pass[100]="";
    char f_email[100]="";
    char f_ignore[100]="";

    FILE *f;

    f = fopen("db_utilizatori.dat", "r");

    fseek (f, 0, SEEK_END);
    int size = ftell(f);

    if (size == 0)
        return;

    fseek (f, 0, SEEK_SET);

    while(!feof(f))
    {
        fgets(f_nume, 100, f);    // Nume
        fgets(f_prenume, 100, f);    // Prenume
        fgets(f_username, 100, f);  // Username
        fgets(f_email, 100, f);     // Email
        fgets(f_pass, 100, f);    // Parola
        fgets(f_ignore, 100, f);    // Delimitator

        if(strcmp(user, f_username) == 0 && strcmp(pass, f_pass) == 0)
        {
            sesiune_get(f_username, sesiune);
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

void meniu_cont_autentificare(int* alegere, Sesiune* sesiune)
{
    char utilizator_username[100];
    char utilizator_parola[100];

    ecran_golire();

    printf("[ Hotel Prima - Autentificare ]");
    printf("\n\n\n");

    printf("Username: ");
    fgets(utilizator_username, 100, stdin);
    if (strcmp(utilizator_username, "\n") == 0) { *alegere = 1; return; }
    printf("Parola: ");
    fgets(utilizator_parola, 100, stdin); // [ TO-DO ] (2)
    if (strcmp(utilizator_parola, "\n") == 0) { *alegere = 1; return; }

    text_lowercase(utilizator_username);

    if(user_find(utilizator_username, utilizator_parola, sesiune))
        *alegere = 4;
    else
    {
        ecran_golire();
        printf("ATENTIE: Utilizatorul nu a fost gasit sau parola a fost scrisa gresit.\n\n");
        printf("Apasa orice pentru a continua...");
        getch();
        *alegere = 1;
    }
}

void verificare_parola(char utilizator_parola[100], char utilizator_parola_confirmare[100], int *ok_parola_lungime, int *ok_parola)
{
    if (strlen(utilizator_parola) < 8)
        *ok_parola_lungime = 0;
    if (strcmp(utilizator_parola, utilizator_parola_confirmare) != 0)
        *ok_parola = 0;
}

void verificare_email(char a[100], int* ok_email)
{
    int l = strlen(a)-1;

    int prefix_l        = 0;
    int prefix_litera   = 0;
    int arond           = 0;
    int sufix_punct     = 0;
    int sufix_litera    = 0;
    int sufix_domeniu   = 0;
    int sufix_site_l    = 0;
    int sufix_domeniu_l = 0;

    if (!((a[0] >= 'a' && a[0] <= 'z') || (a[0] >= '0' && a[0] <= '9')))
        *ok_email = 0;
    if (!(a[l - 1] >= 'a' && a[l - 1] <= 'z'))
        *ok_email = 0;

    for (int i = 1; i < l && ok_email; i++)
    {

        if (arond == 0)
        {
            if (a[i] != '@')
            {
                if ((strchr("_.", a[i]) && strchr("_.", a[i - 1])) || !((a[i] >= 'a' && a[i] <= 'z') || (a[i] >= '0' && a[i] <= '9') || a[i] == '.' || a[i] == '_')) {
                    *ok_email = 0; break;
                }
                if (a[i] >= 'a' && a[i] <= 'z')
                    prefix_litera = 1;

                prefix_l += 1;
            }
            else
            {
                if (!prefix_litera || strchr("_.", a[i - 1]) || prefix_l < 6 || prefix_l > 30) {
                    *ok_email = 0; break;
                }

                arond = 1;
            }
        }
        else
        {
            if (a[i-1] == '@')  {
                if (!((a[i] >= 'a' && a[i] <= 'z') || (a[i] >= '0' && a[i] <= '9'))) {
                    *ok_email = 0; break;
                }
                if (a[i] >= 'a' && a[i] <= 'z')
                    sufix_litera = 1;
                sufix_site_l += 1;
            }
            else {
                if (a[i] >= 'a' && a[i] <= 'z')
                    sufix_litera = 1;
                if (!((a[i] >= 'a' && a[i] <= 'z') || (a[i] >= '0' && a[i] <= '9') || a[i] == '.' || a[i] == '-')) {
                    *ok_email = 0; break;
                }
                if (a[i] == '-' && a[i - 1] == '-') {
                    *ok_email = 0; break;
                }
                if (a[i] == '.')
                {
                    if(sufix_site_l < 1) {
                        *ok_email = 0; break;
                    }
                    if(sufix_punct == 0)
                        sufix_punct = 1;
                    else {
                        *ok_email = 0; break;
                    }
                }

                if (sufix_punct && a[i] != '.')
                {
                    if (a[i] >= 'a' && a[i] <= 'z')
                        sufix_domeniu_l += 1;
                    else {
                        *ok_email = 0; break;
                    }
                }

                sufix_site_l += 1;
            }
        }
    }

    if (!arond || !sufix_punct || sufix_domeniu_l < 2)
        *ok_email = 0;
}

void verificare_exista(char utilizator_username[100], char utilizator_email[100], int* ok_utilizator, int* ok_emaildisponibil)
{
    *ok_utilizator  = 1;
    *ok_emaildisponibil = 1;

    char f_username[100]="";
    char f_email[100]="";
    char f_ignore[100]="";

    FILE *f;

    f = fopen("db_utilizatori.dat", "r");

    fseek (f, 0, SEEK_END);
    int size = ftell(f);

    if (size == 0)
        return;

    fseek (f, 0, SEEK_SET);

    while(!feof(f))
    {
        fgets(f_ignore, 100, f);    // Nume
        fgets(f_ignore, 100, f);    // Prenume
        fgets(f_username, 100, f);  // Username
        fgets(f_email, 100, f);     // Email
        fgets(f_ignore, 100, f);    // Parola
        fgets(f_ignore, 100, f);    // Delimitator

        if(strcmp(utilizator_username, f_username) == 0)
        {
            *ok_utilizator = 0;
            fclose(f);
            return;
        }
        if(strcmp(utilizator_email, f_email) == 0)
        {
            *ok_emaildisponibil = 0;
            fclose(f);
            return;
        }
    }

    fclose(f);
}

void user_add(char nume[100], char prenume[100], char user[100], char email[100], char pass[100])
{
    FILE* f;

    f = fopen("db_utilizatori.dat", "a");
    fprintf(f, "%s", nume);
    fprintf(f, "%s", prenume);
    fprintf(f, "%s", user);
    fprintf(f, "%s", email);
    fprintf(f, "%s", pass);
    fprintf(f, "%s", "-\n");

    fclose(f);
}

void meniu_cont_inregistrare(int* alegere)
{
    int ok_parola_lungime   = 1;
    int ok_parola           = 1;
    int ok_nume             = 1;
    int ok_utilizator       = 1;
    int ok_email            = 1;
    int ok_emaildisponibil  = 1;

    char utilizator_nume[100];
    char utilizator_prenume[100];
    char utilizator_username[100];
    char utilizator_email[100];
    char utilizator_parola[100];
    char utilizator_parola_confirmare[100];

    ecran_golire();

    printf("[ Hotel Prima - Inregistrare ]");
    printf("\n\n\n");

    printf("Nume: ");
    fgets(utilizator_nume, 100, stdin);
    if (strcmp(utilizator_nume, "\n") == 0) { *alegere = 1; return; }
    printf("Prenume: ");
    fgets(utilizator_prenume, 100, stdin);
    if (strcmp(utilizator_prenume, "\n") == 0) { *alegere = 1; return; }
    text_namecase(utilizator_nume);
    text_namecase(utilizator_prenume);

    printf("Username: ");
    fgets(utilizator_username, 100, stdin);
    if (strcmp(utilizator_username, "\n") == 0) { *alegere = 1; return; }
    text_lowercase(utilizator_username);
    printf("Email: ");
    fgets(utilizator_email, 100, stdin);
    if (strcmp(utilizator_email, "\n") == 0) { *alegere = 1; return; }
    printf("Parola (CASE-SENSITIVE): ");
    fgets(utilizator_parola, 100, stdin);
    if (strcmp(utilizator_parola, "\n") == 0) { *alegere = 1; return; }
    printf("Confirmare Parola: ");
    fgets(utilizator_parola_confirmare, 100, stdin);
    if (strcmp(utilizator_parola_confirmare, "\n") == 0) { *alegere = 1; return; }

    verificare_email(utilizator_email, &ok_email);
    verificare_parola(utilizator_parola, utilizator_parola_confirmare, &ok_parola_lungime, &ok_parola);
    verificare_exista(utilizator_username, utilizator_email, &ok_utilizator, &ok_emaildisponibil);

    if(ok_email && ok_parola_lungime && ok_parola && ok_utilizator && ok_email && ok_emaildisponibil)
    {
        user_add(utilizator_nume, utilizator_prenume, utilizator_username, utilizator_email, utilizator_parola);
        sesiune_create(utilizator_nume, utilizator_prenume, utilizator_username, utilizator_email);

        ecran_golire();
        printf("Utilizatorul a fost creat!");
        printf("\n\nApasa orice pentru a continue...");
        getch();
        *alegere = 1;
    }
    else
    {
        ecran_golire();

        if (!ok_email)
            printf("ATENTIE: Adresa de email nu exista sau a fost scrisa gresit.\n\n");
        else if (!ok_parola_lungime)
            printf("ATENTIE: Parola este prea scurta. Trebuie sa aiba cel putin 8 caractere.\n\n");
        else if (!ok_parola)
            printf("ATENTIE: Parolele nu se potrivesc.\n\n");
        else if (!ok_nume)
            printf("ATENTIE: Numele si prenumele trebuie sa contina numai litere.\n\n");
        else if (!ok_utilizator)
            printf("ATENTIE: Utilizatorul exista deja.\n\n");
        else if (!ok_emaildisponibil)
            printf("ATENTIE: Acest email este deja folosit.\n\n");

        printf("Apasa orice pentru a continua...");
        getch();
        *alegere = 1;
    }
}


void meniu_principal(int* alegere, Sesiune* sesiune)
{
    char alegere_text[100];

    ecran_golire();

    printf("[ Hotel Prima - Meniu principal ]");
    printf("\nBine ai venit, %s!", sesiune->prenume); // TO-DO (1)
    printf("\n\n\n");

    printf("1 - Rezervare noua\n");
    printf("2 - Rezervarile mele\n");
    printf("\n0 - Iesi din cont\n");

    do
    {
        printf(">> ");
        fgets(alegere_text, 100, stdin);

        if (!strcmpi(alegere_text, "0\n") || !strcmpi(alegere_text, "iesire\n") || !strcmpi(alegere_text, "iesire din cont\n"))
            *alegere = 1;    // Iesire din cont
        else if (!strcmpi(alegere_text, "1\n") || !strcmpi(alegere_text, "rezervare noua\n"))
            *alegere = 5;    // Te duce la meniul de creare a unei rezervari
        else if (!strcmpi(alegere_text, "2\n") || !strcmpi(alegere_text, "rezervarile mele\n"))
            *alegere = 6;    // Te duce la meniul cu rezervarile tale
        else
            *alegere = -1;   // Optiune invalida...

    } while (*alegere == -1);
}

void afisare_rezervarile_mele(Sesiune* sesiune)
{
    char fileName[100] = "user_";
    strcat(fileName, sesiune->user); strcat(fileName, ".dat");

    FILE *f;
    f = fopen(fileName, "r");

    char f_input[100];

    fgets(f_input, 100, f); // prenume
    fgets(f_input, 100, f); // nume
    fgets(f_input, 100, f); // parola
    fgets(f_input, 100, f); // email
    fgets(f_input, 100, f); // nr camere

    for(int i=0; i<sesiune->camere_rezervate; i++)
    {
        printf("%2d. " , i);
        fgets(f_input, 100, f);
        f_input[strlen(f_input)-1] = '\0';
        char *t;
        t = strtok(f_input, " ");
        printf("Camera: %s | " , t);
        t = strtok(NULL, " ");
        printf("Nr. Pers: %s | " , t);
        t = strtok(NULL, " ");
        printf("Perioada: %s " , t);
        t = strtok(NULL, " ");
        printf(" %s " , t);
        t = strtok(NULL, " ");
        /*
        printf("%s | Facilitati: " , t);
        do
        {
            t = strtok(NULL, " ");
            if(t)
                printf("%s " , t);
        }while(t);
        */
        printf("\n");
    }
}

void rezervare_detalii(int index, Sesiune* sesiune)
{
    ecran_golire();

    if(index >= sesiune->camere_rezervate || index < 0)
        printf("Atentie! Index invalid.");
    else
    {
        char f_input[100];

        printf("Rezervare nr. %d - Detalii\n\n", index);
        printf("Nume: %s %s\n", sesiune->nume, sesiune->prenume);
        printf("Email: %s\n\n", sesiune->email);
        strcpy(f_input, sesiune->camere[index]);
        char *t;
        t = strtok(f_input, " ");
        printf("Nr. camera: %s\n", t);
        t = strtok(NULL, " ");
        printf("Nr. persoane: %s\n", t);
        t = strtok(NULL, " ");
        printf("Data checkin: %s\n", t);
        t = strtok(NULL, " ");
        printf("Data checkout: %s\n", t);
        printf("Facilitati: " , t);
        do
        {
            t = strtok(NULL, " ");
            if(t)
                printf("%s " , t);
        }while(t);
    }

    printf("\n\n");
    printf("Apasa orice pentru a continua...");
    getch();
}

void rezervare_stergere(int index_rezervare, Sesiune* sesiune)
{
    char aux[100];
    for(int i=index_rezervare; i<sesiune->camere_rezervate-1; i++)
    {
        strcpy(sesiune->camere[i], sesiune->camere[i+1]);
    }
    strcpy(sesiune->camere[sesiune->camere_rezervate-1], "\0");
}

void meniu_rezervarile_mele(int* alegere, Sesiune* sesiune, Camera* camere, int nr_total_camere)
{
    char alegere_text[100];

    do
    {
        ecran_golire();

        printf("[ Hotel Prima - Rezervarile mele ]");
        printf("\n\n\n");

        printf("---------------------------------------------------------------\n");
        if(sesiune->camere_rezervate == 0)
            printf("(Momentan nu ai nici o rezervare!)\n");
        else
            afisare_rezervarile_mele(sesiune);
        printf("---------------------------------------------------------------\n\n");

        printf("1 - Detalii rezervare\n");
        printf("2 - Sterge rezervare\n");
        printf("\n0 - Inapoi\n\n");
        printf(">> ");
        fgets(alegere_text, 100, stdin);

        if (!strcmpi(alegere_text, "0\n") || !strcmpi(alegere_text, "inapoi\n"))
            *alegere = 4;    // Inapoi la meniul principal;
        else if (!strcmpi(alegere_text, "1\n") || !strcmpi(alegere_text, "detalii rezervare\n") || !strcmpi(alegere_text, "detalii\n"))
        {
            if(sesiune->camere_rezervate == 0)
            {
                ecran_golire();
                printf("Momentan nu ai nici o rezervare!");
                printf("\n\n");
                printf("Apasa orice pentru a continua...");
                getch();
                //alegere = -1;
                return;
            }

            char index_rezervare[100];
            int ok_index=1;

            printf("\nIntroduce numarul rezervarii pentru a vedea detaliile sale...\n>>");
            fgets(index_rezervare, 100, stdin);
            index_rezervare[strlen(index_rezervare)-1] = '\0';
            for(int i=0; ok_index && i<strlen(index_rezervare); i++)
                if(index_rezervare[i] < '0' || index_rezervare[i] > '9')
                    ok_index = 0;
            //fflush(stdin);
            if(ok_index)
                rezervare_detalii(atoi(index_rezervare), sesiune);
            else
            {
                printf("Atentie! Index invalid.\n\nApasa orice pentru a continua...");
                getch();
                break;
            }
            *alegere = -1;
        }
        else if (!strcmpi(alegere_text, "2\n") || !strcmpi(alegere_text, "sterge rezervare\n") || !strcmpi(alegere_text, "sterge\n"))
        {
            if(sesiune->camere_rezervate == 0)
            {
                ecran_golire();
                printf("Momentan nu ai nici o rezervare!");
                printf("\n\n");
                printf("Apasa orice pentru a continua...");
                getch();
                break;
            }

            int index_rezervare;

            printf("\nIntroduce numarul rezervarii pentru a o sterge...\n>>");
            fflush(stdin);
            scanf("%d", &index_rezervare);
            if(index_rezervare < 0 || index_rezervare > sesiune->camere_rezervate)
            {
                ecran_golire();
                printf("Atentie! Index invalid.\n\nApasa orice pentru a continua...");
                getch();
            }
            char aux[100], *t;
            strcpy(aux, sesiune->camere[index_rezervare]);
            t = strtok(aux, " "); // nr camera
            int index_camera = atoi(t);

            rezervare_stergere(index_rezervare, sesiune);
            update_user(index_camera, camere, sesiune, 1);
            update_camere(index_camera, 0, 0, "0", "0", camere);
            db_update_camere(camere, nr_total_camere);

            *alegere = -1;
        }
        else
            *alegere = -1;   // Optiune invalida...

    } while (*alegere == -1);
}

void convert_data(char data[100], int* zi, int* luna, int* an)
{
    char aux[100];
    strcpy(aux, data);

    char *t;
    t = strtok(aux, "-");   *zi   = atoi(t);
    t = strtok(NULL, "-");  *luna = atoi(t);
    t = strtok(NULL, "-");  *an   = atoi(t);
}

int verificare_data(char data[100])
{
    int l=strlen(data);
    if(l != 10)
        return 0;

    if(!(data[0] >= '0' && data[0] <= '9') || !(data[1] >= '0' && data[1] <= '9') || data[2] != '-' ||
       !(data[3] >= '0' && data[3] <= '9') || !(data[4] >= '0' && data[4] <= '9') || data[5] != '-' ||
       !(data[6] >= '0' && data[6] <= '9') || !(data[7] >= '0' && data[7] <= '9') || !(data[8] >= '0' && data[8] <= '9') || !(data[9] >= '0' && data[9] <= '9'))
        return 0;

    int zi, luna, an;
    convert_data(data, &zi, &luna, &an);

    if((luna < 1 || luna > 12) || (zi<1 || zi>31))
        return 0;
    if(zi > 31 && (luna == 1 || luna == 3 || luna == 5 || luna == 7 || luna == 8 || luna == 10 || luna == 12))
        return 0;
    if(zi > 30 && (luna == 4 || luna == 6 || luna == 9 || luna == 11))
        return 0;
    if(zi > 29 && luna == 2)
        return 0;
    return 1;
}

int numar_ani_bisecti(int an, int luna)
{
    if(luna <= 2)
        an--;
    return an/4-an/100+an/400;
}

int zile_data(char checkin[100], char checkout[100])
{
    int zi_in, luna_in, an_in;
    int zi_out, luna_out, an_out;
    int zile_luni[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    convert_data(checkin, &zi_in, &luna_in, &an_in);
    convert_data(checkout, &zi_out, &luna_out, &an_out);

    int nr_in, nr_out;

    nr_in = an_in*365 + zi_in;
    for(int i=0; i<luna_in; i++)
        nr_in += zile_luni[i];

    nr_in += numar_ani_bisecti(an_in, luna_in);

    nr_out = an_out*365 + zi_out;
    for(int i=0; i<luna_out; i++)
        nr_out += zile_luni[i];

    nr_out += numar_ani_bisecti(an_out, luna_out);

    return nr_out-nr_in;
}

void update_user(int nr_cam, Camera camere[100], Sesiune *sesiune, int adauga_sterge)
{
    // adauga_sterge = 0    adauga
    // adauga_sterge = 1    sterge

    if(!adauga_sterge)
    {
        char aux[10];
        strcpy(sesiune->camere[sesiune->camere_rezervate], "");
        itoa(nr_cam, aux, 10);                  strcat(sesiune->camere[sesiune->camere_rezervate], aux); strcat(sesiune->camere[sesiune->camere_rezervate], " ");
        itoa(camere[nr_cam].nr_pers, aux, 10);  strcat(sesiune->camere[sesiune->camere_rezervate], aux); strcat(sesiune->camere[sesiune->camere_rezervate], " ");
        strcat(sesiune->camere[sesiune->camere_rezervate], camere[nr_cam].checkin); strcat(sesiune->camere[sesiune->camere_rezervate], " ");
        strcat(sesiune->camere[sesiune->camere_rezervate], camere[nr_cam].checkout); strcat(sesiune->camere[sesiune->camere_rezervate], " ");
        for(int i=0; i<camere[nr_cam].nr_facilitati; i++){
            strcat(sesiune->camere[sesiune->camere_rezervate], camere[nr_cam].facilitati[i]); strcat(sesiune->camere[sesiune->camere_rezervate], " ");
        }
    }


    char fileName[100] = "user_";
    strcat(fileName, sesiune->user); strcat(fileName, ".dat");

    FILE *f;
    fclose(fopen(fileName, "w"));
    f = fopen(fileName, "w");

    fprintf(f, "%s", sesiune->nume); fprintf(f, "\n");
    fprintf(f, "%s",sesiune->prenume); fprintf(f, "\n");
    fprintf(f, "%s",sesiune->user); fprintf(f, "\n");
    fprintf(f, "%s",sesiune->email); fprintf(f, "\n");
    int modifier = (adauga_sterge)?-1:1;
    fprintf(f, "%d",sesiune->camere_rezervate+modifier); fprintf(f, "\n");
    for(int i=0; i<sesiune->camere_rezervate+modifier; i++)
    {
        fprintf(f, "%s",sesiune->camere[i]);
        fprintf(f, "\n");
    }

    fclose(f);

    if(adauga_sterge == 0) // adauga
        sesiune->camere_rezervate++;
    else    // sterge
        sesiune->camere_rezervate--;
}

void update_camere(int nr_cam, int status, int nr_pers, char checkin[100], char checkout[100], Camera camere[100])
{
    //status = 0    liber
    //status = 1    ocupat
    camere[nr_cam].rezervat = status;
    camere[nr_cam].nr_pers = nr_pers;
    strcpy(camere[nr_cam].checkin, checkin);
    strcpy(camere[nr_cam].checkout, checkout);
}

void db_update_camere(Camera camere[100], int nr_total_camere)
{
    FILE *f;
    fclose(fopen("db_camere.dat", "w"));
    f = fopen("db_camere.dat", "w");

    for(int i=0; i<=nr_total_camere; i++)
    {
        fprintf(f, "%d %d %d %d %d %d %s %s ", camere[i].index, camere[i].rezervat, camere[i].nr_max_pers, camere[i].pret, camere[i].nr_facilitati, camere[i].nr_pers, camere[i].checkin, camere[i].checkout);
        for(int j=0; j<camere[i].nr_facilitati; j++)
            fprintf(f, "%s ", camere[i].facilitati[j]);
        fprintf(f, "\n");
    }
}

int confirmare_rezervare(int nr_cam, int nr_pers, char checkin[100], char checkout[100], int nr_nopti, int pret, Camera camere[100], Sesiune* sesiune, int nr_total_camere)
{
    ecran_golire();
    printf("Nume: %s\n", sesiune->nume);
    printf("Prenume: %s\n", sesiune->prenume);
    printf("Email: %s\n", sesiune->email);
    printf("\n");
    printf("Numarul camerei: %d\n", nr_cam);
    printf("Numarul de persoane: %d\n", nr_pers);
    if(nr_nopti == 1)
        printf("Perioada: %s - %s (o noapte)\n", checkin, checkout);
    else
        printf("Perioada: %s - %s (%d nopti)\n", checkin, checkout, nr_nopti);
    printf("Pret total: %d\n\n", pret);

    char alegere[100];
    int ok = 0;

    printf("Confirmati rezervarea?\n\n1. DA\n2. NU\n\n");
    do
    {
        printf(">>");
        fgets(alegere, 100, stdin);
        if(strcmpi(alegere, "1\n") == 0 || strcmpi(alegere, "da\n") == 0 || strcmpi(alegere, "d\n") == 0)
        {
            update_camere(nr_cam, 1, nr_pers, checkin, checkout, camere);
            update_user(nr_cam, camere, sesiune, 0);
            db_update_camere(camere, nr_total_camere);
            ok = 1;
            return 1;
        }
        else if(strcmpi(alegere, "2\n") == 0 || strcmpi(alegere, "nu\n") == 0 || strcmpi(alegere, "n\n") == 0)
        {
            ecran_golire();
            printf("Rezervarea a fost anulata.");
            printf("\n\n");
            printf("Apasa orice pentru a continua...");
            getch();
            ok = 0;
            return 0;
        }
    }while(1);
}

void meniu_rezervare_noua_input(Camera camere[], Sesiune* sesiune, int* ok, int nr_total_camere)
{
    char nr_cam[100], nr_pers[100], checkin[100], checkout[100];
    int l, int_nr_pers, int_nr_cam, nr_nopti, pret;

    *ok = 0;

    printf("\nNumarul camerei:");
    fgets(nr_cam, 100, stdin);
    nr_cam[strlen(nr_cam)-1]='\0';
    l = strlen(nr_cam);
    for(int i=0; i<l; i++)
        if(!(nr_cam[i] >= '0' && nr_cam[i] <= '9'))
        {
            ecran_golire();
            printf("Atentie! Camera invalida.");
            printf("\n\n");
            printf("Apasa orice pentru a continua...");
            getch();
            return;
        }

    int_nr_cam = atoi(nr_cam);
    if(int_nr_cam > nr_total_camere || int_nr_cam < 0)
    {
        ecran_golire();
        printf("Ne pare rau, dar aceasta camera nu exista.");
        printf("\n\n");
        printf("Apasa orice pentru a continua...");
        getch();
        return;
    }
    if(camere[int_nr_cam].rezervat)
    {
        ecran_golire();
        printf("Ne pare rau, dar aceasta camera nu este disponibila.");
        printf("\n\n");
        printf("Apasa orice pentru a continua...");
        getch();
        return;
    }

    printf("Numarul de persoane:");
    fgets(nr_pers, 100, stdin);
    nr_pers[strlen(nr_pers)-1]='\0';
    l = strlen(nr_pers);
    for(int i=0; i<l; i++)
        if(!(nr_pers[i] >= '0' && nr_pers[i] <= '9'))
        {
            ecran_golire();
            printf("Atentie! Numar invalid de persoane.");
            printf("\n\n");
            printf("Apasa orice pentru a continua...");
            getch();
            return;
        }
    int_nr_pers = atoi(nr_pers);
    if(int_nr_pers > camere[int_nr_cam].nr_max_pers+2)
    {
        ecran_golire();
        printf("Atentie! Sunt prea multe persoane. Numarul de persoane poate depasi cu maxim 2.");
        printf("\n\n");
        printf("Apasa orice pentru a continua...");
        getch();
        return;
    }
    if(int_nr_pers <= 0)
    {
        ecran_golire();
        printf("Atentie! Numar invalid de persoane.");
        printf("\n\n");
        printf("Apasa orice pentru a continua...");
        getch();
        return;
    }

    printf("Data check-in (ZZ-LL-AAAA): ");
    fgets(checkin, 100, stdin);
    checkin[strlen(checkin)-1]='\0';
    if(!verificare_data(checkin))
    {
        ecran_golire();
        printf("Atentie! Data invalida.");
        printf("\n\n");
        printf("Apasa orice pentru a continua...");
        getch();
        return;
    }
    printf("Data check-out (ZZ-LL-AAAA): ");
    fgets(checkout, 100, stdin);
    checkout[strlen(checkout)-1]='\0';
    if(!verificare_data(checkout))
    {
        ecran_golire();
        printf("Atentie! Data invalida.");
        printf("\n\n");
        printf("Apasa orice pentru a continua...");
        getch();
        return;
    }

    nr_nopti = zile_data(checkin, checkout);
    if(nr_nopti < 1)
    {
        ecran_golire();
        printf("Atentie! Perioada este prea scurta sau nu este valida. Perioada trebuie sa fie de minim o noapte.");
        printf("\n\n");
        printf("Apasa orice pentru a continua...");
        getch();
        return;
    }
    if(nr_nopti>13)
    {
        ecran_golire();
        printf("Atentie! Perioada este prea lunga. Puteti rezerva o camera pentru maxim 14 zile (13 nopti).");
        printf("\n\n");
        printf("Apasa orice pentru a continua...");
        getch();
        return;
    }

    pret = nr_nopti*(camere[int_nr_cam].pret);

    *ok = confirmare_rezervare(int_nr_cam, int_nr_pers, checkin, checkout, nr_nopti, pret, camere, sesiune, nr_total_camere);
}

void meniu_rezervare_filtru(int filtru_facilitati[7], int *filtru_pret_maxim, int *filtru_pret_minim, int *filtru_tip, int *filtrat)
{
    for(int i=0; i<7; i++)
        filtru_facilitati[i] = 0;
    *filtru_pret_maxim = 0;
    *filtru_pret_minim = 0;
    *filtru_tip = 0;

    ecran_golire();
    printf("Filtreaza camerele (1/2)\n\n");

    char alegere[100];
    *filtrat = 0;
    printf("Tip camera (1, 2, 4, lasa campul gol pentru a ignora): ");
    fgets(alegere, 100, stdin); alegere[strlen(alegere)-1] = '\0';
    if(strlen(alegere) == 0)
    {
        *filtru_tip = 0;
    }
    for(int i=0; i<strlen(alegere); i++)
        if(alegere[i] != '1' && alegere[i] != '2' && alegere[i] != '4')
        {
            ecran_golire();
            printf("Atentie! Tip de camera invalida.\n\nApasa orice pentru a continua...");
            getch();
            *filtrat = 0;
            return;
        }
    *filtru_tip = atoi(alegere);


    printf("Pret maxim (lasa campul gol pentru a ignora): ");
    fgets(alegere, 100, stdin); alegere[strlen(alegere)-1] = '\0';
    for(int i=0; i<strlen(alegere); i++)
        if(alegere[i]<'0' || alegere[i]>'9')
        {
            ecran_golire();
            printf("Atentie! Pret invalid.\n\nApasa orice pentru a continua...");
            getch();
            *filtrat = 0;
            return;
        }
    if(alegere[0] == '\0') *filtru_pret_maxim = 999999999;
    else *filtru_pret_maxim = atoi(alegere);

    printf("Pret minim (lasa campul gol pentru a ignora): ");
    fgets(alegere, 100, stdin); alegere[strlen(alegere)-1] = '\0';
    for(int i=0; i<strlen(alegere); i++)
        if(alegere[i]<'0' || alegere[i]>'9')
        {
            ecran_golire();
            printf("Atentie! Pret invalid.\n\nApasa orice pentru a continua...");
            getch();
            *filtrat = 0;
            return;
        }
    if(alegere[0] == '\0') *filtru_pret_minim = 0;
    else *filtru_pret_minim = atoi(alegere);

    if(*filtru_pret_maxim < *filtru_pret_minim)
    {
        ecran_golire();
        printf("Atentie! Pretul maxim nu poate fi mai mic decat cel minim.\n\nApasa orice pentru a continua...");
        getch();
        *filtrat = 0;
        return;
    }

    int facilitati_alese = 0;
    do
    {
        ecran_golire();
        printf("Filtreaza camerele (2/2)\n\n");
        printf("Alegeti facilitatile (introduceti numai numarul pentru a selecta/deselecta):\n");
        printf("1. Bucatarie ["); if(filtru_facilitati[0]) printf("X"); else printf(" "); printf("]\n");
        printf("2. Vedere la plaja ["); if(filtru_facilitati[1]) printf("X"); else printf(" "); printf("]\n");
        printf("3. Pat etajat ["); if(filtru_facilitati[2]) printf("X"); else printf(" "); printf("]\n");
        printf("4. Seif ["); if(filtru_facilitati[3]) printf("X"); else printf(" "); printf("]\n");
        printf("5. Uscator ["); if(filtru_facilitati[4]) printf("X"); else printf(" "); printf("]\n");
        printf("6. Dus ["); if(filtru_facilitati[5]) printf("X"); else printf(" "); printf("]\n");
        printf("7. Cada ["); if(filtru_facilitati[6]) printf("X"); else printf(" "); printf("]\n");
        printf("\n0. Filtreaza\n\n");
        printf(">>");
        fgets(alegere, 100, stdin);
        alegere[strlen(alegere)-1] = '\0';
        if(strlen(alegere) == 1 && !(!strcmp(alegere, "8") || !strcmp(alegere, "9")))
        {
            if(!strcmp(alegere, "0"))
            {
                *filtrat = 1;

                if(facilitati_alese == 0)
                {
                    for(int i=0; i<7; i++)
                        filtru_facilitati[i] = 1;
                }
                return;
            }
            int index = atoi(alegere)-1;
            filtru_facilitati[index] = !filtru_facilitati[index];
            if(filtru_facilitati[index]==1)
                facilitati_alese++;
            else
                facilitati_alese--;

        }

    }while(1);
}

void meniu_rezervare_noua(int* alegere, Camera camere[], int nr_total_camere, Sesiune* sesiune)
{
    int ok=0;
    char alegere_text[100];
    char filtru[200];
    int filtrat = 0;
    int filtru_facilitati[7], filtru_tip=0, filtru_pret_minim=0, filtru_pret_maxim=0;

    do
    {

        ecran_golire();

        printf("[ Hotel Prima - Rezervare noua ]");
        printf("\n\n\n");

        printf("--------------------------------------------------------------\n");
        camere_afisare(camere, nr_total_camere, filtru_facilitati, filtru_pret_maxim, filtru_pret_minim, filtru_tip, filtrat);
        printf("--------------------------------------------------------------\n\n");

        printf("1 - Rezerva o camera\n");
        printf("2 - Cauta\n");
        if(filtrat == 1)
            printf("3 - Sterge filtrul\n");
        printf("\n0 - Inapoi\n");
        printf(">> ");

        fgets(alegere_text, 100, stdin);

        if (!strcmpi(alegere_text, "0\n") || !strcmpi(alegere_text, "inapoi\n"))
            *alegere = 4;    // Inapoi la meniul principal;
        else if (!strcmpi(alegere_text, "1\n") || !strcmpi(alegere_text, "rezerva\n") || !strcmpi(alegere_text, "rezerva camera\n") || !strcmpi(alegere_text, "rezerva o camera\n"))
        {
            meniu_rezervare_noua_input(camere, sesiune, &ok, nr_total_camere);
            if(ok)
                *alegere = 4;
            else
                *alegere = -1;
        }
        else if (!strcmpi(alegere_text, "2\n") || !strcmpi(alegere_text, "cauta\n"))
        {
            meniu_rezervare_filtru(filtru_facilitati, &filtru_pret_maxim, &filtru_pret_minim, &filtru_tip, &filtrat);
            *alegere = -1;
        }
        else if (filtrat && (!strcmpi(alegere_text, "3\n") || !strcmpi(alegere_text, "sterge\n") || !strcmpi(alegere_text, "sterge filtrul\n")))
        {
            filtrat = 0;
            *alegere = -1;
        }
        else
            *alegere = -1;   // Optiune invalida...

    } while (*alegere == -1);
}
