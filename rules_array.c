
#include <stdio.h>
#include <stdint.h>
#include <string.h>


//Allophone Defines


#define Pa1    64 // should be 0 converted back in output.
#define PA2    1
#define PA3    2
#define PA4    3
#define PA5    4
#define OY1    5
#define AY1    6
#define EH1    7
#define KK3    8
#define PP1    9
#define JH1    10
#define NN1    11
#define IH1    12
#define TT2    13
#define RR1    14
#define AX1    15
#define MM1    16
#define TT1    17
#define DH1    18
#define IY1    19
#define EY1    20
#define DD1    21
#define UW1    22
#define AO1    23
#define AA1    24
#define YY2    25
#define AE1    26
#define HH1    27
#define BB1    28
#define TH1    29
#define UH1    30
#define UW2    31
#define AW1    32
#define DD2    33
#define GG3    34
#define VV1    35
#define GG1    36
#define SH1    37
#define ZH1    38
#define RR2    39
#define FF1    40
#define KK2    41
#define KK1    42
#define ZZ1    43
#define NG1    44
#define LL1    45
#define WW1    46
#define XR1    47
#define WH1    48
#define YY1    49
#define CH1    50
#define ER1    51
#define ER2    52
#define OW1    53
#define DH2    54
#define SS1    55
#define NN2    56
#define HH2    57
#define OR1    58
#define AR1    59
#define YR1    60
#define GG2    61
#define EL1    62
#define BB2    63

/*
Code-To-Speech Rules extracted from CTS256A-AL2
===============================================

Patterns:
---------
#	09	one or more vowels
.	0A	voiced consonant: B D G J L M N R V W X
%	0B	suffix: ER E ES ED ING ELY (FUL?)
&	0C	sibilant: S C G Z X J CH SH
@	0D	T S R D L Z N J TH CH SH preceding long U
^	0E	one consonant
+	0F	front vowel: E I Y
:	10	zero or more consonants
*	11	one or more consonants
>	12	back vowel: O U
<	13	anything other than a letter
?	14	two or more vowels
$	1F	Not a pattern symbol, ignored by the ROM
		Should probably be a D: [I]D% = [AY] instead of [I]$% = [AY]
		
*/

/* 
  RULES from https://github.com/GmEsoft/CTS256A-AL2/ and extracted from the origional CTS256 Chip

  Exceptions array from  https://github.com/mecparts/CTS256-exceptions 
*/

char Vowels[]={'A','E','I','O','U',0};
char VoicedConsonants[]={'B','D','G','J','L','M','N','R','V','W','X',0};
char Letters[]={'A','B','C','D','E','F','G','H','I','J','L','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',0};
char NoLetters[]={' ','.',',',0};

char Consonants[]={'A','B','C','D','F','G','H','J','K','L','M','N','P','Q','R','S','T','V','W','X','Y','Z',0};
char FrontVowels[]={'E','I','Y',0};
char BackVowels[]={'O','U'};

char SuffixsRight[][4]={{'E','R',0},{'E',0},{'E','S',0},{'E','D',0},{'I','N','G',0},{'E','L','Y',0}};
char SibilantsRight[][4]={{'S',0},{'C',0},{'G',0},{'Z',0},{'X',0},{'J',0},{'C','H',0},{'S','H',0}};
char AtRight[][4]={{'T',0},{'S',0},{'R',0},{'D',0},{'L',0},{'Z',0},{'N',0},{'J',0},{'T','H',0},{'C','H',0},{'S','H',0}};

//reversed arrays
char SibilantsLeft[][4]={{'S',0},{'C',0},{'G',0},{'Z',0},{'X',0},{'J',0},{'H','C',0},{'H','S',0}};
char AtLeft[][4]={{'T',0},{'S',0},{'R',0},{'D',0},{'L',0},{'Z',0},{'N',0},{'J',0},{'H','T',0},{'H','C',0},{'H','S',0}};




struct Rule {
    
    char letter;
	uint16_t RuleNo;
    char rule[15];
	char allophone[20];
};


