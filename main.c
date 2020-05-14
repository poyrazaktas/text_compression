#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define OFFSETBITS 5
#define LENGTHBITS (8-OFFSETBITS)

#define OFFSETMASK ((1 << (OFFSETBITS)) - 1)
#define LENGTHMASK ((1 << (LENGTHBITS)) - 1)

#define GETOFFSET(x) (x >> LENGTHBITS)
#define GETLENGTH(x) (x & LENGTHMASK)
#define OFFSETLENGTH(x,y) (x << LENGTHBITS | y)

struct token {
    uint8_t offset_len;
    char c;
};


/*
* iki string'in ilk kaç karakteri özdeş?
* maksimum limit sayısı kadar kontrol yapar.
*/
static inline int prefix_match_length(char *s1, char *s2, int limit)
{
    int len = 0;

    while (*s1++ == *s2++ && len < limit)
        len++;

    return len;
}

/*
* memcpy fonksiyonu ile benzer. Byte düzeyinde
* kopyalama yapma garantisi olduğu için, bu
* versiyonu kullanıyoruz.
*/
static inline void lz77memcpy(char *s1, char *s2, int size)
{
    while (size--)
        *s1++ = *s2++;
}

/*
* token array'ini, karakter array'ine dönüştürür.
*/

/*
* LZ77 ile sıkıştırılacak bir metin alır.
* token array'i döndürür.
* Eğer numTokens NULL değilse, token sayısını
* numTokens ile gösterilen yere kaydeder.
*/
static struct token *encode(char *text, int limit, int *numTokens)
{
    // cap (kapasite) hafızada kaç tokenlik yer ayırdığımız.
    int cap = 1 << 3;

    // kaç token oluşturduğumuz.
    int _numTokens = 0;

    // oluşturulacak token
    struct token t;

    // lookahead ve search buffer'larının başlangıç pozisyonları
    char *tampon_ileri, *tampon_arama;

    // tokenler için yer ayır.
    struct token *encoded = malloc(cap * sizeof(struct token));

    // token oluşturma döngüsü
    for (tampon_ileri = text; tampon_ileri < text + limit; tampon_ileri++)
    {

        // search buffer'ı lookahead buffer'ın 31 (OFFSETMASK) karakter
        // gerisine koyuyoruz.
        tampon_arama = tampon_ileri - OFFSETMASK;

        // search buffer'ın metnin dışına çıkmasına engel ol.
        if (tampon_arama < text)
            tampon_arama = text;

        // search bufferda bulunan en uzun eşleşmenin
        // boyutu
        int max_len = 0;

        // search bufferda bulunan en uzun eşleşmenin
        // pozisyonu
        char *max_match = tampon_ileri;

        // search buffer içinde arama yap.
        for (; tampon_arama < tampon_ileri; tampon_arama++)
        {
            int len = prefix_match_length(tampon_arama, tampon_ileri, LENGTHMASK);

            if (len > max_len)
            {
                max_len = len;
                max_match = tampon_arama;
            }
        }

        /*
        * ! ÖNEMLİ !
        * Eğer eşleşmenin içine metnin son karakteri de dahil olmuşsa,
        * tokenin içine bir karakter koyabilmek için, eşleşmeyi kısaltmamız
        * gerekiyor.
        */
        if (tampon_ileri + max_len >= text + limit)
        {
            max_len = text + limit - tampon_ileri - 1;
        }

        //printf("Lookahead: %s\n",lookahead);
        // bulunan eşleşmeye göre token oluştur.
        t.offset_len = OFFSETLENGTH(tampon_ileri - max_match-1, max_len);
        printf("###: %d %d %c\n",tampon_ileri-max_match,max_len,tampon_ileri[max_len]);
        tampon_ileri += max_len;
        t.c = *tampon_ileri;
        //printf("%c",t.c);
        // gerekirse, hafızada yer aç
        if (_numTokens + 1 > cap)
        {
            cap = cap << 1;
            encoded = realloc(encoded, cap * sizeof(struct token));
        }

        // oluşturulan tokeni, array'e kaydet.

        encoded[_numTokens++] = t;
    }

    if (numTokens)
        *numTokens = _numTokens;

    return encoded;
}

// bir dosyanın tamamını tek seferde
// okur. Büyük dosyaları okumak için uygun
// değildir.
static char *file_read(FILE *f, int *size)
{
    char *content;
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    content = malloc(*size);
    fseek(f, 0, SEEK_SET);
    fread(content, 1, *size, f);
    return content;
}

int main(void)
{
    FILE *f;
    int metin_boyutu = 56, token_sayisi, decode_boyutu;
    char *kaynak_metin = "Bu kose yaz kosesi bu kose kis kosesi ortadaki su sisesi", *decoded_metin = "";
    struct token *encoded_metin;

    if (f = fopen("source.txt", "rb"))
    {
        kaynak_metin = file_read(f, &metin_boyutu);
        fclose(f);
    }



    encoded_metin = encode(kaynak_metin, metin_boyutu, &token_sayisi);

    if (f = fopen("encoded.txt", "wb"))
    {
        fwrite(encoded_metin, sizeof(struct token), token_sayisi, f);
        fclose(f);
    }

/*
    decoded_metin = decode(encoded_metin, token_sayisi, &decode_boyutu);

    if (f = fopen("decoded.txt", "wb"))
    {
        fwrite(decoded_metin, 1, decode_boyutu, f);
        fclose(f);
    }
*/
    printf("\nOrjinal Boyut: %d, Encode Boyutu: %d", metin_boyutu, token_sayisi * sizeof(struct token));
    return 0;
}
