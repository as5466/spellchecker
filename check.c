#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

typedef struct node {
    char *word;
    struct node *next;
} Node;

typedef struct HashTable {
        int size;
        Node **table;
    } HashTable;

HashTable *initializeHashTable(int numWords) {
    HashTable *hashtable = malloc(sizeof(HashTable));
    hashtable->size = numWords/50;
    hashtable->table = malloc(sizeof(Node *)*hashtable->size);
    for (int i=0;i<hashtable->size;i++) {
        hashtable->table[i] = NULL;
    }
    return hashtable;
}

Node *initializeNode(char *word, Node *next) {
    Node *newnode = malloc(sizeof(Node));
    newnode->word = malloc(sizeof(char)*(strlen(word)+1));
    strcpy(newnode->word, word);
    newnode->next = next;
    return newnode;
}

int hash(char *word, int size) {
    unsigned long long int total=0;
    for (int i=0;i<strlen(word);i++) {
        total = total + word[i];
        total = total*101;
    }
    return (total % size);
}

int isMember(HashTable *hashTable, char *word) {
    int position = hash(word, hashTable->size);
    Node *ptr = hashTable->table[position];
    while (ptr!=NULL) {
        if (strcmp(ptr->word, word) == 0) {
            return 1;
        }
        else {
            ptr = ptr->next;
        }
    }
    return 0;
}

void insert(HashTable *hashTable, char *word) {
    int position = hash(word, hashTable->size);
    Node *ptr = hashTable->table[position];
    while (ptr!=NULL) {
        if (strcmp(ptr->word, word) == 0) {
            return ;
        }
        else {
            ptr = ptr->next;
        }
    }
    hashTable->table[position] = initializeNode(word, hashTable->table[position]);
}

void printHashTable(HashTable *hashTable) {
    int max = 0;
    int min = 100000;
    int count = 0;
    for (int i=0; i<hashTable->size; i++) {
        Node *ptr = hashTable->table[i];
        while (ptr!=NULL) {
            ptr = ptr->next;
            count++;
        }
        if (count > max) {
            max = count;
        }
        if (count < min) {
            min = count;
        }
        count = 0;
    }
    printf("max elements: %d\nminelements: %d\n", max, min);
}

char *swap(char *word, int left, int right) {
    char temp = word[left];
    word[left] = word[right];
    word[right] = temp;
    return word;
}

void deleteHashTable(HashTable *hashTable) {
    Node *ptr = hashTable->table[0];
    Node *temp = hashTable->table[0];
    for (int i=0;i<hashTable->size;i++) {
        ptr = hashTable->table[i];
        temp = hashTable->table[i];
        while (ptr != NULL) {
            temp = ptr;
            ptr = ptr->next;
            free(temp->word);
            free(temp);
        }
    }
    free(hashTable->table);
    free(hashTable);   
}

int main(int argc, char **argv)
{
	char *dictionaryFilePath = argv[1]; //this keeps the path to the dictionary file 
	char *inputFilePath = argv[2]; //this keeps the path to the input text file
	char *check = argv[3]; // this keeps the flag to whether we should insert mistyped words into dictionary or ignore
	int numOfWords=0; 

	
    FILE *fp = fopen(dictionaryFilePath, "r");
    char *line = NULL; 
    size_t lineBuffSize = 0; 
    ssize_t lineSize; 

    if(fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
        numOfWords++;

    HashTable *hashTable = initializeHashTable(numOfWords);
    
    fseek(fp, 0, SEEK_SET);

    char wrd[BUFSIZE];

    for (int i = 0; i < numOfWords; i++)
    {
        fscanf(fp, "%s \n", wrd);
        insert(hashTable, wrd);
    }
    fclose(fp);
    
    fp = fopen(inputFilePath, "r");
	
	if(fp == NULL)
	{
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

	int noTypo=1;
    int printNoTypo = 1;

	while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
	{
		char *word;
		const char delimiter[]= " ,.:;!\n";

		word = strtok(line,delimiter); 
		
		while(word!=NULL)
		{
            if (isMember(hashTable, word) == 0) {
              printf("Misspelled word: %s\n",word); 
              noTypo = 0; 
              printNoTypo = 0;
            }
            
            if (noTypo == 0) {
                printf("Suggestions: ");
                char newWord[strlen(word)+2];
                //adding letter to beginning
                for (int i=1;i<=strlen(newWord);i++) {
                    newWord[i] = word[i-1];
                }
                for (int i=97;i<=122;i++) {
                    newWord[0] = i;
                    if (isMember(hashTable, newWord) == 1) {
                        printf("%s ",newWord);
                    }
                }

                //adding letter to end
                for (int i=0;i<strlen(word);i++) {
                    newWord[i] = word[i];
                }

                for (int i=97;i<=122;i++) {
                    newWord[strlen(newWord)-1] = i;
                    newWord[strlen(newWord)] = '\0';
                    if (isMember(hashTable, newWord) == 1) {
                        printf("%s ",newWord);
                    }
                }

                //removing letter from beginning
                for (int i=0;i<strlen(word);i++) {
                    newWord[i] = word[i+1];
                }

                if (isMember(hashTable, newWord) == 1) {
                        printf("%s ",newWord);
                }

                //removing letter from end
                strcpy(newWord,word);
                newWord[strlen(word)-1] = '\0';
                if (isMember(hashTable, newWord) == 1) {
                        printf("%s ",newWord);
                }

                //swapping letters
                for (int i=0;i<strlen(word)-1;i++) {
                    char swapWord[strlen(word)+1];
                    strcpy(swapWord,word);
                    swapWord[i]=word[i+1];
                    swapWord[i+1]=word[i];
                    if (isMember(hashTable, swapWord) == 1) {
                        printf("%s ",swapWord);
                    }
                }

                printf("\n");

            if (strcmp(check,"add") == 0) {
                insert(hashTable, word);
            }

            noTypo = 1;
            }
                        
			word = strtok(NULL,delimiter); 
		}
	}

	fclose(fp);
    
    if(printNoTypo==1)
        printf("No typo!\n");
    
    deleteHashTable(hashTable);
    
	return 0;
}
