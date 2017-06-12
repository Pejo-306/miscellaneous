// TODO:
/*
    New option: change a pizza from a file;
    Shorten the code via funcions!!!
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH "/home/pesho/Documents/My Programs/C/TUES/pizza_file_system/files/"

#include <termios.h>

static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) 
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new = old; /* make new settings same as old settings */
  new.c_lflag &= ~ICANON; /* disable buffered i/o */
  new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) 
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) 
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void) 
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void) 
{
  return getch_(1);
}

struct Pizza {
    char name[50];
    char ingredients[20][60];
    double price; // in USD
    int diameter; // in cm
    int weight; // in grams
};

char char_lower(char);
char * str_lower(char *);
char * cut_str(char *, char);
char * get_filename();
char * nix_filename(char *);

int create_file(char *);
int delete_file(char *);
int display_pizza(char *);
int add_pizza(char *);
int edit_pizza(char *, char);

int main() {
    const char options[10][50] = {
        "[D]isplay pizzas",
        "[A]dd pizza",
        "[C]reate file",
        "[E]dit pizza",
        "[Q]uit"
    };

    char opt;
    char *filename;
    short int i;
    do {
        for(i = 0; i < sizeof(options) / sizeof(options[0]); i++) {
            if(!strcmp(options[i], "\0")) // break when the end of the array is reached
                break;
            else printf("~ %s\n", options[i]);
        }
        
        printf("\n:");
        opt = char_lower(getchar());
        getchar(); // eat up the space
        switch(opt) {
            case 'd':                    
                printf("Please, specify the path of the file from which you wish to read data:\n");
                filename = get_filename();
                system("clear");
                edit_pizza(filename, 'd');
                break;
            case 'a':
                printf("Please, specify the path of the file in which you wish to add a pizza:\n");
                filename = get_filename();
                system("clear");
                add_pizza(filename);
                break;
            case 'c':
                printf("Please, specify the name of the file you wish to create:\n");
                filename = get_filename();
                create_file(filename);
                break;
            case 'e':
                printf("Please, specify the path of the file in which your pizza is located:\n");
                filename = get_filename();
                system("clear");
                edit_pizza(filename, 'e');
                break;
            case 'q':
                printf("Goodbye!\n");
                break;
            default:
                printf("Please enter a valid option (in brackets).\n");
                break;
        }

        printf("\nPress any key to continue...\n");
        getchar();
        system("clear");
    }
    while(opt != 'q');
    
    return 0;
}

char char_lower(char symbol) {
    if(symbol >= 'A' && symbol <= 'Z') return symbol + 32;
    else return symbol;
}

char * str_lower(char *str) {
    char *new_str = malloc(sizeof(char) * (strlen(str)+1));
    
    short int i;
    for(i = 0; i < strlen(str); i++)
        *(new_str+i) = char_lower(*(str+i));

    return new_str;
}

// this func caused me a lot of problems
char * cut_str(char *str, char symbol) {
    char *new_str = (char *) malloc(sizeof(str));
 
    short int i = 0;
    while(*(str+i) != symbol && i < strlen(str)) {
        *(new_str+i) = *(str+i);
        i++;
    }
    *(new_str+i) = '\0';

    return new_str;
}

char * get_filename() {
    char *filename = (char *) malloc(sizeof(char) * 200);

    // input filename
    char inp[200];
    printf("%s", PATH);
    fgets(inp, 200, stdin);

    // prepend PATH to the filename
    strcpy(filename, PATH);
    strcat(filename, cut_str(inp, '\n'));

    return filename;
}

char * nix_filename(char *filename) {
    char *nix_fn = (char *) malloc(sizeof(filename));
    
    strcpy(nix_fn, "\"");
    strcat(nix_fn, filename);
    strcat(nix_fn, "\"");

    free(filename); // filename is obsolete at this point

    return nix_fn;
}

int create_file(char *filename) {
    FILE *file;

    file = fopen(filename, "rb");
    if(file) {
        printf("This file already exits.\n");
        fclose(file);
        return 1;
    }
    else {
        file = fopen(filename, "wb");
        if(file) {
            printf("File has successfully been created.\n");
            fclose(file);
            return 0;
        }
        else {
            printf("An unexpected error has occured during file creation!\n");
            return -1;
        }
    }
}

