#include <stdio.h>
#ifdef __WIN32__
	#include <windows.h>
#endif
#include <stdlib.h>
#include <strings.h>
#include <mysql/mysql.h> //biblioteca responsavel para conexao com o banco de dados
#include <time.h> //biblioteca para lidar com datas

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
 * funcao com o comando ping 127.0.0.1 -n 5 -w 2000 > null
 * executa um ping com um tempo de 2000 ms
 * da um tempo ao usuario para visualisar informacao na tela antes de proseguir para outro comando
 */
void tempo() 
{
	system("ping 127.0.0.1 -n 5 -w 2000 > nul");
}

void cadastro_pesquisa() 
{
	char *nome;
	char *query;
	char *cidade;
	char *estado;
	nome = (char*)malloc(255*sizeof(char));
	query = (char*)malloc(255*sizeof(char));
	cidade = (char*)malloc(255*sizeof(char));
	estado = (char*)malloc(255*sizeof(char));
    int ultimoid;
    MYSQL *conn;
    conn = mysql_config(); 
    printf("Digite o nome para a pesquisa: ");
	scanf(" %[^\n]", nome);
	printf("Digite a cidade da pesquisa: ");
	scanf(" %[^\n]", cidade);
	printf("Digite o nome do estado: ");
	scanf(" %[^\n]", estado);
	/*
	 * Montando a query sql
	 *
	 * Insere dentro da tabela pesquisa nas colunas nome e data_cad os valores nome, cidade, estado obtido no scanf acima e salvo no
     * ponteiro de caracteres nome, cidade, estado e pega a data corrente do servidor de banco de dados
     * 
     */
	sprintf(query, "INSERT INTO pesquisa (nome, data_cad, cidade, estado) VALUES ('%s', CURDATE(), '%s', '%s')", nome, cidade, estado);
	/**
	 * Liberando memoria dos ponteiros nome, cidade, estado
	 */
	 free(nome);
	 free(cidade);
	 free(estado);
    /*
	 * Passando para a funcao mysql_sql_query o endereco do ponteiro conn do tipo MYSQL
     * e a query sql montada acima
	 */
    mysql_sql_query(conn, query);
    //free(query);
    //query = (char*)malloc(255*sizeof(char));
	printf("\nPesquisa Cadastrada com sucesso!\n");
	printf("\nAguarde o programa retornar ao menu inicial!\n");
	/**
	 * Pega o Ultimo ID cadastrado na tabela pesquisa para ser usado na insercao de candidatos padroes na tabela
	 * candidatos 
     * Voto em Branco
     * Voto em nulo
     * Nao respondeu
     */
	ultimoid = mysql_insert_id(conn);
	/*
	 * opcao_padrao = 's' difere dos candidados normais que tem como opcao_padrao = 'n' 
     */
	sprintf(query, "INSERT INTO candidatos (nome, opcao_padrao, pesquisa_idpesquisa) VALUES ('Voto em Branco', 's', '%d')", ultimoid);
    mysql_sql_query(conn, query);// executa o cadastro da query acima
    sprintf(query, "INSERT INTO candidatos (nome, opcao_padrao, pesquisa_idpesquisa) VALUES ('Voto em Nulo', 's', '%d')", ultimoid);
	mysql_sql_query(conn, query);// executa o cadastro da query acima
    sprintf(query, "INSERT INTO candidatos (nome, opcao_padrao, pesquisa_idpesquisa) VALUES ('Não Respondeu', 's', '%d')", ultimoid);
    mysql_sql_query(conn, query);// executa o cadastro da query acima
    free(query);//liberando memoria do ponteiro query
	mysql_close(conn);//fecha a conexao aberta - liberando recursos
	tempo();//funcao que da um tempo para exibir a mensagem ao usuario
}

int seleciona_pesquisa()
{
	MYSQL *conn; // Ponteiro conn do tipo MYSQL
    MYSQL_RES *res; // Ponteiro res do tipo MYSQL_RES - recebe os resultados
    MYSQL_ROW row; // row do tipo MYSQL_ROW - resultados linha por linha
    conn = mysql_config(); //Configura e inicia conexao com o MYSQL
    int idpesquisa = 0;
    /*
     * seleciona todos as colunas da tabela pesquisa ordenado pela data_cad em ordem DESC e coloca no ponteiro res
     */
    res = mysql_sql_query(conn, "SELECT idpesquisa, nome, DATE_FORMAT(data_cad, '%d/%m/%Y') FROM pesquisa ORDER BY data_cad DESC");
    #ifdef __WIN32__
    	system("cls");
    #endif
    #ifdef __linux__
    	system("clean");
    #endif
    printf("PESQUISAS CADASTRADAS:\n\n");
    if (res) {
		printf("ID | DATA         | NOME\n");
       	while ((row = mysql_fetch_row(res)) !=NULL) { 
        	printf("%s  | %s   | %s\n", row[0], row[2], row[1]); //row[0] = idpesquisa, row[2] = data_cad, row[1] = nome
       	}
       	printf("Selecione a pesquisa que deseja cadastrar candidatos a partir do seu ID: ");
       	scanf("%d", &idpesquisa);
       	
		return idpesquisa;
    
	} else {
    	printf("Nenhuma pesquisa cadastrada!\n");
        tempo();
        
		return idpesquisa;
    
	}

	mysql_free_resulta(res);
	mysql_close(conn);

}

