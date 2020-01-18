#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "Ngram.h"
#include "Vocab.h"
#include "VocabMap.h"

#define MAXWORDS 256
#define FAKE_ZERO -100

using namespace std;

const VocabIndex emptyContext[] = {Vocab_None};
static Vocab voc; // Vocabulary in LM

VocabIndex getIndex(VocabString w)
{
	VocabIndex wid = voc.getIndex(w);
    if(wid == Vocab_None)
        return voc.getIndex(Vocab_Unknown);
	return wid; 
}

LogP Viterbi(Ngram& lm, VocabMap& map, Vocab& vocZ, Vocab& vocB, char* line)
{
	LogP Pro[MAXWORDS][1024] = {0.0};
	VocabIndex BackTrack[MAXWORDS][1024];
	VocabIndex IntToIndex[MAXWORDS][1024];
	int size[MAXWORDS] = {0};

	VocabString WordsInLine[MAXWORDS];
	unsigned count = Vocab::parseWords(line, &WordsInLine[1], MAXWORDS);
	WordsInLine[0] = Vocab_SentStart;
	WordsInLine[count+1] = Vocab_SentEnd;
	count += 2;

	//initial
	Prob p;
	VocabIndex w;
	VocabMapIter iter(map, vocZ.getIndex(WordsInLine[0]));
	for(int i = 0; iter.next(w, p); size[0]++, i++){
		VocabIndex index = getIndex(vocB.getWord(w));
		LogP curProb = lm.wordProb(index, emptyContext);
		if(curProb == LogP_Zero) 
			curProb = FAKE_ZERO;
		Pro[0][i] = curProb;
		IntToIndex[0][i] = w;
		BackTrack[0][i] = 0;
	}

	
	// Viterbi Algorithm
	for(int t = 1; t < count; t++){
		Prob p;
		VocabIndex w;
		VocabMapIter iter(map, vocZ.getIndex(WordsInLine[t]));
		for(int j = 0; iter.next(w, p); size[t]++, j++){
			VocabIndex index = getIndex(vocB.getWord(w));
			LogP maxProb = -1.0/0.0;
			LogP pre;
			for(int i = 0; i < size[t-1]; i++){
				VocabIndex context[] = {getIndex(vocB.getWord(IntToIndex[t-1][i])), Vocab_None};
				LogP curProb = lm.wordProb(index, context);
				LogP unigramProb = lm.wordProb(index, emptyContext);
				
				if(curProb == LogP_Zero && unigramProb == LogP_Zero)
					curProb = FAKE_ZERO;

				curProb += Pro[t-1][i];		
		
				if(curProb > maxProb){
					maxProb = curProb;
					BackTrack[t][j] = i;
				}
			}
			Pro[t][j] = maxProb;
			IntToIndex[t][j] = w;
		}
	}

	LogP maxProb = -1.0/0.0;
	int end = -1;
	for(int i = 0; i < size[count-1]; i++){
		LogP curProb = Pro[count-1][i];
		if(curProb > maxProb){
			maxProb = curProb;
			end = i;
		}
	}

	//BackTrack
	int path[MAXWORDS];
	path[count-1] = end;
	for(int t = count-2; t >= 0; t--)
		path[t] = BackTrack[t+1][path[t+1]];

	for(int t = 0; t < count; t++)
		if(t == count -1)
			printf("%s%s", vocB.getWord(IntToIndex[t][path[t]]), "\n");
		else
			printf("%s%s", vocB.getWord(IntToIndex[t][path[t]]), " ");

	return maxProb;
}

int main(int argc, char *argv[])
{
	Vocab vocZ, vocB; //ZhuYin and Big5 in ZhuYin-to-Big5 map

	Ngram lm(voc, atoi(argv[8]));	//ngram_order
	VocabMap map(vocZ, vocB);
	
	//read LM and map
	File lmFile(argv[6], "r" );
	lm.read(lmFile);
	lmFile.close();

	File mapfile(argv[4], "r");
	map.read(mapfile);
	mapfile.close();
	
	File testdata(argv[2], "r");
	char* line = NULL;
	while(line = testdata.getline()){
		LogP MaxP = Viterbi(lm, map, vocZ, vocB, line);
	}
	
	testdata.close();

	return 0;
}