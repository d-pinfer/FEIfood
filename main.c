// -------------------- BIBLIOTECAS UTILIZADAS ------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// -------------------- CONSTANTES GERAIS ------------------------

// Definicoes gerais para tamanhos de buffers e limites do sistema
#define MAX_LINE  2048       // Tamanho maximo para linhas de texto
#define MAX_SMALL 256        // Tamanho para strings pequenas
#define MAX_EMAIL 128        // Tamanho maximo de email
#define MAX_NOME  128        // Tamanho maximo de nome
#define MAX_LINHAS_ARQUIVO 1000  // Maximo de linhas lidas de um arquivo .txt

// Estrutura para guardar itens de pedido no formato "id:qtd"
typedef struct {
    char id[64];
    int qtd;
} ItemQtd;

// Estrutura usada para mostrar avaliacoes
typedef struct {
    char pedido[64];
    int nota;
} AvalRow;

// -------------------- FUNCOES DE APOIO SIMPLES ------------------------

// Remove caracteres de quebra de linha (\n, \r) do final de uma string
void strip_newline(char texto[]) {
    int i = 0;

    // Percorre a string ate encontrar fim ou caractere de quebra de linha
    while (texto[i] != '\0') {
        if (texto[i] == '\n' || texto[i] == '\r') {
            texto[i] = '\0';   // Substitui por fim de string
            break;
        }
        i++;
    }
}

// Converte todos os caracteres de uma string para minusculo
void to_lower_str(char texto[]) {
    int i = 0;

    // Percorre a string convertendo cada caractere para minusculo
    while (texto[i] != '\0') {
        texto[i] = (char)tolower((unsigned char)texto[i]);
        i++;
    }
}

// Compara duas strings ignorando diferenca entre maiusculas e minusculas
int ci_equal(const char *a, const char *b) {
    int i = 0;

    // Compara caractere a caractere ja convertendo para minusculo
    while (a[i] != '\0' && b[i] != '\0') {
        char ca = (char)tolower((unsigned char)a[i]);
        char cb = (char)tolower((unsigned char)b[i]);

        if (ca != cb) {
            return 0; // Se algum caractere difere, as strings sao diferentes
        }
        i++;
    }

    // Se os dois terminaram juntos, as strings sao iguais
    if (a[i] == '\0' && b[i] == '\0') {
        return 1;
    } else {
        return 0;
    }
}

// Troca todas as ocorrencias de um caractere por outro em uma string
void replace_char(char texto[], char de, char para) {
    int i = 0;

    // Percorre a string e substitui sempre que achar o caractere "de"
    while (texto[i] != '\0') {
        if (texto[i] == de) {
            texto[i] = para;
        }
        i++;
    }
}

// Verifica se a string contem apenas digitos de 0 a 9 e nao esta vazia
int is_digits(const char *s) {
    int i;

    // Se string vazia, nao e valida
    if (s[0] == '\0') {
        return 0;
    }

    // Confere cada caractere para ver se esta entre '0' e '9'
    for (i = 0; s[i] != '\0'; i++) {
        if (s[i] < '0' || s[i] > '9') {
            return 0;
        }
    }
    return 1;
}

// Faz um prompt, le uma linha do usuario e remove o \n do final
void input_line(const char *prompt, char *buf, size_t bufsz) {
    // Mostra o texto do prompt se nao estiver vazio
    if (prompt[0] != '\0') {
        printf("%s", prompt);
    }

    // Usa fgets para ler com seguranca e tira a quebra de linha
    fgets(buf, (int)bufsz, stdin);
    strip_newline(buf);
}

// Duplica uma string usando malloc (versao simples de strdup)
char *strdup2(const char *s) {
    int tamanho = (int)strlen(s);
    char *p = (char*)malloc(tamanho + 1);

    // Copia o conteudo da string original para a nova area
    strcpy(p, s);
    return p;
}

// Le todas as linhas de um arquivo .txt para memoria em um vetor de strings
// Se o arquivo nao existir, out_count = 0 e out_lines = NULL
void read_all_lines(const char *path, char ***out_lines, int *out_count) {
    FILE *f;
    char **linhas;
    char buffer[MAX_LINE];
    int qtd = 0;

    // Inicializa saidas com valores padrao
    *out_lines = NULL;
    *out_count = 0;

    // Tenta abrir o arquivo para leitura
    f = fopen(path, "r");
    if (f == NULL) {
        // Se nao existe, apenas retorna (nao e erro para o sistema)
        return;
    }

    // Aloca um vetor de ponteiros para armazenar as linhas
    linhas = (char**)malloc(sizeof(char*) * MAX_LINHAS_ARQUIVO);

    // Le cada linha ate fim de arquivo ou atingir limite
    while (fgets(buffer, sizeof(buffer), f) != NULL && qtd < MAX_LINHAS_ARQUIVO) {
        strip_newline(buffer);                          // Remove \n do final
        linhas[qtd] = (char*)malloc(strlen(buffer) + 1); // Aloca espaco para linha
        strcpy(linhas[qtd], buffer);                    // Copia conteudo
        qtd++;
    }

    // Fecha o arquivo apos leitura
    fclose(f);

    // Devolve o vetor de linhas e a quantidade lida
    *out_lines = linhas;
    *out_count = qtd;
}

// Escreve TODAS as linhas de um vetor em um arquivo .txt
void write_all_lines(const char *path, char **lines, int count) {
    FILE *f;
    int i;

    // Abre o arquivo em modo de escrita (sobrescreve conteudo)
    f = fopen(path, "w");
    if (f == NULL) {
        return;
    }

    // Escreve cada linha seguida de uma quebra de linha
    for (i = 0; i < count; i++) {
        fputs(lines[i], f);
        fputc('\n', f);
    }

    // Fecha o arquivo apos escrita
    fclose(f);
}

// Libera o vetor de linhas criado pela read_all_lines
void free_lines(char **lines, int count) {
    int i;

    // Libera cada linha individualmente
    for (i = 0; i < count; i++) {
        free(lines[i]);
    }
    // Libera o vetor de ponteiros
    free(lines);
}

// Divide um buffer em partes usando strtok e salva os ponteiros em parts[]
// Exemplo: "a,b,c" -> parts[0]="a", parts[1]="b", parts[2]="c"
int split_inplace(char *buf, const char *delim, char **parts, int maxparts) {
    int n = 0;
    char *p;

    // Usa strtok para pegar o primeiro token
    p = strtok(buf, delim);
    while (p != NULL && n < maxparts) {
        parts[n] = p;        // Salva endereco do token
        n++;
        p = strtok(NULL, delim); // Continua pegando os proximos
    }

    return n;
}

// Converte uma string "id:qtd|id:qtd" em um vetor de ItemQtd
int parse_conteudo(const char *conteudo, ItemQtd *arr, int maxn) {
    int count = 0;
    char tmp[MAX_LINE];
    char *tok;

    // Se conteudo vazio, nao ha itens
    if (conteudo[0] == '\0') {
        return 0;
    }

    // Copia o conteudo para um buffer temporario para poder usar strtok
    strcpy(tmp, conteudo);

    // Separa cada "id:qtd" usando '|'
    tok = strtok(tmp, "|");
    while (tok != NULL && count < maxn) {
        char *p = strchr(tok, ':'); // Procura separador entre id e qtd
        if (p != NULL) {
            *p = '\0';              // Quebra a string em duas partes
            strcpy(arr[count].id, tok); // Copia o id
            arr[count].qtd = atoi(p + 1); // Converte quantidade para inteiro
            if (arr[count].qtd < 0) {
                arr[count].qtd = 0; // Garante que quantidade nao seja negativa
            }
            count++;
        }
        tok = strtok(NULL, "|"); // Pega o proximo item
    }

    return count;
}

// Monta uma string "id:qtd|id:qtd" a partir de um vetor de ItemQtd
void build_conteudo(char *out, size_t outsz, const ItemQtd *arr, int n) {
    int i;
    char pedaco[128];

    // Comeca com string vazia
    out[0] = '\0';

    // Adiciona cada item no formato id:qtd, separados por '|'
    for (i = 0; i < n; i++) {
        sprintf(pedaco, "%s:%d", arr[i].id, arr[i].qtd);
        if (i > 0) {
            strcat(out, "|");
        }
        strcat(out, pedaco);
    }
}

