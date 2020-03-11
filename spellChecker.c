/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Andrew Helmsworth
 * Date: 2020/02/18
 * Sources: See Works Cited at end of file
 */

//Inclusions
#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Recursively computes Levenshtein distance.
 * @param pointer to char - stringA - string being compared
 * @param int - lengthOfA - length of string being compared
 * @param pointer to char - stringB - string from dictionary
 * @param int - lengthOfB - length of string from dictionary
 */
//CITATION: Adapted from RosettaCode, Wikipedia - see Works Cited at end of file
int levenshtein(char* stringA, int lengthOfA, char* stringB, int lengthOfB) {
    //Variable declarations
    int editOne;
    int editTwo;
    int editThree;

    //Check if A is empty
    if (!lengthOfA) {
        return lengthOfB;
    }
    //Check if B is empty
    if (!lengthOfB) {
        return lengthOfA;
    }

    //Last letters are the same so difference is whatever is req'd to edit the rest
    if (stringA[lengthOfA - 1] == stringB[lengthOfB - 1]) {
        return levenshtein(stringA, lengthOfA - 1, stringB, lengthOfB - 1);
    }

    //Try removing last letter of A
    editOne = levenshtein(stringA, lengthOfA, stringB, lengthOfB - 1);
    //Try changing last letter of A to that of B
    editTwo = levenshtein(stringA, lengthOfA - 1, stringB, lengthOfB - 1);
    //Try removing last letter of B
    editThree = levenshtein(stringA, lengthOfA - 1, stringB, lengthOfB);

    //Get the smallest of the edits
    if (editOne > editTwo) {
        editOne = editTwo;
    }
    if (editOne > editThree) {
        editOne = editThree;
    }

    //Return smallest plus one for the edit
    return editOne + 1;
}

/**
 * Iteratively computes Levenshtein distance.
 * @param pointer to char - stringA - string being compared
 * @param int - lengthOfA - length of string being compared
 * @param pointer to char - stringB - string from dictionary
 * @param int - lengthOfB - length of string from dictionary
 */
 //CITATION: Adapted from RosettaCode, Wikipedia - see Works Cited at end of file
int levenshteinIter(char* stringA, int lengthOfA, char* stringB, int lengthOfB) {
    
    //Variable declarations
    int levenCost;
    int editOne;
    int editTwo;
    int editThree;
    int finalCost;
    int i;
    int j;
    int k;
    int x;
    int y;
    int z;

    //Make a new array matrix
    int** levenArr = malloc(sizeof(int*) * (lengthOfA + 1));
    assert(levenArr != 0);

    //Populate matrix with space for ints
    for (i = 0; i < lengthOfA; i++) {
        levenArr[i] = malloc(sizeof(int) * (lengthOfB + 1));
        assert(levenArr[i] != 0);
    }

    //Set 0 along y axis
    for (j = 0; j < lengthOfA; j++) {
        levenArr[j][0] = j;
    }

    //Set 0 along x axis
    for (k = 0; k < lengthOfB; k++) {
        levenArr[0][k] = k;
    }

    //Loop over the x axis
    for (x = 1; x < lengthOfB; x++) {
        //Loop over the y axis
        for (y = 1; y < lengthOfA; y++) {
            if (stringA[y - 1] == stringB[x - 1]) {
                //Last letters are the same
                levenCost = 0;
            }
            else {
                levenCost = 1;
            }

            //Try three types of edits
            editOne = (levenArr[y - 1][x]) + 1;
            editTwo = (levenArr[y][x - 1]) + 1;
            editThree = (levenArr[y - 1][x - 1]) + levenCost;

            //Get the smallest
            if (editOne > editTwo) {
                editOne = editTwo;
            }
            if (editOne > editThree) {
                editOne = editThree;
            }
            //Set the smallest
            levenArr[y][x] = editOne;
        }
    }
    //Calc final cost
    finalCost = levenArr[lengthOfA - 1][lengthOfB - 1];

    //Free the 2dArr
    for (z = 0; z < lengthOfA; z++) {
        free(levenArr[z]);
    }

    //Return final cost
    return finalCost;
}

/**
 * Sets Levenshtein for each word in the dictionary.
 * @param pointer to hashMap - incMap - dictionary
 * @param pointer to char - comparisonWord - word being compared
 */
