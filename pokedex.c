// Assignment 2 19T3 COMP1511: Pokedex
// pokedex.c
//
// This program was written by Campbell McDonogh (z5259364)
// on 22/1//2019
//
// Version 2.0.0: Release

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Note you are not allowed to use <string.h> in this assignment
// There are some techniques we would like you to learn to code
// that the string library trivialises.

// ----------------------------------------------
// Add any extra #includes your code needs here.
// ----------------------------------------------

#include "pokedex.h"


// ----------------------------------------------
// Add your own #defines here.
// ----------------------------------------------

#define FALSE                      0
#define TRUE                       1
#define bot_ID                     32767


// Note you are not permitted to use arrays in struct pokedex,
// you must use a linked list.
//
// The only exception is that char arrays are permitted for
// search_pokemon and functions it may call, as well as the string
// returned by pokemon_name (from pokemon.h).
//
// You will need to add fields to struct pokedex.
// You may change or delete the head field.

struct pokedex {
    struct pokenode *head;
    struct pokenode *current_cursor;
    struct pokenode *tail;
    int addition;
    int found;

};


// You don't have to use the provided struct pokenode, you are free to
// make your own struct instead.
//
// If you use the provided struct pokenode, you will need to add fields
// to it to store other information.

struct pokenode {
    struct pokenode *next;
    struct pokenode *previous;
    struct pokenode *evolution;
    struct pokenode *previous_evolution;
    Pokemon         pokemon;
    int             found;
};


// ----------------------------------------------
// Add your own structs here.
// ----------------------------------------------


// ----------------------------------------------
// Add prototypes for your own functions here.
// ----------------------------------------------

static void print_name_if_found(struct pokenode *pokenode);

static void print_hidden_pokemon_data(Pokedex pokedex,
    struct pokenode *current_cursor_pokenode);

static int pokemon_id_in_pokedex(Pokedex pokedex, int given_id);

static void get_first_lowest_node(Pokedex pokedex, int lowest_in_new_pokedex);

static int is_found(struct pokenode *current_node);

static int subStringAssess(const char *input1, const char *input2);

static int randomNum(int top, int bot);

static struct pokenode *last_pokenode_in_pokedex(Pokedex pokedex);

static void add_to_end_of_pokedex(struct pokenode *last_pokenode,
    struct pokenode *new_pokenode, Pokemon pokemon);

static void print_pokemon_data(Pokedex pokedex,struct pokenode *current_cursor_pokenode);

static int empy_dex(Pokedex pokedex);

static void printP_evolutions(struct pokenode *current_pokenode,
    int is_found, int evolved_found);

static struct pokenode *id_of_poke(Pokedex pokedex, int given_pokemon_id);

static void poke_found(Pokedex pokedex);

static int strlength(char *name);

static void delete_evolutions(Pokedex pokedex);

static void delete_last_node(Pokedex pokedex, struct pokenode *current);

static void delete_first_node(Pokedex pokedex, struct pokenode *current);

static void delete_nth_node(Pokedex pokedex, struct pokenode *current);

static void delete_only_node(Pokedex pokedex, struct pokenode *current);



// You need to implement the following functions in their stages.
// In other words, replace the lines calling fprintf & exit with your code.
// You can find descriptions of what each function should do in pokedex.h


// Creates a new Pokedex, and returns a pointer to it.
// Note: you will need to modify this function.
Pokedex new_pokedex(void) {
    // Malloc memory for a new Pokedex, and check that the memory
    // allocation succeeded.
    Pokedex new_pokedex = malloc(sizeof (struct pokedex));
    assert(new_pokedex != NULL);
    // Set the head of the linked list to be NULL.
    // (i.e. set the Pokedex to be empty)
    new_pokedex->head = NULL;
    new_pokedex->current_cursor = NULL;
    new_pokedex->tail = NULL;
    new_pokedex->addition = 0;
    new_pokedex->found = 0;
    return new_pokedex;
}


////////////////////////////////////////////////////////////////////////
//                         Stage 1 Functions                          //
////////////////////////////////////////////////////////////////////////