// Gera uma chave simples aleatoria para PIX com letras e numeros
void gen_pix_key(char *out, size_t sz) {
    const char *alfabeto = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int i;
    int L = 32;    // Tamanho da chave PIX
    int nalpha = (int)strlen(alfabeto);

    // Sorteia caracteres aleatorios do "alfabeto" ate o limite L ou do buffer
    for (i = 0; i < L && i + 1 < (int)sz; i++) {
        out[i] = alfabeto[rand() % nalpha];
    }
    out[i] = '\0';
}

// -------------------- MENUS E SLOGAN ------------------------

// Mostra a tela inicial com slogan e identificacao do sistema
void slogan(void) {
    printf("\n============================================================\n");
    printf("                 BEM-VINDO AO FEIFOOD                       \n");
    printf("============================================================\n");
    printf("             CONECTANDO PESSOAS E SABORES                   \n");
    printf("------------------------------------------------------------\n");
    printf("  CREATED BY: DAVI PINHEIRO FERREIRA                        \n");
    printf("============================================================\n\n");
}

// Exibe o menu principal (antes de logar) e devolve a opcao escolhida
int exibir_menu(void) {
    char buf[32];

    // Mostra as opcoes principais do sistema
    printf("MENU PRINCIPAL\n");
    printf("------------------------------------------------------------\n");
    printf("1 - CRIAR NOVA CONTA\n");
    printf("2 - ENTRAR NA SUA CONTA\n");
    printf("0 - SAIR DO FEIFOOD\n");
    printf("------------------------------------------------------------\n");
    input_line("ESCOLHA UMA OPCAO: ", buf, sizeof(buf));

    // Converte a entrada do usuario para inteiro
    return atoi(buf);
}

// Exibe o menu apos o usuario estar logado e devolve a opcao
int exibir_menu2(void) {
    char buf[32];

    // Mostra as opcoes disponiveis para o usuario logado
    printf("\nMENU DO USUARIO\n");
    printf("------------------------------------------------------------\n");
    printf("1 - BUSCAR ALIMENTOS DISPONIVEIS\n");
    printf("2 - GERENCIAR SEUS PEDIDOS\n");
    printf("3 - PAGINA DE AVALIACAO\n");
    printf("0 - SAIR DA CONTA\n");
    printf("------------------------------------------------------------\n");
    input_line("ESCOLHA UMA OPCAO: ", buf, sizeof(buf));

    return atoi(buf);
}

// Exibe o menu para o item selecionado e devolve a opcao
int exibir_menu3(void) {
    char buf[32];

    // Opcoes relacionadas ao item que acabou de ser encontrado
    printf("\nMENU DO ITEM SELECIONADO\n");
    printf("------------------------------------------------------------\n");
    printf("1 - ADICIONAR ALIMENTO A UM PEDIDO\n");
    printf("0 - VOLTAR\n");
    printf("------------------------------------------------------------\n");
    input_line("ESCOLHA UMA OPCAO: ", buf, sizeof(buf));

    return atoi(buf);
}

// Exibe o menu de pedidos (visualizar, editar, excluir, finalizar)
int exibir_menu4(void) {
    char buf[32];

    // Opcoes para gerenciar o pedido em aberto do usuario
    printf("\nMENU DE PEDIDOS\n");
    printf("------------------------------------------------------------\n");
    printf("1 - VISUALIZAR PEDIDOS\n");
    printf("2 - EDITAR PEDIDO\n");
    printf("3 - EXCLUIR (ESVAZIAR) PEDIDO\n");
    printf("4 - FINALIZAR PEDIDO\n");
    printf("0 - VOLTAR\n");
    printf("------------------------------------------------------------\n");
    input_line("ESCOLHA UMA OPCAO: ", buf, sizeof(buf));

    return atoi(buf);
}

// Exibe o menu de edicao do pedido (remover item ou ajustar quantidade)
int exibir_menu5(void) {
    char buf[32];

    // Opcoes para editar o conteudo do pedido
    printf("\nMENU DE EDICAO DE PEDIDO\n");
    printf("------------------------------------------------------------\n");
    printf("1 - REMOVER ITEM DE UM PEDIDO\n");
    printf("2 - AJUSTAR QUANTIDADE DE UM ITEM NO PEDIDO\n");
    printf("0 - VOLTAR\n");
    printf("------------------------------------------------------------\n");
    input_line("ESCOLHA UMA OPCAO: ", buf, sizeof(buf));

    return atoi(buf);
}

// Exibe o menu de pagamento e devolve a forma selecionada
int exibir_menu6(void) {
    char buf[32];

    // Opcoes para escolher a forma de pagamento do pedido
    printf("\nMENU DE PAGAMENTO\n");
    printf("------------------------------------------------------------\n");
    printf("1 - PIX (COPIAR E COLAR)\n");
    printf("2 - CARTAO DE CREDITO (NA ENTREGA)\n");
    printf("3 - CARTAO DE DEBITO (NA ENTREGA)\n");
    printf("0 - VOLTAR\n");
    printf("------------------------------------------------------------\n");
    input_line("ESCOLHA UMA OPCAO: ", buf, sizeof(buf));

    return atoi(buf);
}

// Exibe o menu da pagina de avaliacoes e devolve a opcao
int exibir_menu7(void) {
    char buf[32];

    // Opcoes para avaliar pedidos e ver historico
    printf("\nMENU DE AVALIACOES\n");
    printf("------------------------------------------------------------\n");
    printf("1 - AVALIAR PEDIDO\n");
    printf("2 - VER HISTORICO DE AVALIACOES\n");
    printf("0 - VOLTAR\n");
    printf("------------------------------------------------------------\n");
    input_line("ESCOLHA UMA OPCAO: ", buf, sizeof(buf));

    return atoi(buf);
}

// -------------------- FUNCOES DE FLUXO GERAL ------------------------

// Encerra o programa mostrando uma mensagem de despedida
void sair(void) {
    printf("\nENCERRANDO SUA EXPERIENCIA NO FEIFOOD...\n");
    printf("OBRIGADO POR USAR NOSSA PLATAFORMA. ATE LOGO!\n\n");
    exit(0);
}

// Cria uma nova conta de usuario e salva em cadastros.txt
void criar_conta(void) {
    char email[MAX_EMAIL];
    char senha[64];
    char confirm[64];
    char nome[MAX_NOME];

    printf("\nCRIACAO DE CONTA\n");
    printf("------------------------------------------------------------\n");

    // Loop para ler e validar o e-mail ate ser valido
    while (1) {
        input_line("DIGITE SEU E-MAIL: ", email, sizeof(email));
        if (email[0] == '\0') {
            printf("O E-MAIL NAO PODE SER VAZIO.\n");
            continue;
        }
        // Verificacao simples de formato: precisa ter '@' e '.'
        if (strchr(email, '@') != NULL && strchr(email, '.') != NULL) {
            printf("E-MAIL VALIDADO COM SUCESSO.\n");
            break;
        }
        printf("E-MAIL INVALIDO. TENTE NOVAMENTE.\n");
    }

    // Loop para ler a senha e a confirmacao ate elas coincidirem
    while (1) {
        input_line("CRIE UMA SENHA: ", senha, sizeof(senha));
        input_line("CONFIRME A SENHA: ", confirm, sizeof(confirm));

        if (senha[0] == '\0' || confirm[0] == '\0') {
            printf("A SENHA NAO PODE SER VAZIA.\n");
            continue;
        }
        if (strcmp(senha, confirm) == 0) {
            printf("SENHA CONFIRMADA COM SUCESSO.\n");
            break;
        }
        printf("AS SENHAS NAO COINCIDEM. TENTE NOVAMENTE.\n");
    }

    // Loop para garantir que o nome nao seja vazio
    while (1) {
        input_line("INFORME SEU NOME: ", nome, sizeof(nome));
        if (nome[0] == '\0') {
            printf("NOME INVALIDO. DIGITE UM NOME VALIDO.\n");
            continue;
        }
        printf("NOME '%s' REGISTRADO COM SUCESSO.\n", nome);
        break;
    }

    // Abre o arquivo de cadastros para adicionar o novo usuario
    FILE *f = fopen("cadastros.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s,%s,%s\n", email, senha, nome);
        fclose(f);
    }

    // Mensagem final de confirmacao
    printf("\nCONTA CRIADA COM SUCESSO!\n");
    printf("SEJA BEM-VINDO(A) AO FEIFOOD, %s!\n\n", nome);
}

