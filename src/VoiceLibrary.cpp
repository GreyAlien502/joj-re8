#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "fileio.h"
#include "VoiceLibrary.h"

using namespace std;

VoiceLibrary::VoiceLibrary(std::string path, int windowOverlap, int windowSize, int rate){
	sampleRate = rate;
	windowLength = windowSize;
	hop = windowLength/windowOverlap;

	phones = vector<Phone>();
	aliases = map<string,int>();
	vector<string> presets = vector<string> ();

	ifstream oto_ini(path+"/oto.ini");
	if(oto_ini.is_open()){
		int i = 0;
		for(string line; getline(oto_ini, line);){
			if(line[0] == '#'){continue;}
			int start = 0;
			int end;

			end = line.find('=');
			string filename = line.substr(start,end-start);

			start = end + 1;
			end = line.find(',', start);
			string alias = line.substr(start,end-start);

			string settings = filename + line.substr(end+1, line.length());
			vector<string>::iterator repeat = find(presets.begin(), presets.end(), settings);
			if(repeat != presets.end()){
				aliases.insert({
					alias,
					distance(presets.begin(), repeat)
				});
				continue;
			}
			cerr<<"\t"<<alias;
			presets.push_back(settings);

			start = end + 1;
			end = line.find(',', start);
			double offset = stod(line.substr(start,end-start))/1000.;

			start = end + 1;
			end = line.find(',', start);
			double consonant = stod(line.substr(start,end-start))/1000.;

			start = end + 1;
			end = line.find(',', start);
			double cutoff = stod(line.substr(start,end-start))/1000.;

			start = end + 1;
			end = line.find(',', start);
			double preutter = stod(line.substr(start,end-start))/1000.;

			start = end + 1;
			end = line.length();
			double overlap = stod(line.substr(start,end-start))/1000.;

			try{
				vector<double> pcm = fileio::read(path+'/'+filename);
				pcm = vector<double>(
							pcm.begin()+offset*sampleRate,
							pcm.end()-cutoff*sampleRate);

				Phone telephone = Phone(
					pcm,
					consonant, preutter, overlap,
					windowOverlap, windowSize, sampleRate);

				phones.push_back(telephone);
				aliases.insert({alias,i});
				i++;
			}catch(fileio::fileOpenError& exc){
				cerr<<filename<<" not found."<<endl;
			}
		}
	}else{
		cerr<<"Couldn't open oto.ini file\n";
		exit(1);
	}		
}

bool VoiceLibrary::hasPhone(string alias){
	return aliases.find(alias) != aliases.end();
}

Phone VoiceLibrary::getPhone(Note note){
	if(hasPhone(note.lyric)){
		Phone phony= phones[aliases.at(note.lyric)].adjustPhone(note);
		return phony;
	}else{
		return Phone();
	}
}