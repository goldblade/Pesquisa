#include <stdio.h>
#ifdef __WIN32__
	#include <windows.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h> //biblioteca responsavel para conexao com o banco de dados
#include <time.h> //biblioteca para lidar com datas

void imprimeMenu();//þrototipo da funcao imprimeMenu
void remover_pesquisa();//prototipo da funcao remover_pesquisa

MYSQL* mysql_config()
{
	//Constantes para conexao do banco de dados
    const char* server = "127.0.0.1";
    const char* user = "pesquisa";
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
	
	//return mysql_use_result(conn);
	return mysql_store_result(conn);

}
 
/**
 * Funcao tempo
 *
 * funcao com o comando ping 127.0.0.1 -n 5 -w 2000 > null
 * executa um ping com um tempo de 2000 ms
 * da um tempo ao usuario para visualisar informacao na tela antes de proseguir para outro comando
 */
void tempo() 
{
	#ifdef __WIN32__
		system("ping 127.0.0.1 -n 5 -w 2000 > nul");
	#endif
	#ifdef __linux__
		system("ping 127.0.0.1 -c 5 -w 2000 > nul");
	#endif
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
	int qtdrows;
    int idpesquisa = 0;
    /*
     * seleciona todos as colunas da tabela pesquisa ordenado pela data_cad em ordem DESC e coloca no ponteiro res
     */
    res = mysql_sql_query(conn, "SELECT idpesquisa, nome, DATE_FORMAT(data_cad, '%d/%m/%Y'), cidade, estado FROM pesquisa ORDER BY data_cad DESC");
    #ifdef __WIN32__
    	system("cls");
    #endif
    #ifdef __linux__
    	system("clear");
    #endif
    printf("PESQUISAS CADASTRADAS:\n\n");
	qtdrows = mysql_num_rows(res);

    if ((res) && (qtdrows > 0)) {
		printf("ID | DATA         | NOME - [CIDADE - ESTADO]  \n");
       	while ((row = mysql_fetch_row(res)) !=NULL) { 
			//row[0] = idpesquisa, row[2] = data_cad, row[1] = nome, row[3] = cidade, row[4] = estado
			printf("%s  | %s   | %s - [%s - %s]     \n", row[0], row[2], row[1], row[3], row[4]);        	
       	}
       	printf("\nSelecione uma pesquisa a partir do seu ID ou digite -1 para voltar ao menu principal: ");
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

void alterar_pesquisa()
{

    char *nome;
    char *cidade;
    char *estado;    
    char opcao;
    char query[1024];
    int idpesquisa;
   	MYSQL *conn; // Ponteiro conn do tipo MYSQL
    conn = mysql_config(); //Configura e inicia conexao com o MYSQL
    idpesquisa = seleciona_pesquisa();
    if ((idpesquisa != 0) && (idpesquisa != -1)) {
        
        

        printf("Deseja alterar o nome?: S/N ");
        scanf(" %[^\n]", &opcao);
        if ((opcao == 's') || (opcao == 'S')) {
			nome = (char*)malloc(255*sizeof(char));
        	printf("\nDigite um novo nome para a pesquisa: ");
            scanf(" %[^\n]", nome);
			sprintf(query, "UPDATE pesquisa SET nome='%s' WHERE idpesquisa = '%d'", nome, idpesquisa);
			mysql_sql_query(conn, query);
			free(nome);
        } 
        printf("\nDeseja alterar a cidade?: S/N ");
        scanf(" %[^\n]", &opcao);
        if ((opcao == 's') || (opcao == 'S')) {
			cidade = (char*)malloc(255*sizeof(char));
        	printf("\nDigite um novo novo nome para a cidade: ");
        	scanf(" %[^\n]", cidade);
			sprintf(query, "UPDATE pesquisa SET cidade='%s' WHERE idpesquisa = '%d'", cidade, idpesquisa);
			mysql_sql_query(conn, query);
			free(cidade);
        } 
        printf("\nDeseja alterar o estado?: S/N ");
        scanf(" %[^\n]", &opcao);
        if ((opcao == 's') || (opcao == 'S')) {
			estado = (char*)malloc(50*sizeof(char));
            printf("\nDigite um novo nome para o estado: ");
            scanf(" %[^\n]", estado); 
			sprintf(query, "UPDATE pesquisa SET estado='%s' WHERE idpesquisa = '%d'", estado, idpesquisa);
			mysql_sql_query(conn, query);
			free(estado);
        } 
        mysql_close(conn);//fecha conexao com o banco de dados       
    } else {
		imprimeMenu();
	}
}

void remover_pesquisa()
{
	int idpesquisa;
	MYSQL *conn;
	char *query;
	char opcao;
	conn = mysql_config(); //Configura e inicia conexao com o MYSQL 			
	idpesquisa = seleciona_pesquisa();
	if ((idpesquisa != 0) && (idpesquisa != -1)) {
		printf("Deseja realmente excluir essa pesquisa? S/N");
		scanf(" %[^\n]", &opcao);
		if ((opcao == 's') || (opcao == 'S')) {
			query = (char*)malloc(255*sizeof(char));//allocando memoria dinamica			
			sprintf(query, "DELETE FROM pesquisa WHERE idpesquisa = '%d' ", idpesquisa);
			mysql_sql_query(conn, query);
			mysql_close(conn);//fecha conexao com o banco de dados
			printf("\nREGISTRO REMOVIDO COM SUCESSO! AGUARDE O PROGRAMA RETONAR AO MENU PRINCIPAL\n");
			tempo();
		} else {
			printf("Operacao cancelada, retornando ao menu principal em alguns segundos. Aguarde!\n");
			tempo();			
		}
		
	} else {
		imprimeMenu();
	}
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
	if ((idpesquisa != 0) && (idpesquisa != -1)) {
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
        printf("\nAGUARDE O PROGRAMA RETONAR AO MENU PRINCIPAL\n");
		tempo();
		
	}
    
}


int seleciona_candidato() 
{

}


void alterar_candidato() 
{
	int idpesquisa;
	int idcandidato;
	idpesquisa = seleciona_pesquisa();
	if ((idpesquisa != 0) && (idpesquisa != -1)) {
		
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

void exibe_resultados()
{
    int idpesquisa;
    idpesquisa = seleciona_pesquisa();
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
    printf("| 2: ALTERAR PESQUISA                          |\n");
    printf("| 3: DELETAR PEQUISA                           |\n");
    printf("|______________________________________________|\n");
    printf("|                                              |\n");
    printf("| 4: CADASTRAR CANDIDATOS                      |\n");
    printf("| 5: ALTERAR CANDIDATOS                        |\n");
    printf("| 6: DELETAR CANDIDATOS                        |\n");
    printf("|______________________________________________|\n");
    printf("|                                              |\n");
    printf("| 7: INICIA PESQUISA                           |\n");
    printf("| 8: EXIBIR RESULTADOS                         |\n");
    printf("| 9: SAIR DO PROGRAMA                          |\n");
    printf("|______________________________________________|\n");
    scanf("%d", &opcao);
    switch(opcao) {
        case 1:
            //Cadastrar pequisa
            cadastro_pesquisa();
			imprimeMenu();
            break;
        case 2:
            //Alterar Pesquisa
            alterar_pesquisa();
            imprimeMenu();
            break;
		case 3:
			//Excluir Pesquisa
			remover_pesquisa();
			imprimeMenu();
			break;
        case 4:
            //Cadastrar Candidatos
        	cadastro_candidatos();
            imprimeMenu();
            break;
		case 5:
			//Alterar Candidato
			alterar_candidato();
			imprimeMenu();
			break;
        case 7:
            inicia_pesquisa();
            imprimeMenu();
            break;
        case 8:
            //exibir_resultados();
            imprimeMenu();
            break;
        case 9:
        	printf("Saindo do programa - Adeus!\n");
            exit(0);
            break;
	}
    //}

}

int main(int argc, char *argv[])
{
	imprimeMenu();
    //getchar();
    return 0;
}
