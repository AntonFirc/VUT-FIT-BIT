#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

int free_resources(); //declaration of cleaning function

//converts string to integer and checks if string contains only digits
int get_number(char* input)
{
  int length = strlen(input);
    for (int i=0;i<length; i++)
        if(!isdigit(input[i])){
          fprintf(stderr, "Error: One of arguments is not an valid integer!\n");
          exit(1);
        };
  return atoi(input);             // returns input string converted to integer
}

//initializes semaphores used
int init_semaphores()
{
  sem_t *semaphore1, *semaphore2, *semaphore3,*semaphore4, *semaphore5;

  if((semaphore1 = sem_open("/semaphore1", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
    {
    	fprintf(stderr, "Semaphore initialization failed !\n");
      return -1;
    }

    if((semaphore2 = sem_open("/semaphore2", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
      fprintf(stderr, "Semaphore initialization failed !\n");
      return -1;
    }

    if((semaphore3 = sem_open("/semaphore3", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
      fprintf(stderr, "Semaphore initialization failed !\n");
      return -1;
    }

    if((semaphore4 = sem_open("/semaphore4", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    {
      fprintf(stderr, "Semaphore initialization failed !\n");
      return -1;
    }

    if((semaphore5 = sem_open("/semaphore5", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
    {
      fprintf(stderr, "Semaphore initialization failed !\n");
      return -1;
    }


  return 1;
}

//allocates all essential shared memory
int init_memory()
{
  int mem_id = shmget(ftok("proj2.c",0), sizeof(int),   //initializes block of shared memory to save operation counter
             IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  if (mem_id == -1) {
    fprintf(stderr,"%s\n","Error : Memory allocation failed !");
    return -1;
  }
  int* counter = (int *) shmat(mem_id, NULL, 0);        //pairs variable with block of shared memory
  *counter = 1;                                         //initializes counter to 0
  shmdt(counter);                                       //detaches variable from block of shared memory

  int mem_idA = shmget(ftok("proj2.c",1), sizeof(int),   //initializes block of shared memory
             IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  if (mem_idA == -1) {
    fprintf(stderr,"%s\n","Error : Memory allocation failed !");
    return -1;
  }
  int* idA = (int *) shmat(mem_idA, NULL, 0);        //pairs variable with block of shared memory
  *idA = 0;                                         //initializes to 0
  shmdt(idA);                                       //detaches variable from block of shared memory

  int mem_idC = shmget(ftok("proj2.c",2), sizeof(int),   //initializes block of shared memory
             IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  if (mem_idC == -1) {
    fprintf(stderr,"%s\n","Error : Memory allocation failed !");
    return -1;
  }
  int* idC = (int *) shmat(mem_idC, NULL, 0);        //pairs variable with block of shared memory
  *idC = 0;                                         //initializes to 0
  shmdt(idC);                                       //detaches variable from block of shared memory

  int mem_proc = shmget(ftok("proj2.c",3), sizeof(int),   //initializes block of shared memory
             IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  if (mem_proc == -1) {
    fprintf(stderr,"%s\n","Error : Memory allocation failed !");
    return -1;
  }
  int* proc = (int *) shmat(mem_proc, NULL, 0);        //pairs variable with block of shared memory
  *proc = 0;                                         //initializes to 0
  shmdt(proc);                                       //detaches variable from block of shared memory

  int mem_adult = shmget(ftok("proj2.c",4), sizeof(int),   //initializes block of shared memory
             IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  if (mem_adult == -1) {
    fprintf(stderr,"%s\n","Error : Memory allocation failed !");
    return -1;
  }
  int* adult = (int *) shmat(mem_adult, NULL, 0);        //pairs variable with block of shared memory
  *adult = 0;                                         //initializes to 0
  shmdt(adult);                                       //detaches variable from block of shared memory


  int mem_child = shmget(ftok("proj2.c",5), sizeof(int),   //initializes block of shared memory
             IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  if (mem_child == -1) {
    fprintf(stderr,"%s\n","Error : Memory allocation failed !");
    return -1;
  }
  int* child = (int *) shmat(mem_child, NULL, 0);        //pairs variable with block of shared memory
  *child = 0;                                         //initializes to 0
  shmdt(child);                                       //detaches variable from block of shared memory

  int mem_wait = shmget(ftok("proj2.c",6), sizeof(int),   //initializes block of shared memory
             IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
  if (mem_wait == -1) {
    fprintf(stderr,"%s\n","Error : Memory allocation failed !");
    return -1;
  }
  int* waiting = (int *) shmat(mem_wait, NULL, 0);        //pairs variable with block of shared memory
  *waiting = 0;                                         //initializes to 0
  shmdt(waiting);                                       //detaches variable from block of shared memory

  return 1;
}

/*prints output file proj2.out
 *Parameters:
 *  type => which text to prints
 *  cnt => count of operation
 *  pers => adult or child
 *  id => id of child or adult
 *  AC => coutnt of adults in centre
 *  CC => count of children in centre
*/
int write_log(int type, int cnt, char pers, int id, int AC, int CC)
{
  FILE *log = fopen("proj2.out","a+");  //output file
  if (log == NULL) {              //error message
    fprintf(stderr, "Error : Opening file \"proj2.out\" failed !\n");
    free_resources();
    exit(2);
  }

  switch (type) {
    case 0:
        fprintf(log,"%-5d: %c %-3d: started\n",cnt,pers,id);
        //fprintf(stdout,"%-5d: %c %-3d: started\n",cnt,pers,id);
        break;
    case 1:
        fprintf(log,"%-5d: %c %-3d: enter\n",cnt,pers,id);
        //fprintf(stdout,"%-5d: %c %-3d: enter\n",cnt,pers,id);
        break;
    case 2:
        fprintf(log,"%-5d: %c %-3d: trying to leave\n",cnt,pers,id);
        //fprintf(stdout,"%-5d: %c %-3d: trying to leave\n",cnt,pers,id);
        break;
    case 3:
        fprintf(log,"%-5d: %c %-3d: leave\n",cnt,pers,id);
        //fprintf(stdout,"%-5d: %c %-3d: leave\n",cnt,pers,id);
        break;
    case 4:
        fprintf(log,"%-5d: %c %-3d: finished\n",cnt,pers,id);
        //fprintf(stdout,"%-5d: %c %-3d: finished\n",cnt,pers,id);
        break;
    case 5:
        fprintf(log,"%-5d: %c %-3d: waiting: %-2d: %-2d\n",cnt,pers,id,AC,CC);
        //fprintf(stdout,"%-5d: %c %-3d: waiting: %-2d: %-2d\n",cnt,pers,id,AC,CC);
        break;
  }

  int close;
  if ((close = fclose(log)) == EOF) {   //error message
    fprintf(stderr, "Error : Closing file \"proj2.out\" failed !\n");
    free_resources();
    exit(2);
  }

  return cnt+1;   //returns new operation count
}

//creates adult processes according to parameters given
int adult_ctor(int A, int AGT, int AWT, int C)
{
  int mem_id = shmget(ftok("proj2.c",0), sizeof(int),S_IRUSR | S_IWUSR);    //attach shared memory of counter
  int *cnt = (int *) shmat(mem_id, NULL, 0);                                //attach shared memory of counter
  int mem1_id = shmget(ftok("proj2.c",1), sizeof(int),S_IRUSR | S_IWUSR);   //shared memory of process id
  int *id = (int *) shmat(mem1_id, NULL, 0);
  int mem_proc = shmget(ftok("proj2.c",3), sizeof(int),S_IRUSR | S_IWUSR);  //"done processes"
  int *proc = (int *) shmat(mem_proc, NULL, 0);
  int mem_adult = shmget(ftok("proj2.c",4), sizeof(int),S_IRUSR | S_IWUSR); //count of adults in centre
  int *adult_cnt = (int *) shmat(mem_adult, NULL, 0);
  int mem_child = shmget(ftok("proj2.c",5), sizeof(int),S_IRUSR | S_IWUSR); //count of children in centre
  int *child_cnt = (int *) shmat(mem_child, NULL, 0);
  int mem_wait = shmget(ftok("proj2.c",6), sizeof(int),S_IRUSR | S_IWUSR); //count of children waiting to enter centre
  int *waiting = (int *) shmat(mem_wait, NULL, 0);

  //opening semaphores
  sem_t *semaphore1 = sem_open("/semaphore1",0);
  sem_t *semaphore2 = sem_open("/semaphore2",0);
  sem_t *semaphore3 = sem_open("/semaphore3",0);
  sem_t *semaphore4 = sem_open("/semaphore4",0);
  sem_t *semaphore5 = sem_open("/semaphore5",0);

  for (int i = 0; i < A; i++) {         //main cycle generating A adult processes

    if (AGT != 0) {
      int AGTr = rand() % AGT;        //calculates random value for timer
      usleep(AGTr * 1000);            //sleep for x microseconds(milisecods * 1000)
    }

    int adult_id = fork();        // creates child ADULT process

    if (adult_id == -1) {         //if fork fails
      fprintf(stderr,"Error: Fork Failed !\n");
      return -1;
    }
    else if (adult_id == 0) {   //child code

      sem_wait(semaphore1);           //I/O semaphore
        *adult_cnt = *adult_cnt + 1;  //count of adults in centre
        *id = *id + 1;
        int idA = *id;                //id of process in output
        *cnt = write_log(0,*cnt,'A',idA,0,0);   //start
        *cnt = write_log(1,*cnt,'A',idA,0,0);   //enter
        for (int i = 0; i < 3; i++) {       //allow 3 childs to enter
          sem_post(semaphore2);
        }

        int can_leave = (*adult_cnt * 3 - *child_cnt) / 3 ; //determines whether adult can leave (if there is enough adults compared to children)
        int sem;
        sem_getvalue(semaphore4,&sem);
        if (sem > can_leave) {              //sets value for "leave" semaphore
          sem_wait(semaphore4);             //disabling semaphore
        }
        else if (sem < can_leave) {
          sem_post(semaphore4);           //enabling semaphore
        }
      sem_post(semaphore1);

      if (AWT != 0) {
        int AWTr = rand() % AWT;    //random value for AWT
        usleep(AWTr * 1000);          //sleep for x microseconds(milisecods * 1000)
      }
      sem_wait(semaphore1);
        if (*waiting > 0) {       //makes process wait until all waiting children enter
          sem_post(semaphore1);
            sem_wait(semaphore5);   //ultimate solution to prevent adults from getting locked
            sem_post(semaphore5);
          sem_wait(semaphore1);
        }
      sem_post(semaphore1);

      sem_wait(semaphore1);
        *cnt = write_log(2,*cnt,'A',idA,0,0);   //trying to leave
        if (*child_cnt > 3*(*adult_cnt - 1)) { //checks if adult can leave, skips if can leave immediately
          *cnt = write_log(5,*cnt,'A',idA,*adult_cnt,*child_cnt);   //waiting
          sem_post(semaphore1);
            sem_wait(semaphore4);             //waiting until adult enters or child leaves
          sem_wait(semaphore1);
          if (*waiting > 0) {             //makes process wait until all waiting children enter
            sem_post(semaphore1);
              sem_wait(semaphore5);       //ultimate solution to prevent adults from getting locked
              sem_post(semaphore5);
            sem_wait(semaphore1);
          }
          if (*child_cnt > 3*(*adult_cnt - 1)) {      //repeats to prevent errors
            if (*waiting > 0) {             //makes process wait until all waiting children enter
              sem_post(semaphore1);
                sem_wait(semaphore5);         //ultimate solution to prevent adults from getting locked
                sem_post(semaphore5);
              sem_wait(semaphore1);
            }
            sem_post(semaphore1);
              sem_wait(semaphore4);             //waiting until adult enters or child leaves
            sem_wait(semaphore1);
          }
        }
        *adult_cnt = *adult_cnt - 1;      //decrease count of adults in centre

        //again decides if another adult can leave centre
        can_leave = (*adult_cnt * 3 - *child_cnt) / 3;
        sem_getvalue(semaphore4,&sem);
        if (sem > can_leave) {
          sem_wait(semaphore4);
        }
        else if (sem < can_leave) {
          sem_post(semaphore4);
        }

        *cnt = write_log(3,*cnt,'A',idA,0,0); //leave

        if ((*id == A) && (*adult_cnt == 0)) {                  //if last adult leaves let processes enter freely
          *id = *id + 1;
        }

        for (int i = 0; i < 3; i++) {
          sem_wait(semaphore2);       //decreases "available" space in centre
        }
      sem_post(semaphore1);

      sem_wait(semaphore1);
        *proc = *proc + 1;      //increases count of processes waiting to finish


        if (*proc == (A+C) ) {      //if all processes are marked as done, lets them finish
          for (int i = 0; i < A+C; i++) {
            sem_post(semaphore3);
          }
        }

      sem_post(semaphore1);

      sem_wait(semaphore3);       //waits for other processes to finish

      sem_wait(semaphore1);
      *cnt = write_log(4,*cnt,'A',idA,0,0); //finish
      sem_post(semaphore1);

      return 1;         //returns 1 on succes
    }
    else{
    }
  } //end of main for cycle

  int wpid;
  while ((wpid = wait(NULL)) > 0);  //waits for all child processes to finish
  //detaches all linked memory
  shmdt(cnt);
  shmdt(id);
  shmdt(proc);
  shmdt(adult_cnt);
  shmdt(child_cnt);
  shmdt(waiting);
  return 0;   //returns 0 on success
}

//creates child processes according to parameters given
int child_ctor(int C, int CGT, int CWT, int A)
{
  //shared memory initialization
  int mem_id = shmget(ftok("proj2.c",0), sizeof(int),S_IRUSR | S_IWUSR);    //counter
  int *cnt = (int *) shmat(mem_id, NULL, 0);
  int mem2_id = shmget(ftok("proj2.c",2), sizeof(int),S_IRUSR | S_IWUSR);    //process ID
  int *id = (int *) shmat(mem2_id, NULL, 0);
  int mem_proc = shmget(ftok("proj2.c",3), sizeof(int),S_IRUSR | S_IWUSR);    //"done" processes
  int *proc = (int *) shmat(mem_proc, NULL, 0);
  int mem_adult = shmget(ftok("proj2.c",4), sizeof(int),S_IRUSR | S_IWUSR);   //count of adults in centre
  int *adult_cnt = (int *) shmat(mem_adult, NULL, 0);
  int mem_child = shmget(ftok("proj2.c",5), sizeof(int),S_IRUSR | S_IWUSR);   //count of children in centre
  int *child_cnt = (int *) shmat(mem_child, NULL, 0);
  int mem1_id = shmget(ftok("proj2.c",1), sizeof(int),S_IRUSR | S_IWUSR);     //adult process ID's
  int *adult_id = (int *) shmat(mem1_id, NULL, 0);
  int mem_wait = shmget(ftok("proj2.c",6), sizeof(int),S_IRUSR | S_IWUSR); //count of children waiting to enter centre
  int *waiting = (int *) shmat(mem_wait, NULL, 0);

  //opening semaphores
  sem_t *semaphore1 = sem_open("/semaphore1",0);
  sem_t *semaphore2 = sem_open("/semaphore2",0);
  sem_t *semaphore3 = sem_open("/semaphore3",0);
  sem_t *semaphore4 = sem_open("/semaphore4",0);
  sem_t *semaphore5 = sem_open("/semaphore5",0);

  for (int i = 0; i < C; i++) {       //main cycle generating C child processes

    if (CGT != 0) {
      int CGTr = rand() % CGT;        //calculates random value for timer
      usleep(CGTr * 1000);          //sleep for x microseconds(milisecods * 1000)
    }

    int child_id = fork();          //creates child process

    if (child_id == -1) {         //fork failure
      fprintf(stderr,"Error: Fork Failed !\n");
      return -1;
    }
    else if (child_id == 0) {     //child code

      sem_wait(semaphore1);
      *id = *id + 1;
      int idC = *id;          //child ID
      *cnt = write_log(0,*cnt,'C',idC,0,0); //start
      sem_post(semaphore1);

      sem_wait(semaphore1);
        if (*adult_id != A + 1) {   //if adult process ID = desired count of adults => no more adults will be generated => no entry condition in centre
          if (((*adult_cnt * 3) <= *child_cnt) || (*waiting > 0)) {
            *waiting = *waiting + 1;  //increase count fo children waiting to enter
            if (*waiting <= 1) {      //if no one was waiting block semaphore
              sem_wait(semaphore5);
            }
            *cnt = write_log(5,*cnt,'C',idC,*adult_cnt,*child_cnt); //waiting
            sem_post(semaphore1);
            sem_wait(semaphore2); //waits till child leaves or adult enters the centre
            sem_wait(semaphore1);
            if (*waiting <= 1) {    //if no one is waiting open semaphore
                sem_post(semaphore5);
            }
            *waiting = *waiting - 1; //decrease count fo children waiting to enter
          }
          else {
          sem_wait(semaphore2); //waits till child leaves or adult enters the centre
         }
      }

        *child_cnt = *child_cnt + 1;      //increase child count
        *cnt = write_log(1,*cnt,'C',idC,0,0); //enter

        int can_leave;
        int sem;

        //determines if adult can leave
        if (*adult_id != A + 1) {           //if all adults left does not calculate
          can_leave = (*adult_cnt * 3 - *child_cnt) / 3 ;
          sem_getvalue(semaphore4,&sem);
          if (sem > can_leave) {
            sem_wait(semaphore4);         //block semaphore (adult is not allowed to leave)
          }
          else if (sem < can_leave) {
            sem_post(semaphore4);         //open semaphore (adult is free to leave)
          }
        }
      sem_post(semaphore1);

      if (CWT != 0) {
        int CWTr = rand() % CWT;      //random CWT value
        usleep(CWTr * 1000);          //sleep for x microseconds(milisecods * 1000)
      }

      sem_wait(semaphore1);
        *cnt = write_log(2,*cnt,'C',idC,0,0);   //trying to leave
        *cnt = write_log(3,*cnt,'C',idC,0,0);   //leave
        *child_cnt = *child_cnt - 1;        //decrease count of children in centre
        sem_post(semaphore2);

        //determines if adult can leave
        if (*adult_id != A + 1) {           //if all adults left does not calculate
          can_leave = (*adult_cnt * 3 - *child_cnt) / 3 ;
          sem_getvalue(semaphore4,&sem);
          sem_post(semaphore1);
          if (sem > can_leave) {      //checks if semaphore value needs to be changed
            sem_wait(semaphore4);       //close semaphore (adults are not allowed to leave)
          }
          else if (sem < can_leave) {   //checks if semaphore value needs to be changed
            sem_post(semaphore4);       //open semaphore (adults are allowed to leave)
          }
          sem_wait(semaphore1);

       }
       sem_post(semaphore1);

      sem_wait(semaphore1);
      *proc = *proc + 1;    //increases count of "done" processes
      if (*proc == A + C ) {
        for (int i = 0; i < A+C; i++) {     //waits for other processes to be marked as "done"
          sem_post(semaphore3);
        }
      }
      sem_post(semaphore1);

      sem_wait(semaphore3);           //waits for other processes to finish

      sem_wait(semaphore1);
      *cnt = write_log(4,*cnt,'C',idC,0,0);     //finish
      sem_post(semaphore1);
      return 1;
    }
    else {
    }

  } //end of main for cycle
  int wpid;
  while ((wpid = wait(NULL)) > 0);    //wait for all children to finish
  //detach all shared memory
  shmdt(proc);
  shmdt(cnt);
  shmdt(id);
  shmdt(adult_id);
  shmdt(adult_cnt);
  shmdt(child_cnt);
  shmdt(waiting);
  return 0;
}

//creates two processes responsible for creating ADULT and CHILD processes
int init_func(int A, int AGT, int AWT, int C, int CGT, int CWT)
{

  int adult_ctor_id = fork();   //adult creator process
  if (adult_ctor_id == -1) {
    fprintf(stderr,"%s\n","Error : Creating adult initialization process failed !");
    return -1;
  }
  else if (adult_ctor_id == 0) {    //child code
    adult_ctor(A,AGT,AWT,C);  //adult creator function
    return 1;
  }
  else {
    int child_ctor_id = fork(); //child creator process
    if (child_ctor_id == -1) {
      fprintf(stderr,"%s\n","Error : Creating child initialization process failed !");
      return -1;
    }
    else if (child_ctor_id == 0) {  //child code
      child_ctor(C,CGT,CWT,A);  //child creator function
      return 1;
    }
    else{
      int wpid;
      while ((wpid = wait(NULL)) > 0);  //waits for both children processes
      return 0;             //returns 0 on success
    }
  }
}

//frees all allocated shared memory
int free_resources()
{
  //shared memory freeing
  int mem_id = shmget(ftok("proj2.c",0), sizeof(int),S_IRUSR | S_IWUSR);
  if (shmctl(mem_id,IPC_RMID,NULL) == -1) {
    fprintf(stderr,"%s\n","Error : Freeing allocated memory failed !");
    return -1;
  }

  int mem_idA = shmget(ftok("proj2.c",1), sizeof(int),S_IRUSR | S_IWUSR);
  if (shmctl(mem_idA,IPC_RMID,NULL) == -1) {
    fprintf(stderr,"%s\n","Error : Freeing allocated memory failed !");
    return -1;
  }

  int mem_idC = shmget(ftok("proj2.c",2), sizeof(int),S_IRUSR | S_IWUSR);
  if (shmctl(mem_idC,IPC_RMID,NULL) == -1) {
    fprintf(stderr,"%s\n","Error : Freeing allocated memory failed !");
    return -1;
  }

  int mem_proc = shmget(ftok("proj2.c",3), sizeof(int),S_IRUSR | S_IWUSR);
  if (shmctl(mem_proc,IPC_RMID,NULL) == -1) {
    fprintf(stderr,"%s\n","Error : Freeing allocated memory failed !");
    return -1;
  }

  int mem = shmget(ftok("proj2.c",4), sizeof(int),S_IRUSR | S_IWUSR);
  if (shmctl(mem,IPC_RMID,NULL) == -1) {
    fprintf(stderr,"%s\n","Error : Freeing allocated memory failed !");
    return -1;
  }

  mem = shmget(ftok("proj2.c",5), sizeof(int),S_IRUSR | S_IWUSR);
  if (shmctl(mem,IPC_RMID,NULL) == -1) {
    fprintf(stderr,"%s\n","Error : Freeing allocated memory failed !");
    return -1;
  }

  mem = shmget(ftok("proj2.c",6), sizeof(int),S_IRUSR | S_IWUSR);
  if (shmctl(mem,IPC_RMID,NULL) == -1) {
    fprintf(stderr,"%s\n","Error : Freeing allocated memory failed !");
    return -1;
  }

  //unlinking semaphores
  sem_t *semaphore1 = sem_open("/semaphore1",0);
  sem_close(semaphore1);
  sem_unlink("/semaphore1");

  sem_t *semaphore2 = sem_open("/semaphore2",0);
  sem_close(semaphore2);
  sem_unlink("/semaphore2");

  sem_t *semaphore3 = sem_open("/semaphore3",0);
  sem_close(semaphore3);
  sem_unlink("/semaphore3");

  sem_t *semaphore4 = sem_open("/semaphore4",0);
  sem_close(semaphore4);
  sem_unlink("/semaphore4");

  sem_t *semaphore5 = sem_open("/semaphore5",0);
  sem_close(semaphore5);
  sem_unlink("/semaphore5");

  return 0;
}

int main(int argc, char *argv[]) {

  if (argc != 7){
    fprintf(stderr, "Error: Incorrect start arguments !\n"
                    "       ./proj2 A C AGT CGT AWT CWT\n"
                    "       A => count of adults\n"
                    "       C => count of children\n"
                    "       AGT => max wait time before adult process is generated\n"
                    "       CGT => max wait time before child process is generated\n"
                    "       AWT => max time adult spends in centre before trying to leave\n"
                    "       CWT => max time child spends in cetre\n");
      return 1;
  };

  int A,C, AGT, CGT, AWT, CWT;

  //associates variables with input arguments and checks range
  A = get_number(argv[1]);
  if (A <= 0){
    fprintf(stderr,"Error: Argument A must be an integer greater than 0!\n");
    return 1;
  };

  C = get_number(argv[2]);
  if (C <= 0){
    fprintf(stderr, "Error: Argument C must be an integer greater than 0!\n");
    return 1;
  };

  AGT = get_number(argv[3]);
  if ((AGT < 0) || (AGT > 5000)){
    fprintf(stderr, "Error: Argument AGT must be an integer from interval <0;5000>!\n");
    return 1;
  };

  CGT = get_number(argv[4]);
  if ((CGT < 0) || (CGT > 5000)){
    fprintf(stderr, "Error: Argument CGT must be an integer from interval <0;5000>!\n");
    return 1;
  };

  AWT = get_number(argv[5]);
  if ((AWT < 0) || (AWT > 5000)){
    fprintf(stderr, "Error: Argument AWT must be an integer from interval <0;5000>!\n");
    return 1;
  };

  CWT = get_number(argv[6]);
  if ((CWT < 0) || (CWT > 5000)){
    fprintf(stderr, "Error: Argument CWT must be an integer from interval <0;5000>!\n");
    return 1;
  };

  //"clears" output file
  FILE *log = fopen("proj2.out","w+");  //output file
  if (log == NULL) {              //error message
    fprintf(stderr, "Error : Opening file \"proj2.out\" failed !\n");
    return 2;
  }
  int close;
  if ((close = fclose(log)) == EOF) {
    fprintf(stderr, "Error : Closing file \"proj2.out\" failed !\n");
    free_resources();
    return 2;
  }

  if (init_memory() == -1) {    //shared memory initialization
    free_resources();
    return 2;
  }

  if (init_semaphores() == -1){   //semaphore initialization
    free_resources();
    return 2;
  }

  int init = init_func(A,AGT,AWT,C,CGT,CWT);
  if (init == -1) {
    free_resources();
    return 2;
  }
  else if (init == 0) {     //init==0 in main process ! ;prevents freeing resources by ancestors before others finish
    if (free_resources() == -1) { //error message
      return 2;
    }
  }
  return 0;
}
