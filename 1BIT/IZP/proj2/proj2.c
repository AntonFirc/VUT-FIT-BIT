/*************************************************************************
 *                                                                       *
 *                                                                       *
 *                      Projekt 2 - Iteracni vypocty                     *
 *                                                                       *
 *                                IZP 2016                               *
 *                                                                       *
 *                         Anton Firc (xfirca00)                         *
 *                                                                       *
 *                                                                       *
 *                                                                       *
 *************************************************************************/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

double taylor_log(double x, unsigned int n);
double taylorcf_pow(double x, double y, unsigned int n);
double cfrac_log(double x, unsigned int n);
double my_pow(double log_main, double y, unsigned int n);

/*
 * Vypis chybovych hlaseni
 */
void error(int err_typ)
{
 if(err_typ == 0)                                   // nespravne argumenty programu
   fprintf(stderr,"Wrong input arguments !\n"
          "--log X N => natural logarithm of X in N iterations\n"
          "--pow X Y N => exponentional function of X with base Y in N iterations\n");

 if(err_typ == 1)                                   // zadany argument nieje cislo
   fprintf(stderr,"Unknown argument (NaN) !\n");
}
/*
 * Funkcia pre vypocet exponencionalnej funkcie podla taylorovho polynomu (cfrac_log)
 */
double taylorcf_pow(double x, double y, unsigned int n)
{
  if(y==0)
    return 1;
  else if(x!=1 && isnan(y))                         // navrat pre x rozne od 1 a y NAN
    return NAN;
  else if(isnan(x))                                 // x = NAN => vysledok = NAN
    return NAN;
  else if(isinf(x) && y<0)                          // x = INFINITY a y je menej ako 0 => vysledok 0
    return 0;
  else if(x==0)                                   // vracia nulu pri umocnovani nuly
   return 0;
  else if((x==1) && (isnan(y) || isinf(y)))     // 1 na inf alebo nan = 1
   return 1;
  else {
    int x_cele = x;

    if(((x - x_cele == 0.0 ) && (y==-INFINITY)) || ((x<1) && (y==INFINITY)))            // cele x na -INF alebo x < 1 na INF vrati 0
      return 0;

    else
      if(isnan(y) || isinf(y))                                                          // desatinne x vrati INF
        return INFINITY;
  }

 double result_cf;
 int y_cele = y;

 if(x>0)
   result_cf = my_pow(cfrac_log(x,n), y, n);

 else
   if((y-y_cele)==0.0)
     result_cf = my_pow(cfrac_log(-x,n), y, n);

   else
     return -NAN;


 if(x < 0 && (y_cele % 2))              // ak je x zaporne testuje ci je exponent parny a priradi poda toho spravne znamienko
   return -result_cf;
 else
   return result_cf;                                              // vracia vypocitanu hodnotu
}



/*
 * Funkcia pre vypocet exponecialnej funkcie podla taylorovho polynomu (taylor_log)
 */
double taylor_pow(double x, double y, unsigned int n)
{

 if(y==0)
   return 1;
 else if(x!=1 && isnan(y))               // navrat pre x rozne od 1 a y NAN
   return NAN;
 else if(isnan(x))                      // x = NAN => vysledok = NAN
   return NAN;
 else if(isinf(x) && y<0)               // x = INFINITY a y je menej ako 0 => vysledok 0
    return 0;
 else if(x==0.0)                                   // vracia nulu pri umocnovani nuly
 return 0;
 else if((x == 1) && (isnan(y) || isinf(y)))           // 1 na inf alebo nan = 1
   return 1;
 else {
    int x_cele = x;

    if(((x - x_cele == 0.0 ) && (y==-INFINITY)) || ((x<1) && (y==INFINITY)))  // cele x na -INF alebo x < 1 na INF vrati 0
      return 0;

    else
      if(isnan(y) || isinf(y))                      // desatinne x vrati INF
        return INFINITY;
  }

 double result_pow;
 int y_cele = y;

 if(x>0)
  result_pow = my_pow(taylor_log(x,n), y, n);                    // vypocet logaritmu z x ak je zaklad vacsi ako 0
 else
   if((y-y_cele)==0.0)                              // testuje ci je exponent cele cislo
     result_pow = my_pow(taylor_log(-x,n), y, n);                    // vypocet logaritmu z x ak je zaklad mensi ako 0
   else
     return -NAN;                                   //vrati NaN ak je zaklad zaporne cislo a exponent nie je cele cislo

  if(x < 0 && (y_cele % 2))              // ak je x zaporne testuje ci je exponent parny a priradi poda toho spravne znamienko
   return -result_pow;
 else
   return result_pow;                                              // vracia vypocitanu hodnotu
}

