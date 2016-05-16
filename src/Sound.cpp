#include <vector>
#include <cmath>
#include <complex>
#include <fftw3.h>
#include "Sound.h"

using namespace std;

vector<double> hamming(int windowLength) {
	double PI = 3.14159265358979323;
	vector<double> output(windowLength);
	for(int i = 0; i < windowLength; i++) {
		output[i] = 0.54 - (0.46 * cos( 2 * PI * (i / ((windowLength - 1) * 1.0))));
	}
	return output;
}

double magnitude(double* array){
	return sqrt(pow(array[0],2) + pow(array[1],2));
}

sound::Sound::Sound(vector<double> pcm){
	int windowLength = 2048;
	vector<double> window = hamming(windowLength);
	int length = pcm.size();

	hop = windowLength/2;
	int hops = length/hop;
	frequencies = vector< vector<double> >(length/hop, vector<double>(windowLength));
	phases = vector< vector<double> >(length/hop, vector<double>(windowLength));

	double in [windowLength];
	fftw_complex* out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*(windowLength/2+1));
	fftw_plan toFreck = fftw_plan_dft_r2c_1d(windowLength,in,out,FFTW_ESTIMATE);

	for(int hopnum=0; hopnum<hops; hopnum++){
		int pos = hopnum * hop;
		//if(pos%(length/100)==0){cout<<100*pos/(length)<<"%"<<endl;}

		copy(pcm.begin()+pos, pcm.begin()+pos+windowLength, in);
		for(int i=0; i<windowLength; i++){
			in[i] *= window[i];
		}

		fftw_execute(toFreck);

		for(int i=0; i<windowLength; i++){
			frequencies[hopnum][i] = magnitude(out[i]);
			phases[hopnum][i] = atan(out[i][1]/out[i][0]);
		}
	}

	fftw_destroy_plan(toFreck);
	fftw_free(out);
}
