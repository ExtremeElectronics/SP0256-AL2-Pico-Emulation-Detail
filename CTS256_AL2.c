#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//define to run tests on the rule set
//#define test 1

//define for inclusion to pico RomWBW
#define WBW 0

//set to 1 to provide default debug output.
#define DEBUG 0

#ifdef test
  //debug level if in test
  #define DEBUG 1
#endif

//base rulez and exceptions
#include "rules_array.c"

//#define maxallophones 4096

//debug indent
uint8_t indent=0;
//buffer for allophones
#define ALLOBUFFERMAX 4096
char AlloBuffer[ALLOBUFFERMAX]="";

uint8_t debug=DEBUG;

//output format - 0=hex, 1=decimal, 2=basic
#define format 2

#ifdef test
#define maxtestoutput 4096
char testoutput[maxtestoutput+20]="";
uint16_t LastRule=0;
uint16_t TestRule=0;
#endif

struct Match {
    uint8_t Matched;
	uint8_t NumChars;
};

uint16_t slen(char *tstr){
	uint16_t l=0;
	while(tstr[l++]!=0);
	return l-1;
}

void Doindent(void){
   if (debug)for( int a=0;a<indent;a++){printf(" ");}	
}

int IsAVowel(char *Sentance , uint16_t sp){
	int r=0;
	for(int x=0;x<sizeof(Vowels);x++){
	   if(	Sentance[sp]==Vowels[x]){r=1;}
	
	}
	return r;
}

int IsAVoicedConsonant(char *Sentance , uint16_t sp){
	int r=0;
	for(int x=0;x<sizeof(VoicedConsonants);x++){
	   if(	Sentance[sp]==VoicedConsonants[x]){r=1;}
	}
	return r;
}

int IsASuffixRight(char *Sentance , uint16_t sp){
	int16_t sentancelength=slen(Sentance);
	int r=0;
	int cnt=0;
	int y=0;
	int x=0;
	uint8_t elements=6;
	for(x=0;x<elements;x++){
		cnt=0;
		for(y=0;SuffixsRight[x][y]!=0;y++){
	        if(	Sentance[sp+y]==SuffixsRight[x][y]){cnt++;}
		}
		if(y==cnt){r=cnt;}		
	}
	return r;
}

int IsASibilantRight(char *Sentance , uint16_t sp){
	int16_t sentancelength=slen(Sentance);
	int r=0;
	int cnt=0;
	int y=0;
	int x=0;
	uint8_t elements=8;
	for(x=0;x<elements;x++){
		cnt=0;
		for(y=0;SibilantsRight[x][y]!=0;y++){
	        if(	Sentance[sp+y]==SibilantsRight[x][y]){cnt++;}
		}
		if(y==cnt){r=cnt;}		
	}
	return r;
}

int IsASibilantLeft(char *Sentance , uint16_t sp){
	int16_t sentancelength=slen(Sentance);
	int r=0;
	int cnt=0;
	int y=0;
	int x=0;
	uint8_t elements=8;
	for(x=0;x<elements;x++){
		cnt=0;
		for(y=0;SibilantsLeft[x][y]!=0;y++){
	        if(	Sentance[sp-y]==SibilantsLeft[x][y]){cnt++;}
		}
		if(y==cnt){r=cnt;}		
	}
	return r;
}

int IsAtRight(char *Sentance , uint16_t sp){
	int16_t sentancelength = slen(Sentance);
	int r=0;
	int cnt=0;
	int y=0;
	int x=0;
	int8_t elements=11;
	for(x=0;x<elements;x++){
		cnt=0;
		for(y=0;((AtRight[x][y]!=0) || ((sp+y)<sentancelength));y++){
	        if(	Sentance[sp+y]==AtRight[x][y]){cnt++;}
		}
		if(y==cnt){r=cnt;}		
	}
	return r;
}

int IsAtLeft(char *Sentance , uint16_t sp){
	int16_t sentancelength = slen(Sentance);
	int r=0;
	int cnt=0;
	int y=0;
	int x=0;
	int8_t elements=11;
	for(x=0;x<elements;x++){
		cnt=0;
		for(y=0;((AtLeft[x][y]!=0) || ((sp+y)<sentancelength));y++){
	        if(	Sentance[sp-y]==AtLeft[x][y]){cnt++;}
		}
		if(y==cnt){r=cnt;}		
	}
	return r;
}


int IsAConsonant(char *Sentance , uint16_t sp){
	int r=0;
	for(int x=0;x<sizeof(Consonants);x++){
	   if(	Sentance[sp]==Consonants[x]){r=1;}
	}
	//printf("C%c ",Sentance[sp]);
	return r;
}


