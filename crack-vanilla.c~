#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "sha256.h"

// Constantes que sirven como opción por defecto si no nos han indicado lo contrario por parámetros
#define HASHLENGTH 64
#define MIN 1
#define MAX 8
#define ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

// Función que recibe un string y un alfabeto y actualiza ese string con la siguiente
// clave candidata en función del alfabeto
void next(char *str, char* alphabet) {
  int i, pos, done = 0;
  int len = strlen(alphabet);
  char *ptr;

  // Empezamos en la última letra del string y de ahí hacia atrás
  for (i = strlen(str)-1; i >= 0; i--) {
    // Si es una letra diferente a la última del alfabeto, es muy fácil, basta con poner la siguiente
    if (str[i] != alphabet[len-1]) {
      ptr = strchr(alphabet, str[i]);
      pos = ptr - alphabet + 1;
      str[i] = alphabet[pos];
      done = 1;
      break;
    // Si es la última letra del alfabeto, hay que poner la primera del alfabeto y continuar hacia atrás con el resto del string
    } else {
      str[i] = alphabet[0];
    }
  }
  // Podemos llegar al final del bucle anterior habiendo puesto todas las letras a la primera del alfabeto, 
  // entonces tenemos que añadir una letra más al principio
  if (!done) {
    sprintf(str, "%c%s", alphabet[0], str);
  }
}

void print_hash(unsigned char hash[])
{
   int idx;
   for (idx=0; idx < 32; idx++)
      printf("%02x",hash[idx]);
   printf("\n");
}

void bin_to_strhex(unsigned char *bin, unsigned int binsz, unsigned char result[HASHLENGTH+1] )
{
  char hex_str[]= "0123456789abcdef";
  unsigned int  i;

  //result = (char *)malloc(binsz * 2 + 1);
  (result)[binsz * 2] = 0;

  if (!binsz)
    return;

  for (i = 0; i < binsz; i++)
    {
      (result)[i * 2 + 0] = hex_str[(bin[i] >> 4) & 0x0F];
      (result)[i * 2 + 1] = hex_str[(bin[i]     ) & 0x0F];
    }  
}

void compute_hash(char *str, unsigned char hash[HASHLENGTH+1] ) {
  // Hacer aquí el cómputo del hash de str y guardarlo en hash[]
  unsigned char hashAux[32];
   SHA256_CTX ctx;
   sha256_init(&ctx);
   sha256_update(&ctx,str,strlen(str));
   //printf("For candidate %s\n", str);
   sha256_final(&ctx,hashAux);
   //printf("Hash en bytes \n");
   //print_hash(hashAux);
   bin_to_strhex( (unsigned char *)hashAux, sizeof(hashAux), hash);
   //printf("Hash en String %s\n", hash);
}

int main(int argc, char *argv[]) {
  int i;
  // Variables para la gestión de claves candidatas:
  char *candidate; // Aquí guardamos el candidato actual
  unsigned char hash[HASHLENGTH+1]; // Aquí guardamos el hash del candidato actual
  char secret[HASHLENGTH+1]; // Aquí guardamos el hash que queremos romper
  // Variables para almacenar los parámetros
  int min = -1;
  int max = -1;
  char *alphabet = NULL;
  // Variables para procesar los parámetros con getopt
  int index;
  int c;

  opterr = 0;
  // Mientras que queden parámetros por procesar, miramos si hay un -a ALGO -n ALGO o -M ALGO
  // Si hay alguno de estos parámetros lo almacenamos en la variable que corresponda
  while ((c = getopt (argc, argv, "a:n:m:")) != -1)
    switch (c) {
      case 'a':
        alphabet = optarg;
        break;
      case 'n':
        min = atoi(optarg);
        break;
      case 'm':
        max = atoi(optarg);
        break;
      case '?':
        // Si se ha usado -a, -n o -m pero no se ha dado un valor, protestamos
        if (optopt == 'a' || optopt == 'n' || optopt == 'm')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
        return 1;
      default:
        abort();
    }
  printf("%d \n",optind);
printf("%d \n",argc);
  // Miramos si queda un parámetro sin -a, -n o -m, que debería ser el hash
  if (optind + 1 == argc) {
    strncpy(secret, argv[optind], HASHLENGTH);
    printf("Secreto obtenido %s\n", secret);
  } else {
    // Si no hay hash o hay más de uno, guardamos en secret un NULL para protestar luego
    secret[0] = '\0';
  }
  // Si no nos han pasado algún parámetro, los inicializamos a valores por defecto (defines)
  if (min == -1)
    min = MIN;
  if (max == -1)
    max = MAX;
  if (!alphabet) {
    alphabet = ALPHABET;
  }
  printf("%d\n",strlen(secret));
  // ¿El hash que nos han pasado tiene el tamaño adecuado?
  if (strlen(secret) == HASHLENGTH+1) { 
    // Sí, pues vamos a generar candidatos, a hashearlos y a compararlos con el secreto
    // hasta encontrarlo o pasarnos del tamaño máximo
    candidate = (char *)malloc(sizeof(char) * max);
    memset(candidate, 0, max);
    // Empecemos por la primera letra del alfabeto
    candidate[0] = alphabet[0];
    compute_hash(candidate, hash);
    printf("Starting the searching of candidates:");
    
    while (strncmp(hash, secret, HASHLENGTH) != 0 && strlen(candidate) <= max) {
      //printf("%d",HASHLENGTH);
      //printf("Secreto obtenido %s\n", secret);
      //printf("Hash obtenido %s\n", hash);
      //printf("Size secret %d\n", strlen(secret) );
      //printf("Size hash %d\n", strlen(hash) );
      int result = strncmp(hash, secret, HASHLENGTH);
      //printf("Result %i\n", result );
      next(candidate, alphabet);
      compute_hash(candidate, hash);
      //printf("Hash obtenido %s\n", hash);
      //print_hash(hash);
    }
    // Si hemos salido del bucle anterior puede que nos hayamos pasado del máximo
    if (strlen(candidate) > max)
      printf("Not found :(\n");
    else 
      printf("FOUND! hash(%s) = %s\n", candidate, secret);
   free(candidate); // Save the whales and free your mallocs!
  } else {
    printf("Error, usage: %s [-a \"ALPHABET\"] [-n MIN] [-m MAX] <hash>\n", argv[0]);
    return -1;
  }

  return 0;
}

