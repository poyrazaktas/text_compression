#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdint.h>

#define OFFSETBITS 5
#define LENGTHBITS (8-OFFSETBITS)

#define OFFSETMASK ((1 << (OFFSETBITS)) - 1)
#define LENGTHMASK ((1 << (LENGTHBITS)) - 1)

#define GETOFFSET(x) (x >> LENGTHBITS)
#define GETLENGTH(x) (x & LENGTHMASK)
#define OFFSETLENGTH(x,y) (x << LENGTHBITS | y)
#define MAX_TREE_HT 100
typedef unsigned char byte;
struct token {
    uint8_t offset_len;
    char c;
};

struct EnDusukDugum {


    char veri;


    unsigned frekans;


    struct EnDusukDugum *left, *right;
};


struct UfakDugum {


    unsigned boyut;


    unsigned kapasite;


    struct EnDusukDugum** array;
};


struct EnDusukDugum* newNode(char veri, unsigned frekans)
{
    struct EnDusukDugum* temp
        = (struct EnDusukDugum*)malloc
(sizeof(struct EnDusukDugum));

    temp->left = temp->right = NULL;
    temp->veri = veri;
    temp->frekans = frekans;

    return temp;
}


struct UfakDugum* EnDusukDugumOlustur(unsigned kapasite)

{

    struct UfakDugum* UfakDugum
        = (struct UfakDugum*)malloc(sizeof(struct UfakDugum));


    UfakDugum->boyut = 0;

    UfakDugum->kapasite = kapasite;

    UfakDugum->array
        = (struct EnDusukDugum**)malloc(UfakDugum->
kapasite * sizeof(struct EnDusukDugum*));
    return UfakDugum;
}


void swapEnDusukDugum(struct EnDusukDugum** a,
                     struct EnDusukDugum** b)

{

    struct EnDusukDugum* t = *a;
    *a = *b;
    *b = t;
}


void minHeapify(struct UfakDugum* UfakDugum, int idx)

{

    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < UfakDugum->boyut && UfakDugum->array[left]->
frekans < UfakDugum->array[smallest]->frekans)
        smallest = left;

    if (right < UfakDugum->boyut && UfakDugum->array[right]->
frekans < UfakDugum->array[smallest]->frekans)
        smallest = right;

    if (smallest != idx) {
        swapEnDusukDugum(&UfakDugum->array[smallest],
                        &UfakDugum->array[idx]);
        minHeapify(UfakDugum, smallest);
    }
}


int BoyutuBirmi(struct UfakDugum* UfakDugum)
{

    return (UfakDugum->boyut == 1);
}


struct EnDusukDugum* KucuguAyikla(struct UfakDugum* UfakDugum)

{

    struct EnDusukDugum* temp = UfakDugum->array[0];
    UfakDugum->array[0]
        = UfakDugum->array[UfakDugum->boyut - 1];

    --UfakDugum->boyut;
    minHeapify(UfakDugum, 0);

    return temp;
}


void kucukDugumEkle(struct UfakDugum* UfakDugum,
                   struct EnDusukDugum* EnDusukDugum)

{

    ++UfakDugum->boyut;
    int i = UfakDugum->boyut - 1;

    while (i && EnDusukDugum->frekans < UfakDugum->array[(i - 1) / 2]->frekans) {

        UfakDugum->array[i] = UfakDugum->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    UfakDugum->array[i] = EnDusukDugum;
}


void KucukDugumKur(struct UfakDugum* UfakDugum)

{

    int n = UfakDugum->boyut - 1;
    int i;

    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(UfakDugum, i);
}


FILE *huffdos;
FILE *huffrak;
void diziyiBastir(int arr[], int n)
{
huffdos = fopen("huffmans.txt", "a+");
huffrak = fopen("huffmanrak.txt", "a+");
    int i;
    for (i = 0; i < n; ++i) {
    	printf("%d", arr[i]);
    	fprintf(huffdos,"%d",arr[i]);
    	fprintf(huffrak,"%d",arr[i]);
	}

    fprintf(huffdos,"\n");
    fprintf(huffrak,"\n");
    printf("\n");
    fclose(huffdos);
    fclose(huffrak);
}


int isLeaf(struct EnDusukDugum* root)

{

    return !(root->left) && !(root->right);
}


struct UfakDugum* createAndBuildMinHeap(char veri[], int frekans[], int boyut)

{

    struct UfakDugum* UfakDugum = EnDusukDugumOlustur(boyut);
  int i;
    for (i = 0; i < boyut; ++i)
        UfakDugum->array[i] = newNode(veri[i], frekans[i]);

    UfakDugum->boyut = boyut;
    KucukDugumKur(UfakDugum);

    return UfakDugum;
}


struct EnDusukDugum* buildHuffmanTree(char veri[], int frekans[], int boyut)

{
    struct EnDusukDugum *left, *right, *top;