int IsAFrontVowel(char *Sentance , uint16_t sp){
	int r=0;
	for(int x=0;x<sizeof(FrontVowels);x++){
	   if(	Sentance[sp]==FrontVowels[x]){r=1;}
	}
	return r;
}

int IsABackVowel(char *Sentance , uint16_t sp){
	int r=0;
	for(int x=0;x<sizeof(BackVowels);x++){
	   if(	Sentance[sp]==BackVowels[x]){r=1;}
	}
	return r;
}

int IsNotLetter(char *Sentance , uint16_t sp){
    int r=0;
    return (Sentance[sp]<'A') || (Sentance[sp]>'Z') ;
}


int IsWhiteSpace(char *Sentance , uint16_t sp){
    int r=0;
    return Sentance[sp]==' ';
}

int IsALetter(char *Sentance , uint16_t sp){
    int r=0;
    return Sentance[sp]>='A';
}


// match rules

//match one or more vowels
struct Match matchVowelsLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
    m.NumChars=0;
    while((IsAVowel(beforebrackets,ptr-m.NumChars)==1) && ((ptr-m.NumChars)>0) ){m.NumChars++;}
    m.Matched=(m.NumChars>0);
    return m;
}
//match one or more vowels
struct Match matchVowelsRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
    m.NumChars=0;
    while((IsAVowel(afterbrackets ,ptr+m.NumChars)==1) && ((ptr+m.NumChars)<sentancelength) ){m.NumChars++;}
    m.Matched=(m.NumChars>0);
    return m;
}


//match a letter
struct Match matchLetterLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
    m.NumChars=0;
    if((IsALetter(beforebrackets,ptr-m.NumChars)==1) && ((ptr-m.NumChars)>0) ){m.NumChars++;}
    m.Matched=(m.NumChars>0);
    return m;
}
//match not a letter
struct Match matchNoLetterLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
    m.NumChars=0;
    if((IsNotLetter(beforebrackets,ptr-m.NumChars)==1)){m.NumChars++;}
    m.Matched=(m.NumChars>0);
    return m;
}


//match A Letter
struct Match matchLetterRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
    m.NumChars=0;
    if((IsALetter(afterbrackets ,ptr+m.NumChars)==1) && ((ptr+m.NumChars)<sentancelength) ){m.NumChars++;}
    m.Matched=(m.NumChars>0);
    return m;
}
//match NotA Letter
struct Match matchNoLetterRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
    m.NumChars=0;
    if((IsNotLetter(afterbrackets ,ptr+m.NumChars)==1)){m.NumChars++;}
    m.Matched=(m.NumChars>0);
    return m;
}

//match single voiced consonant
struct Match matchVoicedConsonantsLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAVoicedConsonant(beforebrackets,ptr-m.NumChars)==1) && ((ptr-m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars==1);
	return m;
}
//match single voiced consonant
struct Match matchVoicedConsonantsRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAVoicedConsonant(afterbrackets,ptr+m.NumChars)==1) && ((ptr+m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars==1);
	return m;
}

//match single front vowel
struct Match matchFrontVowelLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAFrontVowel(beforebrackets,ptr-m.NumChars)==1) && ((ptr-m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars==1);
	return m;
}
//match single front vowel
struct Match matchFrontVowelRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAFrontVowel(afterbrackets,ptr+m.NumChars)==1) && ((ptr+m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars==1);
	return m;
}

//match a single consonant
struct Match matchOneConsonantLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    if((IsAConsonant(beforebrackets,ptr-m.NumChars)==1) && ((ptr-m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars==1);
	return m;
}
//match a single consonant
struct Match matchOneConsonantRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    if((IsAConsonant(afterbrackets,ptr+m.NumChars)==1) && ((ptr+m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars==1);
	return m;
}

//matchZeroOrMoreConsonants
struct Match matchZeroOrMoreConsonantsLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAConsonant(beforebrackets,ptr-m.NumChars)==1) && ((ptr-m.NumChars)>0) ){m.NumChars++;}
	m.Matched=1;
	return m;
}
//matchZeroOrMoreConsonants
struct Match matchZeroOrMoreConsonantsRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAConsonant(afterbrackets,ptr+m.NumChars)==1) && ((ptr+m.NumChars)>0) ){m.NumChars++;}
	m.Matched=1;
	return m;
}

//matchOneOrMoreConsonants
struct Match matchOneOrMoreConsonantsLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAConsonant(beforebrackets,ptr-m.NumChars)==1) && ((ptr-m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars>0);
	return m;
}
//matchOneOrMoreConsonants
struct Match matchOneOrMoreConsonantsRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAConsonant(afterbrackets,ptr+m.NumChars)==1) && ((ptr+m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars>0);
	return m;
}

