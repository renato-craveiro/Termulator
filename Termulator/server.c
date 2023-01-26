#include "termulator.h"

/**
 * 
 * SERVER SIDE
 * 
 * Todo o programa está adaptado e tem variadíssimos bugs, pelo que não devem ser considerados erros de escrita,
 * confirmação de erros de leitura/abertura de pipes ou ficheiros e alguns outros bugs.
 * 
 * Se alguém quiser modificar e melhorar este pequeno projeto de revisão sinta-se à vontade.
 * 
 * Enorme abraço e bom estudo!
 * 
 * Renato (lol, ok / raoky / cenas fixes e porreiras man) 
*/

//USADO THREADS


//VARS GLOBAIS
/**Var. de controlo de tempo. Indica há quanto tempo o server se encontra em execução */
int timer;
/**Var. de controlo que indica se é suposto estar em execução (1) ou não (0)*/
int running;
/**string para guardar output de um comando. Não deveria ser global, mas coloquei aqui já nem me lembro porquê lol.*/
char comm[BUFFERMAX];
/**FIFO Do servidor. Bastava apenas chutar o define mas pronto às vezes dá jeito estar numa string. Global por preguiça de o definir nas funções*/
char * befifo = SRVFIFO; 
/**Mutex Locker do timer. VAR. GLOBAL = LOCK PARA QUE OUTRAS THREAD SIMULTÂNEAS NÃO ACEDAM SIMULTANEAMENTE À MESMA VAR. (unico mutex de exemplo lol)*/
pthread_mutex_t mlTimer;


//THREAD CODE
/**incrementa o timer a cada segundo*/
void *relogio(); 
/**lê e interpreta teclado*/
void *KB(); 
/**lê pipe de receção de dados e responde ao fifo cliente*/
void *recClt(); 


//AUX
/**imprime tempo e envia tempo ao cliente*/
void reminder(); 
//**elimina pipe, coloca running a 0 e sai (NÃO É O MAIS CORRETO SAIR DESTA MANEIRA SEM ESPERAR FIM DE THREADS NEM MUTEX NA VAR RUNNING)*/
void sair(); 
/**executa algo passado pela string prog (literalmente faz um fork e execlp do prog: pode não executar tudo à primeira, aconselho um ls ou um tree, apenas para dar algum output)*/
int execComand(char *prog); 




int main(int argc, char*agrv[]){
    running=1; //em exec.

    //Not sure se os handlers se fazem assim mas resulta
    struct sigaction actSair; 
    struct sigaction actReminder;

    memset(&actSair, 0, sizeof(actSair));
    actSair.sa_handler = sair;//definimos a func. sair para responder ao SIGINT (CTRL+C)

    memset(&actReminder, 0, sizeof(actReminder));
    actReminder.sa_handler = reminder;//e para responder ao SIGALRM

    //efetivamos os sigations
    sigaction(SIGINT,  &actSair, 0); 
    sigaction(SIGALRM, &actReminder, 0);

    alarm(10);

    if(argc<2){
        printf("Admin missing!\n");
        exit(-1);
    }
    if(argc>2){
        printf("Too many arguments!\n");
        exit(-1);
    }

    printIntro();
    printf("Welcome Admin!\n");
    setbuf(stdout, NULL);

    pthread_t thrRel, thrKB, thrClt; //3 threads: relogio(timer), keyboard e receção de clts
	int iretRel, iretKB, iretClt; //para obter os returns das threads (caso erro)

    //iniciar as threads	
    iretRel = pthread_create( &thrRel, NULL, relogio, NULL); 
    iretKB = pthread_create( &thrKB, NULL, KB, NULL);
    iretClt = pthread_create( &thrClt, NULL, recClt, NULL);

    //e receciona-las quando acabarem
    pthread_join( thrKB, NULL);
    pthread_join( thrRel, NULL);
    pthread_join( thrClt, NULL);
	
    //receber o return values delas (para error handling - se desejável)
	printf("ThrTempo retorna: %d\n\n",iretRel);
    printf("ThrKB retorna: %d\n\n",iretKB);
    printf("ThrPipe retorna: %d\n\n",iretClt);

    //saímos do programa em "safe mode" (lol)
    sair();
    //não é suposto aqui chegar 
    exit(-1);
}

