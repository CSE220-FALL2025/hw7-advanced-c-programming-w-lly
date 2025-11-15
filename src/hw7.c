/* 
Name: Will Li
SBU ID: 116186168
*/

#include "hw7.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Node{
    void *val;
    struct Node *next;
} Node;

// stack of void pointers so it can use any values
typedef struct {
    Node *head;
} Stack;

Stack *create_stack(){
    Stack *stack = malloc(sizeof(Stack));
    stack->head = NULL;
    return stack;
}

void push_stack(Stack *stack, void *val){
    Node *node = malloc(sizeof(Node));
    node->val = val;
    node->next = stack->head;
    stack->head = node;
}

void *pop_stack(Stack *stack){
    if (!stack){
        printf("popping from empty stack\n");
        return NULL;
    }
    Node *top = stack->head;
    stack->head = top->next;    // update stack
    void *val = top->val;
    free(top);
    return val;
}

void *peek_stack(Stack *stack){
    if (!stack){
        printf("peeking from empty stack\n");
        return NULL;
    }
    return stack->head->val;
}

void free_stack(Stack *stack){
    if (!stack) return;
    while (stack->head){
        Node *temp = stack->head;
        stack->head = temp->next;
        free(temp);
    }
    free(stack);
}

bst_sf* insert_bst_sf(matrix_sf *mat, bst_sf *root) {
    if (!root){
        bst_sf *bst = malloc(sizeof(bst_sf));
        if (!bst){
            printf("failed to allocate memory for bst: creation\n");
            return NULL;
        }
        bst->mat = mat;
        bst->left_child = NULL;
        bst->right_child = NULL;
        return bst;
    }
    if (mat->name < root->mat->name){
        root->left_child = insert_bst_sf(mat, root->left_child);
    }
    else {  // ignore cases where names are the same (hw doc assumes all names are unique)
        root->right_child = insert_bst_sf(mat, root->right_child);
    }

    return root;
}

matrix_sf* find_bst_sf(char name, bst_sf *root) {
    if (!root) return NULL;
    if (name == root->mat->name){
        return root->mat;
    }
    else if (name < root->mat->name){
        return find_bst_sf(name, root->left_child);
    }
    else {
        return find_bst_sf(name, root->right_child);
    }
}

void free_bst_sf(bst_sf *root) {
    if (root){
        free_bst_sf(root->left_child);
        free_bst_sf(root->right_child);
        free(root->mat);
        free(root);
    }
}

matrix_sf* add_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    unsigned int num_rows = mat1->num_rows, num_cols = mat1->num_cols;
    // matrix dimensions should always match according to hw doc
    if (mat2->num_rows != num_rows || mat2->num_cols != num_cols){
        printf("matrix dimensions do not match: addition\n");
        return NULL;
    }
    
    matrix_sf *m = malloc(sizeof(matrix_sf) + num_rows*num_cols*sizeof(int));
    if (!m){
        printf("failed to allocate memory for matrix\n");
        return NULL;
    }
    m->name = '?';
    m->num_rows= num_rows;
    m->num_cols = num_cols;
    
    for (unsigned int i = 0; i < num_rows * num_cols; i++){
        m->values[i] = mat1->values[i] + mat2->values[i];
    }
    return m;
}

matrix_sf* mult_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) {
    unsigned int num_rows = mat1->num_rows, num_cols = mat2->num_cols;
    // matrix dimensions should always match according to hw doc
    if (mat1->num_cols != mat2->num_rows){
        printf("matrix dimensions do not match: multiplication\n");
        return NULL;
    }

    // overlapping num of rows and cols for matrix multiplication
    unsigned int multIter = mat1->num_cols;
    
    matrix_sf *m = malloc(sizeof(matrix_sf) + num_rows*num_cols*sizeof(int));
    if (!m){
        printf("failed to allocate memory for matrix\n");
        return NULL;
    }
    m->name = '?';
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    
    for (unsigned int i = 0; i < num_rows; i++){
        for (unsigned int j = 0; j < num_cols; j++){
            m->values[i*num_cols + j] = 0;
            for (unsigned int k=0; k<multIter; k++){
                m->values[i*num_cols + j] += mat1->values[i*multIter + k] * mat2->values[k*num_cols + j];
            }
        }
    }

    return m;
}