// Add a new Pokemon to the Pokedex.
void add_pokemon(Pokedex pokedex, Pokemon pokemon) {
    // Note: you should use the new_pokenode function here, and add
    // the newly-created pokenode to the end of the linked list of
    // pokenodes in the Pokedex.
    
    struct pokenode *new_pokenode = malloc(sizeof(struct pokenode));
    
    ///If no pokemon have been added
    if(pokedex->head == NULL){ 
        pokedex->head = new_pokenode;
        pokedex->tail = new_pokenode;
        pokedex->current_cursor = new_pokenode;
        new_pokenode->next = NULL;
        new_pokenode->previous = NULL;
        new_pokenode->pokemon = pokemon;
        new_pokenode->found = FALSE;
        pokedex->addition +=1;
        new_pokenode->evolution = NULL;
        new_pokenode->previous_evolution = NULL;
    } else{
        //If already added, show error
        int given_id = pokemon_id(pokemon);
        if (pokemon_id_in_pokedex(pokedex, given_id)){
            fprintf(stderr,"ERROR: pokemon already added.\n");
            exit(1);
        } else {
            struct pokenode *last_pokenode = last_pokenode_in_pokedex(pokedex);
            pokedex->tail = new_pokenode;
            add_to_end_of_pokedex(last_pokenode,new_pokenode,pokemon);
            pokedex->addition +=1;
        }
    }
}

// Print out the details of the currently selected Pokemon.
void detail_pokemon(Pokedex pokedex) {
    struct pokenode *current_cursor_pokenode = pokedex->current_cursor;
    if(current_cursor_pokenode != NULL){
        if (current_cursor_pokenode->found == 0){  
            print_hidden_pokemon_data(pokedex,current_cursor_pokenode);
        } else {
            print_pokemon_data(pokedex,current_cursor_pokenode);
        }
    }
}

// Return the currently selected Pokemon.
Pokemon get_current_pokemon(Pokedex pokedex) {
    //If there are no pokemon..
    if(empy_dex(pokedex)){
        printf("ERROR: There are no pokemon in the pokedex!\n");
        exit(1);
    } else{
        return pokedex->current_cursor->pokemon;
    }
}

// Sets the currently selected Pokemon to be 'found'.
void find_current_pokemon(Pokedex pokedex) {
    if(empy_dex(pokedex)){
        //If there are no pokemon..
        return;
    } else if(pokedex->current_cursor->found != TRUE){
        pokedex->current_cursor->found = TRUE;
        pokedex->found +=1;
    }
}

// Print out all of the Pokemon in the Pokedex.
void print_pokemon(Pokedex pokedex) {
    struct pokenode *current_pokenode = pokedex->head;
    if(current_pokenode != NULL){
        struct pokemon *current_pokemon = current_pokenode->pokemon;
        while (current_pokenode != NULL){
            current_pokemon = current_pokenode->pokemon;
            //Prints current current_cursor
            if(current_pokenode == pokedex->current_cursor){
                printf("--> #");
            } else{
                printf("    #");
            }
            int pokemon_ID = pokemon_id(current_pokemon);
            printf("%03d: ", pokemon_ID);
            print_name_if_found(current_pokenode);
            printf("\n");
            current_pokenode = current_pokenode->next;
        }
    } else  {
        return;
    }
}

////////////////////////////////////////////////////////////////////////
//                         Stage 2 Functions                          //
////////////////////////////////////////////////////////////////////////

// Change the currently selected Pokemon to be the next Pokemon in the Pokedex.
void next_pokemon(Pokedex pokedex) {
    if(empy_dex(pokedex)){
        return;
    } else if(pokedex->current_cursor->next == NULL){
        return;
    }else { 
        pokedex->current_cursor = pokedex->current_cursor->next;
    }
}

// Change the currently selected Pokemon to be the previous Pokemon in the Pokedex.
void prev_pokemon(Pokedex pokedex) {
    if(empy_dex(pokedex)){
        return;
        //exits if the pevious pokemon is not been entered
    } else if(pokedex->current_cursor->previous == NULL){
        return;
    } else{
        pokedex->current_cursor = pokedex->current_cursor->previous;
    }
}

// Change the currently selected Pokemon to be the Pokemon with the ID `id`.
void change_current_pokemon(Pokedex pokedex, int given_pokemon_id) {
    if (pokemon_id_in_pokedex(pokedex,given_pokemon_id)==FALSE){
        //if Pokemon id is not in pokedex..
        return;
    } else{
        pokedex->current_cursor = id_of_poke(pokedex, given_pokemon_id);
    }
}