/* kept for future use in another program */
int delete_file(char *filename) {
    FILE *file;

    file = fopen(filename, "rb");
    if(file) {
        fclose(file);
        char opt;       
 
        printf("Are you sure you wish to delete this file ");
        do {
            printf("(y/N)?: ");
            opt = char_lower(getchar());
            getchar(); // eat up a space
        }
        while(opt != 'y' && opt != 'n');
        
        if(opt == 'y') {
            char cmd[] = {"rm "};
            strcat(cmd, nix_filename(filename));
            system(cmd);
            printf("File has been successfully deleted.\n");
            return 0;
        }
        else {
            printf("File destruction cancelled.\n");
            return 1;
        }
    }
    else {
        printf("This file doesn't exist.\n");
        return -1;
    }
}

int display_pizza(char *filename) {
    FILE *file;

    file = fopen(filename, "rb");
    if(file) {
        struct Pizza pizza;
 
        short int i, done = 0, cur_pos = 0;
        char opt;
        while(!done) {
            system("clear");
            printf("Current directory: %s\n\n", filename);            

            fseek(file, sizeof(pizza) * cur_pos, SEEK_SET);
            if(fread(&pizza, sizeof(pizza), 1, file)) {
                printf("\"%s\":\n", pizza.name);
                printf("weight: %dg, diameter: %dcm\n", pizza.weight, pizza.diameter);
                    
                printf("ingredients:\n");
                for(i = 0; i < sizeof(pizza.ingredients); i++) {
                    if(!strcmp(pizza.ingredients[i], "\0"))
                        break;
                    printf("%4c %-s\n", '-', pizza.ingredients[i]);
                }
                printf("price: $%4.2lf\n", pizza.price);
            }
            
            printf("\nUse the up and down arrow keys to navigate the menu.\n");
            printf("Press 'q' to exit.\n");
            printf("Current pizza: %d\n", cur_pos+1);

            opt = getch();
            if(opt == '\033') { // ESC ASCII value
                getch(); // skip the '['
                switch(getch()) {
                    case 'A': // up arrow
                        if(cur_pos > 0) cur_pos--;
                        break;
                    case 'B': // down arrow 
                        if(fgetc(file) != EOF) cur_pos++;
                        break;
                }
            }
            else if(char_lower(opt) == 'q') {
                done = 1;
            }
        }

        fclose(file);
        return 0;
    }
    else {
        printf("This file doesn't exist.\n");
        return -1;
    }
}

int add_pizza(char *filename) {
    FILE *file;
    
    file = fopen(filename, "rb");
    if(file) {
        fclose(file);

        file = fopen(filename, "ab"); 
        struct Pizza pizza;
 
        char ingr[sizeof(pizza.ingredients[0])];
        short int done = 0, i;
        char opt;
        while(!done) {
            system("clear");
            printf("Current directory: %s\n\n", filename);
 
            printf("Pizza name: ");
            fgets(pizza.name, sizeof(pizza.name), stdin);
            strcpy(pizza.name, cut_str(pizza.name, '\n'));

            printf("\nDiameter (in cm): ");
            scanf("%d", &pizza.diameter);
            printf("Weight (in grams): ");
            scanf("%d", &pizza.weight);
            printf("Price (in USD): ");
            scanf("%lf", &pizza.price);

            getchar(); // eat up the space
            printf("\nIngredients (max 20) (type ':q' to quit)):\n");
            
            for(i = 0; i < sizeof(pizza.ingredients) / sizeof(pizza.ingredients[0]); i++) {
                printf("%d(/20): ", i+1);
                fgets(ingr, sizeof(ingr), stdin);
                strcpy(ingr, cut_str(ingr, '\n'));
                if(!strcmp(ingr, ":q"))
                    break;
                strcpy(pizza.ingredients[i], ingr);
            }

            fwrite(&pizza, sizeof(pizza), 1, file);

            printf("Would you like to add another ");
            do {
                printf("(y/N)?: ");
                opt = char_lower(getchar());
                getchar(); // eat up a space
            }
            while(opt != 'y' && opt != 'n');
            if(opt  == 'n') done = 1;
            else printf("\n");
        }

        fclose(file);
        return 0;
    }
    else {
        printf("This file doesn't exist!\n");
        return -1;
    }
}