void cadastro_candidatos()
{
	int idpesquisa;
	char *nome;
    char opcao;
	int numero;
	MYSQL *conn;
	char *query;
    idpesquisa = seleciona_pesquisa();
	if (idpesquisa != 0) {
        opcao = 's';
        while ((opcao == 's') || (opcao == 'S')) {
            nome = (char*)malloc(255*sizeof(char)); //Allocando memoria dinamica
	        query = (char*)malloc(255*sizeof(char)); //Allocando memoria dinamica
		    printf("Digite o nome para o candidato: ");
		    scanf(" %[^\n]", nome);
		    printf("\nDigite o numero para o candidato: ");
		    scanf("%d", &numero);
		    conn = mysql_config();
		    sprintf(query, "INSERT INTO candidatos (nome, numero, pesquisa_idpesquisa) VALUES ('%s', '%d', '%d')", nome, numero, idpesquisa);
		    mysql_sql_query(conn, query);
		    mysql_close(conn);
		    printf("Candidato cadastrado com sucesso!\n");
		    printf("Deseja Cadastrar outro candidato. s\\n: ");
		    scanf("%s", &opcao);
		    #ifdef __WIN32__
                system("cls");
            #endif
            #ifdef __linux__
                system("clear");
            #endif
            /**
             * Liberando memoria dinamica
             */
            free(nome);
            free(query);
        }
        
		tempo();
		
	}
    
}



/**
 * PARA MARIO :D
 * Funcao void inicia_pesquisa
 *
 * Da inicio a pesquisa eleitoral exibindo na tela para o usuario escolher qual pesquisa deve se iniciar
 * 
 * PASSO A PASSO
 * 
 * 1 - Deve-se perguntar se o eleitor mora na cidade que foi cadastrada na tabela pesquisa
 * 2 - Perguntar se o eleitor deseja responder a pesquisa
 * 2.1 - Escolheu Sim
 * 2.1.1 - Pegar o nome do eleitor
 * 2.1.2 - Pegar Telefone do eleitor
 * 2.1.3 - Pegar o bairro do eleitor
 * 2.1.4 - Inserir na tabela eleitores coluna vota_cidade = 'S' se ele mora na cidade ou vota_cidade = 'N' se não mora
 *         resp_pesquisa = 'S', nome, telefone, bairro
 * 2.2 - Escolheu Nao
 * 2.2.1 -  Inserir na tabela eleitores coluna_vota_cidade = 'S' se ele mora na cidade ou vota_cidade = 'N' se não mora e 
 *          resp_pesquisa = 'N', e na tabela candidatos procurar pelo o id da opcao Não Respondeu de acordo com o id da
            respectiva pesquisa e incrementar a coluna qtdvotos
 * 2.3 - Exibir as opcoes para o eleitor escolher seu candidato menos a opcao "Não Respondeu"
 * 2.4 - Atualizar a coluna qtdvotos da opcao escolhida na tabela candidatos de acordo com o id da respectiva pesquisa
 *
 */
void inicia_pesquisa()
{
    int idpesquisa;
    idpesquisa = seleciona_pesquisa(); // Chama a funcao seleciona_pequisa e retorna o Inteiro com o ID da pesquisa
    if (idpesquisa != 0) {
        
    }
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
    printf("| 3: INICIA PESQUISA                           |\n");
    printf("| 5: SAIR DO PROGRAMA                          |\n");
    printf("|______________________________________________|\n");
    scanf("%d", &opcao);
    switch(opcao) {
    	case 1:
        	//opcao = 3;
            cadastro_pesquisa();
			imprimeMenu();
            break;
        case 2:
        	cadastro_candidatos();
            imprimeMenu();
            break;
        case 3:
            inicia_pesquisa();
            imprimeMenu();
            break;
        case 5:
        	printf("Saindo do programa - Adeus!\n");
            exit(0);
            break;
	}
    //}

}

int main(int argc, char *argv[])
{
	imprimeMenu();
    getchar();
    return 0;
}