// Faz login do usuario conferindo email e senha em cadastros.txt
// Retorna 1 se logar com sucesso e 0 se falhar
int entrar_conta(char *out_email) {
    char email[MAX_EMAIL];
    char senha[64];
    char **linhas = NULL;
    int n = 0;
    int i;

    printf("\nLOGIN NA CONTA\n");
    printf("------------------------------------------------------------\n");

    // Coleta email e senha informados pelo usuario
    input_line("E-MAIL: ", email, sizeof(email));
    input_line("SENHA: ", senha, sizeof(senha));

    // Le todos os cadastros do arquivo
    read_all_lines("cadastros.txt", &linhas, &n);

    int logado = 0;
    int encontrou_email = 0;

    // Percorre cada linha procurando o email informado
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[3];
        int qtd_campos;

        // Copia a linha para um buffer temporario e divide por virgulas
        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 3);
        if (qtd_campos == 3) {
            char *email_arq = campos[0];
            char *senha_arq = campos[1];
            char *nome = campos[2];

            // Deixa email do arquivo e email digitado em minusculo para comparar
            char email_arq_lower[MAX_EMAIL];
            char email_digitado_lower[MAX_EMAIL];

            strncpy(email_arq_lower, email_arq, sizeof(email_arq_lower));
            email_arq_lower[sizeof(email_arq_lower)-1] = '\0';
            to_lower_str(email_arq_lower);

            strncpy(email_digitado_lower, email, sizeof(email_digitado_lower));
            email_digitado_lower[sizeof(email_digitado_lower)-1] = '\0';
            to_lower_str(email_digitado_lower);

            // Compara os emails ignorando maiusculas/minusculas
            if (strcmp(email_arq_lower, email_digitado_lower) == 0) {
                encontrou_email = 1;
                // Se email bate, confere a senha
                if (strcmp(senha, senha_arq) == 0) {
                    printf("\nLOGIN BEM-SUCEDIDO! BEM-VINDO(A), %s!\n\n", nome);
                    strncpy(out_email, email_digitado_lower, MAX_EMAIL);
                    logado = 1;
                } else {
                    printf("\nSENHA INCORRETA. TENTE NOVAMENTE.\n\n");
                }
                break; // Para apos encontrar o email
            }
        }
    }

    // Se nenhum email foi encontrado, avisa o usuario
    if (!encontrou_email) {
        printf("\nE-MAIL NAO ENCONTRADO. VERIFIQUE OU CRIE UMA NOVA CONTA.\n\n");
    }

    // Libera memoria usada pelas linhas
    free_lines(linhas, n);
    return logado;
}

// -------------------- FUNCOES DE BUSCA E CARRINHO ------------------------

// Busca um item no cardapio pelo nome e devolve se encontrou e o ID
int buscar_item(int *out_encontrou, char *out_id) {
    char nomeBusca[MAX_SMALL];
    char **linhas = NULL;
    int n = 0;
    int i;

    printf("\nBUSCAR ITENS\n");
    printf("------------------------------------------------------------\n");
    input_line("DIGITE O NOME DO ITEM: ", nomeBusca, sizeof(nomeBusca));

    // Le todas as linhas do cardapio
    read_all_lines("cardapio.txt", &linhas, &n);

    *out_encontrou = 0;
    out_id[0] = '\0';

    // Percorre cada linha do cardapio procurando o nome informado
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[5];
        int qtd_campos;

        // Copia a linha e separa os campos por virgulas
        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 5);
        if (qtd_campos >= 5) {
            char *id = campos[0];
            char *nome_item = campos[1];
            char *loja = campos[2];
            char *tipo = campos[3];
            char *valor = campos[4];

            char nome_item_lower[MAX_SMALL];
            char nomeBusca_lower[MAX_SMALL];

            // Converte nome do cardapio e nome buscado para minusculo
            strncpy(nome_item_lower, nome_item, sizeof(nome_item_lower));
            nome_item_lower[sizeof(nome_item_lower)-1] = '\0';
            to_lower_str(nome_item_lower);

            strncpy(nomeBusca_lower, nomeBusca, sizeof(nomeBusca_lower));
            nomeBusca_lower[sizeof(nomeBusca_lower)-1] = '\0';
            to_lower_str(nomeBusca_lower);

            // Se os nomes forem iguais (ignorando maiusculas/minusculas)
            if (strcmp(nome_item_lower, nomeBusca_lower) == 0) {
                *out_encontrou = 1;
                strncpy(out_id, id, MAX_SMALL);
                out_id[MAX_SMALL-1] = '\0';

                // Mostra os dados do item encontrado
                printf("\nITEM ENCONTRADO!\n");
                printf("ID: %s\n", id);
                printf("NOME: %s\n", nome_item);
                printf("LOJA: %s\n", loja);
                printf("CULINARIA: %s\n", tipo);
                printf("VALOR: %s\n\n", valor);
                break;
            }
        }
    }

    // Se nao achou nenhum item, avisa o usuario
    if (!*out_encontrou) {
        printf("\nITEM NAO ENCONTRADO. TENTE NOVAMENTE.\n");
    }

    free_lines(linhas, n);
    return 0;
}

// Adiciona um item ao pedido aberto do usuario ou cria um novo pedido
void add_carrinho(const char *email_login, const char *codigo_item) {
    char **linhas = NULL;
    int n = 0;
    int i;

    // Le todos os pedidos
    read_all_lines("pedidos.txt", &linhas, &n);

    // Variaveis para controlar pedido aberto e numeracao Pn
    int indice_pedido_aberto = -1;
    int maior_num_pedido = 0;

    // Percorre todas as linhas de pedidos para achar um aberto desse usuario
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[4];
        int qtd_campos;

        // Copia a linha e separa em campos
        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 4);
        if (qtd_campos == 4) {
            char *nome_pedido = campos[0];
            char *conteudo = campos[1];
            char *email = campos[2];
            char *status = campos[3];

            // Confere se o pedido pertence a esse usuario
            if (ci_equal(email, email_login)) {
                // Se status comeca com 'a', consideramos "aberto"
                if (tolower((unsigned char)status[0]) == 'a') {
                    indice_pedido_aberto = i;
                }

                // Atualiza o maior numero de pedido baseado em "Pn"
                if (nome_pedido[0] == 'P') {
                    int num = atoi(nome_pedido + 1);
                    if (num > maior_num_pedido) {
                        maior_num_pedido = num;
                    }
                }
            }
        }
    }

    // Solicita quantidade do item ao usuario e valida a entrada
    int quantidade = 0;
    while (1) {
        char entrada[32];
        input_line("INFORME A QUANTIDADE: ", entrada, sizeof(entrada));
        if (!is_digits(entrada) || atoi(entrada) <= 0) {
            printf("QUANTIDADE INVALIDA. DIGITE UM INTEIRO MAIOR QUE ZERO.\n");
            continue;
        }
        quantidade = atoi(entrada);
        break;
    }

    // Se ja existe um pedido em aberto, apenas atualiza o conteudo dele
    if (indice_pedido_aberto >= 0) {
        char buf[MAX_LINE];
        char *campos[4];
        int qtd_campos;

        // Copia a linha do pedido aberto para um buffer
        strncpy(buf, linhas[indice_pedido_aberto], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        // Separa os campos do pedido (nome, conteudo, email, status)
        qtd_campos = split_inplace(buf, ",", campos, 4);
        if (qtd_campos == 4) {
            char *nome_pedido = campos[0];
            char *conteudo = campos[1];
            char *email = campos[2];
            char *status = campos[3]; // status nao usado aqui diretamente

            ItemQtd itens[512];
            // Converte string de conteudo em vetor de itens
            int qtd_itens = parse_conteudo(conteudo, itens, 512);

            // Procura se o item ja existe no pedido
            int pos = -1;
            int j;
            for (j = 0; j < qtd_itens; j++) {
                if (strcmp(itens[j].id, codigo_item) == 0) {
                    pos = j;
                    break;
                }
            }

            // Se já existe, apenas soma a quantidade
            if (pos >= 0) {
                itens[pos].qtd += quantidade;
            } else {
                // Se ainda nao existe, adiciona novo item se houver espaco
                if (qtd_itens < 512) {
                    strncpy(itens[qtd_itens].id, codigo_item, sizeof(itens[qtd_itens].id));
                    itens[qtd_itens].id[sizeof(itens[qtd_itens].id)-1] = '\0';
                    itens[qtd_itens].qtd = quantidade;
                    qtd_itens++;
                }
            }

            // Construi novamente a string de conteudo com os itens atualizados
            char novo_conteudo[MAX_LINE];
            build_conteudo(novo_conteudo, sizeof(novo_conteudo), itens, qtd_itens);

            // Monta a nova linha do pedido com o conteudo atualizado
            char nova_linha[MAX_LINE];
            snprintf(nova_linha, sizeof(nova_linha), "%s,%s,%s,aberto",
                     nome_pedido, novo_conteudo, email);

            // Substitui a linha antiga pela nova
            free(linhas[indice_pedido_aberto]);
            linhas[indice_pedido_aberto] = strdup2(nova_linha);

            // Salva todas as linhas atualizadas de volta no arquivo
            write_all_lines("pedidos.txt", linhas, n);

            printf("ITEM ADICIONADO AO PEDIDO '%s'.\n", nome_pedido);
        }

        // Libera memoria das linhas
        free_lines(linhas, n);
        return;
    }

    // Se nao havia pedido em aberto, cria um novo pedido P(n+1) para o usuario
    char nome_pedido[32];
    if (maior_num_pedido >= 1) {
        snprintf(nome_pedido, sizeof(nome_pedido), "P%d", maior_num_pedido + 1);
    } else {
        snprintf(nome_pedido, sizeof(nome_pedido), "P1");
    }

    // Monta a nova linha do pedido com um unico item
    char linha_nova[MAX_LINE];
    snprintf(linha_nova, sizeof(linha_nova),
             "%s,%s:%d,%s,aberto",
             nome_pedido, codigo_item, quantidade, email_login);

    // Abre o arquivo de pedidos e adiciona a nova linha
    FILE *f = fopen("pedidos.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s\n", linha_nova);
        fclose(f);
    }

    printf("NOVO PEDIDO '%s' CRIADO E ITEM ADICIONADO.\n", nome_pedido);

    // Libera memoria das linhas lidas anteriormente
    free_lines(linhas, n);
}

