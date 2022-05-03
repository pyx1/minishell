#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "parser.h" //Libreria para estructurar las ordenes
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include "simplelinkedlist.h"
#define MAX 1024

pid_t currentChild;
char currentLine[MAX];
TLinkedList jobsList;
void prompt()
{
    printf("msh>"); // Linea que se repite en continuo
}

void haltProccess()
{

    if (currentChild != 0)
    {

        TElemento e;
        crear(currentChild, currentLine, 1, &e);
        insertarFinal(e, &jobsList);
        kill(e.pid, SIGSTOP);
    }
}

void fgMsh()
{
    if (longitud(&jobsList) > 0)
    {
        TElemento e;
        ultimo(&jobsList, &e);
        strcpy(currentLine, e.nombre);
        currentChild = e.pid;
        kill(e.pid, SIGCONT);
        eliminar(e, &jobsList);
        waitpid(e.pid, NULL, WUNTRACED);
    }
    else
        fprintf(stderr, "No hay procesos en segundo plano\n");
}

void fgMsh2(int pos)
{
    if (longitud(&jobsList) > 0)
    {
        TElemento e;
        obtenerBusquedaIndice(&jobsList, pos, &e);
        strcpy(currentLine, e.nombre);
        currentChild = e.pid;
        kill(e.pid, SIGCONT);
        eliminar(e, &jobsList);
        waitpid(e.pid, NULL, WUNTRACED);
    }
    else
        fprintf(stderr, "No hay procesos en segundo plano\n");
}

void jobsMsh()
{
    int longlist = longitud(&jobsList);
    if (longlist > 0)
    {
        TElemento ult;
        ultimo(&jobsList, &ult);
        TElemento e;
        for (int i = 0; i < longlist; i++)
        {
            obtenerBusquedaIndice(&jobsList, i, &e);
            printf("[%d]%c %s\t\t%s\n", i + 1, igual(e, ult) == 1 ? '+' : ' ', e.state == 1 ? "Stopped" : "Running", e.nombre);
        }
    }
    else
        fprintf(stderr, "No hay procesos en jobs\n");
}

void changeSignals(tline *line)
{
    //Reactivamos las señales
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}

//Ejecuta un proceso
void launchSingleCommand(tline *line)
{
    pid_t pid;
    int status;
    pid = fork();
    currentChild = pid;
    for (int i = 0; i < line->commands[0].argc; i++)
    {
        strcat(currentLine, line->commands[0].argv[i]);
        strcat(currentLine, " ");
    }
    if (pid < 0)
    {
        fprintf(stderr, "Se ha producido un error en la creacion del proceso.\n");
    }
    else if (pid == 0)
    {
        changeSignals(line);
        execvp(line->commands[0].argv[0], line->commands[0].argv);
        //Codigo alcanzable solo si sucede un error. Execvp sustituira el proceso de otro modo.
        fprintf(stderr, "%s: No se encuentra el mandato.\n", line->commands[0].argv[0]);
        exit(1);
    }
    else
    {
        if (line->background == 0)
        {
            waitpid(pid, &status, WUNTRACED);
            if (status != 0)
            {
                if (WIFSTOPPED(status))
                    fprintf(stdout, "Se ha detenido el proceso\n");
                else if (WIFEXITED(status) == 0)
                {
                    fprintf(stderr, "Se ha cerrado el proceso\n");
                }
                else
                    fprintf(stderr, "Se ha producido un error en la ejecución.\n");
            }
        }
        else
        {
            TElemento e;
            crear(currentChild, currentLine, 0, &e);
            insertarFinal(e, &jobsList);
        }
    }
}