HashMap* walkThroughLevenshtein(HashMap* incMap, char* comparisonWord) {
    
    //Variable declarations
    HashLink* currentLink;
    HashLink* newLink;
    HashMap* newMap;
    int i;
    int j;
    int levValue;
    char* dictString;

    // FIXED: implement
    //Make new map, assert not null
    newMap = hashMapNew(5);
    assert(newMap != 0);

    //Loop over each bucket in map
    for (i = 0; i < incMap->capacity; i++) {

        //Get link at bucket
        currentLink = incMap->table[i];

        //If bucket not empty
        if (currentLink != NULL) {

            //While bottom of bucket not reached
            while (currentLink != NULL) {
                
                //Get the key
                dictString = currentLink->key;

                //Calc Levenshtein value
                levValue = levenshtein(comparisonWord, (strlen(comparisonWord) + 0), dictString, (strlen(currentLink->key)+0));
                currentLink->value = levValue;

                //Loop over the 5 buckets in newMap
                for (j = 0; j < 5; j++) {
                    if (newMap->table[j] != NULL) {
                        //There was a value in the bucket
                        if (levValue < newMap->table[j]->value) {
                            //Value in bucket greater than new value
                            //Free the old key and link
                            free(newMap->table[j]->key);
                            free(newMap->table[j]);

                            //Make a new link, check not null
                            newLink = malloc(sizeof(struct HashLink));
                            assert(newLink != 0);

                            //Make a new key, check not null
                            newLink->key = malloc(sizeof(char) * (strlen(currentLink->key) + 1));
                            assert(newLink->key != 0);

                            //Copy old key to new link
                            strcpy(newLink->key, currentLink->key);

                            //Set value and next
                            newLink->value = currentLink->value;
                            newLink->next = NULL;

                            //Set bucket index to new link and exit loop
                            newMap->table[j] = newLink;
                            j = 100;
                        }
                    }
                    else {
                        //No link in bucket
                        //Make a new link, check not null
                        newLink = malloc(sizeof(struct HashLink));
                        assert(newLink != 0);

                        //Make space for key, check not null
                        newLink->key = malloc(sizeof(char) * (strlen(currentLink->key) + 1));
                        assert(newLink->key != 0);

                        //Copy in new key and value, set next to null
                        strcpy(newLink->key, currentLink->key);
                        newLink->value = currentLink->value;
                        newLink->next = NULL;

                        //Set bucket to new link, exit loop
                        newMap->table[j] = newLink;
                        j = 100;
                    }
                }
                //Advance currentLink
                currentLink = currentLink->next;
            }
        }
    }
    //Return finished suggestion map
    return newMap;
}

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
int maxLength = 16;
int length = 0;
char* word = malloc(sizeof(char) * maxLength);
while (1)
{
    char c = fgetc(file);
    if ((c >= '0' && c <= '9') ||
        (c >= 'A' && c <= 'Z') ||
        (c >= 'a' && c <= 'z') ||
        c == '\'')
    {
        if (length + 1 >= maxLength)
        {
            maxLength *= 2;
            word = realloc(word, maxLength);
        }
        word[length] = c;
        length++;
    }
    else if (length > 0 || c == EOF)
    {
        break;
    }
}
if (length == 0)
{
    free(word);
    return NULL;
}
word[length] = '\0';
return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    //Variable declarations
    int keepLoading;
    char* currentWord;

    //Check map and file not null
    assert(file != NULL);
    assert(map != NULL);

    keepLoading = 1;

    // FIXED: implement
    //Keep looping over nextWord until map filled
    while (keepLoading == 1) {
        currentWord = nextWord(file);
        if (currentWord != NULL) {
            //Put the next word in the dictionary
            hashMapPut(map, currentWord, -1);
            free(currentWord);
        }
        else {
            //Reached end of file
            keepLoading = 0;
        }
    }
}

/**
 * Loops until valid string input
 * @param pointer to char - input buffer
 */
