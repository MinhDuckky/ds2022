#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Find out rank, size
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  if (world_size < 2) {
    fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  char *filename;
  char *filecontent;

  if (world_rank == 0) {

    filename = "output.txt";
    MPI_Send(
      /* data         = */ &filename, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_CHAR, 
      /* destination  = */ 1, 
      /* tag          = */ 0, 
      /* communicator = */ MPI_COMM_WORLD);

    FILE *file  = fopen("input.txt", "r");

    char s[256] = {0};

    char *line;

    while (fgets(line, 100, file))
    {
        strncat(s, line, 100);
    }
    filecontent = s;
    MPI_Send(
      /* data         = */ &filecontent, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_CHAR, 
      /* destination  = */ 1, 
      /* tag          = */ 0, 
      /* communicator = */ MPI_COMM_WORLD);
    
    fclose(file);


  } else if (world_rank == 1) {

    MPI_Recv(
      /* data         = */ &filename, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_CHAR, 
      /* source       = */ 0, 
      /* tag          = */ 0, 
      /* communicator = */ MPI_COMM_WORLD, 
      /* status       = */ MPI_STATUS_IGNORE);
    printf("Open file: \'%s\' from process 0 for writing\n", filename);
    FILE *file  = fopen(filename, "w");

    MPI_Recv(
      /* data         = */ &filecontent, 
      /* count        = */ 1, 
      /* datatype     = */ MPI_CHAR, 
      /* source       = */ 0, 
      /* tag          = */ 0, 
      /* communicator = */ MPI_COMM_WORLD, 
      /* status       = */ MPI_STATUS_IGNORE);
    printf("Write: \n%s\n from process 0 to \'%s\'\n", filecontent, filename);
    
    fputs(filecontent, file);
    fclose(file);
  }
  MPI_Finalize();
}