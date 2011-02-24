//#include <vector>
using namespace std;

class Mask {
public:
	Mask(float* matrix,int size,float multiplier);
	virtual ~Mask();
	//vector<vector<float> > matrix;
	float *matrix;
	int size;
	float multiplier;
	int idx(int i,int j);
	float get(int i,int j);
};
Mask::Mask(float* matrix,int size,float multiplier) {
	this->multiplier=multiplier;
	this->size = size;
	//this->matrix = new vector<vector<float> >(0);
	int sizeM = sizeof(float)*size*size;
	cout << "size:"<<sizeM <<"\n";
	this->matrix=(float*)malloc(sizeM);
	for (int i=0;i<size;i++) {
		for (int j=0;j<size;j++) {
			int idx1 = idx(i,j);
			cout << i << ":"<< j <<":"<< idx1 << ":"<< matrix[idx1] <<"\n";
			this->matrix[idx(i,j)] = matrix[idx1];
		}
	}
	cerr << "mult:"<<this->multiplier<<"\n";
}
int Mask::idx(int i,int j) {
	return j*size+i;
}
float Mask::get(int i,int j) {
	return this->matrix[idx(i,j)];
}
Mask::~Mask(){
	free(matrix);
}
int idx(int i,int j,int size) {
	return j*size+i;
}
float* genAvg(int size) {
	int sizeM = sizeof(float)*size*size;
	float *matrix=(float*)malloc(sizeM);
	for (int i=0;i<size;i++) {
		for (int j=0;j<size;j++) {
			int idx1 = idx(i,j,size);
			matrix[idx1] = (float)1;
			cout <<"avg:" << i << ":"<< j <<":"<<  matrix[idx1] <<"\n";
		}
	}
	//some bug with the way the aray is passed
	return (float*)matrix;//new Mask((float*)matrix,size,1.0/(float)size*size);
}

float average3x3[3][3] = {
		{1,1,1},
		{1,1,1},
		{1,1,1}
};
float average3x3_mult = 1.0/9.0;

Mask* genWAvg(float *matrix,int size) {
	int midpoint = size/2+1;
	float sum=0;
	int sizeM = sizeof(float)*size*size;
	//float *matrix=(float*)malloc(sizeM);
	for (int i=1;i<size+1;i++) {
		for (int j=1;j<size+1;j++) {
			int idx1 = idx(i-1,j-1,size);
			matrix[idx1] = (midpoint -abs(i-midpoint))*(midpoint -abs(j-midpoint));
			sum+=matrix[idx1];
			cout <<"wavg:"<< i << ":"<<j<<":"<<matrix[idx1]<<"\n";
		}
	}
	cout << sum <<"\n";
	//some bug with the way the aray is passed
	return new Mask((float*)matrix,size,1.0/sum);
}

float weight3x3[3][3] = {
		{1,2,1},
		{2,4,2},
		{1,2,1}
};
float weight3x3_mult = 1.0/16.0;

float laplacian4[3][3] = {
		{0,-1,0},
		{-1,4,-1},
		{0,-1,0}
};

float laplacian8[3][3] = {
		{-1,-1,-1},
		{-1, 8,-1},
		{-1,-1,-1}
};

float laplacian4enhance[3][3] = {
		{0,-1,0},
		{-1,5,-1},
		{0,-1,0}
};

float laplacian8enhance[3][3] = {
		{-1,-1,-1},
		{-1, 9,-1},
		{-1,-1,-1}
};

float roberts[3][3] = {
		{0,0,0},
		{0,0,-1},
		{0,1,0}
};
float robertsabs[3][3] = {
		{0,0,0},
		{0,-1,0},
		{0,0,1}
};
float sobelY[3][3] = {
		{-1,0,1},
		{-2,0,2},
		{-1,0,1}
};
float sobelX[3][3] = {
		{-1,-2,-1},
		{ 0, 0, 0},
		{ 1, 2, 1}
};
float sobelDiagNeg[3][3] = {
		{ 0, 1, 2},
		{-1, 0, 1},
		{-2,-1, 0}
};
float sobelDiagPos[3][3] = {
		{0,-1,-2},
		{1, 0,-1},
		{2, 1, 0}
};
float prewittX[3][3] = {
		{-1,-1,-1},
		{ 0, 0, 0},
		{ 1, 1, 1}
};
float prewittY[3][3] = {
		{-1,0,1},
		{-1,0,1},
		{-1,0,1}
};
float prewittDiagNeg[3][3] = {
		{0 , 1, 1},
		{-1, 0, 1},
		{-1,-1, 0}
};
float prewittDiagPos[3][3] = {
		{0,-1,-1},
		{1, 0,-1},
		{1, 1, 0}
};
