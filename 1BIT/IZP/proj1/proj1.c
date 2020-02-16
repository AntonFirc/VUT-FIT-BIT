/**
 * Anton Firc (xfirca00)
 *
 * IZP 2016
 *
 * Projekt 1 - Praca s textom
 *
*/

#include <stdio.h>
#include <ctype.h>

void bad_arg(int chyba) // pri zadani nespravnych argumentov vypise napovedu
{
 if(chyba == 0)
   printf("Wrong input arguments !\n"
  	  "No arguments -> converts binary input to text in default format.\n"
	  "-s M -n N -> converts binary input to text.\n[-s M] defines to skip M input characters.\n[-n N] defines to print N input characters.\n"
 	  "Both -s M & -n N arguments can be used separately !\n"
 	  "-x -> converts binary input to sequence of hexadecimal numbers.\n"
 	  "-S N -> prints character sequences as strings. N predefines the minimal lenght of sequence to be considered a string. [0 < N < 200]\n"
 	  "-r -> converts text input to binary format.\n");
 if(chyba == 1)
   printf("Wrong parameter for -s M or -n N !\nM and N must be from the integer.\n");
 if(chyba == 2)
   printf("Wrong parameter for -S M !\nM must be from the integer.\n");
 if(chyba == 3)
   printf("N is out of limits !\nMust be 0 < N < 200.\n");
 if(chyba == 4)
   printf("Error : not hexadecimal number on input !");
}

int pismeno()
{
return getchar();		// zaistuje iba jeden vstup
}

/*
 *  Prepisanie postupnosti znakov ako retazec
 */
void bin_to_txt_string(int retazec)
{
 char chS,memS[retazec];  			// memS - pole pre ulozenie nacitanych znakov pre neskorsie vypisanie
 int nacitane=0,plati=0,i=0;

 while((chS=pismeno()) != EOF) {
   if(isprint(chS) || isblank(chS)) {			// cita tlacitelne a prazdne znaky
     if((retazec - 1) > nacitane)				// nacitava pokial sa nedosiahne pocet znakov potrebnych pre retazec
       memS[nacitane] = chS; 
     else{
       if(plati==0){ 				// ked sa nacita pocet znakov potrebnych pre retazec vytlaci ostatne tlacitelne a prazdne znaky
         for(i=0;i<retazec - 1;i++)
           printf("%c",memS[i]);
          plati=1;
       }
       if(plati==1)
       printf("%c",chS);
     }
     ++nacitane;
   }
   else {				// zresetovanie pri nacitani netlacitelneho znaku
     if(plati==1)
     putchar('\n');
     nacitane=0;
     i=0;
     plati=0;
   }
 }
}

/*
 * Prevod binarneho vstupu na postupnost hexadecimalnych cisel
 */
void bin_to_txt_hex()
{
 int chx;

 while((chx=pismeno()) != EOF)  	//kym je co citat vypisuje hexadecimalne kody nacitanych pismen
 {
   printf("%02x",chx);
 }
 if(chx == EOF)
   printf("\n");		// odriadkovanie po ukonceni vypisovania
}

/*
 *  Prevod binarneho vstupu na text. argumenty -s = pocet preskocenych znakov -n = pocet znakov na vypisanie
 */