matrix_sf* transpose_mat_sf(const matrix_sf *mat) {
    unsigned int num_rows = mat->num_cols, num_cols = mat->num_rows;
    matrix_sf *m = malloc(sizeof(matrix_sf) + mat->num_rows*mat->num_cols*sizeof(int));
    if (!m){
        printf("failed to allocate memory for matrix: transpose\n");
        return NULL;
    }
    m->name = '?';
    m->num_rows= num_rows;
    m->num_cols = num_cols;
    
    for (unsigned int i = 0; i < num_rows; i++){
        for (unsigned int j = 0; j < num_cols; j++){
            m->values[i*num_cols + j] = mat->values[j*num_rows + i];
        }
    }
    return m;
}

matrix_sf* create_matrix_sf(char name, const char *expr) {
    unsigned int num_rows, num_cols; 
    char *after_num;
    num_rows = (unsigned int) strtol(expr, &after_num, 10);
    num_cols = (unsigned int) strtol(after_num, &after_num, 10);
    matrix_sf *m = malloc(sizeof(matrix_sf) + num_rows*num_cols*sizeof(int));
    if (!m){
        printf("failed to allocate memory for matrix: create matrix\n");
        return NULL;
    }
    m->name = name;
    m->num_rows = num_rows;
    m->num_cols = num_cols;

    expr = after_num;
    unsigned int num_count = 0;
    while (*expr != '\0' && num_count < num_rows * num_cols){
        // skip non digit characters like '[' or ']', keep only numbers and negative sign for strtol
        while (*expr && !((*expr >= '0' && *expr <= '9') || *expr == '-')){
            expr++;
        }
        if (!*expr) break;
        
        int num = (int) strtol(expr, &after_num, 10);
        if (expr == after_num) break; // if strtol didn't move pointer then no numbers found (for trailing whitespace)
        m->values[num_count++] = num;
        expr = after_num;
    }

    return m;
}


int operator_precedence(char c){
    if (c == '\'') return 3;
    else if (c == '*') return 2;
    else if (c == '+') return 1;
    else return 0;
}

char* infix2postfix_sf(char *infix) {
    if (!infix) return NULL;

    int cnt = 0;

    // find number of valid operators in infix string
    char *tmp = infix;
    while (*tmp){
        while (isspace(*tmp)) tmp++;   // skip spaces
        if (*tmp) cnt++;
        tmp++;
    }
    
    Stack *stack = create_stack();  // for operators
    char *postfix = malloc((cnt + 1) * sizeof(char));
    char *curr = postfix;

    while (*infix){
        while (isspace(*infix)) infix++;    // skip spaces
        if (*infix == '\0') continue;

        char c = *infix;
        // printf("%c\n", c);

        if (isalpha(c)){    // matrices
            *curr++ = c;
        }
        else if (c == ')'){ // pop until next '('
            while (stack->head && *(char*)peek_stack(stack) != '('){
                char *popped = pop_stack(stack);
                *curr++ = *popped;
                free(popped);
            }
            if (!stack->head) fprintf(stderr, "error in infix string: missing '(' for a ')'\n");
            else {
                char *popped = pop_stack(stack);
                free(popped);
            }
        }
        else if (c == '('){
            char *p = malloc(sizeof(char));
            *p = c;
            push_stack(stack, p);
        }
        else {  // handle operators
            if (!operator_precedence(c)) fprintf(stderr, "invalid operator- %c\n", c);
            while (stack->head && operator_precedence(c) <= operator_precedence(*(char*)peek_stack(stack))){    // if operator is less important than last operator
                char *popped = pop_stack(stack);
                *curr++ = *popped;
                free(popped);
            }
            char *p = malloc(sizeof(char));
            *p = c;
            push_stack(stack, p);
        }
        infix++;
    }

    while (stack->head && *(char*)peek_stack(stack) != '('){
        char *popped = pop_stack(stack);
        *curr++ = *popped;
        free(popped);
    }

    *curr = '\0';
    
    free(stack);
    return postfix;
}