// -------------------- FUNCOES DE PEDIDO (VISUALIZAR / EDITAR) ------------------------

// Mostra os itens do pedido em aberto do usuario com nomes e valores
void visualizar_pedido(const char *email_login) {
    char **linhas = NULL;
    int n = 0;
    int i;

    // PASSO 1: Le todos os pedidos do arquivo
    read_all_lines("pedidos.txt", &linhas, &n);

    // Variaveis para guardar informaçoes do pedido aberto
    int indice_pedido = -1;
    char nome_pedido[64] = "";
    char conteudo[MAX_LINE] = "";

    // PASSO 2: Procura o pedido que esteja em aberto para esse email
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[4];
        int qtd_campos;

        // Copia a linha e separa os campos
        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 4);
        if (qtd_campos == 4) {
            char *nome = campos[0];
            char *conteudo_pedido = campos[1];
            char *email = campos[2];
            char *status = campos[3];

            // Confere se o pedido pertence ao usuario e esta "aberto"
            if (ci_equal(email, email_login) && tolower((unsigned char)status[0]) == 'a') {
                indice_pedido = i;
                strncpy(nome_pedido, nome, sizeof(nome_pedido));
                nome_pedido[sizeof(nome_pedido)-1] = '\0';
                strncpy(conteudo, conteudo_pedido, sizeof(conteudo));
                conteudo[sizeof(conteudo)-1] = '\0';
                break;
            }
        }
    }

    // Se nao encontrou nenhum pedido aberto, informa o usuario
    if (indice_pedido < 0) {
        printf("\nVOCE NAO POSSUI PEDIDOS ABERTOS NO MOMENTO.\n");
        free_lines(linhas, n);
        return;
    }

    // Se o conteudo estiver vazio, mostra que o pedido esta vazio
    if (conteudo[0] == '\0' || strcmp(conteudo, "{}") == 0) {
        printf("\nO PEDIDO %s ESTA VAZIO.\n", nome_pedido);
        free_lines(linhas, n);
        return;
    }

    // PASSO 3: Le o cardapio para poder traduzir IDs em nomes e valores
    char **cardapio = NULL;
    int nc = 0;
    read_all_lines("cardapio.txt", &cardapio, &nc);

    // PASSO 4: Transforma a string de conteudo em vetor de itens
    ItemQtd itens[512];
    int qtd_itens = parse_conteudo(conteudo, itens, 512);

    if (qtd_itens == 0) {
        printf("\nO PEDIDO %s ESTA VAZIO.\n", nome_pedido);
        free_lines(cardapio, nc);
        free_lines(linhas, n);
        return;
    }

    // Cabecalho visual do pedido
    printf("\n=========================================\n");
    printf("        ITENS DO PEDIDO %s\n", nome_pedido);
    printf("=========================================\n");

    double total = 0.0;

    // PASSO 5: Para cada item do pedido, busca nome e preco no cardapio
    for (i = 0; i < qtd_itens; i++) {
        int j;
        char nome_item[64] = "(NAO ENCONTRADO NO CARDAPIO)";
        double preco_unitario = 0.0;

        // Percorre o cardapio para achar o item pelo ID
        for (j = 0; j < nc; j++) {
            char buf2[MAX_LINE];
            char *campos2[5];
            int qtd2;

            strncpy(buf2, cardapio[j], sizeof(buf2));
            buf2[sizeof(buf2)-1] = '\0';

            qtd2 = split_inplace(buf2, ",", campos2, 5);
            if (qtd2 >= 5) {
                char *id = campos2[0];
                char *nome_do_item = campos2[1];
                char *valor_str = campos2[4];

                // Compara o ID do cardapio com o ID do item do pedido
                if (strcmp(id, itens[i].id) == 0) {
                    strncpy(nome_item, nome_do_item, sizeof(nome_item));
                    nome_item[sizeof(nome_item)-1] = '\0';

                    // Copia o preco para um buffer e troca virgula por ponto
                    char tmp_valor[MAX_SMALL];
                    strncpy(tmp_valor, valor_str, sizeof(tmp_valor));
                    tmp_valor[sizeof(tmp_valor)-1] = '\0';
                    replace_char(tmp_valor, ',', '.');

                    // Converte a string "12.50" em double
                    preco_unitario = strtod(tmp_valor, NULL);
                    break;
                }
            }
        }

        // Calcula subtotal do item (preco * quantidade)
        double subtotal_item = preco_unitario * itens[i].qtd;
        total += subtotal_item;

        // Mostra informacoes completas do item
        printf("ID: %s\n", itens[i].id);
        printf("NOME: %s\n", nome_item);
        printf("QUANTIDADE: %d\n", itens[i].qtd);
        printf("PRECO UNITARIO: R$ %.2f\n", preco_unitario);
        printf("SUBTOTAL: R$ %.2f\n", subtotal_item);
        printf("-----------------------------------------\n");
    }

    // Mostra total acumulado do pedido
    printf("TOTAL DO PEDIDO: R$ %.2f\n", total);
    printf("=========================================\n\n");

    // Libera memoria utilizada
    free_lines(cardapio, nc);
    free_lines(linhas, n);
}