// Remove the currently selected Pokemon from the Pokedex.
void remove_pokemon(Pokedex pokedex) {
    if(pokedex->current_cursor != NULL){
        struct pokenode *current_pokenode = pokedex->current_cursor;
        if(pokedex->current_cursor != NULL && pokedex->current_cursor->found == TRUE){
            pokedex->found -= 1;
        }
        struct pokenode *pokenode_to_be_removed = current_pokenode; 
        if(pokedex->current_cursor == NULL){
            //Ends if no pokemon in pokedex
            return;
        } else if (current_pokenode->next == NULL && 
                   current_pokenode->previous == NULL) {
            delete_only_node(pokedex, current_pokenode);
        } else if (current_pokenode->next == NULL) {
            delete_last_node(pokedex, current_pokenode);
        } else if (current_pokenode->previous == NULL) {
            delete_first_node(pokedex, current_pokenode); 
        } else {
            delete_nth_node(pokedex, current_pokenode);
        } 
        delete_evolutions(pokedex);    
        destroy_pokemon(pokenode_to_be_removed->pokemon);
        free(pokenode_to_be_removed);  
        pokedex->addition -= 1;
    } 
}

// Destroy the given Pokedex and free all associated memory.
void destroy_pokedex(Pokedex pokedex) {
    struct pokenode *current_pokenode = pokedex->head;
    //Moves through all nodes until last
    if(current_pokenode != NULL){ 
        while(current_pokenode->next != NULL){
            pokedex->current_cursor = current_pokenode;
            struct pokenode *next_pokenode = current_pokenode->next;
            destroy_pokemon(current_pokenode->pokemon);
            free(current_pokenode);
            current_pokenode = next_pokenode;
        }
        destroy_pokemon(current_pokenode->pokemon);
        free(current_pokenode);
    }
    free(pokedex);
    return;
}


////////////////////////////////////////////////////////////////////////
//                         Stage 3 Functions                          //
////////////////////////////////////////////////////////////////////////

// // Print out all of the different types of Pokemon in the Pokedex.
void show_types(Pokedex pokedex){
    //create an array to store types 
    //check if something exists in the array already
    //loop through each pokedex node and parse array
    //pass to check function
    //need to check this malloc for memeroy sufficeny
    
    pokemon_type *arraytype;
    arraytype = calloc(100, sizeof(pokemon_type));
    int array_len = 0;

    //needs to be changed the length of the pokedex
    //error must have pointer to object type
    // for (int i=0; i<100; i++){
    //     arraytype[i] = NULL;
    // }


    struct pokenode *current = (pokedex->head);
    while (current != NULL){
        int addFirst = 1;
        int addSecond = 1;
        pokemon_type first_type = pokemon_first_type(current->pokemon);
        pokemon_type second_type = pokemon_second_type(current->pokemon);
        if (second_type == NONE_TYPE) {
            addSecond = 0;
        }
        for (int i = 0; i < array_len; i++) {
            if (arraytype[i] == first_type) {
                addFirst = 0;
            }
            if (arraytype[i] == second_type) {
                addSecond = 0;
            }
        }
        if (addFirst) {
            arraytype[array_len] = first_type;
            array_len++;
            // printf(pokemon_type_to_string(first_type));
            // printf("\n");
        }
        if (addSecond) {
            arraytype[array_len] = second_type;
            array_len++;
            // printf(pokemon_type_to_string(second_type));
            // printf("\n");
        }
        //checkarray(arraytype, pokemon_first_type(current->pokemon), pokemon_second_type(current->pokemon));
        current = current->next;
    }
    free(arraytype);
    return;
}

// void checkarray(pokemon_type * array, pokemon_type newtype, pokemon_type secondtype){

//     int threshhold = sizeof(array)/sizeof(pokemon_type);
//     int sentinal = 0;
//     int newtypein = 0;
//     int secondtypein = 0;

//     if(pokemon_second_type == NONE_TYPE){
//         secondtype = 1;
//     }

//     while(sentinal < threshhold){
//         if (array[sentinal] == newtype){
//             newtypein = 1;
//         }
//         if (array[sentinal] == secondtype){
//             secondtype == 1;
//         }
//     }

//     //if not found in array append to the array
//     if (newtype == 0){
//         for(int i = 0; i < threshhold; i++){
//             if(array[i] == NULL){
//                 array[i] = newtype;
//             }
//         }
//     }

