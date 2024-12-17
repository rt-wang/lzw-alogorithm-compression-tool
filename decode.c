#include "decode.h"
#include "encode.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

int isempty(st *s) {
  if (s->top == -1)
    return 1;
  else
    return 0;
}

int isfull(st *s){
    if (s->top == s->MAX)
        return 1;
    else
        return 0;
}

void push(st *s, int newitem) {
  if (!isfull(s)) {
    s->top++;
    s->items[s->top] = newitem;
  }
}

int pop(st *s) {
  if (!isempty(s)) {
    int element = s->items[s->top];
    s->top--;
    return element;
  } else {
    return -1;
  }
}

void showstack(st *s){
    if (isempty(s)){
        printf("\nit's empty \n");
    }
    for (int i = 0; i < s->top; i++){
        printf("%d ", s->items[i]);
    }
    printf("\n");
}

st* stack_init(int max_num){
    st* s = malloc(sizeof(st));
    s->MAX = max_num;
    s->items = malloc(max_num * sizeof(int));
    s->top = -1;
    return s;
}

void free_stack(st* s){
    free(s->items);
    free(s);
}

void decode(int *input, int length, int maxBits, int p){
    //fprintf(stderr, "DEBUG: Number of codes to decode: %d\n", length);
    map* m = create_map(1<<maxBits);
    st* s = stack_init(length);
    int i = 0;
    int oldC = -1;
    int finalK = -1;
    int over_capacity = 0;

    while(i < length){
        int curr = input[i];
        if (curr == 256){
            i++;
            continue;
        }
        int newC = curr;
        
        //fprintf(stderr,"curr: %d, m_size: %d\n",curr, m->size);
        if (curr >= m->capacity){
          exit(1);
        }

        //if (m->size >= (m->capacity-1)){
            //printf("\ncurr: %d, m_size %d\n", curr, m->size);
        //}

        if (curr >= m->size){
            push(s, finalK);
            curr = oldC;
        }
        

        node* new = m->array[curr];


        while (new->prefix != -1){
            //if (m->size >= (m->capacity-1)){
                //printf("\nnew->prefix: %d\n", new->prefix);
            //}
            push(s, new->curr);
            curr = new->prefix;
            new = m->array[curr];
        }
        
        finalK = new->curr;
        putchar(finalK);
        //if (m->size >= (m->capacity-1)){
           // printf("\nfinalK: %d\n", finalK);
            //printf("\noldC: %d\n", oldC);
        //}

        while (isempty(s)==0){
            int temp = pop(s);
            if (m->size >= (m->capacity-1)){
                //showstack(s);
                //printf("s_top %d", s->top);
                //printf("\nfrom stack:%d\n", temp);
                putchar(temp);
                //printf("\n");
            } else {putchar(temp);} 
        }
        //fprintf(stderr,"decode: %d\n", i);
        if (oldC!=-1){
            if (!over_capacity) {
                if (m->size < m->capacity) {
                    put_map(m, oldC, finalK);
                } else {
                    if (p==1){
                        //printf("\n(oldC:%d, newC:%d)\n", oldC, finalK);
                        map* new_m = pruned_map(m, maxBits);
                        //oldC = find_code_in_new_map(m, new_m, oldC);
                        //newC = find_code_in_new_map(m, new_m, newC);
                        free_map(m);
                        m = new_m;
                        //fprintf(stderr,"m_size:%d\n",m->size);
                    } else {
                        over_capacity = 1;
                    }
                }
            }
        }
        oldC = newC;
        i++;
        //printf("\n");
    }
    
    if (getenv("DEBUG")){
        show_map(m);
    }
    free_stack(s);
    free_map(m);

}