//match a back vowel
struct Match matchBackVowelLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    if((IsABackVowel(beforebrackets,ptr-m.NumChars)==1) && ((ptr-m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars==1);
	return m;
}
//match a back vowel
struct Match matchBackVowelRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    if((IsABackVowel(afterbrackets,ptr+m.NumChars)==1) && ((ptr+m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars==1);
	return m;
}

//matchOneOrMoreVowels
struct Match matchOneOrMoreVowelsLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAVowel(beforebrackets,ptr-m.NumChars)==1) && ((ptr-m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars>0);
	return m;
}
//matchOneOrMoreVowels
struct Match matchOneOrMoreVowelsRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
    struct Match m;
	m.NumChars=0;
    while((IsAVowel(afterbrackets,ptr+m.NumChars)==1) && ((ptr+m.NumChars)>0) ){m.NumChars++;}
	m.Matched=(m.NumChars>0);
	return m;
}

//match single sibilant
struct Match matchIsASibilantLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){ //wont work
	struct Match m;
    m.NumChars=IsASibilantLeft(beforebrackets,ptr);
	m.Matched=(m.NumChars==1);
	return m;
	
}
//match single sibilant
struct Match matchIsASibilantRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){ 
	struct Match m;
    m.NumChars=IsASibilantRight(afterbrackets,ptr);
	m.Matched=(m.NumChars==1);
	return m;
	
}

//match T S R D L Z N J TH CH SH 
struct Match matchIsAtLeft(char *beforebrackets , uint16_t ptr, int16_t sentancelength){//wont work
	struct Match m;
    m.NumChars=IsAtLeft(beforebrackets,ptr);
	m.Matched=(m.NumChars==1);
	return m;
	
}

//match T S R D L Z N J TH CH SH 
struct Match matchIsASuffixRight(char *afterbrackets , uint16_t ptr, int16_t sentancelength){
	struct Match m;
    m.NumChars=IsASuffixRight(afterbrackets,ptr);
	m.Matched=(m.NumChars==1);
	return m;
	
}


#ifdef test
//test generators

uint16_t RandomN(uint16_t Rfrom,uint16_t Rto){
    uint16_t value = rand() % (Rto - Rfrom + 1)+ Rfrom;
    return value;	
}

void AddVowels(uint8_t number){
	uint16_t p=slen(testoutput);
	for (int x=0;x<number;x++){
	    testoutput[p+x]=Vowels[RandomN(0,slen(Vowels))];	
	}
	testoutput[p+number]=0;
}

void AddConsonants(uint8_t number){
	uint16_t p=slen(testoutput);
	for (int x=0;x<number;x++){
	    testoutput[p+x]=Consonants[RandomN(0,slen(Consonants))];	
	}
	testoutput[p+number]=0;
}

void AddVoicedConsonants(uint8_t number){
	uint16_t p=slen(testoutput);
	for (int x=0;x<number;x++){
	    testoutput[p+x]=VoicedConsonants[RandomN(0,slen(VoicedConsonants))];	
	}
	testoutput[p+number]=0;
}

void AddFrontVowel(uint8_t number){
	uint16_t p=slen(testoutput);
	for (int x=0;x<number;x++){
	    testoutput[p+x]=FrontVowels[RandomN(0,slen(FrontVowels))];	
	}
	testoutput[p+number]=0;
}

void AddBackVowel(uint8_t number){
	uint16_t p=slen(testoutput);
	for (int x=0;x<number;x++){
	    testoutput[p+x]=BackVowels[RandomN(0,slen(BackVowels))];	
	}
	testoutput[p+number]=0;
}

void AddLetter(uint8_t number){
	uint16_t p=slen(testoutput);
	for (int x=0;x<number;x++){
	    testoutput[p+x]=Letters[RandomN(0,slen(Letters))];	
	}
	testoutput[p+number]=0;
}

void AddNotALetter(uint8_t number){
	uint16_t p=slen(testoutput);
	for (int x=0;x<number;x++){
	    testoutput[p+x]=NoLetters[RandomN(0,slen(NoLetters))];	
	}
	testoutput[p+number]=0;
}

void AddAt(uint8_t number){ //multiple chars added
	uint8_t ArrayLen=11;
	uint16_t p=slen(testoutput);
	uint8_t r=RandomN(0,ArrayLen);
	uint8_t y=0;
	char c;
	for (int x=0;x<number;x++){
		while((c=AtRight[r][y])>0){
	        testoutput[p+x+y]=c;	
		    y++;
		}
	}
	testoutput[p+y+number]=0;
}

