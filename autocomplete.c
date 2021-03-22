#include "autocomplete.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int comparefunc(const void *a, const void *b) 
{
    struct term *ia = (struct term *)a;
    struct term *ib = (struct term *)b;
    return strcmp(ia->term, ib->term);
}

int comparefunc2(const void *a, const void *b)
{
    struct term *ia = (struct term *)a;
    struct term *ib = (struct term *)b;
    return (int)(ib->weight - ia->weight);
    
}

void read_in_terms(struct term **terms, int *pnterms, char *filename){
    char line[200];
    
    //STORING NUMBER OF TERMS IN *PNTERMS
    FILE *fp;
    fp = fopen(filename, "r");
    if(fp == NULL){
        //perror("Error opening file");
        *pnterms = 0;
    }
    if( fgets (line, sizeof(line), fp)!= NULL) {
        *pnterms = atoi(line);    
    }

    //ALLOCATING MEMORY FOR TERMS --> store pointer to block in *terms

    *terms = (struct term *)malloc(sizeof(struct term)*(*pnterms));

    //STORING TERMS AND WEIGHTS AS STRUCT ELEMENTS --> place in block pointed to by *terms

    int k,j,n = 0;
    /* parse 1 line at a time, extract "weight" and "term" of each line, and store in an array */
   
    double *all_weights = malloc(*pnterms*sizeof(double));
    char **all_terms = malloc(*pnterms * sizeof(char*));
    for (int i = 0; i < *pnterms; i++) {
        all_terms[i] = malloc(200);
    }

    
    for(int i = 0; i < *pnterms; i++){
        k= 0;
        j= 0;
        fgets (line, sizeof(line), fp);
        
        line[strcspn(line, "\n")] = 0;  /// gives index of "\n" on each line

        while(isdigit(line[j]) == 0){   //non-numeric character passed

            j++;                        //traverse until first digit of weight
        }

        k = j;                          // k stores the index of first digit of weight

        while(isdigit(line[j]) != 0){   //numeric character passed
            j++;                        //traversing until last digit of weight
        }
        
        char line_weight_str[j-k+1];
        char line_term_str[strlen(line)-j];
        /* traverse the line,  grabbing the weight and term */
        for(n=k; n<strlen(line); n++){
            if(n<j){
                line_weight_str[n-k] = line[n];
            }
            else if(n>j){
                line_term_str[n-j-1] = line[n];
            }
        }

        /* null terminating strings */
        line_weight_str[j-k] = '\0';
        line_term_str[strlen(line)-j-1] = '\0';

        //printf("line_weight_str: %s\n", line_weight_str);

        double line_weight = atoll(line_weight_str);
        all_weights[i] = line_weight;
        strcpy(&all_terms[i][0], line_term_str);

        //printf("weight = %lf\n", all_weights[i]);
    }
    
    /* populating array of struct terms */    
    //struct term *terms = malloc(*pnterms * sizeof(struct term));
    for (int i = 0; i < *pnterms; i++){
        strcpy((*terms)[i].term, all_terms[i]);
        (*terms)[i].weight = all_weights[i];
    }
    
    //sorting lexicographic
    qsort(*terms, *pnterms, sizeof(struct term), comparefunc);
    
    
    /*printf("contents of terms: \n");
    for (int i = 0; i < *pnterms; i++){
        printf("term = %s weight = %lf\n", (*terms)[i].term, (*terms)[i].weight);
    }*/
        
    /*
    printf("contents of terms: \n");
    for(int i = 0; i < *pnterms; i++){
        printf("term = %s weight = %d\n", terms[i]->term, (int) terms[i]->weight);
    }*/
    

    free(all_weights);
    for (int i = 0; i < *pnterms; i++) {
        free(all_terms[i]);
    }
    free(all_terms);

    fclose(fp);

}

    int lowest_match(struct term *terms, int nterms, char *substr){
        int mid = 0;
        int start = 0;
        int end = nterms-1;
        int len_substr = strlen(substr);
        char *substr2 = malloc(sizeof(char)*len_substr+1);
        int cur_lowest = -1;
        int result;

        while(start<=end){
            mid = (start + end)/2;
            result = mid;

            
            strncpy(substr2, terms[result].term, len_substr);

            substr2[len_substr] = '\0';

            if(start == end){
              if(strncmp(substr, substr2, len_substr) == 0){
                cur_lowest = result;
                break;
              }else{
                break;
              }
            }

            if(strncmp(substr, substr2, len_substr) < 0){
                end = result - 1;
            }

            if(strncmp(substr, substr2, len_substr) == 0){
                cur_lowest = result;
                end = result-1;
            }

            if(strncmp(substr, substr2, len_substr) > 0){
                start = result + 1;
            }
            
        }

        return cur_lowest;
    }

    int highest_match(struct term *terms, int nterms, char *substr){
        //int nterms = sizeof(terms)/(sizeof(char)*200); // check this after --> nterms is the end
        int mid = 0;
        int start = 0;
        int end = nterms-1;
        int len_substr = strlen(substr);
        char *substr2 = malloc(sizeof(char)*len_substr+1);
        int cur_highest = -1;
        int result;

        while(start<=end){
            mid = (start + end)/2;
            result = mid;

            
            strncpy(substr2, terms[result].term, len_substr);

            substr2[len_substr] = '\0';

            if(start == end){
              if(strncmp(substr, substr2, len_substr) == 0){
                cur_highest = result;
                break;
              }else{
                break;
              }
            }

            if(strncmp(substr, substr2, len_substr) < 0){
                end = result - 1;
            }

            if(strncmp(substr, substr2, len_substr) == 0){
                cur_highest = result;
                start = result+1;
            }

            if(strncmp(substr, substr2, len_substr) > 0){
                start = result + 1;
            }
        }

        return cur_highest;
    }

void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr){
    /// assigning n_answer
    int lowest_index = lowest_match(terms, nterms, substr);
    int highest_index = highest_match(terms, nterms, substr);
    int n_values = highest_index - lowest_index + 1;
    *n_answer = n_values;

    if(lowest_index == -1 && highest_index == -1){
        *n_answer = 0;
    }else{
        // assign struct term **answer
        *answer = (struct term *)malloc(sizeof(struct term)*(n_values+1));
        

        for(int i = lowest_index; i<= highest_index; i++){
            strcpy((*answer)[i-lowest_index].term, terms[i].term);
            (*answer)[i-lowest_index].weight = terms[i].weight;
        }

        /*for (int i = 0; i < n_values; i++){
            printf("term = %s weight = %lf\n", (*answer)[i].term, (int) (*answer)[i].weight);
        }*/

        qsort(*answer, n_values, sizeof(struct term), comparefunc2);
    }
}