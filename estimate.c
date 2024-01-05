#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Definition of the Matrix structure
struct Matrix {
    double *m;    // Dynamic array to hold matrix elements
    int rows;     // Number of rows in the matrix
    int cols;     // Number of columns in the matrix
};

// Global arrays for memory management
double *mrmalloc[10000];   // Array to store pointers to allocated memory for matrix data
int mrmallocNum = 0;       // Counter for mrmalloc
struct Matrix *freeme[1000];  // Array to store pointers to allocated Matrix structures
int freemeNum = 0;            // Counter for freeme

// Function to retrieve a value from a given matrix at specified row and column
double getVal(struct Matrix *M, int row, int col) {
    return M->m[row * (M->cols) + col];
}

// Function to set a value in a given matrix at specified row and column
void setVal(struct Matrix *M, int row, int col, double value) {
    M->m[row * (M->cols) + col] = value;
}

// Function to create a new matrix with specified rows and columns
// Type 0 for zero matrix, 1 for identity matrix
struct Matrix* makeMatrix(int r, int c, int type) {
    struct Matrix *result = malloc(sizeof(struct Matrix));
    freeme[freemeNum++] = result;
    result->rows = r;
    result->cols = c;
    result->m = malloc(sizeof(double) * r * c);
    mrmalloc[mrmallocNum++] = result->m;
    // Initialize matrix elements
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++) {
            setVal(result, i, j, (type == 1 && i == j) ? 1 : 0);
        }
    }
    return result;
}

// Function to create a copy of a given matrix
struct Matrix* cpMatrix(struct Matrix *M) {
    struct Matrix *x = makeMatrix(M->rows, M->cols, 0);
    for (int i = 0; i < M->rows; i++) {
        for (int j = 0; j < M->cols; j++) {
            setVal(x, i, j, getVal(M, i, j));
        }
    }
    return x;
}

// Function to print the contents of a matrix
void printMatrix(struct Matrix *M) {
    for (int i = 0; i < M->rows; i++) {
        for (int j = 0; j < M->cols; j++) {
            printf("%.0lf ", getVal(M, i, j));
        }
        printf("\n");
    }
}

// Function to transpose a given matrix
struct Matrix* transpose(struct Matrix *M1) {
    struct Matrix *result = makeMatrix(M1->cols, M1->rows, 0);
    for (int i = 0; i < M1->rows; i++) {
        for (int j = 0; j < M1->cols; j++) {
            setVal(result, j, i, getVal(M1, i, j));
        }
    }
    return result;
}

// Function to multiply two matrices
struct Matrix* multiplyMatrix(struct Matrix *M1, struct Matrix *M2) {
    struct Matrix *result = makeMatrix(M1->rows, M2->cols, 0);
    for (int i = 0; i < M1->rows; i++) {
        for (int j = 0; j < M2->cols; j++) {
            for (int k = 0; k < M1->cols; k++) {
                setVal(result, i, j, getVal(result, i, j) + getVal(M1, i, k) * getVal(M2, k, j));
            }
        }
    }
    return result;
}
// Function to calculate the inverse of a matrix
struct Matrix* inverse(struct Matrix *M1){
	// Gaussian elimination algorithm for matrix inversion
	struct Matrix *N = makeMatrix(M1->rows, M1->rows, 1);
	for (int p=0;p<M1->rows;p++){
		double f = getVal(M1, p, p);
		for (int j=0;j<M1->rows;j++){
			setVal(M1, p, j, getVal(M1, p, j)/f);
			setVal(N, p, j, getVal(N, p, j)/f);
		}
		for (int i=p+1;i<M1->rows;i++){
			f = getVal(M1, i, p);
			for (int j=0;j<M1->rows;j++){
				setVal(M1, i, j, getVal(M1, i, j) - getVal(M1, p, j)*f);
				setVal(N, i, j, getVal(N, i, j) - getVal(N, p, j)*f);
			}
		}
	}
	for (int p=M1->rows-1; p>=0;p--){
		for(int i= p-1; i>=0;i--){
			double f = getVal(M1,i,p);
			for (int j=0;j<M1->rows;j++){
				setVal(M1, i, j, getVal(M1, i, j) - getVal(M1, p, j)*f);
				setVal(N, i, j, getVal(N, i, j) - getVal(N, p, j)*f);
			}
		}
	}
	return N;
}
// Main function: reads data from files, performs matrix operations, and prints results
int main(int argc, char *argv[]){

	FILE *s = fopen(argv[1], "r");
	char type[20];
	fscanf(s, "%[^\n]", type);
	int numAt, numH;
	fscanf(s,"%d ", &numAt);
	fscanf(s,"%d ", &numH);

	struct Matrix *houses = makeMatrix(numH, numAt+1, 0);
	struct Matrix *prices = makeMatrix(numH, 1, 0);
	double temp;
	
    // Calculate weights using matrix operations for linear regression
	for( int i=0; i<houses->rows; i++){
	   for(int j=0; j<houses->cols;j++){
		   fscanf(s, "%lf ", &temp);
			 if (j==houses->cols-1){
				 setVal(prices, i, 0, temp);
				 setVal(houses, i, j, 1);
			 } else {
				 setVal(houses, i, j, temp);
			 }
	   }
	}

	fclose(s);
	FILE *r = fopen(argv[2], "r");

	fscanf(r, "%[^\n]", type);
	fscanf(r,"%d ", &numAt);
	fscanf(r,"%d ", &numH);

	struct Matrix *data = makeMatrix(numH, numAt+1, 0);

	for( int i=0; i<data->rows; i++){
		 for(int j=0; j<data->cols;j++){
			 if (j==data->cols-1){
				setVal(data, i, j, 1);
			 } else {
				fscanf(r, "%lf ", &temp);
				setVal(data, i, j, temp);
			 }
		 }
	}
	fclose(r);

	// Calculate weights using matrix operations for linear regression
	struct Matrix *weights = multiplyMatrix(multiplyMatrix(inverse(multiplyMatrix(transpose(houses),houses)),transpose(houses)),prices);
	printMatrix(multiplyMatrix(data,weights));

	// Free allocated memory
	for (int i=0;i<freemeNum;i++){
		free(freeme[i]);
	}
	for (int i=0;i<mrmallocNum;i++){
		free(mrmalloc[i]);
	}
}