void AddSibilants(uint8_t number){ //multiple chars added
    uint8_t ArrayLen=8;
	uint16_t p=slen(testoutput);
	uint8_t r=RandomN(0,ArrayLen);
	uint8_t y=0;
	char c;
	for (int x=0;x<number;x++){
		while((c=SibilantsRight[r][y])>0){
	        testoutput[p+x+y]=c;	
		    y++;
		}
	}
	testoutput[p+y+number]=0;
}

void AddSuffixs(uint8_t number){ //multiple chars added
    uint8_t ArrayLen=6;
	uint16_t p=slen(testoutput);
	uint8_t r=RandomN(0,ArrayLen);
	uint8_t y=0;
	char c;
	for (int x=0;x<number;x++){
		while((c=SuffixsRight[r][y])>0){
	        testoutput[p+x+y]=c;	
		    y++;
		}
	}
	testoutput[p+y+number]=0;
}

void AddtoTestOutput(char * teststring){
	uint16_t p=slen(testoutput);
	uint16_t n=slen(teststring);
	
	for (int x=0;x<n;x++){
		testoutput[p+x]=teststring[x];
	}
	testoutput[p+n]=0;
}	


void AddTestStringfromBrackets(char * bracketcontent){
	uint8_t l=slen(bracketcontent);
	//printf("Test String L %i",l);
	char bc;
	uint16_t p=0;
	for(int x=0;x<l;x++){
		bc=bracketcontent[x];
		if(bc=='#'){AddVowels(2);}
		if(bc=='.'){AddVoicedConsonants(1);} 
			
		if(bc=='&'){AddSibilants(1);} 
		if(bc=='@'){AddAt(1);}
		
		if(bc=='^'){AddConsonants(1);}
		if(bc=='+'){AddFrontVowel(1);} 
		if(bc==':'){AddConsonants(2);}
		if(bc=='*'){AddConsonants(2);}
		if(bc=='>'){AddBackVowel(1);}
	
		if(bc=='<'){AddNotALetter(1);} 
		
			
		if(bc=='?'){AddVowels(2);}
			
		if(((bc>='A') && (bc<='Z')) || bc=='\''){
			p=slen(testoutput);
			testoutput[p++]=bc;	
	        testoutput[p]=0;
		}
		p=slen(testoutput);
		if (p>maxtestoutput){
			p=0;
	        testoutput[p]=0;
			printf("Test output buffer ovverrun \n");
		}
		
	}
}

void CreateTestString(char * beforebrackets,char * inbrackets, char * afterbrackets){
	int x=0;
	char c=0;
	AddTestStringfromBrackets(beforebrackets);
	
	AddtoTestOutput(inbrackets);
	
	AddTestStringfromBrackets(afterbrackets);			
}

#endif


//others

void AddtoOutput(char * allophones){
    uint16_t p=slen(AlloBuffer);
    uint16_t n=slen(allophones);
    indent=3;
    if(debug)printf("\n");
    Doindent();
    if(debug)printf("Add %i allophones ",n);
    for (int x=0;x<n;x++){
        AlloBuffer[p]=allophones[x];
	if(debug)printf("%02X ",AlloBuffer[p]);
	p++;
	AlloBuffer[p]=0;
	if (p>ALLOBUFFERMAX){
	    printf("Allophone buffer Overrun\n");
	    p=0;
	    AlloBuffer[p]=0;
	}
    }
#ifdef test
    printf(" From 0x%X\n",LastRule);	
#endif
    if(debug)printf("\n");
}	

void PrintOutput(void){
    char c=0;
    uint16_t x=0;
    if(debug)printf("Allophones (%i)\n",slen(AlloBuffer));
    uint8_t d=0;
    uint8_t l=0;
    while((c=AlloBuffer[x])>0){
		if(c==Pa1){c=0;} //convert PA1 back to 0
		if(format==2){
			if(d==0){
				printf("%i DATA ",1000+l*10);
				l++;
			}
			d++;
			if(d==9){
				printf("%i\n",c);
				d=0;
			}else{
			    printf("%i,",c);	
			}
		}
		if(format==0)printf("%02X ",c);
		if(format==1)printf("%i ",c);
		x++;
	}		
	if(format==2)printf("\n%i DATA 255\n",1000+l*10);
	printf("\n");
	
}

void stringtoupper(char * string){
	uint16_t x=0;
	while(string[x]>0){
		string[x]=toupper(string[x]);
		x++;
	}
}


