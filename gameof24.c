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

char getNextOperator(char operator) {
    switch(operator) {
        case '+': return '-';
        case '-': return '*';
        case '*': return '/';
        case '/': return '+';
    }
}

// Get the value of an equation
int getValue(node* root) {
    if (!root->isExpression) {
        return root->value.number;
    }

    // Get the value of the left side
    int a = getValue(root->value.expr.left);
    // Get the value of the right side
    int b = getValue(root->value.expr.right);

    // Calculate the value
    switch (root->value.expr.operator) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
    }
}

int setNumbers(node* root, int size, int numbers[]) {
    if (size == 0) {
        return 0;
    }
    
    if (root->isExpression) {
        // Set leftSize = the amount of numbers on the left side
        int leftSize = setNumbers(root->value.expr.left, size, numbers);
        // Set rightSize = the amount of numbers on the right size
        int rightSize = setNumbers(root->value.expr.right, size - leftSize, &numbers[leftSize]);
        return leftSize + rightSize;
    } else {
        root->value.number = numbers[0];
        return 1;
    }
}

int isValidEquation(node* root) {
    if (!root->isExpression) {
        return 1;
    }
    
    int left = isValidEquation(root->value.expr.left);
    int right = isValidEquation(root->value.expr.left);
    int isValid = left && right;
    
    if (root->value.expr.operator == '/') {
        int lv = getValue(root->value.expr.left);
        int rv = getValue(root->value.expr.right);
        isValid = isValid && lv % rv == 0;
    }
    
    return isValid;
}

void freeEquation(node* root) {
    // If the node is an expression, first free the left and right
    if (root->isExpression) {
        freeEquation(root->value.expr.left);
        freeEquation(root->value.expr.right);
    }
    
    free(root);
}

// Shift all elements to the left by 1, and move the first to the end
void rotate(int size, int numbers[]) {
    int first = numbers[0];
    for (int i = 1; i < size; i++) {
        numbers[i - 1] = numbers[i];
    }
    numbers[size - 1] = first;
}

node* calculateEquation(int size, int numbers[], int goal) {
    // If there is only 1 element left, return it if it is equal to goal
    if (size == 1) {
        if (goal != numbers[0]) {
            return NULL;
        }
        return constructNumber(numbers[0]);            
    }
    
    // Set the right equal to the first element
    node *right = constructNumber(numbers[0]);
    node *left = NULL;
    char operator = '+';

    // let k = the amount of numbers on the right side of the equation
    for (int k = 1; k <= (size + 1) / 2; k++) {
        for (int i = 0; i < size; i++) {
            // Set the numbers on the right equation to the numbers in numbers[]
            setNumbers(right, k, numbers);
            if (!isValidEquation(right)) {
                rotate(size, numbers);
                continue;
            }
            
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
            rotate(size, numbers);
        }
        
        if (left != NULL) {
            // Found valid equation
            break;
        } else {
            if (right->isExpression && right->value.expr.operator != '/') {
                right->value.expr.operator = getNextOperator(right->value.expr.operator);
                k--;
                continue;
            }
            
            // Add the another number to the right equation
            node* n = malloc(sizeof(node));
            n->isExpression = 1;
            n->value.expr.right = right;
            n->value.expr.left = constructNumber(numbers[1]);
            n->value.expr.operator = '+';
            right = n;
        }
    }
    
    // If we failed to find anything, clean up and return NULL
    if (left == NULL) {
        freeEquation(right);
        return NULL;
    }
    
    // Construct equation and return it
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
        printf("No equation\n");
    } else {
        printEquation(eq);
        printf("\n");
        freeEquation(eq);
    }
}