//     if (secondtype == 0){
//         for(int i = 0; i < threshhold; i++){
//             if(array[i] == NULL){
//                 array[i] = secondtype;
//             }
//         }
//     }

//     return NULL;
// }



// Set the first not-yet-found Pokemon of each type to be found.
void go_exploring(Pokedex pokedex) {
    int root = 0;
    int factor = 0;
    int how_many = 0;
    int possible_pokemon = FALSE;
    int i = 0;

    
    for(i = 0; i < factor; i++){
        if(pokemon_id_in_pokedex(pokedex, i)){
            possible_pokemon = TRUE;
            break;
        } 
    }

    int found = 0;
    if(possible_pokemon != TRUE){
        printf("Error: no pokemon within range 0 and factor - 1 \n");
        exit(1);
    } else{
        srand(root);
        while(found < how_many){
            //Gets randon Pokemon ID
            int pokemon_found_id = randomNum(1,factor-1);
            if (pokemon_id_in_pokedex(pokedex, pokemon_found_id)){
                struct pokenode *pokemon_id_node = id_of_poke(pokedex, pokemon_found_id);
                pokemon_id_node->found = TRUE;
                pokedex->found +=1;
                how_many +=1;
            }
        }
    }
}

// Return the total number of Pokemon in the Pokedex.
int count_found_pokemon(Pokedex pokedex) {
    return pokedex->found;
}

// Return the number of Pokemon in the Pokedex that have been found.
int count_total_pokemon(Pokedex pokedex) {
    return pokedex->addition;
}

////////////////////////////////////////////////////////////////////////
//                         Stage 4 Functions                          //
////////////////////////////////////////////////////////////////////////


// Add the information that the Pokemon with the ID `from_id` can
// evolve into the Pokemon with the ID `to_id`.
void add_pokemon_evolution(Pokedex pokedex, int from_id, int to_id) {
    struct pokenode *from_evolution = NULL;
    struct pokenode *becomeEvolved = NULL;
    int exisitingFirstId = FALSE;
    int exisitingSecondId = FALSE;
    //If ID matches = no evolution
    if (from_id == to_id){
        fprintf(stderr,"A pokemon cannot evolve into itself \n");
        exit(1);
    }
    if (pokemon_id_in_pokedex(pokedex,from_id) == TRUE){
        from_evolution  = id_of_poke(pokedex, from_id);
        exisitingFirstId = TRUE;
    } else {
        fprintf(stderr,"from_id does not exist \n");
        exit(1);
    }
    if (pokemon_id_in_pokedex(pokedex,to_id) == TRUE){
        becomeEvolved  = id_of_poke(pokedex, to_id);
        exisitingSecondId = TRUE; 
    } else {
        fprintf(stderr, "to_id does not exist \n");
        exit(1);
    }
    if(exisitingFirstId && exisitingSecondId){
        from_evolution->evolution = becomeEvolved;
        becomeEvolved->previous_evolution = from_evolution;
    }
}

// Show the evolutions of the currently selected Pokemon.
void show_evolutions(Pokedex pokedex) {
    struct pokenode *current_pokenode = pokedex->current_cursor;
    while(current_pokenode!=NULL){
        int is_found = current_pokenode->found;
        int evolved_found =0;
        if(current_pokenode->evolution != NULL){
            evolved_found = current_pokenode->evolution->found;
        }
        printP_evolutions(current_pokenode,is_found,evolved_found);
        current_pokenode = current_pokenode->evolution;
    }
}

// Return the pokemon_id of the Pokemon that the currently selected
// Pokemon evolves into.
int get_next_evolution(Pokedex pokedex) {
    struct pokenode *current_pokenode = pokedex->current_cursor;
    //While there are no pokemon..
    if(pokedex->head == NULL){
        fprintf(stderr, "exiting because the pokedex is empty\n");
        exit(1);    
    } else if(current_pokenode->evolution == NULL){
        return DOES_NOT_EVOLVE;
    }
    else{
        return pokemon_id(current_pokenode->evolution->pokemon);
    }

}

////////////////////////////////////////////////////////////////////////
//                         Stage 5 Functions                          //
////////////////////////////////////////////////////////////////////////

// Create a new Pokedex which contains only the Pokemon of a specified
// type from the original Pokedex.
Pokedex get_pokemon_of_type(Pokedex pokedex, pokemon_type type) {

    return NULL;
}