void printmatchposition(char *beforebrackets,uint8_t beforeptr,char *inbrackets,uint8_t inptr,char *afterbrackets,uint8_t afterptr){
    
	uint8_t beforelen=slen(beforebrackets);
	uint8_t inlen=slen(inbrackets);
	uint8_t afterlen=slen(afterbrackets);
	
	Doindent();
	int bl=slen(beforebrackets)-5;
	if (bl<0){bl=0;}
	printf("%s[%s](%.5s)\n",beforebrackets+bl,inbrackets,afterbrackets);
	
	printf("\n");
}

void printsentancepositionFull(char *sentance,uint16_t sentptr){
	Doindent();
	printf("--- %s ---\n    ",sentance);
	uint16_t sentancelen=slen(sentance);
	for(int a=0;a<sentancelen;a++){
		if(a==sentptr){printf("^");}else{printf("_");}
	}
	printf("\n");
}

void printsentanceposition(char *sentance,uint16_t sentptr){
	uint8_t excerpt=20;
	uint16_t x=0;
	uint16_t sentancelen=slen(sentance);
	//Doindent();
	printf("   ");
	int start=sentptr-excerpt/2;
	if (start<0)start=0;
	int end=start+excerpt;
	if (end>sentancelen)end=sentancelen;
	
	for(x=start;x<end;x++){
	    printf("%c",sentance[x]);
	}
	printf("\n   ");
	///Doindent();
	for(x=start;x<end;x++){
		if(x==sentptr){
	        printf("^");
		}else{
		    printf("_");
		}
	}
	printf("\n");
}




int ExtractBrackets(uint16_t R,char *beforebrackets, char *inbrackets,char *afterbrackets){
   	char c;
	uint8_t incptr=0;
	uint8_t outcptr=0;
	uint8_t brackets=0;
	
	beforebrackets[0]=0;
	inbrackets[0]=0;
	afterbrackets[0]=0;
	
	while((c=Rules[R].rule[incptr])>0){
	   
		if(c=='[') {
		    brackets=1;
			beforebrackets[outcptr]=0;
			outcptr=0;
		}
		if(c==']') {
			brackets=2;
			inbrackets[outcptr]=0;
			outcptr=0;
		}  	  
		if(c!=']' && c!='[') {
			if (brackets==0){
				beforebrackets[outcptr]=c;
				outcptr++;
			}
			if (brackets==1){
				inbrackets[outcptr]=c;
				outcptr++;
			}
			if (brackets==2){
				afterbrackets[outcptr]=c;
				outcptr++;
			}
		}
		incptr++;
		if(incptr>10){
			printf("Rule parse fail rule longer than 10\n");
			while(1);
		}
	}
	afterbrackets[outcptr]=0;
	return incptr;
}	



/*
Patterns:
---------
#	09	one or more vowels
.	0A	voiced consonant: B D G J L M N R V W X
%	0B	suffix: ER E ES ED ING ELY (FUL?) -- suffix only
&	0C	sibilant: S C G Z X J CH SH -- Prefix only
@	0D	T S R D L Z N J TH CH SH preceding long U --- Prefix Only
^	0E	one consonant
+	0F	front vowel: E I Y
:	10	zero or more consonants
*	11	one or more consonants
>	12	back vowel: O U
<	13	anything other than a letter
?	14	two or more vowels

*/