//Ejecuta varios procesos
void launchPipeCommand(tline *line)
{
    int n = line->ncommands;
    int pipes[n - 1][2]; //Un pipes menos que mandatos (3 mandatos se conectan con 2 pipes). Cada pipes entrada y salida. La entrada sera el 0 y la salida el 1
    pipe(pipes[0]);      //Creamos el primer pipes. Siempre habra por lo menos uno

    /* Primer hijo */
    int pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "Se ha producido un error en la creacion del proceso.\n");
    }
    else if (pid == 0)
    {
        changeSignals(line);
        close(pipes[0][0]);   //Cerramos entrada este solo escribe
        dup2(pipes[0][1], 1); //Asginamos el pipes como salida estandar
        execvp(line->commands[0].argv[0], line->commands[0].argv);
        //Codigo alcanzable solo si sucede un error. Execvp sustituira el proceso de otro modo.
        fprintf(stderr, "%s: No se encuentra el mandato.\n", line->commands[0].argv[0]);
        exit(1);
    }
    /*Fin del primer hijo*/
    /*Resto de hijos*/
    if (n > 2)
    {
        for(int i=1; i<n-1;i++) pipe(pipes[i]);
        for (int i = 1; i < n - 1; i++)
        {
            pid = fork();
            
            if (pid < 0)
            {
                fprintf(stderr, "Se ha producido un error en la creacion del proceso. En el mandato numero %d del pipes.\n", i + 1);
            }
            else if (pid == 0)
            {
                //Falta bg
                changeSignals(line);
                close(pipes[i - 1][1]); //Cerramos la salida del anterior ya que solo va a leer
                close(pipes[i][0]);     //Cerramos la entrada
                for (int j = 0; j < n - 1; j++)
                {
                    if (j != i && j != (i - 1))
                    {
                        close(pipes[j][1]);
                        close(pipes[j][0]);
                    }
                }
                dup2(pipes[i - 1][0], 0); //Redireccionna la entrada del anterior mandato a este
                dup2(pipes[i][1], 1);     //Redirecciona la salida de este mandato al pipes solo si no es el ultimo mandato
                execvp(line->commands[i].argv[0], line->commands[i].argv);
                //Codigo alcanzable solo si sucede un error. Execvp sustituira el proceso de otro modo.
                fprintf(stderr, "%s: No se encuentra el mandato.\n", line->commands[i].argv[0]);
                exit(1);
            }
        }
    }
    /*Ultimo hijo. Redireccion salida diferente*/
    pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "Se ha producido un error en la creacion del proceso.\n");
    }
    else if (pid == 0)
    {
        changeSignals(line);
        close(pipes[n - 2][1]);   //Cerramos entrada del ultimo pipe
        for (int j=0; j<(n-2); j++) {
			close(pipes[j][1]);
			close(pipes[j][0]);
		}
        dup2(pipes[n - 2][0], 0); //Asignamos la entrada a la salida del ultimo pipe
        execvp(line->commands[n-1].argv[0], line->commands[n-1].argv);
        //Codigo alcanzable solo si sucede un error. Execvp sustituira el proceso de otro modo.
        fprintf(stderr, "%s: No se encuentra el mandato.\n", line->commands[0].argv[0]);
        exit(1);
    }
    /* Esta parte del codigo solo la alcanzara el padre */
    for (int c = 0; c < (n - 1); c++)
    {
        close(pipes[c][0]);
        close(pipes[c][1]);
        //Cerramos todos los pipes
    }
    for (int c = 0; c < n; c++)
    {
        wait(NULL);
        //Espera tantas veces como hijos tenga
    }
}

void exitMsh()
{
    exit(0);
}

void cdMsh(tline *line)
{
    if (line->commands[0].argc < 2)
        chdir(getenv("HOME")); //Cambia al directorio "HOME" si no hay argumentos.
    else if (line->commands[0].argc == 2)
    {
        if (opendir(line->commands[0].argv[1]))
            chdir(line->commands[0].argv[1]); //Comprobamos si el directorio existe tratando de abrirlo
        else
            fprintf(stderr, "Error: No se encuentra el directorio\n");
    }
    else
        fprintf(stderr, "¡Solo se admite un argumento!\n");
}

void zombieland()
{
    int status;
    pid_t pid = 0;
    do
    {
        pid = waitpid(-1, &status, WNOHANG);
        if (WIFEXITED(status) != 0 && pid > 0)
        {
            
            TElemento e = obtenerBusqueda(&jobsList, pid);
            
            if(igualPid(&e, pid) == 1){
                eliminar(e, &jobsList);
                printf("\nProceso terminado. PID: %d\n", pid);
            }
        }
    } while (pid > 0);
}