void getString(char* inputBuffer) {
    //Variable declarations
    int invalid;
    int i;
    int foundSpace;
    int foundNonAlpha;

    //Set initial conditions
    invalid = 1;
    foundSpace = 0;
    inputBuffer[0] = 0;
    foundNonAlpha = 1;

    //Loop until valid input
    while (invalid == 1) {
        //GetLine, nothing I can think of that would throw errors here
        fgets(inputBuffer, 250, stdin);
        //Reject if bad input size
        if ((strlen(inputBuffer) == 1) || (strlen(inputBuffer) > 250)) {
            //String too short or long
            printf("Invalid string length. Please try again: ");
        }
        else {
            //Check for spaces
            for (i = 0; i < 250; i++) {
                if (inputBuffer[i] == ' ') {
                    //Space found, exiting loop early
                    foundSpace = 1;
                    //i = 500;
                    inputBuffer[i] = '\0';
                }
            }

            if (foundSpace == 1) {
                //Loop over all possible indices
                for (i = 0; i < 250; i++) {
                    //Clear input, since space found
                    inputBuffer[i] = '\0';
                }
            }

            if (foundSpace == 1) {
                //Space was found, retry
                printf("Spaces found in input. Please try again: ");
                foundSpace = 0;
            }
            else {
                //No spaces found
                for (i = 0; i < strlen(inputBuffer) - 1; i++) {
                    //Check for nonalpha
                    foundNonAlpha = isalpha(inputBuffer[i]);
                    if (foundNonAlpha == 0) {
                        //Found nonalpha, exiting loop
                        i = 500;
                    }
                }

                if (foundNonAlpha == 0) {
                    //Loop over all possible indices
                    for (i = 0; i < 250; i++) {
                        //Clear input, since space found
                        inputBuffer[i] = '\0';
                    }
                    //reset foundNonAlpha
                    foundNonAlpha = 1;
                    printf("Non-alphabetic characters found in input. Please try again: ");
                }
                else {
                    //Valid input
                    printf("Input accepted.\n\n");

                    //Set null terminator
                    inputBuffer[(strlen(inputBuffer)) - 1] = '\0';

                    //Convert to lower case
                    for (i = 0; i < strlen(inputBuffer); i++) {
                        inputBuffer[i] = tolower(inputBuffer[i]);
                    }

                    //Set input valid now
                    invalid = 0;
                }
            }
        }
    }
}

/**
 * Checks the spelling of the word provded by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    //Variable declarations
    int* returnedDictVal;
    HashMap* map;
    HashMap* levenMap;
    int i;

    // FIXED: implement
    //Make new map, check not null
    map = hashMapNew(1000);
    assert(map != 0);

    //Load dictionary
    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;

    //Loop until quit entered
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        //scanf("%s", inputBuffer);

        getString(inputBuffer);

        // Implemented the spell checker code here..
        if (strcmp(inputBuffer, "quit") == 0)
        {
            printf("You chose to quit. Terminating.");
            //Quit was selected
            quit = 1;
        }
        else {
            //Try checking the word
            returnedDictVal = hashMapGet(map, inputBuffer);

            if (returnedDictVal != NULL) {
                //Word spelled correctly
                printf("The inputted word %s is spelled correctly\n\n", inputBuffer);
            }
            else {
                //Not found in dict
                printf("The inputted word %s is spelled incorrectly\n\n", inputBuffer);
                printf("Generating suggesstions (this may take a while)...\n\n");

                //Populate suggestion map
                levenMap = walkThroughLevenshtein(map, inputBuffer);

                printf("Did you mean...?\n\n");

                //Print suggestions
                for (i = 0; i < 5; i++) {
                    printf("Suggestion #%d: ", i);
                    if (levenMap->table[i] != NULL) {
                        printf("%s\n\n", levenMap->table[i]->key);
                        //Free old key and link
                        free(levenMap->table[i]->key);
                        free(levenMap->table[i]);
                    }
                }
                //Free table and map
                free(levenMap->table);
                free(levenMap);
            }
        }
    }
    //Free old map
    hashMapDelete(map);
    return 0;
}

/* WORKS CITED

RosettaCode contributors, "Levenshtein distance," RosettaCode, https://rosettacode.org/wiki/Levenshtein_distance (accessed March 11, 2020).

Wikipedia contributors, "Levenshtein distance," Wikipedia, The Free Encyclopedia, https://en.wikipedia.org/w/index.php?title=Levenshtein_distance&oldid=944604589 (accessed March 11, 2020).

*/