uint8_t lefthandmatch(char *beforebrackets,char *Sentance,uint16_t sp){
	//passed all of before brackets, sentance and sentance pointer
	Doindent();
	//printf("LeftHandMatch {%s} ",beforebrackets);
	
	//if at the begining of the sentance, cant match, otherwise start -1 position.
	if(sp>0){sp--;}else{return 1;}
	
	if(debug>1)printf("LHM %c ",Sentance[sp]);
	int bb=0;
	int b=0;
	struct Match m;
	//uint8_t r=0;
	uint8_t rr=0;
	uint8_t result=0;
	uint16_t sentancelength=slen(Sentance);
	uint8_t bbl=slen(beforebrackets);
	if(debug>1)printf("Lbb-sp %i %i\n",bbl,sp);
	//if matches longer than before brackets truncate
	if(bbl>sp)bbl=sp;
	if (bbl==0){
	    result=1;
	}else{
		for (uint8_t b=bbl;(b>0) && (sp>0);b--){
			bb=beforebrackets[b-1];
			m.NumChars=0;m.Matched=0;
			//printf("B%i(%i) ",b,bb);
			if(bb=='#'){m=matchVowelsLeft(Sentance , sp,sentancelength);}
			if(bb=='.'){m=matchVoicedConsonantsLeft(Sentance , sp,sentancelength);} //return IsAVoicedConsonants(Sentance , sp);}
			
			if(bb=='&'){m=matchIsASibilantLeft(Sentance , sp, sentancelength);} //multiple
			if(bb=='@'){m=matchIsAtLeft(Sentance , sp, sentancelength);} //multiple
		
			if(bb=='^'){m=matchOneConsonantLeft(Sentance , sp,sentancelength);}
			if(bb=='+'){m=matchFrontVowelLeft(Sentance , sp,sentancelength);} 
			if(bb==':'){m=matchZeroOrMoreConsonantsLeft(Sentance , sp, sentancelength);}//needs work
			if(bb=='*'){m=matchOneOrMoreConsonantsLeft(Sentance , sp, sentancelength);}
			if(bb=='>'){m=matchBackVowelLeft(Sentance , sp, sentancelength);}
	
			if(bb=='<'){m=matchNoLetterLeft(Sentance , sp,sentancelength);} 
			
			if(bb=='?'){m=matchOneOrMoreVowelsLeft(Sentance , sp, sentancelength);} //needs work
		
			//match single letters 
			if(debug>1)printf("MSL %c=='%c'\n",bb,Sentance[sp]);
			if(bb==Sentance[sp]){m.NumChars=1;m.Matched=1;}
	
			//if match fails at any letter stop & return
			if(m.Matched==0){
			   //if(debug)printf("MF ");
			   return 0;
			}
			
			//matched, move on
			sp=sp-m.NumChars;
			rr=rr+m.NumChars;
			if(sp>64000){sp=0;printf("SP! ");}
			//printf("sp:%i \n",sp);
		}
		//printf("MBB %i\n",rr);
		//if(rr>=bbl){result=1;}
		//if(rr>=abl){result=1;}

                //return matched chars limit matches to length
                result=rr;
                if(rr>bbl)result=bbl;

	}
	if(debug>1)printf("LM:%i ",result);
	
	return result;
	
}

uint8_t righthandmatch(char *afterbrackets,char *Sentance,uint16_t sp){
	//passed all of after brackets, sentance and sentance pointer
	Doindent();
//	printf("RightHandMatch %s ",afterbrackets);
	int ab=0;
	int b=0;
	struct Match m;
	uint8_t rr=0;
	uint8_t result=0;
	uint16_t sentancelength=slen(Sentance);
	if(debug>1)printf("RHM %c ",Sentance[sp]);
	uint8_t abl=slen(afterbrackets);
	if (abl==0){
	    //nothing to match return matched
	    result=1; 
	}else{
		for (uint8_t b=0;b<abl;b++){
			m.NumChars=0;m.Matched=0;
			ab=afterbrackets[b];
			if(ab=='#'){m=matchVowelsRight(Sentance , sp,sentancelength);}
			if(ab=='.'){m=matchVoicedConsonantsRight(Sentance , sp,sentancelength);}
			
			if(ab=='%'){m=matchIsASuffixRight(Sentance , sp,sentancelength);} //multiple
			if(ab=='&'){m=matchIsASibilantRight(Sentance , sp, sentancelength);} //multiple

			if(ab=='^'){m=matchOneConsonantRight(Sentance , sp,sentancelength);}
			if(ab=='+'){m=matchFrontVowelRight(Sentance , sp,sentancelength);} 
			if(ab==':'){m=matchZeroOrMoreConsonantsRight(Sentance , sp, sentancelength);}//needs work
			if(ab=='*'){m=matchOneOrMoreConsonantsRight(Sentance , sp, sentancelength);}
			if(ab=='>'){m=matchBackVowelRight(Sentance , sp, sentancelength);}
			
			if(ab=='<'){m=matchNoLetterRight(Sentance , sp,sentancelength);} 
			
			if(ab=='?'){m=matchOneOrMoreVowelsRight(Sentance , sp, sentancelength);} //needs work

			//match single letters
			if(debug>1)printf("MSL %c=='%c'\n",ab,Sentance[sp]);
			if(ab==Sentance[sp]){m.NumChars=1;m.Matched=1;}
			
			
			//if match fails at any letter
			if(m.Matched==0){
			   //if(debug)printf("MF ");
			   return 0;
			}
			
			//matched, move on
			sp=sp+m.NumChars;
			rr=rr+m.NumChars;
		}
		//printf("rhm %i %i - ",r,abl);
		//if(rr>=abl){result=1;}
		//return matched chars limit matches to length
		result=rr;
		if(rr>abl)result=abl;
		
	}
	if(debug>1)printf("RM:%i ",result);
	return result; //skip matched letters in input text, matched is different.
	
}



void PrintSentancePos(char * Sentance, int16_t sp,int8_t match){
    uint16_t splen=slen(Sentance);
    uint16_t spstart=sp-10;
    if(spstart<0)spstart=0;
    uint16_t c=0;
    for(int x=spstart;x<splen;x++){
        if(c<20){
            if(x==sp){printf("|");}
			printf("%c",Sentance[x]);
			if(x==sp+match-1){printf("|");}
			c++;
	    }
    }
	
}

