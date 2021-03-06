#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>      // std::setprecision
#include <thread>

using namespace std;

typedef std::vector<std::vector<float>> matrix;
typedef std::vector<float> vetor;

string fileA = "A4x4.txt";
string fileB = "B4x4.txt";
string outFile = "out/C4x4.txt";

bool showTimeFlag = true;
bool writeMatrixFlag = true;
bool showMatricesFlag = false;

int threads_number = 1;

void showMatriz(string label, matrix &X){
	cout << "___________" << label<< "___________" << endl;
	for (int i = 0; i < X.size(); ++i){
		for (int j = 0; j < X.size(); ++j){
			cout << X[i][j]<<"\t";
		}
		cout << endl;
	}
}

void readMatrix(string file, matrix &X){
	ifstream arq;
	arq.open(file);
	if(arq.is_open()){
		int m, n;
	    arq >> m;
	    arq >> n;
	    X = matrix(m, vetor(n, 0));
		for(int i = 0; i < m; i++){
			for (int j = 0; j < n; j++){
				arq >> X[i][j];
			}
		}
		arq.close();
	} 
}

void writeMatrix(string file, matrix &X){
	
	std::ofstream outfile;

	outfile.open(file);
 
	if(outfile.is_open()){
		for(int i = 0; i < X.size(); i++){
			for (int j = 0; j < X[0].size(); j++){
				outfile << X[i][j];
				if(j<X[0].size()) outfile << '\t';
			}
			outfile << '\n';
		}
		outfile.close();
	} 
}

void partial_product(matrix &X, matrix &Y, matrix &Z, int i, int n) {
	int size = Y[0].size();
	for(; i < n; i++){
		for(int j = 0; j < size; j++){
			for(int k = 0; k < size; k++){
				Z[i][j] += X[i][k] * Y[k][j];
			}
		}
	}
}


matrix product(matrix &X, matrix &Y, int threads_number) {
	
	int m = X.size();
	int n = Y[0].size();

	matrix Z(m, vetor(n, 0));

	std::thread threads[threads_number];

	int factor = m/threads_number;

	for(int i = 0; i < m; i+=factor){
		threads[i/factor] = std::thread(partial_product, ref(X), ref(Y), ref(Z), i, i+factor);
	}
	for(int i = 0; i < threads_number; i++){
		threads[i].join();
	}

	return Z;
}


void lerArgs(int argc, const char * argv[]){
	if(argc > 1){
		fileA = string(argv[1]);
	}if(argc > 2){
		fileB = string(argv[2]);
	}if(argc > 3){
		outFile = string(argv[3]);
	}if(argc > 4){
		if(*argv[4] == 'y'){
			showMatricesFlag = true;
		}else{
			showMatricesFlag = false;
		}
	}if(argc > 5){
		if(*argv[5] == 'y'){
			writeMatrixFlag = true;
		}else{
			writeMatrixFlag = false;
		}
	}if(argc > 6){
		if(*argv[6] == 'y'){
			showTimeFlag = true;
		}else{
			showTimeFlag = false;
		}
	}if(argc > 7){
		threads_number = atoi(argv[7]);
	}
	
}


void showTime(double timeRead, double timeProduct, double timeShow){
	std::cout << std::fixed;
	cout << std::setprecision(4) << timeRead*1000;
	cout << '\t';
	cout << std::setprecision(4) << timeProduct*1000;
	cout << '\t';
	cout << std::setprecision(4) << timeShow*1000;
	cout << endl;
}

void showMatrices(matrix &A, matrix &B, matrix &P){
	showMatriz("A", A);
    showMatriz("B", B);
    showMatriz("P", P);
}

int main(int argc, const char * argv[]) {
	
	lerArgs(argc, argv);
	
    matrix A;
    matrix B;
    matrix P;
    
    auto t1 = std::chrono::high_resolution_clock::now();
    readMatrix(fileA, A);
    readMatrix(fileB, B);
    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> timeRead = t2 - t1;
   
   
    t1 = std::chrono::high_resolution_clock::now();
    P = product(A, B, threads_number);
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> timeProduct = t2 - t1;
    
    
    t1 = std::chrono::high_resolution_clock::now();
    
    if(showMatricesFlag)
    	showMatrices(A, B, P);
    
    if(writeMatrixFlag)
    	writeMatrix(outFile, P);
    
    t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> timeShow = t2 - t1;
    
    if(showTimeFlag)
    	showTime(timeRead.count(), timeProduct.count(), timeShow.count());
    
    return 0;
}