struct Rule Rules[450]={

////== [ : ] ==
{'-',0xF78C,"[-]",{Pa1,0}},
{'\'',0xF78E,"['S]",{ZZ1,0}},
{'\'',0xF792,"#:.E['S]",{ZZ1,0}},
{'\'',0xF799,"#['S]",{ZZ1,0}},
{'\'',0xF79D,"['S]",{SS1,0}},
{',',0xF7A2,"[,]",{PA4,0}},
{';',0xF7A4,"[;]",{PA4,0}},
{' ',0xF7A6,"[ ]",{PA2,0}},
{'.',0xF7A8,"[.]",{PA5,PA5,0}},
{'!',0xF7AB,"[!]",{PA5,PA5,0}},
{'?',0xF7AE,"[?]",{PA5,PA5,0}},
{':',0xF7B1,"[:]",{PA5,0}},
{'%',0xF7B3,"[%]",{PP1,ER2,SS1,SS1,EH1,NN1,TT2,Pa1,0}},
{'$',0xF7BC,"[$]",{DD2,AA1,LL1,ER1,ZZ1,Pa1,0}},
{'#',0xF7C3,"[#]",{NN2,AX1,MM1,BB1,ER1,Pa1,0}},
//{'',0xF7CA,"[]",{PA3,0}},

////== [ A ] ==
{'A',0xF7CC,"<[A]<",{EY1,0}},
{'A',0xF7D0,"[ACHE]",{EY1,PA3,KK2,0}},
{'A',0xF7D6,"[A]<",{AX1,0}},
{'A',0xF7D9,"<[ARE]<",{AR1,0}},
{'A',0xF7DE,"^[AS]#",{EY1,SS1,0}},
{'A',0xF7E3,"<[AR]O",{AX1,RR2,0}},
{'A',0xF7E8,"<[A]^R",{AX1,0}},
{'A',0xF7ED,"[AR]#",{XR1,0}},
{'A',0xF7F0,"<:[ANY]",{EH1,NN1,IY1,0}},
{'A',0xF7F7,"[AGAIN]",{AX1,PA2,GG1,EH1,EH1,NN1,0}},
{'A',0xF801,"[A]WA",{AX1,0}},
{'A',0xF805,"[AW]",{AO1,AO1,0}},
{'A',0xF808,"<:[A]^+<",{EY1,0}},
{'A',0xF80F,"<[A]^#",{AX1,0}},
{'A',0xF814,"[A]^+#",{EY1,0}},
{'A',0xF819,"#:[ALLY]",{AX1,LL1,IY1,0}},
{'A',0xF821,"<[AL]#",{AX1,LL1,0}},
{'A',0xF826,"#:[AG]E",{IH1,PA2,JH1,0}},
{'A',0xF82D,"[A]^%",{EY1,0}},
{'A',0xF831,"[A]^+:#",{AE1,0}},
{'A',0xF837,"<[ARR]",{AX1,RR2,0}},
{'A',0xF83C,"[ARR]",{AE1,RR2,0}},
{'A',0xF840,"<:[AR]<",{AR1,0}},
{'A',0xF845,"[AR]<",{ER1,0}},
{'A',0xF848,"[AR]",{AR1,0}},
{'A',0xF84A,"[AIR]",{EH1,XR1,0}},
{'A',0xF84E,"[AI]",{EY1,0}},
{'A',0xF850,"[AY]",{EY1,0}},
{'A',0xF852,"[AU]",{AO1,0}},
{'A',0xF854,"#:[AL]<",{EL1,0}},
{'A',0xF859,"#:[ALS]<",{EL1,ZZ1,0}},
{'A',0xF860,"[ALK]",{AO1,PA3,KK2,0}},
{'A',0xF865,"[A]L^",{AO1,0}},
{'A',0xF869,"<:[ABLE]",{EY1,PA2,BB2,EL1,0}},
{'A',0xF872,"[ABLE]",{AX1,PA2,BB2,EL1,0}},
{'A',0xF879,"[ANG1]+",{EY1,NN1,PA2,JH1,0}},
{'A',0xF880,"[A]",{AE1,0}},

//== [ B ] ==
{'B',0xF882,"<[B]<",{PA2,BB2,IY1,0}},
{'B',0xF888,"MAY[BE]",{BB2,IY1,0}},
{'B',0xF88E,"<[BE]^#",{PA2,BB2,IY1,0}},
{'B',0xF895,"<[BEEN]<",{BB2,IH1,NN1,0}},
{'B',0xF89D,"<[BOTH]<",{PA2,BB2,OW1,TH1,0}},
{'B',0xF8A6,"<[BUS]#",{PA2,BB2,IH1,ZZ1,0}},
{'B',0xF8AE,"[BUIL]",{PA2,BB2,IH1,IH1,LL1,0}},
{'B',0xF8B6,"[B]B",{0}},
{'B',0xF8B9,"[B]<",{PA2,BB1,0}},
{'B',0xF8BD,"[B]S",{PA2,BB1,0}},
{'B',0xF8C1,"[BT]",{PA3,TT2,0}},
{'B',0xF8C4,"<[B]^",{PA2,BB1,0}},
{'B',0xF8C9,"[B]",{PA2,BB2,0}},


//== [ C ] ==
{'C',0xF8CC,"<[C]<",{SS1,SS1,IY1,0}},
{'C',0xF8D2,"<[CH]^",{PA3,KK1,0}},
{'C',0xF8D7,"^E[CH]",{PA3,KK1,0}},
{'C',0xF8DC,"[CH]",{PA3,CH1,0}},
{'C',0xF8DF,"S[CI]#",{SS1,SS1,AY1,0}},
{'C',0xF8E5,"[CI]#",{SH1,0}},
{'C',0xF8E8,"[CI]O",{SH1,0}},
{'C',0xF8EB,"[CI]EN",{SH1,0}},
{'C',0xF8EF,"[C]+",{SS1,SS1,0}},
{'C',0xF8F3,"C[C]",{0}},
{'C',0xF8F6,"[CK]#",{PA3,KK1,0}},
{'C',0xF8FA,"[CK]",{PA3,KK2,0}},
{'C',0xF8FD,"[COM]%",{PA3,KK3,AX1,MM1,0}},
{'C',0xF904,"[CC]+",{PA3,KK1,SS1,SS1,0}},
{'C',0xF90A,"[C]<",{PA3,KK2,0}},
{'C',0xF90E,"[C]S",{PA3,KK2,0}},
{'C',0xF912,"[C]>",{PA3,KK3,0}},
{'C',0xF916,"[C]",{PA3,KK1,0}},

//== [ D ] ==
{'D',0xF919,"<[D]<",{PA2,DD2,IY1,0}},
{'D',0xF91F,"[D]D",{0}},
{'D',0xF922,"#:[DED]<",{PA2,DD2,IH1,PA2,DD1,0}},
{'D',0xF92C,".E[D]<",{PA2,DD1,0}},
{'D',0xF932,"#*E[D]<",{PA3,TT2,0}},
{'D',0xF939,"<[DE]^#",{PA2,DD2,IH1,0}},
{'D',0xF940,"<[DO]<",{PA2,DD2,UW2,0}},
{'D',0xF946,"<[DOES]",{PA2,DD2,AX1,ZZ1,0}},
{'D',0xF94E,"<[DOING]",{PA2,DD2,UW2,IH1,NG1,0}},
{'D',0xF958,"<[DOW]",{PA2,DD2,AW1,0}},
{'D',0xF95E,"#[DU]:A",{PA2,JH1,UW1,0}},
{'D',0xF965,"[DG]",{PA2,JH1,0}},
{'D',0xF968,"[DJ]",{PA2,JH1,0}},
{'D',0xF96B,"[D]<",{PA2,DD1,0}},
{'D',0xF96F,"[D]S",{PA2,DD1,0}},
{'D',0xF973,"[D]",{PA2,DD2,0}},

//== [ E ] ==
{'E',0xF976,"<[E]<",{IY1,0}},
{'E',0xF97A,"#:[E]<",{0}},
{'E',0xF97F,"'*[E]<",{0}},
{'E',0xF984,"*[E]<",{IY1,0}},
{'E',0xF988,"#[ED]<",{PA2,DD1,0}},
{'E',0xF98D,"#:[E]D<",{0}},
{'E',0xF993,"[EV]ER",{EH1,VV1,0}},
{'E',0xF998,"#*[EL]",{EL1,0}},
{'E',0xF99C,"[ERI]#","[YR IY1]"},
{'E',0xF9A1,"#:[ER]#",{ER1,0}},
{'E',0xF9A6,"[E]^%",{IY1,0}},
{'E',0xF9AA,"[ERI]",{EH1,EH1,RR1,IH1,0}},
{'E',0xF9B0,"[ER]#",{EH1,XR1,0}},
{'E',0xF9B4,"[ER]",{ER1,0}},
{'E',0xF9B6,"<[EVEN]<",{IY1,VV1,IH1,NN1,0}},
{'E',0xF9BF,"<[EVEN]",{IY1,VV1,EH1,EH1,NN1,0}},
{'E',0xF9C8,"#:[EW]",{YY1,UW2,0}},
{'E',0xF9CD,"@[EW]",{UW2,0}},
{'E',0xF9D0,"[EW]",{YY1,UW2,0}},
{'E',0xF9D3,"[E]O",{IY1,0}},
{'E',0xF9D6,"#:&[ES]<",{IH1,ZZ1,0}},
{'E',0xF9DD,"#:[E]S<",{0}},
{'E',0xF9E3,"#:[ELY]<",{LL1,IY1,0}},
{'E',0xF9EA,"#:[EMENT]",{MM1,IH1,NN1,PA3,TT2,0}},
{'E',0xF9F5,"[EFUL]",{FF1,UH1,LL1,0}},
{'E',0xF9FB,"[EER]",{YR1,0}},
{'E',0xF9FE,"[EE]",{IY1,0}},
{'E',0xFA00,"[EARN]",{ER2,NN1,0}},
{'E',0xFA05,"<[EAR]^",{ER2,0}},
{'E',0xFA0A,"*[EAR]",{YR1,0}},
{'E',0xFA0E,"[EAD]",{EH1,EH1,PA2,DD1,0}},
{'E',0xFA14,"#:[EA]<",{IY1,AX1,0}},
{'E',0xFA1A,"[EA]SU",{EH1,0}},
{'E',0xFA1E,"[EA]",{IY1,0}},
{'E',0xFA20,"[EIGH]",{EY1,0}},
{'E',0xFA24,"[EI]",{IY1,0}},
{'E',0xFA26,"<[EYE]",{AY1,0}},
{'E',0xFA2A,"[EY]",{IY1,0}},
{'E',0xFA2C,"[EU]",{UW1,0}},
{'E',0xFA2E,"[E]",{EH1,0}},


//== [ F ] ==
{'F',0xFA30,"<[F]<",{EH1,EH1,FF1,0}},
{'F',0xFA36,"[FU]L",{FF1, UH1,0}},
{'F',0xFA3A,"[F]F",{0}},
{'F',0xFA3D,"[FOUR]",{FF1,OR1,0}},
{'F',0xFA42,"[F]",{FF1,0}},

//== [ G ] ==
{'G',0xFA44,"<[G]<",{PA2,JH1,IY1,0}},
{'G',0xFA4A,"[GIV]",{PA2,GG1,IH1,VV1,0}},
{'G',0xFA50,"<[G]I^",{PA2,GG1,0}},
{'G',0xFA56,"[GE]T",{PA2,GG1,EH1,0}},
{'G',0xFA5B,"SU[GGES]",{PA2,GG2,PA2,JH1,EH1,EH1,SS1,0}},
{'G',0xFA67,"[GG]",{PA2,GG1,0}},
{'G',0xFA6A,"[GREAT]",{PA2,GG3,RR2,EY1,TT2,0}},
{'G',0xFA73,"[G]<",{PA2,GG3,0}},
{'G',0xFA77,"<B#[G]",{PA2,GG2,0}},
{'G',0xFA7D,"[G]+",{PA2,JH1,0}},
{'G',0xFA81,"#[GH]",{FF1,0}},
{'G',0xFA84,"[GH]",{PA2,GG2,0}},
{'G',0xFA87,"[G]",{PA2,GG2,0}},

//== [ H ] ==
{'H',0xFA8A,"<[H]<",{EY1,PA3,CH1,0}},
{'H',0xFA90,"<[HAV]",{HH1,AE1,VV1,0}},
{'H',0xFA96,"<[HERE]",{HH1,YR1,0}},
{'H',0xFA9C,"<[HOUR]",{AW1,ER1,0}},
{'H',0xFAA2,"[HOW]",{HH1,AW1,0}},
{'H',0xFAA6,"[HYP]",{HH1,IH1,PA3,PP1,0}},
{'H',0xFAAC,"[H]>",{HH2,0}},
{'H',0xFAAF,"[H]#",{HH1,0}},
{'H',0xFAB2,"[H]",{0}},

//== [ I ] ==
{'I',0xFAB4,"<[IN]",{IH1,NN1,0}},
{'I',0xFAB8,"N[I]NE",{AY1,0}},
{'I',0xFABD,"[I]<",{AY1,0}},
{'I',0xFAC0,"[IN]D",{AY1,NN1,0}},
{'I',0xFAC4,"<:[I]%",{AY1,0}},
{'I',0xFAC9,"<:[IED]<",{AY1,PA2,DD1,0}},
{'I',0xFAD1,"#*[IED]<",{IY1,PA2,DD1,0}},
{'I',0xFAD9,"FR[IE]ND",{EH1,0}},
{'I',0xFADF,"[IEN]",{IY1,IH1,NN1,0}},
{'I',0xFAE4,"[IE]T",{AY1,IH1,0}},
{'I',0xFAE8,"[IER]",{IY1,ER1,0}},
{'I',0xFAEC,"[I]%",{IY1,0}},
{'I',0xFAEF,"[IE]",{IY1,0}},
{'I',0xFAF1,"[IN]%",{IY1,NN1,0}},
{'I',0xFAF5,"[IR]#",{AY1,ER1,0}},
{'I',0xFAF9,"[I]^%",{AY1,0}},
{'I',0xFAFD,"[I]^+:#",{IH1,0}},
{'I',0xFB03,"[IZ]%",{AY1,ZZ1,0}},
{'I',0xFB07,"[IS]%",{AY1,ZZ1,0}},

//{'I',0xFB0B,"[I]$%",{AY1,0}}, //Maybe [I]D%",{AY1
{'I',0xFB0B,"[I]D%",{AY1,0}}, 

{'I',0xFB0F,"+^[I]^+",{IH1,0}},
{'I',0xFB15,"[I]T%",{AY1,0}},
{'I',0xFB19,"#*[I]^+",{IH1,0}},
{'I',0xFB1F,"[IR]",{ER2,0}},
{'I',0xFB21,"*[I]ON",{YY1,0}},
{'I',0xFB26,"[IGH]",{AY1,0}},
{'I',0xFB29,"[ILD]",{AY1,EL1,PA2,DD1,0}},
{'I',0xFB2F,"[IGN]",{AY1,NN1,0}},
{'I',0xFB33,"[IGN]^",{AY1,NN1,0}},
{'I',0xFB38,"[IGN]%",{AY1,NN1,0}},
{'I',0xFB3D,"[IQUE]",{IY1,PA3,KK2,0}},
{'I',0xFB43,"[I]A",{AY1,0}},
{'I',0xFB46,"M[I]C",{AY1,0}},
{'I',0xFB4A,"[I]",{IH1,0}},

//== [ J ] ==
{'J',0xFB4C,"<[J]<",{PA2,JH1,EY1,0}},
{'J',0xFB52,"[J]",{PA2,JH1,0}},

//== [ K ] ==
{'K',0xFB55,"<[K]<",{PA3,KK1,EY1,0}},
{'K',0xFB5B,"<[K]N",{0}},
{'K',0xFB5F,"[K]<",{PA3,KK2,0}},
{'K',0xFB63,"[K]",{PA3,KK1,0}},

//== [ L ] ==
{'L',0xFB66,"<[L]<",{EH1,EH1,LL1,0}},
{'L',0xFB6C,"[LO]C#",{LL1,OW1,0}},
{'L',0xFB71,"[L]L",{0}},
{'L',0xFB74,"[L]%",{EL1,0}},
{'L',0xFB77,"[LEAD]",{LL1,IY1,PA2,DD1,0}},
{'L',0xFB7E,"[LAUGH]",{LL1,AE1,FF1,0}},
{'L',0xFB85,"[L]",{LL1,0}},

//== [ M ] ==
{'M',0xFB87,"[MB]",{MM1,0}},
{'M',0xFB89,"<[M]<",{EH1,EH1,MM1,0}},
{'M',0xFB8F,"[MOV]",{MM1,UW2,VV1,0}},
{'M',0xFB94,"[M]M",{0}},
{'M',0xFB97,"[M]",{MM1,0}},

//== [ N ] ==
{'N',0xFB99,"<[N]<",{EH1,EH1,NN1,0}},
{'N',0xFB9F,"E[NG1]+",{NN1,PA2,JH1,0}},
{'N',0xFBA5,"[NG]R",{NG1,PA2,GG1,0}},
{'N',0xFBAA,"[NG]#",{NG1,PA2,GG1,0}},
{'N',0xFBAF,"[NGL]%",{NG1,PA2,GG1,EL1,0}},
{'N',0xFBB6,"[NG]",{NG1,0}},
{'N',0xFBB8,"[NK]<",{NG1,PA3,KK2,0}},
{'N',0xFBBD,"[NK]S",{NG1,PA3,KK2,0}},
{'N',0xFBC2,"[NK]",{NG1,PA3,KK1,0}},
{'N',0xFBC6,"<[NOW]<",{NN2,AW1,0}},
{'N',0xFBCC,"[N]N",{0}},
{'N',0xFBCF,"#:[NU]",{NN1,YY1,UW1,0}},
{'N',0xFBD5,"<[N]",{NN2,0}},
{'N',0xFBD8,"[N'T]",{NN1,PA3,TT2,0}},
{'N',0xFBDD,"[N]",{NN1,0}},

//== [ O ] ==
{'O',0xFBDF,"<[O]<",{OW1,0}},
{'O',0xFBE3,"[OF]<",{AX1,VV1,0}},
{'O',0xFBE7,"[OROUGH]",{AX1,AX1,RR2,OW1,0}},
{'O',0xFBF0,"#:[OR]<",{ER1,0}},
{'O',0xFBF5,"#:[ORS]<",{ER1,ZZ1,0}},
{'O',0xFBFC,"[OR]",{OR1,0}},
{'O',0xFBFE,"<[ONE]",{WW1,AX1,NN1,0}},
{'O',0xFC04,"+[ONE]",{WW1,AX1,NN1,0}},
{'O',0xFC0A,"*[OW]N",{AW1,0}},
{'O',0xFC0E,"[OW]",{OW1,0}},
{'O',0xFC10,"<[OVER]",{OW1,VV1,ER1,0}},
{'O',0xFC17,"[OV]",{AX1,VV1,0}},
{'O',0xFC1A,"[O]^%",{OW1,0}},
{'O',0xFC1E,"[O]^EN",{OW1,0}},
{'O',0xFC23,"[O]^I#",{OW1,0}},
{'O',0xFC28,"[OL]D",{OW1,LL1,0}},
{'O',0xFC2C,"[OUGHT]",{AO1,AO1,PA3,TT2,0}},
{'O',0xFC34,"[OUGH]",{AX1,AX1,FF1,0}},
{'O',0xFC3A,"&[OUR]",{OR1,0}},
{'O',0xFC3E,":[OUR]",{AW1,ER1,0}},
{'O',0xFC43,"<[OU]",{AW1,0}},
{'O',0xFC46,":[OU]S#",{AW1,0}},
{'O',0xFC4B,"[OUS]",{AX1,SS1,0}},
{'O',0xFC4F,"[OULD]",{UH1, PA2,DD1,0}},
{'O',0xFC55,"^[OU]^L",{AX1,0}},
{'O',0xFC5A,"[OUP]",{UW2,PA3,PP1,0}},
{'O',0xFC5F,"[OU]",{AW1,0}},
{'O',0xFC61,"[OY]",{OY1,0}},
{'O',0xFC63,"[OING]",{OW1,IH1,NG1,0}},
{'O',0xFC69,"[OI]",{OY1,0}},
{'O',0xFC6B,"[OOR]",{OR1,0}},
{'O',0xFC6E,"[OOK]<",{UH1, PA3,KK2,0}},
{'O',0xFC74,"[OOK]S",{UH1, PA3,KK2,0}},
{'O',0xFC7A,"[OOK]",{UH1, PA3,KK1,0}},
{'O',0xFC7F,"[OOD]<",{UH1, PA2,DD1,0}},
{'O',0xFC85,"[OO]D",{UH1,0}},
{'O',0xFC88,"[OO]",{UW2,0}},
{'O',0xFC8A,"[O]E",{OW1,0}},
{'O',0xFC8D,"[O]<",{OW1,0}},
{'O',0xFC90,"[OAR]",{OR1,0}},
{'O',0xFC93,"[OA]",{OW1,0}},
{'O',0xFC95,"<[ONLY]",{OW1,NN1,LL1,IY1,0}},
{'O',0xFC9D,"<[ONCE]",{WW1,AX1,NN1,SS1,0}},
{'O',0xFCA5,"[ON'T]",{OW1,NN1,PA3,TT2,0}},
{'O',0xFCAC,"C[O]N",{AX1,0}},
{'O',0xFCB0,"[O]NG",{AO1,0}},
{'O',0xFCB4,"<*[O]N",{AX1,0}},
{'O',0xFCB9,"I[ON]",{AX1,NN1,0}},
{'O',0xFCBD,"#:[ON]<",{AX1,NN1,0}},
{'O',0xFCC3,"[O]ST<",{OW1,0}},
{'O',0xFCC8,"[OF]^",{AO1,FF1,0}},
{'O',0xFCCC,"[OTHER]",{AX1,DH2,ER1,0}},
{'O',0xFCD3,"[OSS]<",{AO1,AO1,SS1,SS1,0}},
{'O',0xFCDA,"#*[OM]",{AX1,MM1,0}},
{'O',0xFCDF,"[O]",{AA1,0}},

//== [ P ] ==
{'P',0xFCE1,"[PSYCH]",{SS1,SS1,AY1,PA2,KK1,0}},
{'P',0xFCEA,"<[P]<",{PA3,PP1, IY1,0}},
{'P',0xFCF0,"[PH]",{FF1,0}},
{'P',0xFCF2,"[PEOP]",{PA3,PP1,IY1,PA3,PP1,0}},
{'P',0xFCFA,"[POW]",{PA3,PP1,AW1,0}},
{'P',0xFCFF,"[PUT]<",{PA3,PP1,UH1,PA3,TT2,0}},
{'P',0xFD07,"[P]P",{0}},
{'P',0xFD0A,"[P]",{PA3,PP1,0}},

//== [ Q ] ==
{'Q',0xFD0D,"<[Q]<",{PA3,KK1,YY1,UW2,0}},
{'Q',0xFD14,"[QUAR]",{PA3,KK3,WH1,AA1,0}},
{'Q',0xFD1B,"[QUE]<",{PA3,KK1,YY1,UW2,0}},
{'Q',0xFD22,"[QU]",{PA3,KK3,WH1,0}},
{'Q',0xFD26,"[Q]",{PA3,KK3,0}},

//== [ R ] ==
{'R',0xFD29,"<[R]<",{AR1,0}},
{'R',0xFD2D,"<[RE]^#",{RR1,IY1,0}},
{'R',0xFD33,"[RH]",{RR1,0}},
{'R',0xFD35,"[R]R",{0}},
{'R',0xFD38,"*[R]",{RR2,0}},
{'R',0xFD3B,"[R]",{RR1,0}},

//== [ S ] ==
{'S',0xFD3D,"<[S]<",{EH1,EH1,SS1,SS1,0}},
{'S',0xFD44,"[SH]",{SH1,0}},
{'S',0xFD46,"#[SION]",{ZH1, AX1,NN1,0}},
{'S',0xFD4D,"[SOME]",{SS1,AX1,MM1,0}},
{'S',0xFD53,"#[SUR]#",{ZH1,ER1,0}},
{'S',0xFD59,"[SUR]#",{SH1,ER1,0}},
{'S',0xFD5E,"#[SU]#",{ZH1,UW1,0}},
{'S',0xFD63,"#[SS1U]#",{SH1,UW1,0}},
{'S',0xFD69,"#[SED]<",{ZZ1,PA2,DD1,0}},
{'S',0xFD70,"#[S]#",{ZZ1,0}},
{'S',0xFD74,"[SAID]",{SS1,SS1,EH1,EH1,PA2,DD1,0}},
{'S',0xFD7D,"^[SION]",{SH1, AX1,NN1,0}},
{'S',0xFD84,"[S]S",{0}},
{'S',0xFD87,".[S]<",{ZZ1,0}},
{'S',0xFD8B,"#:.E[S]<",{ZZ1,0}},
{'S',0xFD92,"#*?[S]<",{ZZ1,0}},
{'S',0xFD98,"#*#[S]<",{SS1,0}},
{'S',0xFD9E,"U[S]<",{SS1,0}},
{'S',0xFDA2,"<:#[S]<",{ZZ1}},
{'S',0xFDA8,"<[SCH]",{SS1,SS1,PA3,KK2,0}},
{'S',0xFDAF,"[S]C+",{0}},
{'S',0xFDB3,"#[SM]",{ZZ1,MM1,0}},
{'S',0xFDB7,"#[S]N'",{ZZ1,0}},
{'S',0xFDBC,"[S]<",{SS1,0}},
{'S',0xFDBF,"[S]",{SS1,SS1,0}},

//== [ T ] ==
{'T',0xFDC2,"[T]'S",{PA3,TT1,0}},
{'T',0xFDC7,"[TCH]",{PA3,CH1,0}},
{'T',0xFDCB,"<[T]<",{PA3,TT2,IY1,0}},
{'T',0xFDD1,"<[THE]<#",{DH1,IY1,0}},
{'T',0xFDD8,"<[THE]<",{DH1,AX1,0}},
{'T',0xFDDE,"[TO]<",{PA3,TT2,UW2,0}},
{'T',0xFDE3,"[TODAY]",{PA3,TT2,UW2,DD2,EY1,0}},
{'T',0xFDEC,"[THA]^<",{DH1,AE1,0}},
{'T',0xFDF2,"<[THIS]<",{DH1,IH1,SS1,SS1,0}},
{'T',0xFDFB,"<[THEY]",{DH1,EY1,0}},
{'T',0xFE01,"<[THERE]",{DH1,XR1,0}},
{'T',0xFE08,"<[THER]",{TH1,ER1,0}},
{'T',0xFE0E,"[THER]",{DH2,ER1,0}},
{'T',0xFE13,"[THEIR]",{DH1,XR1,0}},
{'T',0xFE19,"<[THEM]:",{DH1,EH1,MM1,0}},
{'T',0xFE21,"[THESE]<",{DH1,IY1,ZZ1,0}},
{'T',0xFE29,"<[THEN]",{DH1,EH1,NN1,0}},
{'T',0xFE30,"[THROUGH]<",{TH1,RR2,UW2,0}},
{'T',0xFE3A,"[THOSE]",{DH1,OW1,SS1,0}},
{'T',0xFE41,"[THOUGH]<",{DH1,OW1,0}},
{'T',0xFE49,"<[THUS]",{DH1,AX1,SS1,SS1,0}},
{'T',0xFE51,"[THE]<",{DH1,0}},
{'T',0xFE55,"[TH]",{TH1,0}},
{'T',0xFE57,"#:[TED]<",{PA3,TT2,IH1,PA2,DD1,0}},
{'T',0xFE61,"S[TI]#N",{PA3,CH1,0}},
{'T',0xFE67,"[TI]O",{SH1,0}},
{'T',0xFE6A,"[TI]A",{SH1,0}},
{'T',0xFE6D,"[TIEN]",{SH1,AX1,NN1,0}},
{'T',0xFE73,"[TUR]#",{PA3,CH1,ER1,0}},
{'T',0xFE79,"[TU]A",{PA3,CH1,UW1,0}},
{'T',0xFE7E,"<[TWO]",{PA3,TT2,UW2,0}},
{'T',0xFE84,"[T]T",{0}},
{'T',0xFE87,"[T]S",{PA3,TT1,0}},
{'T',0xFE8B,"[T]",{PA3,TT2,0}},

//== [ U ] ==
{'U',0xFE8E,"<[U]<",{YY1,UW2,0}},
{'U',0xFE93,"[UN]I",{YY2,UW1,NN1,0}},
{'U',0xFE98,"<[UN]",{AX1,NN1,0}},
{'U',0xFE9C,"<[UPON]",{AX1,PA3,PP1,AA1,NN1,0}},
{'U',0xFEA5,"@[UR]#",{UW1,ER1,0}},
{'U',0xFEAA,"[UR]#",{YY1,UW1,ER1,0}},
{'U',0xFEAF,"[UR]*",{ER1,0}},
{'U',0xFEB2,"[U]^<",{AX1,0}},
{'U',0xFEB6,"[U]^^",{AX1,0}},
{'U',0xFEBA,"[UY]",{AY1,0}},
{'U',0xFEBC,"<G[U]#",{0}},
{'U',0xFEC1,"G[U]%",{0}},
{'U',0xFEC5,"G[U]#",{WW1,0}},
{'U',0xFEC9,"@[U]",{UW2,0}},
{'U',0xFECC,"[U]",{YY1,UW1,0}},

//== [ V ] ==
{'V',0xFECF,"<[V]<",{VV1,IY1,0}},
{'V',0xFED4,"[VIEW]",{VV1,YY1,UW2,0}},
{'V',0xFEDA,"[V]",{VV1,0}},

//== [ W ] ==
{'W',0xFEDC,"<[W]<",{PA2,DD2,AX1,PA2,BB2,EL1,YY1,UW1,0}},
{'W',0xFEE7,"<[WERE]",{WW1, ER2,0}},
{'W',0xFEED,"<[WAS]<",{WW1,AX1,ZZ1,0}},
{'W',0xFEF4,"[WA]S",{WW1,AA1,0}},
{'W',0xFEF8,"[WA]T",{WW1,AO1,AO1,0}},
{'W',0xFEFD,"[WAN]",{WW1,AA1,NN1,0}},
{'W',0xFF02,"[WHERE]",{WH1,XR1,0}},
{'W',0xFF08,"[WHAT]",{WH1,AA1,PA3,TT2,0}},
{'W',0xFF0F,"[WHOL]",{HH2,OW1,LL1,0}},
{'W',0xFF15,"[WHO]",{HH2,UW2,0}},
{'W',0xFF19,"[WO]M",{WW1,AX1,0}},
{'W',0xFF1D,"[WH]",{WH1,0}},
{'W',0xFF1F,"[WAR]",{WW1,OR1,0}},
{'W',0xFF23,"[WOR]^",{WW1,ER1,0}},
{'W',0xFF28,"[WR]",{RR1,0}},
{'W',0xFF2A,"[W]",{WW1,0}},

//== [ X ] ==
{'X',0xFF2C,"<[X]<",{EH1,PA3,KK2,SS1,0}},
{'X',0xFF33,"<[X]",{ZZ1,0}},
{'X',0xFF36,"[X]",{PA3,KK2,SS1,0}},

//== [ Y ] ==
{'Y',0xFF3A,"[YOUR]",{YY2,OR1,0}},
{'Y',0xFF3F,"<[Y]<",{WW1,AY1,0}},
{'Y',0xFF44,"[YOUNG]",{YY2,AX1,NG1,0}},
{'Y',0xFF4B,"<[YOU]",{YY2,UW2,0}},
{'Y',0xFF50,"[YEAR]:",{YY2,YR1,0}},
{'Y',0xFF56,"<[YES]",{YY2,EH1,SS1,SS1,0}},
{'Y',0xFF5D,"<[Y]",{YY2,0}},
{'Y',0xFF60,"#*[Y]<",{IY1,0}},
{'Y',0xFF65,"#*[Y]I",{IY1,0}},
{'Y',0xFF6A,"<:[Y]<",{AY1,0}},
{'Y',0xFF6F,"<:[Y]#",{AY1,0}},
{'Y',0xFF74,"<:[Y]^+:#",{IH1,0}},
{'Y',0xFF7C,"<:[Y]^#",{AY1,0}},
{'Y',0xFF82,"[Y]",{IH1,0}},

//== [ Z ] ==
{'Z',0xFF84,"<[Z]<",{ZZ1,IY1,0}},
{'Z',0xFF89,"[Z]Z",{0}},
{'Z',0xFF8C,"[Z]",{ZZ1,0}},

//== [ # ] ==
{'0',0xFF8E,"[0]",{ZZ1,YR1,OW1,0}},
{'1',0xFF92,"[1]",{WW1,AX1,AX1,NN1,0}},
{'2',0xFF97,"[2]",{PA3,TT2,UW2,0}},
{'3',0xFF9B,"[3]",{TH1,RR1,IY1,0}},
{'4',0xFF9F,"[4]",{FF1,OR1,0}},
{'5',0xFFA2,"[5]",{FF1,AY1,VV1,0}},
{'6',0xFFA6,"[6]",{SS1,SS1,IH1,PA3,KK2,SS1,0}},
{'7',0xFFAD,"[7]",{SS1,SS1,EH1,VV1,IH1,NN1,0}},
{'8',0xFFB4,"[8]",{EY1,PA3,TT2,0}},
{'9',0xFFB8,"[9]",{NN2,AY1,NN1,0}},

//{'#',0xFFB9,"[#]",{HH1,AY1,SS1,HH1,0}},
//end of rulez
{0,0,"",{0}}

};