    struct UfakDugum* UfakDugum = createAndBuildMinHeap(veri, frekans, boyut);


    while (!BoyutuBirmi(UfakDugum)) {


        left = KucuguAyikla(UfakDugum);
        right = KucuguAyikla(UfakDugum);


        top = newNode('$', left->frekans + right->frekans);

        top->left = left;
        top->right = right;

        kucukDugumEkle(UfakDugum, top);
    }


    return KucuguAyikla(UfakDugum);
}


void HuffmanKoduBastir(struct EnDusukDugum* root, int arr[], int top)

{
  huffdos = fopen("huffmans.txt", "a+");

    if (root->left) {

        arr[top] = 0;
        HuffmanKoduBastir(root->left, arr, top + 1);
    }


    if (root->right) {

        arr[top] = 1;
        HuffmanKoduBastir(root->right, arr, top + 1);
    }


    if (isLeaf(root)) {
        fprintf(huffdos,"%c ",root->veri);
        printf("%c: ", root->veri);
        fclose(huffdos);
        diziyiBastir(arr, top);
    }
	fclose(huffdos);
}


void huffmanKodu(char veri[], int frekans[], int boyut)

{

    struct EnDusukDugum* root
        = buildHuffmanTree(veri, frekans, boyut);


    int arr[MAX_TREE_HT], top = 0;

    HuffmanKoduBastir(root, arr, top);
}


static inline int EslesmeUzunluguBul(char *s1, char *s2, int limit)
{
    int len = 0;

    while (*s1++ == *s2++ && len < limit)
        len++;

    return len;
}


static inline void lz77memcpy(char *s1, char *s2, int boyut)
{
    while (boyut--)
        *s1++ = *s2++;
}





char tut[10000];
int d=0;
static struct token *encode(char *text, int limit, int *numTokens)
{

    int cap = 1 << 3;


    int _numTokens = 0;


    struct token t;


    char *tampon_ileri, *tampon_arama;


    struct token *encoded = malloc(cap * sizeof(struct token));


    for (tampon_ileri = text; tampon_ileri < text + limit; tampon_ileri++)
    {





        tampon_arama = tampon_ileri - OFFSETMASK;


        if (tampon_arama < text)
            tampon_arama = text;


        int max_len = 0;


        char *max_match = tampon_ileri;


        for (; tampon_arama < tampon_ileri; tampon_arama++)
        {
            int len = EslesmeUzunluguBul(tampon_arama, tampon_ileri, LENGTHMASK);

            if (len > max_len)
            {
                max_len = len;
                max_match = tampon_arama;
            }
        }


        if (tampon_ileri + max_len >= text + limit)
        {
            max_len = text + limit - tampon_ileri - 1;
        }


        t.offset_len = OFFSETLENGTH(tampon_ileri - max_match-1, max_len);
        printf("###: %d %d %c\n",tampon_ileri-max_match,max_len,tampon_ileri[max_len]);
        tampon_ileri += max_len;

        t.c = *tampon_ileri;
        tut[d]=t.c;
    	d++;


        if (_numTokens + 1 > cap)
        {
            cap = cap << 1;
            encoded = realloc(encoded, cap * sizeof(struct token));
        }



        encoded[_numTokens++] = t;
    }

    if (numTokens)
        *numTokens = _numTokens;