uint16_t MatchRuleBrackets(char *Sentance , uint16_t sp){
	
  	char letter = Sentance[sp];
	char beforebrackets[12];
	char inbrackets[12];
	char afterbrackets[12];
	uint8_t match=0;

	uint8_t beforeptr=0;
	uint8_t inptr=0;
	uint8_t afterptr=0;
	//uint8_t result=0;
	uint8_t resultl=0;
	uint8_t resultr=0;
	uint8_t lib=0;
	
    uint16_t R=0;
	for (R=0;Rules[R].RuleNo>0;R++){
		// fast match on first letter
		inptr=0;
		match=0;
		
		if(letter==Rules[R].letter){
			//printf("Match on %c \n",Rules[R].letter);
			//get content in brackets for rule
	        ExtractBrackets(R,beforebrackets,inbrackets,afterbrackets);
			//compare letters in Sentance with letters in brackets
			while(inbrackets[inptr]>0){
				if(inbrackets[inptr]==Sentance[sp+inptr]){match++;}
				inptr++;
			}	
			lib=slen(inbrackets);
			//result=0;resultl=0;resultr=0;
			//inbrackets match
			if(lib==match){
				if(debug){
					if(debug>1)printf("\n");
				    indent=3;
			        Doindent();
				    printf("RULE %i(0x%X) ML%i MIB[%s] : ",R,Rules[R].RuleNo,match,inbrackets);
				}
#ifdef test
				LastRule=Rules[R].RuleNo;
#endif
				//printmatchposition(beforebrackets,beforeptr,inbrackets,inptr,afterbrackets,afterptr);
				
				//int l=strlen(beforebrackets);
				//if (l<0){l=0;}
				//Doindent();
//				printf("Matching %s[%s](%.5s)\n",beforebrackets+l,inbrackets,afterbrackets);
				if(debug){
					printf("Matching %s[%s]%s  : ",beforebrackets,inbrackets,afterbrackets);
				    //Doindent();
					PrintSentancePos(Sentance,sp,match);
				    //indent=6;
				    //printf("\n");
					printf(" : ");
				}
				
				//lefthand match
				resultl=lefthandmatch(beforebrackets,Sentance,sp);
				
				//righthand match
				resultr=righthandmatch(afterbrackets,Sentance,sp+match); //start after the [] contents
				
				//printf("\n");
				//Doindent();
				if(debug)printf("L%i R%i \n",resultl,resultr);
				if(resultl>0 && resultr>0){
				    //Doindent();
 				    
				    AddtoOutput(Rules[R].allophone);
				    if(debug)printf("\n");
					//printf(" : ");
				    return match;
				}
			}
		}
	}
	
//	printf("Failed to match %i %i %s \n",R,Rules[R].RuleNo,Rules[R].letter);
	if(debug)printf("Failed to match Pos:%i(%c) Len:%i\n",sp,Sentance[sp],slen(Sentance));
	
	return 0;
}


uint16_t MatchExeptionRule(char *Sentance , uint16_t sp){
	//return unmatched if not a space, or at the begining
	if ((Sentance[sp]!=' ') && (sp!=0)) return 0;
	
	uint16_t R=0;
	uint16_t spl=slen(Sentance);
	uint16_t wl=0;
	char Space[3]={PA2,0};
	//find next letter
	while((Sentance[sp]<'A') && (sp<spl) ) {
		sp++;
		R++;
	}	
	
	//find length of word
	while((Sentance[sp+wl]>='A') && ((sp+wl)<spl )) {
		wl++;
	}	
	//wl--;
	//printf("WL %i ",wl);
	//if end of sentance return unmatched
	if (sp==spl)return 0;
	
	uint16_t x=0;
	uint8_t y=0;
	uint8_t el=0;
	uint8_t m=0;
	while(RuleExceptions[x].exept[0]!=0){
	    el=slen(RuleExceptions[x].exept);
		if(debug>2)printf("Comparing %s(%i) %i\n",RuleExceptions[x].exept,el,wl);
		
	    m=0;
	    //do lengths match?
		if(el==wl){
			for(y=0;y<el;y++){
			   if(Sentance[sp+y]==RuleExceptions[x].exept[y])m++;
			}	   
			if((m==el) && (el!=0) ){
				R=R+el;
				indent=3;
				if(debug){
					Doindent();
					if(debug)printf("Matched Exception '%s' returned %i\n",RuleExceptions[x].exept,R);
				}
				
				//add in leading space
				AddtoOutput(Space);
				
				//add allophones
				AddtoOutput(RuleExceptions[x].allophone);
				
				if(debug){printf("\n");}
				return R;
			}
		}
	    x++;	
	}	
	return 0;
}