// Remove um item especifico de um pedido em aberto do usuario
void remover_i_pedido(const char *email_login) {
    char **linhas = NULL;
    int n = 0;
    int i;

    // Le todos os pedidos do arquivo
    read_all_lines("pedidos.txt", &linhas, &n);

    int indice_pedido = -1;
    char nome_pedido[64] = "";
    char conteudo[MAX_LINE] = "";

    // Procura o pedido do usuario que esteja em aberto
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[4];
        int qtd_campos;

        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 4);
        if (qtd_campos == 4) {
            char *nome = campos[0];
            char *conteudo_pedido = campos[1];
            char *email = campos[2];
            char *status = campos[3];

            // Confere se o pedido e do usuario e esta "aberto"
            if (ci_equal(email, email_login) && tolower((unsigned char)status[0]) == 'a') {
                indice_pedido = i;
                strncpy(nome_pedido, nome, sizeof(nome_pedido));
                nome_pedido[sizeof(nome_pedido)-1] = '\0';
                strncpy(conteudo, conteudo_pedido, sizeof(conteudo));
                conteudo[sizeof(conteudo)-1] = '\0';
                break;
            }
        }
    }

    // Se nao encontrou pedido aberto, informa
    if (indice_pedido < 0) {
        printf("\nVOCE NAO POSSUI PEDIDO ABERTO NO MOMENTO.\n\n");
        free_lines(linhas, n);
        return;
    }

    // Se conteudo vazio, nao ha itens para remover
    if (conteudo[0] == '\0' || strcmp(conteudo, "{}") == 0) {
        printf("\nO PEDIDO %s ESTA VAZIO.\n\n", nome_pedido);
        free_lines(linhas, n);
        return;
    }

    // Converte a string de conteudo em vetor de itens
    ItemQtd itens[512];
    int qtd_itens = parse_conteudo(conteudo, itens, 512);

    if (qtd_itens == 0) {
        printf("\nO PEDIDO %s ESTA VAZIO.\n\n", nome_pedido);
        free_lines(linhas, n);
        return;
    }

    // Mostra todos os itens para o usuario escolher qual remover
    printf("\nITENS NO PEDIDO:\n");
    for (i = 0; i < qtd_itens; i++) {
        printf("ID: %s | QUANTIDADE: %d\n", itens[i].id, itens[i].qtd);
    }

    char alvo[64];
    input_line("DIGITE O ID DO ITEM QUE DESEJA REMOVER: ", alvo, sizeof(alvo));

    // Cria um novo vetor sem o item removido
    ItemQtd novo[512];
    int qtd_novo = 0;
    int removido = 0;

    // Copia todos os itens para o novo vetor, menos o alvo
    for (i = 0; i < qtd_itens; i++) {
        if (strcmp(itens[i].id, alvo) == 0) {
            removido = 1;   // Indica que achou o item para remover
        } else {
            novo[qtd_novo] = itens[i];
            qtd_novo++;
        }
    }

    // Se nao encontrou o item, avisa e sai
    if (!removido) {
        printf("ITEM NAO ENCONTRADO NO PEDIDO.\n");
        free_lines(linhas, n);
        return;
    }

    // Reconstrui o conteudo do pedido sem o item removido
    char novo_conteudo[MAX_LINE];
    build_conteudo(novo_conteudo, sizeof(novo_conteudo), novo, qtd_novo);

    // Monta a nova linha mantendo status como "aberto"
    char nova_linha[MAX_LINE];
    snprintf(nova_linha, sizeof(nova_linha), "%s,%s,%s,aberto",
             nome_pedido, novo_conteudo, email_login);

    // Substitui a linha antiga do pedido por essa nova
    free(linhas[indice_pedido]);
    linhas[indice_pedido] = strdup2(nova_linha);

    // Salva as mudancas no arquivo
    write_all_lines("pedidos.txt", linhas, n);

    printf("ITEM REMOVIDO COM SUCESSO DO PEDIDO.\n");
    if (qtd_novo == 0) {
        printf("ATENCAO: O PEDIDO '%s' AGORA ESTA VAZIO.\n", nome_pedido);
    }

    // Libera memoria
    free_lines(linhas, n);
}

// Altera a quantidade de um item ou remove se a nova quantidade for zero
void alterar_unidades(const char *email_login) {
    char **linhas = NULL;
    int n = 0;
    int i;

    // Le todos os pedidos
    read_all_lines("pedidos.txt", &linhas, &n);

    int indice_pedido = -1;
    char nome_pedido[64] = "";
    char conteudo[MAX_LINE] = "";

    // Procura pedido aberto do usuario
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[4];
        int qtd_campos;

        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 4);
        if (qtd_campos == 4) {
            char *nome = campos[0];
            char *conteudo_pedido = campos[1];
            char *email = campos[2];
            char *status = campos[3];

            // Confere se pertence ao usuario e está "aberto"
            if (ci_equal(email, email_login) && tolower((unsigned char)status[0]) == 'a') {
                indice_pedido = i;
                strncpy(nome_pedido, nome, sizeof(nome_pedido));
                nome_pedido[sizeof(nome_pedido)-1] = '\0';
                strncpy(conteudo, conteudo_pedido, sizeof(conteudo));
                conteudo[sizeof(conteudo)-1] = '\0';
                break;
            }
        }
    }

    // Se nao ha pedido aberto, nao ha o que editar
    if (indice_pedido < 0) {
        printf("\nVOCE NAO POSSUI PEDIDO ABERTO NO MOMENTO.\n");
        free_lines(linhas, n);
        return;
    }

    // Se conteudo vazio, nao ha itens
    if (conteudo[0] == '\0' || strcmp(conteudo, "{}") == 0) {
        printf("\nO PEDIDO %s ESTA VAZIO.\n", nome_pedido);
        free_lines(linhas, n);
        return;
    }

    // Transforma conteudo em vetor de itens
    ItemQtd itens[512];
    int qtd_itens = parse_conteudo(conteudo, itens, 512);

    if (qtd_itens == 0) {
        printf("\nO PEDIDO %s ESTA VAZIO.\n", nome_pedido);
        free_lines(linhas, n);
        return;
    }

    // Lista todos os itens e suas quantidades atuais
    printf("\nITENS NO PEDIDO:\n");
    for (i = 0; i < qtd_itens; i++) {
        printf("ID: %s | QUANTIDADE ATUAL: %d\n", itens[i].id, itens[i].qtd);
    }

    // Pergunta qual item sera alterado
    char id_alvo[64];
    input_line("\nDIGITE O ID DO ITEM QUE DESEJA ALTERAR: ", id_alvo, sizeof(id_alvo));

    // Procura o indice do item no vetor
    int indice_item = -1;
    for (i = 0; i < qtd_itens; i++) {
        if (strcmp(itens[i].id, id_alvo) == 0) {
            indice_item = i;
            break;
        }
    }

    // Se o item nao foi encontrado, nao ha como alterar
    if (indice_item < 0) {
        printf("ITEM NAO ENCONTRADO NO PEDIDO.\n");
        free_lines(linhas, n);
        return;
    }

    // Pede a nova quantidade (pode ser zero para remover)
    int nova_qtd = 0;
    while (1) {
        char buf[32];
        input_line("INFORME A NOVA QUANTIDADE (>= 0): ", buf, sizeof(buf));
        if (!is_digits(buf)) {
            printf("QUANTIDADE INVALIDA. DIGITE UM INTEIRO >= 0.\n");
            continue;
        }
        nova_qtd = atoi(buf);
        break;
    }

    // Cria um novo vetor de itens com a quantidade atualizada
    ItemQtd novos[512];
    int qtd_novos = 0;

    for (i = 0; i < qtd_itens; i++) {
        if (i == indice_item) {
            // Se nova qtd > 0, mantem o item com valor novo
            if (nova_qtd > 0) {
                strcpy(novos[qtd_novos].id, itens[i].id);
                novos[qtd_novos].qtd = nova_qtd;
                qtd_novos++;
            }
            // Se nova qtd == 0, item e removido do vetor (nao copia)
        } else {
            // Copia os outros itens normalmente
            novos[qtd_novos] = itens[i];
            qtd_novos++;
        }
    }

    // Monta a string de conteudo com itens atualizados
    char novo_conteudo[MAX_LINE];
    build_conteudo(novo_conteudo, sizeof(novo_conteudo), novos, qtd_novos);

    // Monta a linha do pedido com status "aberto"
    char nova_linha[MAX_LINE];
    snprintf(nova_linha, sizeof(nova_linha), "%s,%s,%s,aberto",
             nome_pedido, novo_conteudo, email_login);

    // Atualiza a linha do pedido no vetor
    free(linhas[indice_pedido]);
    linhas[indice_pedido] = strdup2(nova_linha);

    // Salva tudo no arquivo
    write_all_lines("pedidos.txt", linhas, n);

    // Mensagens de resultado para o usuario
    if (nova_qtd == 0) {
        printf("ITEM REMOVIDO DO PEDIDO (QUANTIDADE ZERADA).\n");
    } else {
        printf("QUANTIDADE ATUALIZADA COM SUCESSO.\n");
    }
    if (qtd_novos == 0) {
        printf("ATENCAO: O PEDIDO '%s' ESTA VAZIO.\n", nome_pedido);
    }

    // Libera memoria
    free_lines(linhas, n);
}