void bin_to_txt_default(int preskok,int vypis)
{								// prevod aj bez argumentov skip = 0 vypis = -1
 char ch=0,mem_def[16];      		// ch - uklada naskenovane pismeno ; mems - pole pre docasne ulozenie znakov na vypisanie na konci
 int pos_def=0,j=0,i=0,opakovanie=0; 			// pos_skip - oznacenie vytlaceneho bajtu, opakovanie - rata pocet vytlacenych znakov podla -n N

 if(preskok != 0)			// zabrani chybnemu vypisovaniu pozicie bajtu v pripade ze je preskok 0
 for( ; j <= preskok; j++) {		// preskocenie o -s M znakov, (nacitanie a zahodenie znakov)
   ch = pismeno();
   ++pos_def;				// rata poziciu vypisaneho bajtu
 }
 else
   ch = pismeno(); 			// nacitanie znaku este pred cyklom aby nedoslo k duplicitnemu nacitaniu znakov z for-cyklu a while cyklu

if(pos_def!=0)
     --pos_def;


 while ((ch!= EOF) && (opakovanie != vypis)) {
   printf("%08x  ",pos_def);			// vypis pozicie bajtu
   for(i=0 ; i < 16 ;++i){ 		// citanie a vypis znaku
     ++pos_def;
     if((ch != EOF) && (opakovanie != vypis)) {
       printf("%02x ",ch);

       if(i==7)
	 printf(" ");			// vytlaci medzeru do stredu hexa kodov

       if(isprint(ch))			// nahradenie netlacitelnych znakov znakom '.'
         mem_def[15-i] = ch;		// ulozenie znaku do pola pre vypis
       else {
	   mem_def[15-i] = '.';	// ak je znak netlacitelny vlozi do pola namiesto neho bodku
       }

       ++opakovanie;
       if(opakovanie == vypis)
	 {}			// preskocenie citania dalsich znakov po vypisani N znakov
       else
       ch = pismeno();
     }

     else {
	if(i==7) 		// doplnenie riadku medzerami, po 8mom znaku sa vytalci navyse medzera na oddelenie v strede
	 printf(" ");
       printf("   ");
       mem_def[15-i] = ' ';   // doplni medzerami aj pole pre vypis
    }
  }

 printf(" |");				// spatny vypis zakodovaneho textu
  for(i=15;i>=0;--i)
     printf("%c",mem_def[i]);
 printf("|\n");
 }
}

/*
 *  Prevod textu do binarnej formy | hex cislo nacitane ako dva znaky hx_znak[1/2], kazdy znak prevedeny na int hodnotu =>
 *	=> int hodnota oboch znakov sa scita - dostaneme kod znaku zakodovaneho pomocou povodneho hex cisla
 */
void txt_to_bin()
{
 char hx_znak[2],in_znak;
 int poc=0,nepar=0,vysledok;				// poc - riadi aby boli ako hex cislo vyhodnotene iba dva po sebe iduce znaky

 while((in_znak = pismeno()) != EOF) {
   if(isalnum(in_znak)) {
     if(isdigit(in_znak))	// prevod hex cisla [0-9] na int
     {
       hx_znak[poc + 1] = in_znak - '0';	// prevod hexa cisla na int
     }
     if((in_znak >= 'a') && (in_znak <= 'f'))	// prevod hex cisla [a-f] na int
     {
       hx_znak[poc + 1] = in_znak - 'a' + 10;	// prevod hexa cisla na int
     }
     if(in_znak > 'f')
     {
       printf("\n");
       bad_arg(4);
       break;
     }
     ++poc;
   }
   else {
     poc = 0;
     if(!isspace(in_znak))
       nepar = 1;
   }
   if(poc==2) {					//ak boli nacitane 2 hexa znaky scita hodnoty a dostaneme int
     poc = 0;
     vysledok = ((16*hx_znak[1]) + hx_znak[2]);	// scitanie int hodnot
     printf("%c",vysledok);
  }
   if(nepar==1) {
     printf("%c",hx_znak[1]);
   }
 }
 printf("\n");
}


