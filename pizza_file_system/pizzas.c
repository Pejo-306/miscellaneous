/*
    TODO:
    - delete pizza ingredients (similar to how I delete pizzas from files)
    - integrate add_pizza into edit_pizza
        -- problems with removing last pizza in file
        -- ingredients when adding a new pizza are copied from first pizza
    - list files in /files/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
int file_exists(char *, short int);

void print_pizza(struct Pizza *);
int create_file(char *);
int delete_file(char *);
int add_pizza(char *);
int edit_pizza(char *, char);

int main() {
    const char options[10][50] = {
        "[D]isplay pizzas",
        "[A]dd pizza",
        "[C]reate file",
        "[R]emove file",
        "[E]dit pizza",
        "[S]et filename",
        "[Q]uit"
    };

    char opt;
    char *filename = NULL, *temp;
    short int i;
    do {
        printf("File:\n\"%s\"\n\n", filename);

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
                edit_pizza(filename, 'd');
                break;
            case 'a':
                system("clear");
                edit_pizza(filename, 'a');
                break;
            case 'c':
                printf("Please, specify the name of the file you wish to create:\n");
                filename = get_filename();
                create_file(filename);
                break;
            case 'r':
                delete_file(filename);
                filename = NULL;
                break;
            case 'e':
                edit_pizza(filename, 'e');
                break;
            case 's':
                printf("Please, specify the file you wish to work with:\n");
                temp = get_filename();
                if(!file_exists(temp, 1)) {
                    printf("This file doesn't exist.\n");
                }
                else {
                    filename = temp;
                }
                break;
            case 'q':
                printf("Goodbye!\n");
                break;
            default:
                printf("Please, enter a valid option (in brackets).\n");
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

// this function is obsolete for this project but is pretty useful
char * nix_filename(char *filename) {
    char *nix_fn = (char *) malloc(sizeof(filename));
    
    strcpy(nix_fn, "\"");
    strcat(nix_fn, filename);
    strcat(nix_fn, "\"");

    free(filename); // filename is obsolete at this point

    return nix_fn;
}

int file_exists(char *filename, short int binary) {
    // return 1 if file exist, otherwise - 0
    FILE *file;
    
    if(binary)
        file = fopen(filename, "rb");
    else
        file = fopen(filename, "r");

    if(file) {
        fclose(file);
        return 1;
    }
    else {
        return 0;
    }
}

void print_pizza(struct Pizza *pizza) {
    short int i;

    printf("\"%s\":\n", pizza->name);
    printf("weight: %dg, diameter: %dcm\n", pizza->weight, pizza->diameter);
    
    printf("ingredients:\n");
    for(i = 0; i < sizeof(pizza->ingredients); i++) {
        if(!strcmp(pizza->ingredients[i], "\0"))
            break;
        printf("%4c %-s\n", '-', pizza->ingredients[i]);
    }
    
    printf("price: $%4.2lf\n", pizza->price);
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

int delete_file(char *filename) {
    FILE *file;

    file = fopen(filename, "rb");
    if(file) {
        printf("WARNING: the specified file will be deleted forever! Continue ");
        
        char opt;
        do {
            printf("(y/N)?: ");
            opt = char_lower(getchar());
            getchar(); // eat up a space
        }
        while(opt != 'y' && opt != 'n');
        
        if(opt  == 'y') {
            int res = remove(filename);
            if(!res) {
                printf("File has successfully been deleted.\n");
            }
            else {
                printf("An error has occurred during file deletion.\n");
            }
            return res;
        }
        else {
            printf("File deletion cancelled.\n");
            return 1;
        }
    }
    else {
        printf("This file doesn't exist.\n");
        return 1;
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
    FILE *file;

    if(mode == 'a' || mode == 'e')
        file = fopen(filename, "rb+");
    else
        file = fopen(filename, "rb");

    if(file) {
        struct Pizza pizza;
 
        short int done = 0, cur_pos = 0, num_pizzas = 0;
        while(fread(&pizza, sizeof(pizza), 1, file))
            num_pizzas++;

        char opt;
        while(!done) {
            system("clear");
            printf("Current directory: %s\n\n", filename); 

            fseek(file, sizeof(pizza) * cur_pos, SEEK_SET);
            if(fread(&pizza, sizeof(pizza), 1, file)) {
                print_pizza(&pizza);
            }
            
            printf("\nUse the up and down arrow keys to navigate the menu.\n");
            printf("Press 'q' to exit.\n");
            if(mode == 'a') printf("Press 'a' to add a new pizza.\n");
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
                        // if(fgetc(file) != EOF) cur_pos++; // old condition (still functional)
                        if(cur_pos < num_pizzas-1) cur_pos++;
                        break;
                }
            }
            else if(opt == 'a' && mode == 'a') { // add a new pizza
                strcpy(pizza.name, "NAME");
                pizza.price = 0.0;
                pizza.diameter = 0;
                pizza.weight = 0;

                fseek(file, 0, SEEK_END);
                fwrite(&pizza, sizeof(pizza), 1, file);

                printf("\nA new pizza has been added to the end of the file.\n");
                printf("(you may edit it by entering edit mode).\n");
                printf("Press any key to continue...\n");
                getchar();
            }
            else if(opt == '\012' && mode == 'e') { // newline ASCII value
                // edit mode
                const char options[10][20] = {
                    "[N]ame",
                    "[I]ngredients",
                    "[P]rice",
                    "[D]iameter",
                    "[W]eight",
                    "[S]ave",
                    "[R]emove",
                    "[C]ancel"
                };

                char opt, arr, del_opt;
                short int i, cursor = 0;
                do {
                    system("clear");
                    print_pizza(&pizza);
                    printf("\nWhat would you like to change?:\n");

                    for(i = 0; i < sizeof(options) / sizeof(options[0]); i++) {
                        if(!strcmp(options[i], "\0")) // break when the end of the array is reached
                            break;
                        else printf("-- %s\n", options[i]);
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
                            do {
                                system("clear");
                                printf("Select the value you wish to change (hit 'ENTER')...\n");
                                printf("Add a new ingredient by pressing 'a'.\n");
                                printf("Return to pizza edit menu by pressing 'q'.\n\n");

                                // list ingredients
                                for(i = 0; i < sizeof(pizza.ingredients) / sizeof(pizza.ingredients[0]); i++) {
                                    if(!strcmp(pizza.ingredients[i], "\0")) break;
                                    else {
                                        printf("- %s", pizza.ingredients[i]);
                                        if(cursor == i) printf(" <<");
                                        printf("\n");
                                    }
                                }
                            
                                arr = getch();
                                if(arr == '\033') { // ESC key
                                    getch(); // skip the '['
                                    switch(getch()) {
                                        case 'A':
                                            if(cursor > 0) cursor--;
                                            break;
                                        case 'B':
                                            if(cursor < i-1) cursor++;
                                            break;
                                    }
                                }
                                else if(arr == '\012') { // newline
                                    printf("New value (modify): ");
                                    fgets(pizza.ingredients[cursor], sizeof(pizza.ingredients[cursor]), stdin);
                                    strcpy(pizza.ingredients[cursor], cut_str(pizza.ingredients[cursor], '\n'));
                                }
                                else if(char_lower(arr) == 'a') {
                                    printf("New value (add): ");
                                    fgets(pizza.ingredients[i], sizeof(pizza.ingredients[i]), stdin);
                                    strcpy(pizza.ingredients[i], cut_str(pizza.ingredients[i], '\n'));
                                }
                            }
                            while(char_lower(arr) != 'q');
                            
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
                            fseek(file, sizeof(pizza) * cur_pos, SEEK_SET);
                            fwrite(&pizza, sizeof(pizza), 1, file);
                            break;
                        case 'r': 
                            printf("WARNING: the pizza '%s' will be deleted forever! Continue ", pizza.name);
                            do {
                                printf("(y/N)?: ");
                                del_opt = char_lower(getchar());
                                getchar(); // eat up a space
                            }
                            while(del_opt != 'y' && del_opt != 'n');
                            
                            if(del_opt == 'y') {
                                // read last pizza
                                fseek(file, sizeof(pizza) * (num_pizzas-1), SEEK_SET);
                                fread(&pizza, sizeof(pizza), 1, file);

                                // copy last pizza to current position
                                fseek(file, sizeof(pizza) * cur_pos, SEEK_SET);
                                fwrite(&pizza, sizeof(pizza), 1, file);

                                // shrink file size
                                num_pizzas--;
                                ftruncate(fileno(file), sizeof(pizza) * num_pizzas);
                                cur_pos = 0; // display first pizza

                                printf("Pizza has successfully been deleted. Press any key to continue...\n");
                                getchar();
                            }
                            else {
                                printf("Pizza deletion cancelled. Press any key to continue...\n");
                                getchar();
                            }
                            break;
                        case 'c':
                            break;
                        default:
                            printf("Please enter a valid option (in brackets).\n");
                            break;
                    }
                }
                while(opt != 's' && opt != 'c' && opt != 'r');
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
