// TODO:
/*
    New option: change a pizza from a file;
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH "/home/pesho/Documents/My Programs/C/TUES/pizza_file_system/files/"

typedef struct {
    char name[50];
    char ingredients[20][60];
    double price; // in USD
    int diameter; // in cm
    int weight; // in grams
} pizza_t;

char char_lower(char);
char * str_lower(char *);
char * cut_str(char *, char);
char * get_filename();
char * nix_filename(char *);

int create_file(char *);
int delete_file(char *);
int display_pizzas(char *);
int add_pizza(char *);

int main() {
    const char options[10][50] = {
        "[D]isplay pizzas",
        "[A]dd pizza",
        "[C]reate file",
        "[Q]uit"
    };

    char inp[4];
    char *opt, *filename;
    short int i;
    do {
        for(i = 0; i < sizeof(options) / sizeof(options[0]); i++) {
            if(!strcmp(options[i], "\0")) // break when the end of the array is reached
                break;
            else printf("~ %s\n", options[i]);
        }
        
        printf("\n:");
        fgets(inp, sizeof(inp), stdin);
        opt = str_lower(cut_str(inp, '\n'));

        if(!strcmp(opt, "d")) {
            printf("Please, specify the path of the file from which you wish to read data:\n");
            filename = get_filename();
            printf("\n");
            display_pizzas(filename);
        }
        else if(!strcmp(opt, "a")) {
            printf("Please, specify the path of the file in which you wish to add a pizza:\n");
            filename = get_filename();
            printf("\n");
            add_pizza(filename);
        }
        else if(!strcmp(opt, "c")) {
            printf("Please, specify the name of the file you wish to create:\n");
            filename = get_filename();
            printf("\n");
            create_file(filename);
        }
        else if(!strcmp(opt, "q")) {
            printf("Goodbye!\n");
        }
        else {
            printf("Please enter a valid option (in brackets).\n");
        }

        printf("\nPress any key to continue...\n");
        getchar();
        system("clear");
    }
    while(strcmp(opt, "q"));
    
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

int display_pizzas(char *filename) {
    FILE *file;

    file = fopen(filename, "rb");
    if(file) {
        pizza_t pizza;
        short int i = 0;
        while(!feof(file)) {
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
            
            printf("\n");
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
        pizza_t pizza;
        
        char ingr[sizeof(pizza.ingredients[0])];
        short int done = 0, i;
        char opt;
        while(!done) {
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

