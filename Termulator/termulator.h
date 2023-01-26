/*Desenvolvido por Renato Craveiro (2018011392) - LEI-PL - Ano Letivo 2022/2023 - Sistemas Operativos*/

//Includes necessários à execução/compilação do programa
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

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



//DEFAULTS
//Tamanho máximo de buffers, por defeito
#define BUFFERMAX 1024


//Definições dos FIFOS (por defeito) do Cliente e Servidor

#define SRVFIFO "/tmp/srv_fifo" //NOME DEFAULT DO FIFO ONDE SERVIDOR RECEBE INFO E PARA ONDE CLIENTE ENVIA 
#define CLTFIFO "/tmp/clt_fifo" //NOME DEFAULT DO FIFO ONDE CLIENTE RECEBE INFO E PARA ONDE SERVIDOR ENVIA


void printIntro(); //Imprime a introdução do software (lol)
