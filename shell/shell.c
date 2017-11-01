#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX 256

int main() {
  char comando[MAX];
  int pid, count = 0, i = 0, splano = 0, flag = 0;
  const char s[2] = " ";
  char **tokens, *token, path[MAX];
  FILE *fp;

  while (1) {
    tokens = malloc(MAX * sizeof(char*));
    count = 0;
    printf("> ");
    fgets(comando, MAX, stdin);
    comando[strlen(comando)-1] = '\0';

    //Tratamento para verificar se existe um & no final da string
    if(comando[strlen(comando)-1] == '&'){
      splano = 1;
      comando[strlen(comando)-2] = '\0';
    }

    //Pegando apenas o comando
    token = strtok(comando, s);

    //Pegando o resto da entrada
    while(token != NULL){
      tokens[count] = token;
      count++;
      token = strtok(NULL, s);
    }

    if (!strcmp(comando, "exit")) {
      exit(EXIT_SUCCESS);
    }

    pid = fork();

    if (pid) {
      //Processo pai
      if(splano == 0){
        waitpid(pid, NULL, 0);
      }else{
        waitpid(pid, NULL, WNOHANG);
      }

    }else{
      //Processo filho
      //Essa flag será utilizada para que comandos não sejam executados de maneira errada
      flag = 0;
      //Tratamento para saber se existe um > ou um <
    for (i = 0; i < count; i++){
        if (strcmp(tokens[i],">") == 0){
          flag = 1;
          fp = popen(comando, "r");                 //Abrindo comando para ler o output da linha de comando
          if (fp == NULL){
            printf("Erro\n");
            exit(1);
          } else {
            FILE *aux = fopen(tokens[i+1],"w");     //Abre um arquivo auxiliar para escrever o output

            while (fgets(path,MAX,fp) != NULL)
              fprintf(aux,"%s", path);

            fclose(aux);
          }
        fclose(fp);                                  //Fecha fp utilizado para ler a linha de comando
        } else if (strcmp(tokens[i],"<") == 0){

          flag = 1;
          //printf("Nome do arquivo que sera aberto: %s\n", tokens[i+1]);
          FILE *aux = fopen(tokens[i+1], "r");
          char **tokens2, auxiliar[MAX];
          tokens2 = malloc(MAX * sizeof(char*));


          tokens2[0] = "a";
          tokens2[1] = "Pasta2";


          fclose(aux);

          //Até aqui, pegamos o conteudo do arquivo e colocamos na variavel auxiliar
          //puts(auxiliar);
          //printf("comando: %s\n", comando);


          execvp(comando,tokens2);
          printf("Erro ao executar comando!\n");
          exit(EXIT_FAILURE);
        }
    }
      if (flag == 0){
        //printf("Entrou!\n");
        execvp(comando, tokens);                    //Executa o comando
        free(tokens);                               //Libera o espaço armazenado para tokens
        printf("Erro ao executar comando!\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}