// Create a new Pokedex which contains only the Pokemon that have
// previously been 'found' from the original Pokedex.
Pokedex get_found_pokemon(Pokedex pokedex) {

    return NULL;
}

// Create a new Pokedex containing only the Pokemon from the original
// Pokedex which have the given string appearing in its name.
Pokedex search_pokemon(Pokedex pokedex, char *text) {

    return NULL;
}



/////////////////////////// 


// Add definitions for your own functions here.
// Make them static to limit their scope to this file.

//Test if in pokedex
static int empy_dex(Pokedex pokedex){
    if (pokedex->current_cursor == NULL){
        return TRUE;
    } else{
        return FALSE;
    }
}


//given two characters ie 'A' and 'a', check if they are the same letter. 
static int char_indentical(char input1, char input2){
    if(input1 == input2){
        return TRUE;
    } else if (input1 + 32 == input2){
        return TRUE;
    } else if(input1 == input2+32){
        return TRUE;
    } else{
        return FALSE;
    } 
    return FALSE;
}

//Deletes and reset evolutions
static void delete_evolutions(Pokedex pokedex){
    if(pokedex->current_cursor == NULL){
        return;
    }

    struct pokenode *evolution_current_node = pokedex->head;

    int current_cursor_id = pokemon_id(pokedex->current_cursor->pokemon);

    while(evolution_current_node != NULL){
        if(evolution_current_node->evolution != NULL){
            int evolution_id = 
                pokemon_id(evolution_current_node->evolution->pokemon);
            if(current_cursor_id == evolution_id){
                evolution_current_node->evolution = NULL;
            }
        }
        evolution_current_node = evolution_current_node->next;
    }
}

//check if a string input2 is a substring of string input1
static int subStringAssess(const char *input1, const char *input2){

    int charLength = strlength((char *)input1);
    int input2_length = strlength((char *)input2);
    int input2_found = 0;

    for(int i = 0; i < charLength; i++){
        if(input2_found == input2_length){
            return TRUE;
        }
        if(char_indentical(input1[i], input2[input2_found])){
            input2_found += 1;
        } else if(char_indentical(input1[i], input2[0])){
            input2_found = 1;
        }else{
            input2_found = 0;
        }
    }

    if(input2_found == input2_length){
        return TRUE;
    } else{
        return FALSE;
    }
}   

//Set all to found

static void poke_found(Pokedex pokedex){
    struct pokenode *current_node = pokedex->head;

    while(current_node != NULL){
        if(!(current_node->found)){
            current_node->found = TRUE;
            pokedex->found +=1;
        }
        current_node = current_node->next;
    }
}

//Is current pokemon found?
static int is_found(struct pokenode *current_node){

    if(current_node->found == 1){
        return TRUE;
    } else{
        return FALSE;
    }
}



//Deletes last pokemode
static void delete_last_node(Pokedex pokedex, struct pokenode *current){

    if (current->previous != NULL){
        pokedex->current_cursor = current->previous;
        current->previous->next = NULL;   
    } else{
        pokedex->current_cursor = NULL;
    }
    pokedex->current_cursor = current->previous;
}

//Delets first pokemon
static void delete_first_node(Pokedex pokedex, struct pokenode *current){

    pokedex->head = current->next;
    pokedex->current_cursor = current->next;
    current->next->previous = NULL;
}  

//Deletes given pokemon in pokedex
static void delete_nth_node(Pokedex pokedex, struct pokenode *current){

    current->previous->next = current->next;
    current->next->previous = current->previous;
    pokedex->current_cursor = current->next;
}

//Deletes single node if its only node
static void delete_only_node(Pokedex pokedex, struct pokenode *current){

    pokedex->head = NULL;
    pokedex->tail = NULL;
    pokedex->current_cursor = NULL;
}


//Provides pokemon data if not found 
static void print_hidden_pokemon_data(Pokedex pokedex,
    struct pokenode *current_cursor_pokenode){

        struct pokemon *current_cursor_pokemon = current_cursor_pokenode->pokemon;

        int pokemon_ID = pokemon_id(current_cursor_pokemon);

        printf("ID: %03d\n",pokemon_ID);
        printf("Name: ");
        print_name_if_found(current_cursor_pokenode);
        printf("\nHeight: --\nWeight: --\nType: --\n");
        
}