// Esvazia completamente o pedido aberto do usuario, mas mantem o pedido
void excluir_pedido(const char *email_login) {
    char **linhas = NULL;
    int n = 0;
    int i;

    // Le todos os pedidos
    read_all_lines("pedidos.txt", &linhas, &n);

    int indice_pedido = -1;
    char nome_pedido[64] = "";

    // Procura o pedido aberto do usuario
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[4];
        int qtd_campos;

        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 4);
        if (qtd_campos == 4) {
            char *nome = campos[0];
            char *email = campos[2];
            char *status = campos[3];

            // Confere se pertence ao usuario e esta "aberto"
            if (ci_equal(email, email_login) && tolower((unsigned char)status[0]) == 'a') {
                indice_pedido = i;
                strncpy(nome_pedido, nome, sizeof(nome_pedido));
                nome_pedido[sizeof(nome_pedido)-1] = '\0';
                break;
            }
        }
    }

    // Se nao possui pedido aberto, avisa o usuario
    if (indice_pedido < 0) {
        printf("\nVOCE NAO POSSUI PEDIDO ABERTO NO MOMENTO.\n");
        free_lines(linhas, n);
        return;
    }

    // Monta nova linha com conteudo vazio, mantendo status aberto
    char nova_linha[MAX_LINE];
    snprintf(nova_linha, sizeof(nova_linha), "%s,,%s,aberto",
             nome_pedido, email_login);

    // Substitui a linha do pedido
    free(linhas[indice_pedido]);
    linhas[indice_pedido] = strdup2(nova_linha);

    // Salva as alteracoes no arquivo
    write_all_lines("pedidos.txt", linhas, n);

    printf("TODOS OS ITENS FORAM REMOVIDOS DO PEDIDO '%s'.\n", nome_pedido);
    printf("O PEDIDO PERMANECE CRIADO E EM ABERTO, POREM VAZIO.\n");

    // Libera memoria
    free_lines(linhas, n);
}

// -------------------- FINALIZACAO E PAGAMENTO ------------------------

// Mostra um resumo do pedido, calcula valores e pega endereco se for finalizar
int confirmar_finalizacao(const char *email_login, char *out_endereco) {
    char **linhas = NULL;
    int n = 0;
    int i;

    // Le todos os pedidos
    read_all_lines("pedidos.txt", &linhas, &n);

    int indice_pedido = -1;
    char nome_pedido[64] = "";
    char conteudo[MAX_LINE] = "";

    // PASSO 1: Procura pedido aberto do usuario
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[4];
        int qtd_campos;

        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 4);
        if (qtd_campos == 4) {
            char *nome = campos[0];
            char *conteudo_pedido = campos[1];
            char *email = campos[2];
            char *status = campos[3];

            // Confere se e o pedido aberto do usuario
            if (ci_equal(email, email_login) && tolower((unsigned char)status[0]) == 'a') {
                indice_pedido = i;
                strncpy(nome_pedido, nome, sizeof(nome_pedido));
                nome_pedido[sizeof(nome_pedido)-1] = '\0';
                strncpy(conteudo, conteudo_pedido, sizeof(conteudo));
                conteudo[sizeof(conteudo)-1] = '\0';
                break;
            }
        }
    }

    // Se nao ha pedido aberto, nao ha o que finalizar
    if (indice_pedido < 0) {
        printf("\nVOCE NAO POSSUI PEDIDO ABERTO NO MOMENTO.\n");
        free_lines(linhas, n);
        return 0;
    }

    // Se o pedido estiver sem itens, nao permite finalizacao
    if (conteudo[0] == '\0' || strcmp(conteudo, "{}") == 0) {
        printf("\nO PEDIDO %s ESTA VAZIO.\n", nome_pedido);
        free_lines(linhas, n);
        return 0;
    }

    // PASSO 2: Le o cardapio para calcular valores
    char **cardapio = NULL;
    int nc = 0;

    read_all_lines("cardapio.txt", &cardapio, &nc);

    // PASSO 3: Converte conteudo "id:qtd" em vetor de itens
    ItemQtd itens[512];
    int qtd_itens = parse_conteudo(conteudo, itens, 512);

    double subtotal = 0.0;

    // Cabecalho visual do resumo
    printf("\n===========================================================================\n");
    printf("RESUMO DO PEDIDO %s\n", nome_pedido);
    printf("===========================================================================\n");

    // PASSO 4: Para cada item, descobre o preco e calcula subtotal
    for (i = 0; i < qtd_itens; i++) {
        int j;
        char nome_item[64] = "(NAO ENCONTRADO)";
        double preco_unitario = 0.0;

        // Procura o item no cardapio pelo ID
        for (j = 0; j < nc; j++) {
            char buf2[MAX_LINE];
            char *campos2[5];
            int qtd2;

            strncpy(buf2, cardapio[j], sizeof(buf2));
            buf2[sizeof(buf2)-1] = '\0';

            qtd2 = split_inplace(buf2, ",", campos2, 5);
            if (qtd2 >= 5) {
                char *id = campos2[0];
                char *nome_do_item = campos2[1];
                char *valor_str = campos2[4];

                if (strcmp(id, itens[i].id) == 0) {
                    strncpy(nome_item, nome_do_item, sizeof(nome_item));
                    nome_item[sizeof(nome_item)-1] = '\0';

                    // Troca virgula por ponto para converter para double
                    char tmp_valor[MAX_SMALL];
                    strncpy(tmp_valor, valor_str, sizeof(tmp_valor));
                    tmp_valor[sizeof(tmp_valor)-1] = '\0';
                    replace_char(tmp_valor, ',', '.');

                    preco_unitario = strtod(tmp_valor, NULL);
                    break;
                }
            }
        }

        // Calcula subtotal do item e acumula no subtotal geral
        double subtotal_item = preco_unitario * itens[i].qtd;
        subtotal += subtotal_item;

        // Imprime linha de resumo para cada item
        printf("ID: %s | NOME: %s | QTD: %d | UNIT: R$ %.2f | SUBTOTAL: R$ %.2f\n",
               itens[i].id, nome_item, itens[i].qtd, preco_unitario, subtotal_item);
    }

    // Calcula frete como 5% do subtotal
    double frete = subtotal * 0.05;
    double total = subtotal + frete;

    // Mostra valores finais
    printf("---------------------------------------------------------------------------\n");
    printf("SUBTOTAL: R$ %.2f\n", subtotal);
    printf("FRETE (5%%): R$ %.2f\n", frete);
    printf("TOTAL: R$ %.2f\n", total);
    printf("===========================================================================\n");

    // Libera memoria do cardapio e dos pedidos lidos
    free_lines(cardapio, nc);
    free_lines(linhas, n);

    // PASSO 5: Pergunta se o usuario realmente quer continuar para finalizar
    char opcao[8];
    while (1) {
        input_line("DESEJA CONTINUAR PARA FINALIZAR? (1-SIM / 0-NAO): ", opcao, sizeof(opcao));
        if (strcmp(opcao, "0") == 0 || strcmp(opcao, "1") == 0) {
            break;
        }
        printf("OPCAO INVALIDA! DIGITE 1 OU 0.\n");
    }

    // Se o usuario desistiu, apenas retorna
    if (strcmp(opcao, "0") == 0) {
        printf("\nOPERACAO CANCELADA. VOLTANDO AO MENU ANTERIOR.\n");
        return 0;
    }

    // PASSO 6: Coleta dados de endereco de entrega
    char rua[128], numero[32], bairro[128], compl[128];

    printf("\nINFORME OS DADOS DE ENTREGA:\n");
    input_line("RUA: ", rua, sizeof(rua));
    input_line("NUMERO: ", numero, sizeof(numero));
    input_line("BAIRRO: ", bairro, sizeof(bairro));
    input_line("COMPLEMENTO (OPCIONAL): ", compl, sizeof(compl));

    // Monta a string final do endereco formatado
    if (compl[0] != '\0') {
        snprintf(out_endereco, MAX_LINE, "%s, %s, %s, %s", rua, numero, bairro, compl);
    } else {
        snprintf(out_endereco, MAX_LINE, "%s, %s, %s", rua, numero, bairro);
    }

    return 1;
}