void *KB(){
    //Nada de muito interessante: literalmente lê cenas do teclado e vê o que se faz com elas

    char buff[BUFFERMAX];
    int l_timer;

    while(running==1){
        printf("Texto: ");
        scanf("%[^\n]", buff);
        while (getchar() != '\n')
            continue;
        if(!strcmp(buff,"sair")){
            printf("Adeus\n");
            running=0; //eu sei: devia ter um mutex_lock, mas já temos um exemplo!
            break;
        }
        if(!strcmp(buff,"tempo")){
            pthread_mutex_lock(&mlTimer);
            l_timer=timer;
            pthread_mutex_unlock(&mlTimer);
            printf("Tempo = %d\n",l_timer);
            continue;
        }
        execComand(buff);

        setbuf(stdout, NULL);
    }
}

void *relogio(){
    //incremento de timer: como se vê mutex_lock para ninguem mais mexer no timer. 

    while(running==1){
        sleep(1);
        pthread_mutex_lock(&mlTimer);
        timer++;
        pthread_mutex_unlock(&mlTimer);
    }

}

void sair(){
    running=0; //O que estiver a verificar esta variável em while: sai


    printf("EXITTING...\n\n");
    unlink(befifo);
    exit(0);
}

void reminder(){
    //print do timer. var. local para não gastar muito tempo em lock. (pois para aceder à var. é bloqueada para certificar que mais ninguem mexe)
    int l_timer;
    int fdr;
    char rsp[BUFFERMAX];

    pthread_mutex_lock(&mlTimer);
    l_timer=timer;
    pthread_mutex_unlock(&mlTimer);

    printf("\n--TEMPO:%d\n\n", l_timer);
    sprintf(rsp,"\n--TEMPO: %d--\n\n", l_timer);
    fdr=open(CLTFIFO, O_WRONLY);
    write(fdr, &rsp , sizeof(rsp));
    close(fdr);
    alarm(10);
}

int execComand(char *prog){
    //fork para podermos execuar o comando à vontade e ele poder morrer à vontade também. Pipe anónimo para obter o resultado do comando (ou erro)

    int pf[2];
    pid_t p, f;
    char rsp[BUFFERMAX];

    char buff[BUFFERMAX];

    printf("PATH: %s\n\n", prog); //literalmente para saber se passou bem a string. sou meio piquinhas com isso lol

    if (pipe(pf) != 0)
    {
        fprintf(stderr, "Pipe Failed");
        return -1;
    }

    f = fork();// --<
    if (f < 0) //ERRO
    {
        printf("SON FORK ERROR \n");
        return -1;
    }
    if (f > 0) //PAI
    {

        close(pf[1]); //APENAS FECHA A PARTE DE ESCRITA DO PIPE POIS NÃO VAI SER USADA
        if(read(pf[0], buff, BUFFERMAX)>0){ //lê o que está escrito no pipe
            printf("Recebi %s\n", buff);
            strcpy(comm, buff); //comm vai ser enviado ao cliente: eventualmente (é suposto enviar só quando ele pede este exec)
        }else{
            strcpy(comm,"FAILED\n");
        }
    }
    if (f == 0) //FILHO
    {
        close(pf[0]); //FECHA A PARTE DE LEITURA DO PIPE
        close(1); //FECHA O STDOUT
        dup(pf[1]); //PARA COLOCAR A PARTE DE ESCRITA NO MESMO
        close(pf[1]); //E FECHA O DUPLICADO
        execlp(prog, prog, (char *)NULL); 
        printf("PROGRAMA NAO ENCONTRADO!\n"); //NAO ENCONTROU O PROGRAMA
        exit(-1);
    }
    return -1;
}


void *recClt(){
    //recebe o dados do cliente no FIFO de SERVER, interpreta e responde. Acho que depois de ler as func. anteriores se percebe o que está aqui

    char cltAsk[BUFFERMAX];
    char rsp[BUFFERMAX];
    int l_timer;
    int fd, fdr;
    char * fiforesp=CLTFIFO;
    int pid;

	char buf[BUFFERMAX];
	int bytes=1;
    int errorType;
    mkfifo(befifo,0666);
	fd = open(befifo, O_RDONLY);

    while(running==1){
		bytes = read(fd, &cltAsk, sizeof(cltAsk));

        
		if(bytes==-1){
			printf("SEM FICHEIRO DE RECEÇÃO DE USERS!\n\n");
			break;
		}
        if(bytes==0){
            continue;
        }
        if(!strcmp(cltAsk,"tempo")){
            pthread_mutex_lock(&mlTimer);
            l_timer=timer;
            pthread_mutex_unlock(&mlTimer);

            sprintf(rsp,"Tempo %d",l_timer);


        }else{
            execComand(cltAsk);
            strcpy(rsp,comm);
        }
        fdr=open(fiforesp, O_WRONLY);
        write(fdr, &rsp , sizeof(rsp));
        close(fdr);
    }

}

