#include "termulator.h"

/**
 * Todo o programa está adaptado e tem variadíssimos bugs, pelo que não devem ser considerados erros de escrita,
 * confirmação de erros de leitura/abertura de pipes ou ficheiros e alguns outros bugs.
 * 
 * Se alguém quiser modificar e melhorar este pequeno projeto de revisão sinta-se à vontade.
 * 
 * Enorme abraço e bom estudo!
 * 
 * Renato (lol, ok / raoky / cenas fixes e porreiras man) 
*/

//USADO SELECT

char *myfifo = CLTFIFO; //FIFO DO CLIENTE

/**Elimina o pipe. Forma não muito simpática de matar o processo*/
void sair(){
    int fd;
    printf("EXITTING!\n\n");
    unlink(myfifo);
    exit(0);
}


/**Lê o queestá presente no fifo de cliente*/
void PipeRead(int fd){
    int bytes;
    char rsp[BUFFERMAX];
    //int fd;
    read(fd, &rsp, sizeof(rsp));
    printf("%s\n\n", rsp);
    setbuf(stdout, NULL);
    
}

/**Lẽ o que está a ser escrito em KB, interpreta e envia (ou não) para FIFO servidor*/
void KB(){
    char * srvfifo = SRVFIFO;
    int fd;
    char msg[BUFFERMAX];

    scanf("%[^\n]", msg);
    while (getchar() != '\n')
        continue;
    if(!strcmp(msg,"sair")){
        printf("Adeus\n");
        sair();
    }
    fd = open(srvfifo, O_WRONLY);
    write(fd, &msg, sizeof(msg));
    close(fd);

    strcpy(msg,"");
}



int main(int argc, char *argv[]){
    signal(SIGINT,sair); //associa função sair ao SIGINT ao CTRL+C
    char msg[BUFFERMAX];
    char rsp[BUFFERMAX];
    int nfd;
    fd_set read_fds;
    struct timeval tv;


    signal(SIGINT,sair);


    int fd;
    int i;
    char olaf[BUFFERMAX];
    mkfifo(myfifo,0666);
    fd = open(SRVFIFO, O_WRONLY); // é apenas para verificar se o server está em execução, verificando se existe o fifo do mesmo
    if(fd==-1){
        printf("BACKEND NÃO SE ENCONTRA EM EXECUÇÃO!!\n\n");
        exit(-1);
    }
    close(fd);

    fd = open(myfifo, O_RDWR | O_NONBLOCK); //deixa fifo cliente para escrita+leitura não bloqueante pois estará sempre à escuta

    setbuf(stdout, NULL);
    
    printIntro();
    printf("Bem Vindo Cliente\n\n");

    while(1){
        tv.tv_sec = 5; //define intervalo de espera de 5 seg. 
        tv.tv_usec = 0; //microsegundos caso desejável
        
        FD_ZERO( & read_fds); //limpa os read file descriptors
        FD_SET(0, & read_fds); //stdin
        FD_SET(fd, & read_fds); //fd fifo cliente
        
        nfd=select(fd+1, &read_fds, NULL, NULL, &tv); //sempre o mair file descriptor +1 (stdin)
        if(nfd==0){ //caso o timer acabe lembra o user que ainda tá à espera
            printf("\nÀ espera de input...\n");
            fflush(stdout);
            continue;
        }
        if(nfd==-1){ //caso exista algum erro: aborta
            printf("\nERRO EM NFD: ABORTAR...");
            sair();
        }
        if(FD_ISSET(0, &read_fds)){//atividade em KB? -> segue para KB
            KB();
        }
        if(FD_ISSET(fd, &read_fds)){//Atividade no FIFO? -> segue para PipeRead
            PipeRead(fd);
        }
        
        
        
    }

    close (fd);//fecha o file descriptor do fifo cliente


    
    unlink(myfifo);//elimina o fifo
    exit(-1);//NAO É SUPOSTO CHEGAR AQUI
}