// Finaliza o pedido: escolhe pagamento, mostra resumo e marca como fechado
void finalizar_pedido(const char *email_login, const char *endereco_fmt, int opcao_pagto) {
    char **linhas = NULL;
    int n = 0;
    int i;

    // Valida a opcao de pagamento
    if (!(opcao_pagto == 1 || opcao_pagto == 2 || opcao_pagto == 3)) {
        printf("OPCAO DE PAGAMENTO INVALIDA.\n");
        return;
    }

    // Le todos os pedidos do arquivo
    read_all_lines("pedidos.txt", &linhas, &n);

    int indice_pedido = -1;
    char nome_pedido[64] = "";
    char conteudo[MAX_LINE] = "";

    // Procura o pedido aberto do usuario
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[4];
        int qtd_campos;

        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 4);
        if (qtd_campos == 4) {
            char *nome = campos[0];
            char *conteudo_pedido = campos[1];
            char *email = campos[2];
            char *status = campos[3];

            // Confere se e o pedido em aberto desse usuario
            if (ci_equal(email, email_login) && tolower((unsigned char)status[0]) == 'a') {
                indice_pedido = i;
                strncpy(nome_pedido, nome, sizeof(nome_pedido));
                nome_pedido[sizeof(nome_pedido)-1] = '\0';
                strncpy(conteudo, conteudo_pedido, sizeof(conteudo));
                conteudo[sizeof(conteudo)-1] = '\0';
                break;
            }
        }
    }

    // Se nao ha pedido aberto, nao ha o que finalizar
    if (indice_pedido < 0) {
        printf("\nVOCE NAO POSSUI PEDIDOS ABERTOS.\n");
        free_lines(linhas, n);
        return;
    }

    // Se o pedido esta vazio, nao pode finalizar
    if (conteudo[0] == '\0' || strcmp(conteudo, "{}") == 0) {
        printf("\nO PEDIDO %s ESTA VAZIO.\n", nome_pedido);
        free_lines(linhas, n);
        return;
    }

    // Mostra informacoes especificas de acordo com a forma de pagamento
    if (opcao_pagto == 1) {
        // Gera uma chave PIX para copiar e colar
        char chave[64];
        gen_pix_key(chave, sizeof(chave));

        printf("\n=========================================\n");
        printf(" PAGAMENTO VIA PIX\n");
        printf("=========================================\n");
        printf("USE A SEGUINTE CHAVE PIX (COPIAR E COLAR):\n");
        printf("%s\n", chave);
    } else if (opcao_pagto == 2) {
        printf("\n=========================================\n");
        printf(" PAGAMENTO NA ENTREGA - CARTAO DE CREDITO\n");
        printf("=========================================\n");
        printf("O PAGAMENTO SERA REALIZADO NA ENTREGA EM CARTAO DE CREDITO.\n");
    } else {
        printf("\n=========================================\n");
        printf(" PAGAMENTO NA ENTREGA - CARTAO DE DEBITO\n");
        printf("=========================================\n");
        printf("O PAGAMENTO SERA REALIZADO NA ENTREGA EM CARTAO DE DEBITO.\n");
    }

    // Mostra endereco e previsao de entrega
    printf("-----------------------------------------\n");
    printf("ENDERECO DE ENTREGA: %s\n", endereco_fmt);
    printf("PREVISAO DE ENTREGA: ENTRE 40 E 60 MINUTOS.\n");
    printf("=========================================\n\n");

    // Atualiza o status do pedido para "fechado"
    char nova_linha[MAX_LINE];
    snprintf(nova_linha, sizeof(nova_linha), "%s,%s,%s,fechado",
             nome_pedido, conteudo, email_login);

    // Substitui linha antiga pela nova com status fechado
    free(linhas[indice_pedido]);
    linhas[indice_pedido] = strdup2(nova_linha);

    // Salva a alteracao no arquivo
    write_all_lines("pedidos.txt", linhas, n);

    printf("O STATUS DO PEDIDO %s AGORA E: FECHADO.\n", nome_pedido);

    // Libera memoria
    free_lines(linhas, n);
}

// -------------------- AVALIACOES ------------------------