//cutdown version of say for WBW
int sayWBW(char * sentance){
        uint16_t R=1;
        uint16_t sp=0;
       
        stringtoupper(sentance);
       
        while(sp<slen(sentance) ){
                indent=0;
                if(debug){
                    printsentanceposition(sentance,sp);
                    printf("\n");
                }
                indent=3;

                //match extensions
                R=MatchExeptionRule(sentance,sp);

                //else match rules
                if(R==0){
                    R=MatchRuleBrackets(sentance,sp);
                }

                if(R>0){sp=sp+R;}else{sp++;}
                indent=0;
        }
        //PrintOutput();

}


int say(char * sentance,uint8_t matchonce){
	uint16_t R=1;
	uint16_t sp=0;
	//printf("String to Upper\n");
	
	stringtoupper(sentance);
	//printf("QfterToUpper\n");
	printf("Sentance |%s| \n",sentance);
	//while(sp<slen(sentance) && R>0){
	while(sp<slen(sentance) ){
		indent=0;
		if(debug){
		    printsentanceposition(sentance,sp);
		    printf("\n");
		}	
		indent=3;
		
		//match extensions
		//if(debug)printf("Match Extension position %i\n",sp);
		R=MatchExeptionRule(sentance,sp);
		
		//else match rules
		if(R==0){
	   	    //if(debug)printf("Match Rule at position %i\n",sp);
		    R=MatchRuleBrackets(sentance,sp);
		}
		
		if(R>0){sp=sp+R;}else{sp++;}
		indent=0;
		//printsentanceposition(sentance,sp);
		//if match once, run until matched.
#ifdef test
		if ((R>0) && (matchonce==1) && (TestRule==LastRule)) sp=65535;
#endif
	}

	//flag is rule matches different to test rule
#ifdef test
	if (TestRule!=LastRule){
	    printf("\n####### TestRule (0x%X), NOT MATCHED to Rule (0x%X) #######\n\n",TestRule,LastRule);
	}
#endif	
	
	
}

#ifdef test
int ruletest(void){
	uint16_t R=0;
	char beforebrackets[12];
	char inbrackets[12];
	char afterbrackets[12];
	testAlloBuffer[0]=0;
	uint16_t p=0;
	uint16_t bts;
	for (R=0;Rules[R].RuleNo>0;R++){
		//printf("%i, Ox%X %c %s - ",R,Rules[R].RuleNo,Rules[R].letter,Rules[R].rule);
		indent=0;
		Doindent();
		printf("\n ======= TEST RULE %i, Ox%X %c - ",R,Rules[R].RuleNo,Rules[R].letter);
		TestRule=Rules[R].RuleNo;
		//testAlloBuffer[0]=0;
		bts=slen(testoutput);
		//printf("Ext Brackets\n");
		ExtractBrackets(R,beforebrackets,inbrackets,afterbrackets);
		//printf("Test string\n");
		CreateTestString(beforebrackets,inbrackets,afterbrackets);
		indent=3;
		Doindent();
		//printf("Before in after\n");
		printf("%s[%s]%s ",beforebrackets,inbrackets,afterbrackets);
		
		p=slen(testoutput);
		//printf("New P %i\n",p);
		testoutput[p]=' ';
		testoutput[p+1]=0;
		//testoutput[p]=' ';
		//testoutput[p]=0;
		for(int x=bts;x<p;x++)printf("%c",testoutput[x]);
		Doindent();
		printf("\n");
		//indent=6;
		//Doindent();
		//printf("Test Say %s\n",testoutput);
		say(testoutput,1);	
		PrintOutput();		
		//reset test output and allophone output
		testoutput[0]=0;
		AlloBuffer[0]=0;
	}
	indent=0;
	Doindent();
	//printf("\nTestOutput\n%s\n\n",testoutput);
	//say(testoutput);
}
#endif

#ifndef WBW
int main(int argc, char *argv[]) {
#ifdef test
	ruletest();
#else
	//char sentance[]="The Quick brown OREGON fox jumps over the lazy dog.";
	//char sentance[]="shure sure chew obliterate everything GREAT ok";
	if (argc < 2){
	   printf("No string to process\n");
	}else{
	  if (argc==3){
		  if (argv[2][0]=='D') debug=2;
		  if (argv[2][0]=='d') debug=1;
      }
	  say(argv[1],0);
	  PrintOutput();
	  
	}
#endif

	
	if (debug)printf("\nDONE\n");
	return 0;
}
#endif