int main()
{
    int a = 1;
    tline *line; // Linea estructurada;
    crearVacia(&jobsList);
    do
    {
        prompt();
        /* Señales de teclado */
        signal(SIGINT, SIG_IGN);  //Desactivamos CTRL+C
        signal(SIGQUIT, SIG_IGN); /*Desactivamos CRTL+\*/
        signal(SIGTSTP, SIG_IGN); //Desactivamos CTRL+Z
        /* Pensabamos que habia que implementar CTRL+Z tambien, asi que es plenamente funcional */
        signal(SIGCHLD, zombieland);
        /* Fin de señales teclado */
        char comando[MAX];
        fgets(comando, MAX, stdin); // Leemos la entrada de usuario y la guardamos en comando
        line = tokenize(comando);   // Convertimos el tipo string al struct de la libreria 
        currentChild = -1;
        strcpy(currentLine, "");
        /* Redirecciones */
        //Almacenamos los descriptores de stdin, stdout, stderr
        int stdi = dup(0);
        int stdo = dup(1);
        int stder = dup(2);

        if (line->redirect_error != NULL)
        {
            int outer = open(line->redirect_error, O_CREAT | O_RDWR, 0666); //El ultimo numero añade permisos de lectura y escritura
            dup2(outer, 2);
            if (strcmp(strerror(errno), "No such file or directory") != 0)
                fprintf(stderr, "%s: Error. %s", line->redirect_error, strerror(errno)); //Comprobacion con el if, ya que si no existe el archivo pese a que lanza un error lo crea
        }
        if (line->redirect_output != NULL)
        {
            int outp = open(line->redirect_output, O_CREAT | O_RDWR, 0666);
            dup2(outp, 1);
            if (strcmp(strerror(errno), "No such file or directory") != 0)
                fprintf(stderr, "%s: Error. %s", line->redirect_output, strerror(errno)); //Comprobacion con el if, ya que si no existe el archivo pese a que lanza un error lo crea
        }
        if (line->redirect_input != NULL)
        {
            if (access(line->redirect_input, R_OK) != -1)
            { //Comprobar que tenemos permisos de lectura sobre el archivo
                int inp = open(line->redirect_input, O_RDONLY, 0666);
                dup2(inp, 0);
            }
            else
                fprintf(stderr, "%s: Error. %s", line->redirect_input, strerror(errno)); //Muestra el fallo
        }
        /*Fin redirecciones*/

        if (strlen(comando) > 1) //Comprueba que haya por lo menos un comando y si no sigue
        {
            signal(SIGTSTP, haltProccess);
            if (line->ncommands < 2)
            {
                if (strcmp(line->commands[0].argv[0], "exit") == 0)
                    exitMsh(); //Cierra la minishell
                else if (strcmp(line->commands[0].argv[0], "cd") == 0)
                    cdMsh(line); //cd
                else if (strcmp(line->commands[0].argv[0], "jobs") == 0)
                    jobsMsh(); //jobs
                else if (strcmp(line->commands[0].argv[0], "fg") == 0)
                {
                    if (line->commands[0].argc > 1)
                    {
                        int posi = line->commands[0].argv[1][0] - 48 - 1; //Conversion desde la tabla ASCII my otro menos porque el usuario ve desde el 1
                        fgMsh2(posi);                                     //fg sobre el inidice
                    }
                    else
                        fgMsh(); //fg preedeterminado
                }

                else
                    launchSingleCommand(line);
            }

            else
            {
                if (strcmp(line->commands[0].argv[0], "cd") != 0)
                { //Comprobacion de que el comando no sea cd
                    currentChild = fork();
                    /* Almacenamos el nombre del programa */
                    char strCommand[MAX] = "";
                    for (int h = 0; h < line->ncommands; h++)
                    {
                        for (int k = 0; k < line->commands[h].argc; k++)
                        {
                            strcat(strCommand, line->commands[h].argv[k]);
                            strcat(strCommand, " ");
                        }
                        if (h != line->ncommands - 1)
                            strcat(strCommand, "| ");
                    }
                    strcpy(currentLine, strCommand);
                    /* Hasta aqui */
                    if (currentChild == 0)
                    {

                        launchPipeCommand(line);
                        exit(0);
                    }
                    else
                    {
                        if (line->background == 0)
                            waitpid(currentChild, NULL, WUNTRACED);
                        else
                        {
                            TElemento e;
                            crear(currentChild, currentLine, 0, &e);
                            insertarFinal(e, &jobsList);
                        }
                    }
                }
            }
        }
        //Reestablecemos la salida estandar al finalizar el bucle
        dup2(stdi, 0);
        dup2(stdo, 1);
        dup2(stder, 2);

    } while (a == 1);
    return 0;
}