    return encoded;
}


static char *dosyaOku(FILE *f, int *boyut)
{
    char *content;
    fseek(f, 0, SEEK_END);
    *boyut = ftell(f);
    content = malloc(*boyut);
    fseek(f, 0, SEEK_SET);
    fread(content, 1, *boyut, f);
    return content;
}
int karakter_sayisi(FILE *f )
{
    f=fopen("source.txt","r");
    int count=0;
    char c;
    for(c=getc(f);c!=EOF;c=getc(f))
        count++;
    fclose(f);
    return count;
}

int tut2[30];

int p=0;
char harfler[30];
int say=0;

void karakterSayisiBul(char string[100000]){

   int c = 0, count[30] = {0};


   while (string[c] != '\0')
   {

      if (string[c] >= 'a' && string[c] <= 'z')
         count[string[c]-'a']++;
      c++;
   }
   printf("\n");

   for (c = 0; c < 30; c++)
   {

      if (count[c] != 0){

      	tut2[p]=count[c];
      	harfler[p]=c+'a';
      	p++;
	  }

   }


}
byte code[5000];
int bitSayisiTut;
void huffmanaCevir(int donusecekHarfSayisi,char kisaltilmisCumle[10000]){
	huffdos = fopen("huffmans.txt", "r+");

	char temp[1000];
	char harfler[donusecekHarfSayisi];
	char rakamlar[donusecekHarfSayisi][1000];
	char cop[3];
	int i;

	for(i=0;i<donusecekHarfSayisi;i++){

	fgets(temp,100,huffdos);
	strcpy(rakamlar[i],temp);

	}

	printf("\n");

int m=0;

	char rakamTut[donusecekHarfSayisi][10];
	char rakamStr[donusecekHarfSayisi][10];
	int j;
	for(j=0;j<donusecekHarfSayisi;j++)
	{

	m=0;
	for(i=0;i<strlen(rakamlar[j]);i++){

			harfler[j]=rakamlar[j][0];

		if(i!=0&&isdigit(rakamlar[j][i])){
			rakamTut[j][m]=rakamlar[j][i];
			m++;
		}

	}
	}
	huffrak = fopen("huffmanrak.txt", "r");

	for(i=0;i<donusecekHarfSayisi;i++){
		fgets(temp,100,huffrak);

		strcpy(rakamStr[i],temp);
	}

	printf("\n");

	int mn;
	int t=0;
	printf("\n*************CUMLENIN LZ77 ILE KISALTILMIS HALI***************\n\n");
	printf("%s",kisaltilmisCumle);
	printf("\n");
	printf("\n***************CUMLENIN DEFLATE HALI****************\n\n");
	int say=0;
	for(i=0;i<strlen(kisaltilmisCumle);i++){
		for(j=0;j<donusecekHarfSayisi;j++)
		if(kisaltilmisCumle[i]==harfler[j]){
			for(mn=0;mn<strlen(rakamStr[j]);mn++){
				if(rakamStr[j][mn]=='\n')
				break;
				printf("%c",rakamStr[j][mn]);

				code[t]=rakamStr[j][mn]-'0';
				t++;
				say++;
			}

			break;
		}

	}



bitSayisiTut=say;
	fclose(huffdos);
}

long int findSize(char file_name[])
{

    FILE* fp = fopen(file_name, "r");


    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);


    long int res = ftell(fp);


    fclose(fp);

    return res;
}

int main(void)
{
	FILE*HUFFMANENCODED;
    HUFFMANENCODED = fopen("HUFFMANencoded.txt", "w");
	huffrak = fopen("huffmanrak.txt", "w");
	fclose(huffrak);
    FILE *f;
    huffdos = fopen("huffmans.txt", "w");
    int metin_boyutu = 56000, token_sayisi, decode_boyutu;
    char *kaynak_metin = "", *decoded_metin = "";
    struct token *encoded_metin;

    if (f = fopen("source.txt", "rb"))
    {
        kaynak_metin = dosyaOku(f, &metin_boyutu);
        fclose(f);
    }
    else{
        printf("Compress edilecek metin bulunamadi");
        return 0;
    }

    kaynak_metin[karakter_sayisi(f)]='\0';
    encoded_metin = encode(kaynak_metin, metin_boyutu, &token_sayisi);

    if (f = fopen("LZ77encoded.txt", "wb"))
    {
        fwrite(encoded_metin, sizeof(struct token), token_sayisi, f);
        fclose(f);
    }



    printf("\nOrjinal Boyut: %d, Encode Boyutu: %d", metin_boyutu, token_sayisi * sizeof(struct token));
    printf("\n%s",tut);
    int i;
    karakterSayisiBul(tut);

    	char harfTut[p];
	int frequenceTut[p];

    for(i=0;i<p;i++){
    	printf("%c den %d tane var .\n",harfler[i],tut2[i]);
    	harfTut[i]=harfler[i];
    	frequenceTut[i]=tut2[i];
	}



	int boyut = sizeof(harfTut) / sizeof(harfTut[0]);
  printf("\nCUMLEDE TOPLAM %d FARKLI HARF GECMEKTEDIR, BU HARFLERE ATANAN HUFFMAN KODLARI: \n",boyut);
  fclose(huffdos);
    huffmanKodu(harfTut,frequenceTut,boyut);
	huffmanaCevir(p,tut);
	    byte buf = 0, nbuf = 0;

    byte bitCode[bitSayisiTut];
    printf("\n");

    for(i=0; i<bitSayisiTut; i++)
    {
        bitCode[i]=code[i];
    }

    for (i =0; i<bitSayisiTut; i++) {
            buf |= bitCode[i] << nbuf;
            nbuf++;
            if (nbuf == 8) {
                fputc(buf, HUFFMANENCODED);
                nbuf = buf = 0;
            }
        }

    fputc(buf, HUFFMANENCODED);

    fclose(HUFFMANENCODED);

	char file_name[] = { "HUFFMANencoded.txt" };
    long int res = findSize(file_name);



	printf("\n\n************************SONUCLAR******************************\n");
	printf("\n-> DOSYANIN ORIJINAL BOYUTU=%d BYTE\n",metin_boyutu);
    printf("\n-> lz77 ALGORITMASI SONUCU SONUC=%d BYTE OLDU\n",token_sayisi);
     if (res != -1)
        printf("\n-> DEFLATE SIKISTIRMASI SONUCU BOYUT=%d BYTE OLDU\n", res);

    printf("\n**************************************************************");



    return 0;
}