// Permite o usuario avaliar um pedido fechado ainda nao avaliado
void avaliar_pedido(const char *email_login) {
    char **pedidos = NULL;
    int np = 0;
    int i;

    // Le todos os pedidos do arquivo
    read_all_lines("pedidos.txt", &pedidos, &np);

    char pedidos_fechados[512][64];
    int qtd_fechados = 0;

    // Passo 1: Descobre todos os pedidos FECHADOS desse usuario
    for (i = 0; i < np; i++) {
        char buf[MAX_LINE];
        char *campos[4];
        int qtd_campos;

        strncpy(buf, pedidos[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 4);
        if (qtd_campos == 4) {
            char *nome = campos[0];
            char *email = campos[2];
            char *status = campos[3];

            // Verifica se pedido e desse usuario e se esta fechado
            if (ci_equal(email, email_login) && tolower((unsigned char)status[0]) == 'f') {
                strncpy(pedidos_fechados[qtd_fechados], nome, sizeof(pedidos_fechados[qtd_fechados]));
                pedidos_fechados[qtd_fechados][sizeof(pedidos_fechados[qtd_fechados])-1] = '\0';
                qtd_fechados++;
            }
        }
    }

    // Se nao tem pedidos fechados, nao ha o que avaliar
    if (qtd_fechados == 0) {
        printf("\nVOCE NAO POSSUI PEDIDOS FECHADOS PARA AVALIAR.\n\n");
        free_lines(pedidos, np);
        return;
    }

    // Le as avaliacoes existentes
    char **avs = NULL;
    int na = 0;
    read_all_lines("avaliacoes.txt", &avs, &na);

    char pedidos_ja_avaliados[512][64];
    int qtd_ja = 0;

    // Passo 2: Descobre quais pedidos esse usuario ja avaliou
    for (i = 0; i < na; i++) {
        char buf2[MAX_LINE];
        char *campos2[3];
        int qtd2;

        strncpy(buf2, avs[i], sizeof(buf2));
        buf2[sizeof(buf2)-1] = '\0';

        qtd2 = split_inplace(buf2, ",", campos2, 3);
        if (qtd2 == 3) {
            char *email_ava = campos2[0];
            char *pedido_nome = campos2[1];

            // Se a avaliacao e desse usuario, registra o pedido
            if (ci_equal(email_ava, email_login)) {
                strncpy(pedidos_ja_avaliados[qtd_ja], pedido_nome, sizeof(pedidos_ja_avaliados[qtd_ja]));
                pedidos_ja_avaliados[qtd_ja][sizeof(pedidos_ja_avaliados[qtd_ja])-1] = '\0';
                qtd_ja++;
            }
        }
    }

    // Passo 3: Monta uma lista de pedidos fechados que ainda nao foram avaliados
    char pendentes[512][64];
    int qtd_pend = 0;

    // Para cada pedido fechado, verifica se ele ja esta na lista de avaliados
    for (i = 0; i < qtd_fechados; i++) {
        int j;
        int ja_aval = 0;

        for (j = 0; j < qtd_ja; j++) {
            if (strcmp(pedidos_fechados[i], pedidos_ja_avaliados[j]) == 0) {
                ja_aval = 1;
                break;
            }
        }

        // Se ainda nao foi avaliado, coloca na lista de pendentes
        if (!ja_aval) {
            strncpy(pendentes[qtd_pend], pedidos_fechados[i], sizeof(pendentes[qtd_pend]));
            pendentes[qtd_pend][sizeof(pendentes[qtd_pend])-1] = '\0';
            qtd_pend++;
        }
    }

    // Se nao ha nenhum pedido pendente, nao ha avaliacao para registrar
    if (qtd_pend == 0) {
        printf("\nTODOS OS SEUS PEDIDOS FECHADOS JA FORAM AVALIADOS.\n\n");
        free_lines(avs, na);
        free_lines(pedidos, np);
        return;
    }

    // Lista os pedidos que ainda podem ser avaliados
    printf("\nPEDIDOS PENDENTES DE AVALIACAO:\n");
    for (i = 0; i < qtd_pend; i++) {
        printf("- %s\n", pendentes[i]);
    }

    // Pede ao usuario que diga qual pedido deseja avaliar
    char alvo[64];
    input_line("\nDIGITE O NOME DO PEDIDO QUE DESEJA AVALIAR (EX: P1): ", alvo, sizeof(alvo));

    // Deixa o nome do pedido em maiusculas para padronizar
    for (i = 0; alvo[i] != '\0'; i++) {
        alvo[i] = (char)toupper((unsigned char)alvo[i]);
    }

    // Confere se o pedido digitado esta na lista de pendentes
    int valido = 0;
    for (i = 0; i < qtd_pend; i++) {
        if (strcmp(alvo, pendentes[i]) == 0) {
            valido = 1;
            break;
        }
    }

    // Se nao for um pedido valido, nao realiza avaliacao
    if (!valido) {
        printf("PEDIDO INVALIDO OU JA AVALIADO.\n");
        free_lines(avs, na);
        free_lines(pedidos, np);
        return;
    }

    // Pergunta a nota do pedido (0 a 5 estrelas)
    int nota = -1;
    while (1) {
        char buf_nota[16];
        input_line("INFORME A NOTA (0 A 5 ESTRELAS): ", buf_nota, sizeof(buf_nota));
        if (is_digits(buf_nota)) {
            int v = atoi(buf_nota);
            if (v >= 0 && v <= 5) {
                nota = v;
                break;
            }
        }
        printf("NOTA INVALIDA. DIGITE UM NUMERO DE 0 A 5.\n");
    }

    // Registra a avaliacao no arquivo avaliacoes.txt
    FILE *f = fopen("avaliacoes.txt", "a");
    if (f != NULL) {
        fprintf(f, "%s,%s,%d\n", email_login, alvo, nota);
        fclose(f);
    }

    // Mensagem final de confirmacao
    printf("\nAVALIACAO REGISTRADA COM SUCESSO!\n");
    printf("PEDIDO %s RECEBEU NOTA %d ESTRELA(S).\n", alvo, nota);
    printf("ESTE PEDIDO NAO PODERA SER AVALIADO NOVAMENTE.\n\n");

    // Libera memoria utilizada
    free_lines(avs, na);
    free_lines(pedidos, np);
}

// Mostra o historico de avaliacoes do usuario logado
void visualizar_avaliacoes(const char *email_login) {
    char **linhas = NULL;
    int n = 0;
    int i;

    // Le todas as linhas do arquivo de avaliacoes
    read_all_lines("avaliacoes.txt", &linhas, &n);

    AvalRow rows[1024];
    int qtd = 0;

    // Percorre cada avaliacao registrada
    for (i = 0; i < n; i++) {
        char buf[MAX_LINE];
        char *campos[3];
        int qtd_campos;

        strncpy(buf, linhas[i], sizeof(buf));
        buf[sizeof(buf)-1] = '\0';

        qtd_campos = split_inplace(buf, ",", campos, 3);
        if (qtd_campos == 3) {
            char *email = campos[0];
            char *pedido = campos[1];
            char *nota_str = campos[2];

            // Seleciona apenas avaliacoes do usuario logado
            if (ci_equal(email, email_login)) {
                int nota = atoi(nota_str);
                if (nota < 0) nota = 0;
                if (nota > 5) nota = 5;

                // Salva os dados tratados no vetor rows
                strncpy(rows[qtd].pedido, pedido, sizeof(rows[qtd].pedido));
                rows[qtd].pedido[sizeof(rows[qtd].pedido)-1] = '\0';
                rows[qtd].nota = nota;
                qtd++;
            }
        }
    }

    // Se nao ha avaliacoes do usuario, apenas informa
    if (qtd == 0) {
        printf("\nVOCE NAO POSSUI AVALIACOES REGISTRADAS.\n\n");
        free_lines(linhas, n);
        return;
    }

    // Mostra todas as avaliacoes encontradas
    printf("\n=========================================\n");
    printf("   SUAS AVALIACOES\n");
    printf("=========================================\n");
    for (i = 0; i < qtd; i++) {
        char estrelas[16];
        int j;

        // Monta uma string de asteriscos representando as estrelas
        for (j = 0; j < rows[i].nota && j < (int)sizeof(estrelas)-1; j++) {
            estrelas[j] = '*';
        }
        estrelas[j] = '\0';

        // Imprime o pedido, nota numerica e as estrelas visuais
        printf("PEDIDO: %s | NOTA: %d | ESTRELAS: %s\n",
               rows[i].pedido, rows[i].nota, estrelas);
    }
    printf("=========================================\n\n");

    // Libera memoria das linhas
    free_lines(linhas, n);
}


// -------------------- FUNCAO PRINCIPAL (MAIN) ------------------------

// Funcao principal que controla o fluxo geral do sistema FEIfood
int main(void) {
    // Define semente para funcoes aleatorias (chave PIX, etc.)
    srand((unsigned)time(NULL));

    // Mostra o cabecalho inicial com slogan e criador
    slogan();

    // Loop principal do sistema (menu inicial)
    while (1) {
        int opcao = exibir_menu();

        if (opcao == 1) {
            // Opcao de criar nova conta
            criar_conta();
        } else if (opcao == 2) {
            // Opcao de entrar na conta (login)
            char email_login[MAX_EMAIL] = "";
            if (entrar_conta(email_login)) {
                // Se login der certo, entra no menu do usuario
                while (1) {
                    int opc2 = exibir_menu2();

                    if (opc2 == 1) {
                        // Buscar item no cardapio e adicionar ao carrinho
                        int encontrou = 0;
                        char codigo_item[64] = "";

                        buscar_item(&encontrou, codigo_item);
                        if (encontrou) {
                            // Menu do item encontrado (adicionar ou voltar)
                            while (1) {
                                int opc3 = exibir_menu3();
                                if (opc3 == 1) {
                                    add_carrinho(email_login, codigo_item);
                                    break;
                                } else if (opc3 == 0) {
                                    break;
                                } else {
                                    printf("OPCAO INVALIDA. TENTE NOVAMENTE.\n");
                                }
                            }
                        }
                    } else if (opc2 == 2) {
                        // Menu para gerenciar pedidos
                        while (1) {
                            int opc4 = exibir_menu4();

                            if (opc4 == 1) {
                                // Visualizar pedido em aberto
                                visualizar_pedido(email_login);
                            } else if (opc4 == 2) {
                                // Editar pedido (remover item / alterar quantidade)
                                while (1) {
                                    int opc5 = exibir_menu5();
                                    if (opc5 == 1) {
                                        remover_i_pedido(email_login);
                                    } else if (opc5 == 2) {
                                        alterar_unidades(email_login);
                                    } else if (opc5 == 0) {
                                        break;
                                    } else {
                                        printf("OPCAO INVALIDA. TENTE NOVAMENTE.\n");
                                    }
                                }
                            } else if (opc4 == 3) {
                                // Esvaziar completamente o pedido
                                excluir_pedido(email_login);
                            } else if (opc4 == 4) {
                                // Finalizar pedido (calcular, pedir endereco, pagar)
                                char endereco_entrega[MAX_LINE] = "";
                                int ok = confirmar_finalizacao(email_login, endereco_entrega);
                                if (ok) {
                                    // Se usuario confirmou endereco, vai para menu de pagamento
                                    while (1) {
                                        int opc6 = exibir_menu6();
                                        if (opc6 == 1 || opc6 == 2 || opc6 == 3) {
                                            finalizar_pedido(email_login, endereco_entrega, opc6);
                                            break;
                                        } else if (opc6 == 0) {
                                            break;
                                        } else {
                                            printf("OPCAO INVALIDA. TENTE NOVAMENTE.\n");
                                        }
                                    }
                                }
                            } else if (opc4 == 0) {
                                // Voltar ao menu do usuario
                                break;
                            } else {
                                printf("OPCAO INVALIDA. TENTE NOVAMENTE.\n");
                            }
                        }
                    } else if (opc2 == 3) {
                        // Pagina de avaliacoes de pedidos
                        while (1) {
                            int opc7 = exibir_menu7();
                            if (opc7 == 1) {
                                avaliar_pedido(email_login);
                            } else if (opc7 == 2) {
                                visualizar_avaliacoes(email_login);
                            } else if (opc7 == 0) {
                                break;
                            } else {
                                printf("OPCAO INVALIDA. TENTE NOVAMENTE.\n");
                            }
                        }
                    } else if (opc2 == 0) {
                        // Usuario escolheu sair da conta logada
                        printf("\nVOCE SAIU DA CONTA.\n\n");
                        break;
                    } else {
                        printf("OPCAO INVALIDA. TENTE NOVAMENTE.\n");
                    }
                }
            }
        } else if (opcao == 0) {
            // Opcao de sair totalmente do sistema
            sair();
        } else {
            // Qualquer outra opcao e invalida
            printf("OPCAO INVALIDA. TENTE NOVAMENTE.\n");
        }
    }

    return 0;
}
