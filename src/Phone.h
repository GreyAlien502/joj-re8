#include <vector>

#include "Speech.h"
#include "Note.h"

class Phone{
	protected:
		 double consonant; //all in samples
		 double preutter;
		 double overlap;
	public:
		Speech sample;

		Phone(double consonant=0, double preutter=0, double overlap=0,
			Speech sample=Speech());
		double getConsonant();
		double getPreutter();
		double getOverlap();
};

class basePhone: public Phone{
	private:

	public:
		basePhone(std::vector<double> pcm = std::vector<double>(2048,0),
			double consonantTime=0, double preutterTime=0, double overlapTime=0,
			int windowOverlap=16, int windowSize=2048, int sampleRate=44100);
		Phone adjustPhone(Note& note, double tempo);
};