int edit_pizza(char *filename, char mode) {
    // this code is very similar to display_pizza    
    FILE *file;

    file = fopen(filename, "rb+");
    if(file) {
        struct Pizza pizza;
 
        short int i, done = 0, cur_pos = 0;
        char opt;
        while(!done) {
            system("clear");
            printf("Current directory: %s\n\n", filename);            

            fseek(file, sizeof(pizza) * cur_pos, SEEK_SET);
            if(fread(&pizza, sizeof(pizza), 1, file)) {
                printf("\"%s\":\n", pizza.name);
                printf("weight: %dg, diameter: %dcm\n", pizza.weight, pizza.diameter);
                    
                printf("ingredients:\n");
                for(i = 0; i < sizeof(pizza.ingredients); i++) {
                    if(!strcmp(pizza.ingredients[i], "\0"))
                        break;
                    printf("%4c %-s\n", '-', pizza.ingredients[i]);
                }
                printf("price: $%4.2lf\n", pizza.price);
            }
            
            printf("\nUse the up and down arrow keys to navigate the menu.\n");
            printf("Press 'q' to exit.\n");
            if(mode == 'e') printf("Hit 'ENTER' to start editing.\n");
            printf("Current pizza: %d\n", cur_pos+1);

            opt = getch();
            if(opt == '\033') { // ESC ASCII value
                getch(); // skip the '['
                switch(getch()) {
                    case 'A': // up arrow
                        if(cur_pos > 0) cur_pos--;
                        break;
                    case 'B': // down arrow 
                        if(fgetc(file) != EOF) cur_pos++;
                        break;
                }
            }
            else if(opt == '\012' && mode == 'e') { // newline ASCII value
                // this is the new bit of code
                const char options[10][20] = {
                    "[N]ame",
                    "[I]ngredients",
                    "[P]rice",
                    "[D]iameter",
                    "[W]eight",
                    "[S]ave"
                };

                char opt;
                short int j, still_editing = 1;
                do { 
                    system("clear");
                    // print pizza
                    printf("\"%s\":\n", pizza.name);
                    printf("weight: %dg, diameter: %dcm\n", pizza.weight, pizza.diameter);
                            
                    printf("ingredients:\n");
                    for(j = 0; j < sizeof(pizza.ingredients); j++) {
                        if(!strcmp(pizza.ingredients[j], "\0"))
                            break;
                        printf("%4c %-s\n", '-', pizza.ingredients[j]);
                    }
                    printf("price: $%4.2lf\n", pizza.price);

                    printf("\nWhat would you like to change?:\n");
                    
                    // print options
                    for(j = 0; j < sizeof(options) / sizeof(options[0]); j++) {
                        if(!strcmp(options[j], "\0")) // break when the end of the array is reached
                            break;
                        else printf("-- %s\n", options[j]);
                    }
                    
                    printf("\n:");
                    opt = char_lower(getchar());
                    getchar(); // eat up the space

                    switch(opt) {
                        case 'n':
                            printf("Please, enter the new value: ");
                            fgets(pizza.name, sizeof(pizza.name), stdin);
                            strcpy(pizza.name, cut_str(pizza.name, '\n'));
                            break;
                        case 'i':
                            // i'll figure this out later
                            break;
                        case 'p':
                            printf("Please, enter the new value: ");
                            scanf("%lf", &(pizza.price));
                            getchar();
                            break;
                        case 'd':
                            printf("Please, enter the new value: ");
                            scanf("%d", &(pizza.diameter));
                            getchar();
                            break;
                        case 'w':
                            printf("Please, enter the new value: ");
                            scanf("%d", &(pizza.weight));
                            getchar();
                            break;
                        case 's':
                            // save changes here
                            still_editing = 0;
                            break;
                        default:
                            printf("Please enter a valid option (in brackets).\n");
                            break;
                    }
                }
                while(still_editing);
            }
            else if(char_lower(opt) == 'q') {
                done = 1;
            }
        }

        fclose(file);
        return 0;
    }
    else {
        printf("This file doesn't exist.\n");
        return -1;
    }
}
