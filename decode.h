#ifndef B6024EEC_47B1_43D8_A5E2_7E0CE27C1E65
#define B6024EEC_47B1_43D8_A5E2_7E0CE27C1E65

typedef struct stack {
    int MAX;
    int *items;
    int top;
} st;

// Stack operations
int isempty(st *s);
void push(st *s, int newitem);
int pop(st *s);
st* stack_init(int max_num);

// Main decode function
void decode(int *input, int length, int max_bit_length, int p);

extern int maxBits;

#endif /* B6024EEC_47B1_43D8_A5E2_7E0CE27C1E65 */