//Checks if already added, loops through all nodes
static int pokemon_id_in_pokedex(Pokedex pokedex, int given_id){
    struct pokenode *current_pokenode = pokedex->head;
    while (current_pokenode != NULL){
        if (pokemon_id(current_pokenode->pokemon) == given_id){
            return TRUE;
        }
        current_pokenode = current_pokenode->next;
    }
    return FALSE;
}

//Prints found pokemon
static void print_name_if_found(struct pokenode *pokenode){
    if(pokenode->found){
        printf("%s",pokemon_name(pokenode->pokemon));
    } else{
        int name_length = strlength(pokemon_name(pokenode->pokemon));
        for (int i =0; i < name_length; i++){
            printf("*");
        }
    }
}

//Fins given pokenode and returns pointer
struct pokenode *last_pokenode_in_pokedex(Pokedex pokedex){
    struct pokenode *current_pokenode = pokedex->head;
    while(current_pokenode->next != NULL){
        current_pokenode = current_pokenode->next;
    }
    return current_pokenode;
}

//Adds note to end of pokedex
static void add_to_end_of_pokedex(struct pokenode *last_pokenode,
        struct pokenode *new_pokenode ,Pokemon pokemon){
    last_pokenode->next = new_pokenode;
    new_pokenode->previous = last_pokenode;
    new_pokenode->next = NULL;
    new_pokenode->found = FALSE;
    new_pokenode->pokemon = pokemon;
    new_pokenode->evolution = NULL;
    new_pokenode->previous_evolution = NULL;
}

//Random number
static int randomNum(int top, int bot) {
    return rand() % (bot - top + 1) + top;
}

//Finds length of name
static int strlength(char *name) {
    int i = 0;
    while(name[i] != '\0'){
        i +=1;
    }
    return i;
}


//Matchs node to pokemon ID
static struct pokenode *id_of_poke(Pokedex pokedex, int given_pokemon_id){
    struct pokenode *current_pokenode = pokedex->head;
    while (current_pokenode != NULL){
        if (pokemon_id(current_pokenode->pokemon) == given_pokemon_id){
            return current_pokenode;
        }
        current_pokenode = current_pokenode->next;
    }
    return NULL;
}



//Prints pokemon data
static void print_pokemon_data(Pokedex pokedex,struct pokenode *current_cursor_pokenode){
    struct pokemon *current_cursor_pokemon = current_cursor_pokenode->pokemon;
    int pokemon_ID = pokemon_id(current_cursor_pokemon);
    const char *name = pokemon_name(current_cursor_pokemon);
    double height = pokemon_height(current_cursor_pokemon);
    double weight = pokemon_weight(current_cursor_pokemon);
    const char *first_type = 
        pokemon_type_to_string(pokemon_first_type(current_cursor_pokemon));
    const char *second_type = 
        pokemon_type_to_string(pokemon_second_type(current_cursor_pokemon));
        
    printf("ID: %03d\n",pokemon_ID);
    printf("Name: %s\n",name);
    printf("Height: %01.01lfm\n",height);
    printf("Weight: %01.01lfkg\n",weight);
    printf("Type: %s",first_type);

    if (pokemon_type_from_string((char *)second_type) != NONE_TYPE){
        printf(" %s \n",second_type);
    } else{
        printf("\n");
    }
}

//Prints evolution
static void printP_evolutions(struct pokenode *current_pokenode, 
                              int is_found, int evolved_found){

    int from_id = pokemon_id(current_pokenode->pokemon);

    const char *from_first_type = 
        pokemon_type_to_string(pokemon_first_type(current_pokenode->pokemon));

    const char *from_second_type = 
        pokemon_type_to_string(pokemon_second_type(current_pokenode->pokemon));
   
    const char *from_name = 
        pokemon_name(current_pokenode->pokemon);

    //Prints possible evolutions
    if(is_found){
        printf("#%03d %s [%s", from_id, from_name, from_first_type);
        if(pokemon_type_from_string((char *)from_second_type) != NONE_TYPE){
            printf(", %s]", from_second_type);
        } else {
            printf("]");
        }
    } else {
        printf("#%03d ???? [????]",from_id);
    }
    if(current_pokenode->evolution != NULL){
        printf(" --> ");
    } else{
        printf("\n");
    }
}