/*
 * Funkcia pre vypocet vysledku pre obe exp funkcie
 */
double my_pow(double log_main, double y, unsigned int n)
{
 double pow = 1.0, expPOW = 1.0,logPOW = 1.0;              // powCF => finalny vysledok | tay_log => logaritmus z funkcie taylor_log | expCF => nasobitel v citateli | logCF => nasobok logaritmu
 double menPOW = 1;                                                  // menCF => menovatel(faktorial)

 for(unsigned int i=1; i <= n; i++) {
   expPOW *= y;
   logPOW *= log_main;                                         // vypocet logaritmu
   if(isinf(menPOW *= i))                                   // vypocet logaritmu + ak je nekonecno ukonci cyklus vypoctu
     break;

   pow += (expPOW * logPOW) / menPOW;
 }

 return pow;
}

/*
 * Funkcia pre vypocet prirodzeneho logaritmu pomocou zretazeneho zlomku
 */
double cfrac_log(double x, unsigned int n)
{
 if(x == INFINITY)
   return x;
 else if(x == -INFINITY || x == NAN)
   return NAN;
 else if(x==0)                       // navrat hodnot pri nespravnych argumentoch ako funkcia z math.h
   return -INFINITY;
 else if(x < 0)
   return NAN;

 double zFRAC,logFRAC=1.0;                  // zFRAC => premenna z v "citateli" zretazeneho zlomku | logFRAC => uloyenie medzivysledkov a vysledku
 int kFRAC = 1;               // iFRAC => nasobi premennu citatela | => kFRAC => nasobi zretazenie zlomku

 zFRAC = ((x-1) / (x+1));       // priradenie podla vzorca x = (z+1)(z-1)

 for(unsigned int i=1;i<n;i++)      // inkrementacia premennej na potrebnu hodnotu podla vzorca (kFRAC - zlomok)
   kFRAC += 2;

 for(unsigned int repFRAC = n; repFRAC >= 1; repFRAC--){                // Prva iteracia vypoctu
   if(repFRAC == n)
     logFRAC = ((zFRAC*zFRAC)*((repFRAC-1)*(repFRAC-1))) / kFRAC;
   if(repFRAC == 1)                                                     // Posledna iteracia vypoctu
     logFRAC = 2*zFRAC / (1-logFRAC);
   else {
     logFRAC = ((zFRAC*zFRAC)*((repFRAC-1)*(repFRAC-1))) / (kFRAC - logFRAC);   // priebezny vypocet
     kFRAC -= 2;                                                                // zmensovanie premennej s postupom cez zlomok
   }

 }
 return logFRAC;                        // vracia vypocitanu hodnotu
}

/*
 * Funkcia pre vypocet prirodzeneho logaritmu pomocou Taylorovho polynomu
 */
double taylor_log(double x, unsigned int n)
{
 if(x == INFINITY)
   return x;
 else if(x == -INFINITY || isnan(x))
   return NAN;

 double       logTAY = 0,xTAY = 1.0;                 // logTAY => konecny vysledok |  xTAY => obratena hodnota pre vypocet
 unsigned int menTAY = 1;                 // menovatel pre delenie pri vypoctoch



 // Vypocet podla vzorca pre x > 1/2

 if(x >= 1) {
   double citTAY = 1;                   // pre vypocet citatela zlomku

   while(menTAY <= n) {                 // n -> zadany pocet iteracii
     xTAY = ((x-1)/x);                  // zakladny vzorec pre citatel
     citTAY *= xTAY;                    // umocnovanie citatela
     logTAY += (citTAY / menTAY++);
   }
 return logTAY;
 }


 // Vypocet podla vzorca pre cisla 0 < X < 2

 if(x > 0 && x <= 1) {
   while(menTAY <= n) {                  // n -> zadany pocet iteracii
      xTAY *= (1-x);                     // umocnenie citatela podla poctu iteracii
     logTAY -= (xTAY / menTAY++);
   }
 }
 else
   if(x<0)
     return NAN;
   else
     return -INFINITY;

 return logTAY;                         // vracia vypocitanu hodnotu z jednej alebo druhej vetvy
}

/*
 * Funkcia pre vypis vysledokov
 */
void logaritmus(double log_x,unsigned int log_n)
{
 printf("       log(%g) = %.12g\n",log_x, log(log_x));                 // Vypisuje vysledok z funkcie log z kniznice math.h
 printf("    cf_log(%g) = %.12g\n",log_x, cfrac_log(log_x,log_n));      // Vypisuje vysledok z funkcie cfrac_log
 printf("taylor_log(%g) = %.12g\n",log_x, taylor_log(log_x,log_n));     // Vypisuje vysledok z funkcie taylor_log
}

