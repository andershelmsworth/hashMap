#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/**
 * Loads the contents of the file into the hash map.
 * @param pointer to char - word
 * @param map
 */

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

HashMap* walkThroughLevenshtein(HashMap* incMap, char* comparisonWord) {
    //Variable declarations
    HashLink* currentLink;
    HashMap* newMap;
    int i;
    int j;
    int levValue;
    //int lengthOfDictWord;
    //int suggestionIterator;
    char* dictString;

    // FIXME: implement
    newMap = hashMapNew(5);
    assert(newMap != 0);

    suggestionIterator = 1;

    for (i = 0; i < incMap->capacity; i++) {

        currentLink = incMap->table[i];

        if (currentLink != NULL) {

            while (currentLink != NULL) {
                
                dictString = currentLink->key;

                levValue = levenshtein(comparisonWord, (strlen(comparisonWord) + 0), dictString, (strlen(currentLink->key)+0));
                currentLink->value = levValue;

                for (j = 0; j < 5; j++) {
                    if (newMap->table[j] != NULL) {
                        if (levValue < newMap->table[j]->value) {
                            newMap->table[j] = currentLink;
                            j = 100;
                        }
                    }
                    else {
                        newMap->table[j] = currentLink;
                        j = 100;
                    }
                }

                currentLink = currentLink->next;
            }
            int x = 1;
        }
        int y = 2;
    }
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

    // FIXME: implement
    map = hashMapNew(1000);
    assert(map != 0);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;
    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        // Implement the spell checker code here..
        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
        else {
            returnedDictVal = hashMapGet(map, inputBuffer);

            if (returnedDictVal != NULL) {
                printf("Spelled correctly.\n\n");
            }
            else {
                printf("Incorrectly spelled.\n\n");

                levenMap = walkThroughLevenshtein(map, inputBuffer);

                for (i = 0; i < 5; i++) {
                    printf("Suggestion #%d: ", i);
                    if (levenMap->table[i] != NULL) {
                        printf("%s\n\n", levenMap->table[i]->key);
                    }
                }

                hashMapDelete(levenMap);

            }
        }
    }

    hashMapDelete(map);
    return 0;
}
