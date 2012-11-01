#include <stdio.h>

#ifdef __WIN32__
	#include <windows.h>
#endif

#include <stdlib.h>
#include <strings.h>
//#include <winsock.h> //necessario para compilar junto com o mysql usando o codeblocks
#include <mysql/mysql.h> //biblioteca responsavel para conexao com o banco de dados

// just going to input the general details and not the port numbers
struct con_detalhes
{
    char *server;
    char *user;
    char *password;
    char *database;
};
//MYSQL* mysql_config(struct con_detalhes mysql_detalhes) 
MYSQL* mysql_config()
{
	//Constantes para conexao do banco de dados
    const char* server = "127.0.0.1";
    const char* user = "root";
    const char* password = "";//alterar aqui 
    const char* database = "pesquisa";
    // first of all create a mysql instance and initialize the variables within
    MYSQL *conn = mysql_init(NULL);
    
     
    // connect to the database with the details attached.
    //if (!mysql_real_connect(conn,mysql_detalhes.server, mysql_detalhes.user, mysql_detalhes.password, mysql_detalhes.database, 0, NULL, 0)) {
    if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
       printf("Erro de Conexao : %s\n", mysql_error(conn));
       exit(1);
    }
    return conn;
}

MYSQL_RES* mysql_sql_query(MYSQL *conn, char *query_sql)
{
   // enviando a query para o banco de dados
   if (mysql_query(conn, query_sql))
   {
      printf("Erro na MySQL query : %s\n", mysql_error(conn));
      exit(1);
   }
 
   return mysql_use_result(conn);
}
 

/*
 MYSQL *conn;		// the connection
  MYSQL_RES *res;	// the results
  MYSQL_ROW row;	// the results row (line by line)
 
  struct connection_details mysqlD;
  mysqlD.server = "localhost";  // where the mysql database is
  mysqlD.user = "mysqlusername";		// the root user of mysql	
  mysqlD.password = "mysqlpassword"; // the password of the root user in mysql
  mysqlD.database = "mysql";	// the databse to pick
 
  // connect to the mysql database
  conn = mysql_connection_setup(mysqlD);
 
  // assign the results return to the MYSQL_RES pointer
  res = mysql_perform_query(conn, "show tables");
 
  printf("MySQL Tables in mysql database:\n");
  while ((row = mysql_fetch_row(res)) !=NULL)
      printf("%s\n", row[0]);
 
  /* clean up the database result set */
//  mysql_free_result(res);
  /* clean up the database link */
//  mysql_close(conn);


/**
 * Funcao tempo
 *
 * funcao com o comando ping 127.0.0.1 -n 5 -w 3000 > null
 * executa um ping com um tempo de 3000 ms
 * da um tempo ao usuario para visualisar informacao na tela antes de proseguir para outro comando
 */
void tempo() {
    system("ping 127.0.0.1 -n 5 -w 2000 > nul");
}

void cadastro_pesquisa() 
{
	char nome[200];
	char query[255];
    MYSQL *conn;
    conn = mysql_config(); 
    printf("Digite o nome para a pesquisa: ");
	scanf(" %[^\n]", nome);
	/*
	 * Montando a query sql
	 *
	 * Insere dentro da tabela pesquisa nas colunas nome e data_cad os valores nome obtido no scanf acima e salvo no
     * vetor de caracteres nome e pega a data corrente do servidor de banco de dados
     * 
     */
	sprintf(query, "INSERT INTO pesquisa (nome, data_cad) VALUES ('%s', CURDATE())", nome);
	/*
	 * Passando para a funcao mysql_sql_query o endereco do ponteiro conn do tipo MYSQL
     * e a query sql montada acima
	 */
    mysql_sql_query(conn, query);
	printf("\nPesquisa Cadastrada com sucesso!\n");
	mysql_close(conn);//fecha a conexao aberta - liberando recursos
	tempo();//funcao que da um tempo para exibir a mensagem ao usuario
}

void imprimeMenu() {
    int opcao = -1;
    //while (opcao != 3) {
		#ifdef __WIN32__
	    	system("cls");
		#endif
		#ifdef __linux__
		    system("clear");
		#endif       

        printf(" ______________________________________________\n");
        printf("|                                              |\n");
        printf("|       Trabalho de LTP1 - C com MySql         |\n");
        printf("|       por Eduardo Junior e Mario Montino     |\n");
        printf("|______________________________________________|\n");
        printf("|             Escolha uma opcao                |\n");
        printf("|                                              |\n");
        printf("| 1: CADASTRAR PESQUISA                        |\n");
        printf("| 2: CADASTRAR CANDIDATOS                      |\n");
        printf("| 3: SAIR DO PROGRAMA                          |\n");
        printf("|______________________________________________|\n");
        scanf("%d", &opcao);
        switch(opcao) {
            case 1:
                opcao = 3;
                cadastro_pesquisa();
				imprimeMenu();
                break;
            case 2:
                printf("escolheu 2");
                break;
            case 3:
                printf("Saindo do programa - Adeus!\n");
                exit(0);
                break;
        }
    //}
}




void inserir() {
    

    
}



int main(int argc, char *argv[])
{
    imprimeMenu();
    getchar();
    return 0;
}