int main(int argc,char* argv[])
{
 int hodnota_preskok,hodnota_vypis;	//premenne pre funkciu skip
 int hodnota_retazec=0;				//pre funkciu string
 int cis = 0,cis2=0,cisS=0,mocnina=1,i_m=0;

 if(argc == 1)			// spustenie bez argumentov
 {
 bin_to_txt_default(0,-1);	// volanie funkcie 0 - preskok znakov -1 - budu vypisane vsetky nacitane znaky
 return 0;
 }

 if(argc == 2) {
   if(((argv[1][0]) == '-') && ((argv[1][1]) == 'r')) {       // spustenie s argumentom -r
     txt_to_bin();
     return 0;
     }
   if(((argv[1][0]) == '-') && ((argv[1][1]) == 'x')) {		//spustenie s argumentom -x
     bin_to_txt_hex();
     return 0;
   }
 }

 if(argc == 3) {					// spustenie samostatne s argumentom -s M, -n N alebo -S M
   if(((argv[1][0]) == '-') && ((argv[1][1]) == 's')) {		//vykona sa iba pri zadani -s M
    while(isalnum(argv[2][cis]))	 // zistenie poctu cislic zadanych ako M
      {
	  ++cis;
      }
      for(i_m = 0; i_m < cis; ++i_m)
	if(!(isdigit(argv[2][i_m]))) {
	  bad_arg(1);
	  return 0;
	}
      mocnina = 1;
      for( ; (cis-1) >= 0; --cis) {		// prevod nacitanych cislic na int
        hodnota_preskok += mocnina * ((argv[2][cis-1])- '0' );
        mocnina *= 10;
      }
      bin_to_txt_default(hodnota_preskok,-1);		// volanie skip fukcie s -s M
      return 0;
   }
   if(((argv[1][0]) == '-') && ((argv[1][1]) == 'n')) {	// supstenie samostatne len s argumentom -n N
     while(isalnum(argv[2][cis2])) 			// zistenie poctu cislic zadanych ako N
     {
      ++cis2;
     }
      for(i_m = 0; i_m < cis2; ++i_m)
	if(!(isdigit(argv[2][i_m]))) {
	  bad_arg(1);
	  return 0;
 	}
     mocnina = 1;
     for( ; (cis2-1) >=0; --cis2) {			// prevod nacitanych cislic na int
       hodnota_vypis += mocnina * ((argv[2][cis2-1]) - '0');
       mocnina = mocnina * 10;
     }
     bin_to_txt_default(0,hodnota_vypis);			// volanie funkcie skip s -n N
     return 0;
   }
   if(((argv[1][0]) == '-') && ((argv[1][1]) == 'S')) { 	// iba pri agrumente -S
     while(isalnum(argv[2][cisS]))         // zistenie poctu cislic zadanych ako N
       {
         ++cisS;
       }
      for(i_m = 0; i_m < cisS; ++i_m)
	if(!(isdigit(argv[2][i_m]))) {
	  bad_arg(2);
	  return 0;
	}
       mocnina = 1;
       for( ; (cisS-1) >= 0; --cisS) {               // prevod nacitanych cislic na int
         hodnota_retazec += mocnina * ((argv[2][cisS-1]) - '0');
         mocnina = mocnina * 10;
       }
      if((hodnota_retazec > 0) && (hodnota_retazec < 200)) {
        bin_to_txt_string(hodnota_retazec);
        return 0;
      }
      else {
	bad_arg(3);
	return 0;
      }
  }
 }
 if(argc == 5) {												// spustenie s agrumentami -s M a -n N
   if((((argv[1][0]) == '-') && ((argv[1][1]) == 's')) && (((argv[3][0]) == '-') && ((argv[3][1]) == 'n'))) {
     while(isalnum(argv[2][cis]))
     {
       ++cis;
     }
      for(i_m = 0; i_m < cis; ++i_m)
	if(!(isdigit(argv[2][i_m]))) {
	  bad_arg(1);
	  return 0;
	}
     mocnina = 1;
     for( ; (cis-1) >= 0; --cis) {					// prevod M do int
       hodnota_preskok += mocnina * ((argv[2][cis-1])- '0' );
       mocnina *= 10;
     }
     while(isalnum(argv[4][cis2]))
     {
       ++cis2;
     }
      for(i_m = 0; i_m < cis2; ++i_m)
	if(!(isdigit(argv[4][i_m]))) {
	  bad_arg(1);
	  return 0;
	}
     mocnina = 1;
     for( ; (cis2-1) >=0; --cis2) {					// prevod N do int
       hodnota_vypis += mocnina * ((argv[4][cis2-1]) - '0');
       mocnina = mocnina * 10;
     }
     bin_to_txt_default(hodnota_preskok,hodnota_vypis); 			//volanie skip funkcie s oboma parametrami
     return 0;
   }
   else{
     bad_arg(0);
     return 0;
   }
 }
 else {
   bad_arg(0);
   return 0;
 }
}
