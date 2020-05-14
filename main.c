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

struct token
{
    uint8_t offset_len;
    char c;
};


/*
* iki string'in ilk kaç karakteri özdeş?
* maksimum limit sayısı kadar kontrol yapar.
*/
static inline int prefix_eslesme_len(char *s1, char *s2, int limit)
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
//static inline void lz77memcpy(char *s1, char *s2, int size)
//{
//    while (size--)
//        *s1++ = *s2++;
//}


/*
* token array'ini, karakter array'ine dönüştürür.
*/

/*
* LZ77 ile sıkıştırılacak bir metin alır.
* token array'i döndürür.
* Eğer numTokens NULL değilse, token sayısını
* numTokens ile gösterilen yere kaydeder.
*/
static struct token *encode(char *text, int limit, int *token_sayisi)
{
    // cap (kapasite) hafızada kaç tokenlik yer ayırdığımız.
    int kapasite = 1 << 3;

    // kaç token oluşturduğumuz.
    int _tokenSayisi = 0;

    // oluşturulacak token
    struct token ornek;

    // lookahead ve search buffer'larının başlangıç pozisyonları
    char *tampon_ileri, *tampon_arama;

    // tokenler için yer ayır.
    struct token *encoded = malloc(kapasite * sizeof(struct token));

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
        int boyut_eslesme = 0;

        // search bufferda bulunan en uzun eşleşmenin
        // pozisyonu
        char *metin_eslesme = tampon_ileri;

        // search buffer içinde arama yap.
        for (; tampon_arama < tampon_ileri; tampon_arama++)
        {
            int len = prefix_eslesme_len(tampon_arama, tampon_ileri, LENGTHMASK);

            if (len > boyut_eslesme)
            {
                boyut_eslesme = len;
                metin_eslesme = tampon_arama;
            }
        }

        /*
        * ! ÖNEMLİ !
        * Eğer eşleşmenin içine metnin son karakteri de dahil olmuşsa,
        * tokenin içine bir karakter koyabilmek için, eşleşmeyi kısaltmamız
        * gerekiyor.
        */
        if (tampon_ileri + boyut_eslesme >= text + limit)
        {
            boyut_eslesme = text + limit - tampon_ileri - 1;
        }

        printf("ileri tampon: %s\n",tampon_ileri);
        // bulunan eşleşmeye göre token oluştur.
        ornek.offset_len = OFFSETLENGTH(tampon_ileri - metin_eslesme-1, boyut_eslesme);

        printf("###: %d %d %c\n",tampon_ileri-metin_eslesme,boyut_eslesme,tampon_ileri[boyut_eslesme]);

        tampon_ileri += boyut_eslesme;
        ornek.c = *tampon_ileri;

        // gerekirse, hafızada yer aç
        if (_tokenSayisi + 1 > kapasite)
        {
            kapasite = kapasite << 1;
            encoded = realloc(encoded, kapasite * sizeof(struct token));
        }

        // oluşturulan tokeni, array'e kaydet.

        encoded[_tokenSayisi++] = ornek;
    }

    if (token_sayisi)
        *token_sayisi = _tokenSayisi;

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
int num_characters(FILE *f )
{
    f=fopen("source.txt","r");
    int sayac=0;
    char c;
    for(c=getc(f); c!=EOF; c=getc(f))
        sayac++;
    fclose(f);
    return sayac;
}
int main(void)
{
    FILE *file;
    int text_size = 56, num_tokens, decode_size;
    char *text = "Bu kose yaz kosesi bu kose kis kosesi ortadaki su sisesi", *decoded_text = "";
    struct token *encoded_text;

    if (file = fopen("source.txt", "rb"))
    {
        text = file_read(file, &text_size);
        fclose(file);
    }


    text[num_characters(file)]='\0';
    encoded_text = encode(text, text_size, &num_tokens);

    if (file = fopen("encoded.txt", "wb"))
    {
        fwrite(encoded_text, sizeof(struct token), num_tokens, file);
        fclose(file);
    }

    /*
        decoded_metin = decode(encoded_metin, token_sayisi, &decode_boyutu);

        if (f = fopen("decoded.txt", "wb"))
        {
            fwrite(decoded_metin, 1, decode_boyutu, f);
            fclose(f);
        }
    */
    printf("\nOrjinal Boyut: %d, Encode Boyutu: %d", text_size, num_tokens * sizeof(struct token));
    return 0;
}