void exponencial(double exp_x, double exp_y, unsigned int exp_n)
{
 printf("         pow(%g,%g) = %.12g\n",exp_x, exp_y, pow(exp_x,exp_y));                // Vypisuje vysledok funkcie pow z math.h
 printf("  taylor_pow(%g,%g) = %.12g\n",exp_x,exp_y,taylor_pow(exp_x,exp_y,exp_n));     // Vypisuje vysledok funkcie taylor_pow
 printf("taylorcf_pow(%g,%g) = %.12g\n",exp_x,exp_y,taylorcf_pow(exp_x,exp_y,exp_n));   // Vypisuje vysledok z funkcie taylorcf_pow
}


int main(int argc,char* argv[])
{
 unsigned short arg_funkcie;

 if(!(strcmp(argv[1],"--pow"))) {             // Zabezpecuje aby sa vyhodnocovalo ci srgument obsahuje cisla iba pre nutne argumenty funkcii
   if(argc == 5) {
     if(atof(argv[4]) > 0) {                // kontrola poctu iteracii
       arg_funkcie = 5;
     }
   else {
     error(0);
     return 1;
   }
 }
 else {
     error(0);
     return 1;
 }
 }

 if(!(strcmp(argv[1],"--log"))) {       // Zabezpecuje aby sa vyhodnocovalo ci srgument obsahuje cisla iba pre nutne argumenty funkcii
   if(argc == 4){
     if(atof(argv[3]) > 0) {        // kontrola poctu iteracii
       arg_funkcie = 4;
     }
   else {
     error(0);
     return 1;
   }
 }
 else {
     error(0);
     return 1;
 }
 }


 double x_func,y_func;

 //Test ci su zadane len ciselne hodnoty

 for(unsigned int i = 2; i < arg_funkcie; i++) {
   for(unsigned int j=0; j < strlen(argv[i]); j++)
       if((!isdigit(argv[i][j])) && (argv[i][j] != '.') && (argv[i][j] != '-') && (argv[i][j] != '+')) {           // vyhodnotenie argumentu - ci obsahuje iba cisla, znamienko - a desatinnu bodku .
         if(!strcmp(argv[i],"inf") || !strcmp(argv[i],"+inf")) {
           if(i==2)
             x_func = INFINITY;                                         // dosadzanie hodnot INF a -INF za argumenty x a y
           else if(i==3)
             y_func = INFINITY;
         }
         else if(!strcmp(argv[i],"-inf")) {
           if(i==2)
             x_func = -INFINITY;
           else if(i==3)
             y_func = -INFINITY;
         }
         else if(!strcmp(argv[i],"nan") || !strcmp(argv[i],"+nan") || !strcmp(argv[i],"-nan")) {        // dosadzanie NAN za argumenty x a y
           if(i==2)
             x_func = NAN;
           else if(i==3)
             y_func = NAN;
         }
          else {
             error(1);                      // ukoncenie ak sa naslo nieco ine ako cislo
             return 1;
       }

       }

 }

  if(argc==5) {
   if(!(strcmp(argv[1],"--pow"))) {

       if(!isinf(x_func) || !isnan(x_func))
       {
        x_func = atof(argv[2]);                     // ak x nieje INF alebo NAN priradi mu ciselnu hodnotu
       }
       if(!isinf(y_func) || !isnan(y_func))
        {
          y_func = atof(argv[3]);                   // ak y nieje INF alebo NAN priradi mu ciselnu hodnotu
        }
       exponencial(x_func,y_func,atoi(argv[4]));      // volanie funkcie pre vypocet exp funkcie
       return 0;

   }
   else
     error(0);          // vola chybovu funkciu ak je nespravny parameter (--pow)
   return 1;
 }
 if(argc == 4) {
   if(!(strcmp(argv[1],"--log"))) {

       if(!isinf(x_func) || !isnan(x_func))
       {
        x_func = atof(argv[2]);                           //  ak x nieje INF alebo NAN priradi mu ciselnu hodnotu
       }
       logaritmus(x_func,atoi(argv[3]));                     // volanie funkcie pre vypocet logaritmu, prevod zadanych hodnot zo stringu na cisla
       return 0;

   }
   else {
     error(0);                                      // vola chybovu funkciu ak je zadany nespravny parametrer (--log)
     return 1;
   }
 }
 else {
   error(0);                         // vola chybovu funkciu ak nebol zadany spravny pocet argumentov
   return 1;
 }
}