matrix_sf* evaluate_expr_sf(char name, char *expr, bst_sf *root) {
    char *postfix_original = infix2postfix_sf(expr);
    char *postfix = postfix_original;
    Stack *stack = create_stack();
    while (*postfix){
        char c = *postfix;
        if (isalpha(c) && isupper(c)) { // matrix name
            matrix_sf *operand = find_bst_sf(c, root);
            if (!operand) fprintf(stderr, "invalid operand");
            push_stack(stack, operand);
        }
        else if (c == '\'') {  // transpose - only needs one operand
            matrix_sf *operand = pop_stack(stack);
            if (!operand) {
                fprintf(stderr, "invalid expression with transpose operator");
            }
            else {
                matrix_sf *transpose = transpose_mat_sf(operand);
                transpose->name = '?';
                push_stack(stack, transpose);
                if (!isalpha(operand->name) || !isupper(operand->name)) free(operand);  // free if temporary matrix
            }
        }
        else if (c == '+' || c == '*') {  // Binary operators
            matrix_sf *second = pop_stack(stack), *first = pop_stack(stack);
            
            if (!first || !second) {
                fprintf(stderr, "invalid expression with '%c' operator", c);
            }
            else {
                matrix_sf *result = (c == '+') ? add_mats_sf(first, second) : mult_mats_sf(first, second);
                result->name = '?';
                push_stack(stack, result);
                
                if (!isalpha(first->name) || !isupper(first->name)) free(first);
                if (!isalpha(second->name) || !isupper(second->name)) free(second);
            }
        }
        else {
            fprintf(stderr, "invalid operator: %c\n", c);
        }
        postfix++;
    }
    matrix_sf *m = pop_stack(stack);
    m->name = name;

    if (!m) fprintf(stderr, "invalid expression: did not produce result");
    free(stack);
    free(postfix_original);

    return m;
}

matrix_sf *execute_script_sf(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    bst_sf *bst = NULL;
    matrix_sf *last_matrix = NULL;

    size_t max_line_size = MAX_LINE_LEN; // defined in hw7.h
    char *line = NULL; 
    int line_count = 0;
    while ((line_count = getline(&line, &max_line_size, file)) != -1){
        char *curr_line = line;
        curr_line[line_count-1] = '\0';

        while (isspace(*curr_line)) curr_line++;
        if (!*curr_line) continue;

        char name = *curr_line++;
        matrix_sf *m;
        curr_line = strchr(curr_line, '=');
        if (!curr_line){
            fprintf(stderr, "line with out '=' sign");
            continue;
        }
        else curr_line++;

        if (strchr(curr_line, '[')){ // if '[' detected then its not an expression and need to create matrix
            m = create_matrix_sf(name, curr_line);
        }
        else {
            m = evaluate_expr_sf(name, curr_line, bst);
        }

        bst = insert_bst_sf(m, bst);
        last_matrix = m;
    }

    matrix_sf *result = copy_matrix(last_matrix->num_rows, last_matrix->num_cols, last_matrix->values);
    result->name = last_matrix->name;

    free(line);
    fclose(file);
    free_bst_sf(bst);
    
    return result;
}

// This is a utility function used during testing. Feel free to adapt the code to implement some of
// the assignment. Feel equally free to ignore it.
matrix_sf *copy_matrix(unsigned int num_rows, unsigned int num_cols, int values[]) {
    matrix_sf *m = malloc(sizeof(matrix_sf)+num_rows*num_cols*sizeof(int));
    m->name = '?';
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    memcpy(m->values, values, num_rows*num_cols*sizeof(int));
    return m;
}

// Don't touch this function. It's used by the testing framework.
// It's been left here in case it helps you debug and test your code.
void print_matrix_sf(matrix_sf *mat) {
    assert(mat != NULL);
    assert(mat->num_rows <= 1000);
    assert(mat->num_cols <= 1000);
    printf("%d %d ", mat->num_rows, mat->num_cols);
    for (unsigned int i = 0; i < mat->num_rows*mat->num_cols; i++) {
        printf("%d", mat->values[i]);
        if (i < mat->num_rows*mat->num_cols-1)
            printf(" ");
    }
    printf("\n");
}
