#include <stdlib.h>
#include <stdio.h>

#define OPCOUNT 4

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

int getValue(node* root) {
    if (!root->isExpression) {
        return root->value.number;
    }

    int a = getValue(root->value.expr.left);
    int b = getValue(root->value.expr.right);

    switch (root->value.expr.operator) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
    }
}

void setNumbers(node* root, int size, int numbers[]) {
    if (root->isExpression) {
        int leftSize = 1 + root->value.expr.left->isExpression;
        setNumbers(root->value.expr.left, leftSize, numbers);
        setNumbers(root->value.expr.right, size - leftSize, &numbers[leftSize]);
    } else {
        root->value.number = numbers[0];
    }
}

void freeEquation(node* root) {
    if (root->isExpression) {
        freeEquation(root->value.expr.left);
        freeEquation(root->value.expr.right);
    }
    
    free(root);
}

void rotate(int size, int numbers[]) {
    int first = numbers[0];
    for (int i = 1; i < size; i++) {
        numbers[i - 1] = numbers[i];
    }
    numbers[size - 1] = first;
}

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
    char operator = '+';

    for (int k = 1; k <= (size + 1) / 2; k++) {
        for (int i = 0; i < size; i++) {
            rotate(size, numbers);
            setNumbers(right, k, numbers);
            
            int rValue = getValue(right);
           
            // Addition
            int subgoal = goal - rValue;
            left = calculateEquation(size - k, &numbers[k], subgoal);
            if (left != NULL) {
                operator = '+';
                break;
            }
            
            // Subtraction
            subgoal = goal + rValue;
            left = calculateEquation(size - k, &numbers[k], subgoal);
            if (left != NULL) {
                operator = '-';
                break;
            }
            
            // Multiplication
            if (rValue > 0 && goal % rValue == 0) {
                subgoal = goal / rValue;
                left = calculateEquation(size - k, &numbers[k], subgoal);
                if (left != NULL) {
                    operator = '*';
                    break;
                }
            }
            
            // Division
            subgoal = goal * rValue;
            left = calculateEquation(size - k, &numbers[k], subgoal);
            if (left != NULL) {
                operator = '/';
                break;
            }
        }
        
        if (left != NULL) {
            break;
        } else {
            node* n = malloc(sizeof(node));
            n->isExpression = 1;
            n->value.expr.right = right;
            n->value.expr.left = constructNumber(numbers[1]);
            n->value.expr.operator = '/';
            right = n;
        }
    }
    
    
    if (left == NULL) {
        freeEquation(right);
        return NULL;
    }
    
    node* n = malloc(sizeof(node));
    n->isExpression = 1;
    n->value.expr.left = left;
    n->value.expr.right = right;
    n->value.expr.operator = operator;
    
    return n;
}

int main(int argc, char* argv[]) {
    int numbers[4];
    for (int i = 0; i < 4; i++) {
        scanf("%d", &numbers[i]);
    }
    
    node* eq = calculateEquation(4, numbers, 24);
    if (eq == NULL) {
        printf("No equation");
    } else {
        printEquation(eq);  
        freeEquation(eq);
    }
}