struct ExeptStruc {
    char exept[15];
	char allophone[30];
};

struct ExeptStruc RuleExceptions[]={
	{"ANSI",{AE1,NN1,SS1,IY1,0}}, // ANSI
	{"ANSWER",{AE1,NN1,SS1,ER1,0}}, // ANSWER
	{"ANSWERING",{AE1,NN1,SS1,ER1,IH1,NG1,0}}, // ANSWERING
	{"ANSWERS",{AE1,NN1,SS1,ER1,ZZ1,0}}, // ANSWERS
	{"BACON",{BB1,EY1,KK1,AX1,NN1,0}}, // BACON
	{"BBS",{BB2,IY1,PA3,BB2,IY1,PA3,EH1,EH1,SS1,SS1,0}}, // BBS
	{"BLOOD",{BB1,LL1,AX1,PA4,DD1,0}}, // BLOOD
	//{"CAP]AB=[KK1,EY1,PP1,0}}, // CAPABILITY, CAPABLE
	{"CEDAR",{SS1,IY1,Pa1,DD2,AE1,ER1,0}}, // CEDAR
	{"CLASSIFIED",{KK3,LL1,AE1,SS1,IH1,FF1,AY1,PA4,DD1,0}}, // CLASSIFIED
	{"CLUTCHES",{KK1,LL1,AX1,CH1,AX1,ZZ1,0}}, // CLUTCHES
	{"CODED",{KK1,OW1,DD2,IH1,PA2,DD1,0}}, // CODED
	{"COUNTRY",{KK1,AX1,NN1,TT2,RR1,IY1,0}}, // COUNTRY
	{"COW",{KK1,AW1,0}}, // COW
	{"COWS",{KK1,AW1,ZZ1,0}}, // COWS
	{"CP/M",{SS1,SS1,IY1,PA3,PP1,IY1,PA3,EH1,EH1,MM1,0}}, // CP/M
	{"CREATE",{KK1,RR1,IY1,Pa1,EY1,TT2,0}}, // CREATE
	{"CREATED",{KK1,RR1,IY1,Pa1,EY1,TT2,AX1,DD1,0}}, // CREATED
	{"CYCLONE",{SS1,AY1,KK2,LL1,OW1,NN1,0}}, // CYCLONE
	{"DECADES",{DD2,IH1,Pa1,KK1,EY1,DD2,ZZ1,0}}, // DECADES
	{"DR",{DD2,AA1,KK2,PA3,TT2,ER1,0}}, // DOCTOR1,
	{"DREAMT",{DD2,RR1,EH1,MM1,TT2,0}}, // DREAMT
	{"DRIVES",{DD2,RR1,AY1,VV1,ZZ1,0}}, // DRIVES
	{"ELLIOTT",{EH1,LL1,IY1,AX1,TT2,0}}, // ELLIOTT
	{"FILE",{FF1,AA1,AY1,LL1,0}}, // FILE
	{"FILES",{FF1,AA1,AY1,LL1,ZZ1,0}}, // FILES
	{"FINALLY",{FF1,AY1,NN1,AX1,LL1,IY1,0}}, // FINALLY
	{"FINE",{FF1,AA1,AY1,NN1,0}}, // FINE
	{"GATES",{GG3,EY1,TT2,ZZ1,0}}, // GATES
	{"GOOSE",{GG2,UW2,ZZ1,0}}, // GOOSE
	{"GSX",{JH1,IY1,PA3,EH1,EH1,SS1,SS1,PA3,EH1,EH1,PA3,KK2,SS1,SS1,0}}, // GSX
	{"HEAVY",{HH2,EH1,VV1,IY1,0}}, // HEAVY
	{"I'D",{AY1,Pa1,DD1,0}}, // I'D
	{"IDIOT",{IH1,DD2,IY1,AX1,TT2,0}}, // IDIOT
	{"IDEA",{IH1,DD2,EH1,AX1,0}}, // IDEA
	{"I'M",{AY1,Pa1,MM1,0}}, // I'M
	{"ID",{AY1,Pa1,DD2,EH1,NN1,PA3,TT2,IH1,FF1,IH1,PA3,KK1,EY1,SH1,AX1,NN1,0}}, // IDENTIFICATION
	{"ISLE",{AY1,PA2,LL1,0}}, // ISLE
	{"ISLAND",{AY1,PA2,EL1,AE1,NN1,DD1,0}}, // ISLAND
	{"JOSHUA",{JH1,AA1,SH1,UW1,AX1,0}}, // JOSHUA
	{"LATER",{LL1,EY1,TT2,ER1,0}}, //LATER
	{"LED",{EH1,EH1,EL1,PA3,IY1,PA3,DD2,IY1,0}}, // LED
	{"LEDS",{EH1,EH1,EL1,PA3,IY1,PA3,DD2,IY1,ZZ1,0}}, // LEDS
	{"LINE",{LL1,AY1,NN1,0}}, // LINE
	{"LINUX",{LL1,IH1,NN1,AX1,KK2,SS1,0}}, // LINUX
	{"LION",{LL1,AY1,AX1,NN1,0}}, // LION
	//{"LIVE(V)",{LL1,IH1,VV1,0}}, // LIVE1,(verb)
	{"LIVE1",{LL1,IH1,VV1,0}}, // LIVE1,(verb)
	{"LIVED",{LL1,IH1,VV1,PA4,DD1,0}}, // LIVED
	{"LOCAL",{LL1,OW1,KK2,AX1,LL1,0}}, // LOCAL
	{"LOUISE",{LL1,UW2,IY1,ZZ1,0}}, // LOUISE
	{"MADE",{MM1,EY1,PA2,DD1,0}}, // MADE
	{"MINUTE",{MM1,IH1,NN1,AX1,TT2,0}}, // MINUTE
	{"MINUTES",{MM1,IH1,NN1,AX1,TT2,ZZ1,0}}, // MINUTES
	{"MISSILE",{MM1,IH1,SS1,AX1,LL1,0}}, // MISSILE
	{"MISSILES",{MM1,IH1,SS1,AX1,LL1,ZZ1,0}}, // MISSILES
	{"MODEM",{MM1,OW1,DD2,AX1,MM1,0}}, // MODEM
	{"MODES",{MM1,OW1,DD2,ZZ1,0}}, // MODES
	{"MODIFIED",{MM1,AA1,DD2,AX1,FF1,AY1,PA2,DD1,0}}, // MODIFIED
	{"MONITOR",{MM1,AA1,NN1,AX1,TT2,ER1,0}}, // MONITOR
	{"NOTICE",{NN1,OW1,TT2,IH1,SS1,0}}, // NOTICE
	{"OH",{OW1,0}}, // OH
	{"OREGON",{AO1,RR1,AX1,GG2,AX1,NN1,0}}, // OREGON
	{"PAGES",{PP1,EY1,PA2,JH1,AX1,ZZ1,0}}, // PAGES
	{"PETER",{PP1,IY1,TT2,ER1,0}}, // PETER
	{"PIPER",{PP1,AY1,PA3,PP1,ER1,0}}, // PIPER
	{"PRIMARY",{PP1,RR2,AY1,Pa1,MM1,EH1,ER1,IY1,0}}, // PRIMARY
	{"PURPOSE",{PP1,ER1,PA3,PP1,AX1,SS1, }}, // PURPOSE
	{"RADIO",{RR1,EY1,DD2,IY1,OW1,0}}, // RADIO
	{"RATIOS",{RR1,EY1,SH1,IY1,OW1,ZZ1,0}}, // RATIOS
	{"SALLY",{SS1,AE1,LL1,IY1,0}}, //SALLY
	{"SCSI",{SS1,KK2,AX1,ZZ1,IY1,0}}, // SCSI
	{"SEATTLE",{SS1,IY1,Pa1,AE1,TT2,Pa1,TT2,AX1,LL1,0}}, // SEATTLE
	{"SIZES",{SS1,AY1,ZZ1,AX1,ZZ1,0}}, // SIZES
	{"SPRITES",{SS1,PA3,PP1,RR1,AY1,TT2,ZZ1,0}}, // SPRITES
	{"STATE",{55,55,2,13,20,2,TT1,0}}, // STATE
	{"STEVEN",{SS1,TT2,IY1,VV1,EH1,NN1,0}}, // STEVEN
	{"SUGAR",{SH1,UH1,GG3,ER1,0}}, // SUGAR
	{"SURPRISED",{SS1,ER1,PP1,RR1,AY1,ZZ1,PA4,DD1,0}}, // SURPRISED
	{"SUPPLIES",{SS1,AX1,PP1,Pa1,LL1,AY1,ZZ1,0}}, // SUPPLIES
	{"SUPPLY",{SS1,AX1,PP1,Pa1,LL1,AY1,0}}, // SUPPLY
	{"TAKEN",{TT2,EY1,KK1,AX1,Pa1,NN1,0}}, // TAKEN
	{"TORNADO",{TT2,AO1,RR1,NN1,EY1,DD2,OW1,0}},// TORNADO
	{"TOTAL",{TT2,OW1,PA2,TT2,Pa1,AX1,EL1,0}}, // TOTAL1,: COULD1,BE1,TT2,OW1,TT2..
	{"TRIO",{TT2,RR1,IY1,OW1,0}}, // TRIO
	{"USERID",{YY1,UW1,ZZ1,ER1,Pa1,AY1,Pa1,DD2,EH1,NN1,PA3,TT2,IH1,FF1,IH1,PA3,KK1,EY1,SH1,AX1,NN1,0}}, // USER1,IDENTIFICATION
	{"WE'RE",{WW1,IY1,ER2,0}}, // WE'RE
	{"WEATHER",{WW1,EH1,DH2,ER1,0}}, // WEATHER
	{"WIFI",{WW1,AY1,PA2,FF1,AY1,0}}, // WIFI
	{"WIKI",{WW1,IH1,PA2,KK1,IY1,0}}, // WIKI
	{"WIKIPEDIA",{WW1,IH1,PA2,KK1,IY1,PA2,PP1,IY1,DD2,IY1,AX1,0}}, // WIKIPEDIA
	{"WIND",{WW1,IH1,NN1,PA3,DD1,0}}, // WIND1,(noun)
	{"WINDOW",{WW1,IH1,NN1,PA3,DD1,OW1,0}}, // WINDOW
	{"WINDOWS",{WW1,IH1,NN1,PA3,DD1,OW1,ZZ1,0}}, // WINDOWS
	{"YOU'D",{YY2,UW2,Pa1,DD1,0}}, // YOU'D
	{"YOU'RE",{YY2,OR1,0}}, //YOU'RE
	{"Z80",{ZZ1,EH1,Pa1,DD1,PA3,EY1,PA3,TT2,IY1,0}}, // Z80
	{"Z80A",{ZZ1,EH1,Pa1,DD1,PA3,EY1,PA3,TT2,IY1,PA3,EY1,0}}, // Z80A
	{"&",{AE1,NN1,DD1,0}}, //AND
	{"/",{SS1,LL1,AE1,SH1,0}}, //SLASH
	{{0},{0}}
};
