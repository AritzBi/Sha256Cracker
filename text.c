#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

	int numtasks, rank, dest, source, rc, tag = 1, found = 0;
	char in_msg, out_msg ='x'; MPI_Status Stat;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int count=0;
	while ( found == 0 )
	{
		count++;
		printf("ELPUTOBUCLE %d y proceso %d \n",count,rank);
		printf("Hacinedo cosas por proceso %d \n", rank);
		if ( rank == 2 && count==15)
			found = 1;
		int i;		
		for ( i = 0; i < numtasks; i++)
		{
			if ( i != rank)
			{
				printf("Soy el proceso %d y mando a %d la variable found a %d\n", rank, i, found);
				MPI_Send(&found, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			}
				
		}
		if ( found == 0 )
		{
			int i;
			for ( i = 0; i < numtasks; i++)
			{
				if ( i != rank && found == 0 )
				{
					MPI_Recv(&found, 1, MPI_INT, i, tag, MPI_COMM_WORLD,&Stat);
					printf("Soy el proceso %d y recibo de %d el valor found a %d\n", rank, Stat.MPI_SOURCE, found);
				}
				
			}
			
		}
			
			 
	}
	printf("Soy proceso %d y tengo la variable found a %d\n", rank, found); 
	/**if (rank == 0) {
		out_msg = 'r';
		dest    = 1;
		printf("Soy %d y envio a %d\n", rank,dest);
		rc = MPI_Send(&out_msg, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD); // Es una llamada bloqueante
																	     // (void *buf, int count,MPI_Datatype datatype, 
																	     //int dest, int tag, MPI_Comm comm)
	}
	else{
		source=0;
		rc = MPI_Recv(&in_msg, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD,&Stat); //(buf, count, MPI_INT, MPI_ANY_SOURCE, 
																		        // MPI_ANY_TAG, MPI_COMM_WORLD, status)
		printf("Soy %d y recibo de %d la siguiente letra %c\n", rank,source,in_msg);
	}**/
	
	MPI_Finalize();
}
