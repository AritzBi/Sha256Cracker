#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "sha256.h"
#include "mpi.h"

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
    // Si es una letra diferente a la última del alfabeto, es muy fácil, basta 		con poner la siguiente
    if (str[i] != alphabet[len-1]) {
      ptr = strchr(alphabet, str[i]);
      pos = ptr - alphabet + 1;
      str[i] = alphabet[pos];
      done = 1;
      break;
    // Si es la última letra del alfabeto, hay que poner la primera del alfabeto 	y continuar hacia atrás con el resto del string
    } else {
      str[i] = alphabet[0];
    }
  }
  // Podemos llegar al final del bucle anterior habiendo puesto todas las letras 	a la primera del alfabeto, 
  // entonces tenemos que añadir una letra más al principio
  if (!done) {
    sprintf(str, "%c%s", alphabet[0], str);
  }
}

//Función utilizada para ir saltando por el conjunto de claves candidatas
void nextR(char *str, char* alphabet, int repetitions) {
  int i;
  for ( i = 0; i < repetitions; i++)
	next(str, alphabet);
}

//Función utilizada para pasar de un array de bytes a hexadecimal
void bin_to_strhex(unsigned char *bin, unsigned int binsz, unsigned char result[HASHLENGTH+1] )
{
  char hex_str[]= "0123456789abcdef";
  unsigned int  i;
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
   sha256_final(&ctx,hashAux);
   //Guardamos sobre hashAux el hash computado y lo pasamos a hexadecimal
   bin_to_strhex( (unsigned char *)hashAux, sizeof(hashAux), hash);
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
  int found=0;
  int global_found=0;
  MPI_Status Stat;
  int numtasks, rank;
  opterr = 0;

  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Mientras que queden parámetros por procesar, miramos si hay un -a ALGO -n 		ALGO o -M ALGO
  // Si hay alguno de estos parámetros lo almacenamos en la variable que 		corresponda
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
    // Si no hay hash o hay más de uno, guardamos en secret un NULL para 		protestar luego
    secret[0] = '\0';
  }
  // Si no nos han pasado algún parámetro, los inicializamos a valores por 		defecto (defines)
  if (min == -1)
    min = MIN;
  if (max == -1)
    max = MAX;
  if (!alphabet) {
    alphabet = ALPHABET;
  }
  printf("%d\n",strlen(secret));


  printf("Soy el proceso %d y mi secreto es %s\n", rank, secret); 
  // ¿El hash que nos han pasado tiene el tamaño adecuado?
  if (strlen(secret) == HASHLENGTH+1) { 
    // Sí, pues vamos a generar candidatos, a hashearlos y a compararlos con el 	secreto
    // hasta encontrarlo o pasarnos del tamaño máximo
    printf("Starting the searching of candidates:\n");
    
    //Inicializamos la variable candidate y le asignamos espacio de memoria
    candidate = (char *)malloc(sizeof(char) * max);   	
    memset(candidate, 0, max);
	
	int i;
	//Calculamos la primera clave candidata que tiene que procesar cada hilo
	for ( i = 0; i < min; i++)
	{
		candidate[i] = alphabet[0];
	}
	if ( rank != 0 )
        {
	        nextR(candidate, alphabet, rank );
	}

	printf("Soy proceso %d y me llevo la clave inicial %s\n", rank, candidate);
	compute_hash(candidate, hash);

	//se sigue realizando la comprobación hasta que no se encuentre una 		clave candidata co el mismo hash u otro hilo haya encontrada ya la 		solución o el tamaño del candidato se pase del máximo permitido
	while (global_found < 1 && strlen(candidate) <= max) 
	{
           if ( strncmp(hash, secret, HASHLENGTH) == 0 )
           {
		found = 1;
           }
	   else
           {
	      //Buscamos el siguiente candidato que tiene que procesar el hilo
	      nextR(candidate, alphabet, numtasks );
	      //Calculamos el hash correspondiente a la clave
	      compute_hash(candidate, hash);
	   }
	   int i;
	   for (i = 0; i < numtasks; i++)
	   {
		MPI_Reduce(&found,&global_found,1,MPI_INT,MPI_SUM,i,MPI_COMM_WORLD);
	   }
	}
	 
	 if ( strlen(candidate) <= max && found == 1 )
	 {
	   printf("Associated hash %s\n", hash);
	   printf("FOUND! hash(%s) = %s\n", candidate, secret);
	 }

    if ( global_found < 1 && rank == 0 )
    {
       printf("Not found :(\n");
    }
  } else {
    printf("Error, usage: %s [-a \"ALPHABET\"] [-n MIN] [-m MAX] <hash>\n", argv[0]);
    return -1;
  }

  MPI_Finalize();

  return 0;
}

