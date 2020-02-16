/*************************************************************************
 *                                                                       *
 *                                                                       *
 *                      Projekt 3 - Zhlukova Analyza                     *
 *                                                                       *
 *                                IZP 2016                               *
 *                                                                       *
 *                         Anton Firc (xfirca00)                         *
 *                                                                       *
 *                                                                       *
 *                                                                       *
 *************************************************************************/

/**
 * Kostra programu pro 3. projekt IZP 2015/16
 *
 * Jednoducha shlukova analyza
 * Complete linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>
#include <ctype.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

   struct obj_t *objekt;            // ukazatel pre alokovanie pamate

   objekt = malloc(sizeof(struct obj_t) * cap);         // alokovanie pamate o velkosti kapacita * velkost objektu
     if(objekt==NULL) {                                 // test ci bola alokacia uspesna
       fprintf(stderr,"***Not enough memeory to allocate object !***\n");
       return;
     }

   c->obj = objekt;                         // priradi pole objektov do clustera
   c->capacity = cap;                       // nastavi velkost a kapacitu clustera
   c->size = 0;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{

  free(c->obj);                         // uvolni pamat alokovanu pre objekty

  c->size = 0;                          // nastavi kapacitu a velkost na 0 => prazdny cluster
  c->capacity = 0;

}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
  if(c->capacity <= c->size)                    // ak je nedostatocna kapacita na pridanie objektu tak rozsiri kapacitu clusteru
    c = resize_cluster(c,c->size + 1);

  c->obj[c->size] = obj;                        // priradi novy objekt na koniec
  c->size += 1;                                 // nastavi novu velkost clusteru

}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    if(c1->capacity < (c1->size + c2->size))        // ak je nedostatocna kapacita zvacsi c1
      resize_cluster(c1,c1->size + c2->size);

    for(int i = 0; i < c2->size; i++) {              // postupne priradi vsetky objekty z c2 do c1
      append_cluster(c1,c2->obj[i]);
    }

    sort_cluster(c1);                               // zoradi vsetky prvky
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

     free(carr[idx].obj);                       // uvolni pamat objektu

    for(int i = idx; i < narr-1; i++) {         // pre zvysne objekty priradi nasledujuci objekt
      carr[i].obj = carr[i+1].obj;
      carr[i].size = carr[i+1].size;
      carr[i].capacity = carr[+1].capacity;
    }



    return narr-1;          // vrati novy pocet clusterov
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float distance;

    distance = sqrt(pow(o1->x - o2->x,2) + pow(o1->y - o2->y,2));           // vracia vzdialenost vypocitanu podla vzorca

    return distance;
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float c_dist, max_dist=0;

    for(int i = 0; i < c1->size; i++)                       // rata vzdialenost clusterov, (najvzdialenejsich objektov v clusteroch)
      for(int j = 0; j < c2->size; j++) {
        c_dist = obj_distance(&c1->obj[i],&c2->obj[j]);
        if(c_dist > max_dist)
          max_dist = c_dist;
      }

   return max_dist;                         // vracia najvacsiu vzdialenost medzi objektmi
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float cls_dist, min_dist;

    min_dist = INT_MAX;            // nastavi prvotnu hodnotu pre porovnanie minimalnej hodnotyc

    for(int i = 0; i < narr; i++) {
      for(int j = 0; j < narr; j++) {
        cls_dist = cluster_distance(&carr[i],&carr[j]);         // prerata vzdialenost vsetkych clusterov
        if(cls_dist < min_dist && i != j) {
          min_dist = cls_dist;                                  // vrati najemnsiu vzdialenost pre porovananie
          *c1 = i;                                              // priradi indexy najblizsich clusterov
          *c2 = j;
        }
      }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

void free_clusters(struct cluster_t **arr, int narr);

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    FILE *vstup;
    char line[29];                  // retazec pre vstup zo suboru

    vstup = fopen(filename,"r");        // otovrenie suobru a test uspesnosti otvorenia
    if(vstup == NULL) {
      fprintf(stderr,"*** Could not open file \"%s\" ***\n",filename);
      *arr = NULL;
      return 0;
      }

    fgets(line, 30, vstup);         // citanie prveho riadku (count=xxx)

    int count = 0;

    for(unsigned m = 6; m < strlen(line); m++) {            // zisti zadany pocet clusterov (count=xxx)
      if(isdigit(line[m]))
        count += (line[m] - 48) * pow(10,strlen(line)-2-m);
      else if(line[m] != '\n' && line[m] != ' ') {
        fprintf(stderr,"***Wrong format of input file !***\n");
        *arr = NULL;
        return 0;
      }

    }

    struct cluster_t *pole_cls;
    pole_cls = malloc(sizeof(struct cluster_t) * count);    // alokuje miesto pre pole clusterov a testuje ci bolo miesto alokovane spravne
    if(pole_cls == NULL) {
          fprintf(stderr,"Not enough memory to allocate array of clusters !");
          *arr = NULL;
          return 0;
        }

    int cislo[] = {0,0,0};                              // pole na ulozenie cisel parametrov zo vstupneho suboru
    int pos = 1, variant = 1, exp = 0;
    float cislo_tmp = 0.0;

    for(int j = 0; j < count; j++) {
        init_cluster(&pole_cls[j],1);                   // alokuje pamat pre objekty clusteru
        if(&pole_cls[j] == NULL) {
          free_clusters(&pole_cls,j);
          *arr = NULL;
          return 0;
        }
        if(fgets(line, 30, vstup) == NULL) {                         // cita dalsi riadok s parametrami clusteru
          fprintf(stderr,"***Wrong format of input file !***\n");
          free_clusters(&pole_cls,j);
          return 0;
        }
        pole_cls[j].size = 1;                           // nastavi velkost clusteru

        for(unsigned i = 0; i <= strlen(line); i++) {
          if(variant == 3) {                                // variant 3 => priradi cislo za Y
            if(isdigit(line[i])) {
              cislo[3-pos] = line[i];                       // nacitanie cisla do pola
              pos++;
            }
            else
            {
              if(line[i] != ' ' && line[i] != '\n') {
                fprintf(stderr,"***Wrong format of input file !***\n");
                free_clusters(&pole_cls,j);
                *arr = NULL;
                return 0;
              }
              variant = 1;
              for(int k = 4-pos; k < 3; k++) {
                cislo_tmp += pow(10,exp++) * (cislo[k] - 48);       // prevod cisla z pola na float hodnotu
              }
              if(cislo_tmp >= 1000 || cislo_tmp < 0) {
                fprintf(stderr,"***Wrong format of input file !***\n");
                free_clusters(&pole_cls,j);
                *arr = NULL;
                return 0;
              }
              pole_cls[j].obj[0].y = cislo_tmp;
              pos = 1;
              cislo_tmp = 0;
              exp = 0;
              break;                            // ukoncuje opakovanie for-cyklu
            }

          }


          if(variant == 2) {                // variant 2 => priradi cislo za X
            if(isdigit(line[i])) {
              cislo[3-pos] = line[i];           // nacitanie cisla do pola
              pos++;
            }
            else
            {
              if(line[i] != ' ') {
                fprintf(stderr,"***Wrong format of input file !***\n");
                free_clusters(&pole_cls,j);
                *arr = NULL;
                return 0;
              }

                variant = 3;
                for(int k = 4-pos; k < 3; k++) {
                  cislo_tmp += pow(10,exp++) * (cislo[k] - 48);     // prevo cisla z pola na float hodnotu
                }
                if(cislo_tmp >= 1000 || cislo_tmp < 0) {
                fprintf(stderr,"***Wrong format of input file !***\n");
                free_clusters(&pole_cls,j);
                *arr = NULL;
                return 0;
                }
                pole_cls[j].obj[0].x = cislo_tmp;
                pos = 1;
                cislo_tmp = 0;
                exp = 0;

            }

          }

          if(variant == 1) {            // variant 1 => priradi cislo za ID
            if(isdigit(line[i])) {
              cislo[3-pos] = line[i];       // nacitanie cisla do pola
              pos++;
            }
            else
            {

              if(line[i] != ' ') {
                fprintf(stderr,"***Wrong format of input file !***\n");
                free_clusters(&pole_cls,j);
                *arr = NULL;
                return 0;
              }

              variant = 2;
              for(int k = 4-pos; k < 3; k++) {
                cislo_tmp += pow(10,exp++) * (cislo[k] - 48);       // prevod cisla z pola na int hodnotu
              }
              if(cislo_tmp >= 1000 || cislo_tmp < 0) {
                fprintf(stderr,"***Wrong format of input file !***\n");
                free_clusters(&pole_cls,j);
                *arr = NULL;
                return 0;
              }
              pole_cls[j].obj[0].id = cislo_tmp;
              pos = 1;
              cislo_tmp = 0;
              exp = 0;
            }

          }


    }

    }

    *arr = pole_cls;            // priradenie pola clusterov do pola ukazatelov

    fclose(vstup);              // uzavretie vstupneho suboru

    return count;               // vracia pocet nacitanych clusterov
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

void free_clusters(struct cluster_t **arr, int narr)
{

 for(int i = 0; i < narr; i++) {              // uvolnenie pamate po skonceni prace s clustrami
        free((*arr)[i].obj);
      }
      free(*arr);
}

int main(int argc, char **argv)
{
    struct cluster_t *clusters;
    int loaded,pocet;

    if(argc==2)                     // ak je zadane iba ./proj3 filename
    {
      pocet = 1;
      loaded = load_clusters(argv[1],&clusters);  // naciatnie zo suboru
      if(loaded == 0) {
        fprintf(stderr,"No cluster has been loaded !");
        return 1;
      }

      if(loaded < pocet) {
        fprintf(stderr,"***Not enough clusters to work with !***\n");
        return 1;
      }

       int cls_pocet, c1, c2;

       cls_pocet = loaded;

       while(cls_pocet != pocet) {                      // zlucovanie 2 najblizsich clusterov kym sa nedosiahne pozadovany pocet
       find_neighbours(clusters,cls_pocet,&c1,&c2);
       merge_clusters(&clusters[c1],&clusters[c2]);
       remove_cluster(clusters,cls_pocet,c2);
       --cls_pocet;
       }
       print_clusters(clusters,pocet);                  // vypise clustre po zluceni
       free_clusters(&clusters,cls_pocet);
       return 0;

    }

    if(argc==3) {                   // spustenie ./proj3 filnename N
      pocet = atoi(argv[2]);
      if(pocet < 1) {
        fprintf(stderr,"***Wrong format for [N] argument !***\n");
        return 1;
      }
      loaded = load_clusters(argv[1],&clusters);        // nacitanie zo suboru
      if(loaded == 0) {
        fprintf(stderr,"*** No cluster has been loaded ! ***\n");
        return 1;
      }

      if(loaded < pocet) {
        fprintf(stderr,"***Not enough clusters to work with !***\n");
        return 1;
      }

       int cls_pocet, c1, c2;

       cls_pocet = loaded;

       while(cls_pocet != pocet) {                   // zlucovanie 2 najblizsich clusterov kym sa nedosiahne pozadovany pocet
       find_neighbours(clusters,cls_pocet,&c1,&c2);
       merge_clusters(&clusters[c1],&clusters[c2]);
       remove_cluster(clusters,cls_pocet,c2);
       --cls_pocet;
       }
       print_clusters(clusters,pocet);              // vypise clustre po zluceni
       free_clusters(&clusters,cls_pocet);
       return 0;
    }

    else {                                  // ak su zadane nespravne parametre vypise napovedu
      fprintf(stderr,
      "***********************************************\n"
      "***        Wrong program arguments !        ***\n"
      "***          ./proj3 filename [N]           ***\n"
      "*** filename => file containing object data ***\n"
      "***  N represents final number of clusters  ***\n"
      "***         N is set to 1 by default        ***\n"
      "***********************************************\n");

      return 1;
    }
}


