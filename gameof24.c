#include <stdlib.h>
#include <stdio.h>

struct node;

typedef struct expression {
    char operator;
    struct node* left;
    struct node* right;
} expression;

typedef struct node {
    int isExpression;
    union wrapper {
        expression expr;
        int number;
    } value;
} node;

void printEquation(node* root) {
    
    if (root->isExpression) {
        printf("(");
        
        printEquation(root->value.expr.left);
        
        printf("%c", root->value.expr.operator);
        
        printEquation(root->value.expr.right);
        
        printf(")");
    } else {
        printf("%d", root->value.number);
    }
    
}

node* constructNumber(int number) {
    node* n = malloc(sizeof(node));
    n->isExpression = 0;
    n->value.number = number;
    return n;
}

// TODO: Allow expression on both left and right
// Might need to swap - and / to check both cases
node* calculateEquation(int size, int numbers[], int goal) {
    if (size == 1) {
        if (goal != numbers[0]) {
            return NULL;
        }
        return constructNumber(numbers[0]);            
    }
    
    node *right = constructNumber(numbers[0]);
    node *left = NULL;
    char operator;

    for (int i = 0; i < size; i++) {
        int temp = numbers[0];
        numbers[0] = numbers[i];
        numbers[i] = temp;
        right->value.number = numbers[0];
        
        // Addition
        int subgoal = goal - numbers[0];
        left = calculateEquation(size - 1, &numbers[1], subgoal);
        if (left != NULL) {
            printf("+\n");
            operator = '+';
            break;
        }
        
        // Subtraction
        subgoal = goal + numbers[0];
        left = calculateEquation(size - 1, &numbers[1], subgoal);
        if (left != NULL) {
            printf("-\n");
            operator = '-';
            break;
        }
        
        // Multiplication
        if (goal % numbers[0] == 0) {
            subgoal = goal / numbers[0];
            left = calculateEquation(size - 1, &numbers[1], subgoal);
            if (left != NULL) {
                printf("*\n");
                operator = '*';
                break;
            }
        }
        
        // Division
        subgoal = goal * numbers[0];
        left = calculateEquation(size - 1, &numbers[1], subgoal);
        if (left != NULL) {
            printf("/\n");
            operator = '/';
            break;
        }
    }
    
    if (left == NULL || right == NULL) {
        return NULL;
    }
    
    node* n = malloc(sizeof(node));
    n->isExpression = 1;
    n->value.expr.left = left;
    n->value.expr.right = right;
    n->value.expr.operator = operator;
    
    return n;
}

void freeEquation(node* root) {
    if (root->isExpression) {
        freeEquation(root->value.expr.left);
        freeEquation(root->value.expr.right);
    }
    
    free(root);
}

int main(int argc, char* argv[]) {
    int numbers[4];
    for (int i = 0; i < 4; i++) {
        scanf("%d", &numbers[i]);
    }
    
    //int numbers[] = {4, 1, 1};
    //node* eq = calculateEquation(3, numbers, 3);
    
    node* eq = calculateEquation(4, numbers, 24);
    if (eq == NULL) {
        printf("No equation");
    } else {
        printEquation(eq);  
        freeEquation(eq);
    }    
}