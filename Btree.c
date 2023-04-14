/* Bibliotecas */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef enum { false, true } bool;
 
/* Tamanho dos campos dos registros */
/* Campos de tamanho fixo */
#define TAM_DATE 9
#define TAM_DATETIME 13
#define TAM_INT_NUMBER 5
#define TAM_FLOAT_NUMBER 14
#define TAM_ID_CURSO 9
#define TAM_ID_USUARIO 12
#define TAM_TELEFONE 12
#define QTD_MAX_CATEGORIAS 3
 
/* Campos de tamanho variável (tamanho máximo) */
#define TAM_MAX_NOME 45
#define TAM_MAX_TITULO 52
#define TAM_MAX_INSTITUICAO 52
#define TAM_MAX_MINISTRANTE 51
#define TAM_MAX_EMAIL 45
#define TAM_MAX_CATEGORIA 21
 
#define MAX_REGISTROS 1000
#define TAM_REGISTRO_USUARIO                                                   \
  (TAM_ID_USUARIO + TAM_MAX_NOME + TAM_MAX_EMAIL + TAM_FLOAT_NUMBER +          \
   TAM_TELEFONE)
#define TAM_REGISTRO_CURSO                                                     \
  (TAM_ID_CURSO + TAM_MAX_TITULO + TAM_MAX_INSTITUICAO + TAM_MAX_MINISTRANTE + \
   TAM_DATE + TAM_INT_NUMBER + TAM_FLOAT_NUMBER +                              \
   QTD_MAX_CATEGORIAS * TAM_MAX_CATEGORIA + 1)
#define TAM_REGISTRO_INSCRICAO                                                 \
  (TAM_ID_CURSO + TAM_ID_USUARIO + TAM_DATETIME + 1 + TAM_DATETIME - 4)
#define TAM_ARQUIVO_USUARIOS (TAM_REGISTRO_USUARIO * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CURSOS (TAM_REGISTRO_CURSO * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_INSCRICOES (TAM_REGISTRO_INSCRICAO * MAX_REGISTROS + 1)
 
#define TAM_RRN_REGISTRO 4
#define TAM_CHAVE_USUARIOS_IDX (TAM_ID_USUARIO + TAM_RRN_REGISTRO - 1)
#define TAM_CHAVE_CURSOS_IDX (TAM_ID_CURSO + TAM_RRN_REGISTRO - 1)
#define TAM_CHAVE_INSCRICOES_IDX                                               \
  (TAM_ID_USUARIO + TAM_ID_CURSO + TAM_RRN_REGISTRO - 2)
#define TAM_CHAVE_TITULO_IDX (TAM_MAX_TITULO + TAM_ID_CURSO - 2)
#define TAM_CHAVE_DATA_CURSO_USUARIO_IDX                                       \
  (TAM_DATETIME + TAM_ID_CURSO + TAM_ID_USUARIO - 3)
#define TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX (TAM_MAX_CATEGORIA - 1)
#define TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX (TAM_ID_CURSO - 1)
 
#define TAM_ARQUIVO_USUARIOS_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CURSOS_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_INSCRICOES_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_TITULO_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_DATA_CURSO_USUARIO_IDX (1000 * MAX_REGISTROS + 1)
#define TAM_ARQUIVO_CATEGORIAS_IDX (1000 * MAX_REGISTROS + 1)
 
/* Mensagens padrões */
#define SUCESSO "OK\n"
#define RRN_NOS "Nos percorridos:"
#define RRN_REGS_PRIMARIOS "Registros primários percorridos:"
#define RRN_REGS_SECUNDARIOS "Registros secundários percorridos:"
#define INDICE_CRIADO "Indice %s criado com sucesso!\n"
#define AVISO_NENHUM_REGISTRO_ENCONTRADO "AVISO: Nenhum registro encontrado\n"
#define ERRO_OPCAO_INVALIDA "ERRO: Opcao invalida\n"
#define ERRO_MEMORIA_INSUFICIENTE "ERRO: Memoria insuficiente\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave %s\n"
#define ERRO_REGISTRO_NAO_ENCONTRADO "ERRO: Registro nao encontrado\n"
#define ERRO_SALDO_NAO_SUFICIENTE "ERRO: Saldo insuficiente\n"
#define ERRO_CATEGORIA_REPETIDA "ERRO: O curso %s ja possui a categoria %s\n"
#define ERRO_VALOR_INVALIDO "ERRO: Valor invalido\n"
#define ERRO_ARQUIVO_VAZIO "ERRO: Arquivo vazio\n"
#define ERRO_NAO_IMPLEMENTADO "ERRO: Funcao %s nao implementada\n"
 
/* Registro de Usuario */
typedef struct {
  char id_usuario[TAM_ID_USUARIO];
  char nome[TAM_MAX_NOME];
  char email[TAM_MAX_EMAIL];
  char telefone[TAM_TELEFONE];
  double saldo;
} Usuario;
 
/* Registro de Curso */
typedef struct {
  char id_curso[TAM_ID_CURSO];
  char titulo[TAM_MAX_TITULO];
  char instituicao[TAM_MAX_INSTITUICAO];
  char ministrante[TAM_MAX_MINISTRANTE];
  char lancamento[TAM_DATE];
  int carga;
  double valor;
  char categorias[QTD_MAX_CATEGORIAS][TAM_MAX_CATEGORIA];
} Curso;
 
/* Registro de Inscricao */
typedef struct {
  char id_curso[TAM_ID_CURSO];
  char id_usuario[TAM_ID_USUARIO];
  char data_inscricao[TAM_DATETIME];
  char status;
  char data_atualizacao[TAM_DATETIME];
} Inscricao;
 
/*----- Registros dos índices -----*/
 
/* Struct para índice de lista invertida */
typedef struct {
  char *chave;
  int proximo_indice;
} inverted_list_node;
 
/* Struct para um nó de Árvore-B */
typedef struct {
  int this_rrn;
  int qtd_chaves;
  char **chaves; // ponteiro para o começo do campo de chaves no arquivo de
                 // índice respectivo
  bool folha;
  int *filhos; // vetor de int para o RRN dos nós filhos (DEVE SER DESALOCADO
               // APÓS O USO!!!)
} btree_node;
 
/* Variáveis globais */
/* Arquivos de dados */
char ARQUIVO_USUARIOS[TAM_ARQUIVO_USUARIOS];
char ARQUIVO_CURSOS[TAM_ARQUIVO_CURSOS];
char ARQUIVO_INSCRICOES[TAM_ARQUIVO_INSCRICOES];
 
/* Ordem das Árvores-B */
int btree_order = 3; // valor padrão
 
/* Índices */
/* Struct para os parâmetros de uma lista invertida */
typedef struct {
  // Ponteiro para o arquivo de índice secundário
  char *arquivo_secundario;
 
  // Ponteiro para o arquivo de índice primário
  char *arquivo_primario;
 
  // Quantidade de registros de índice secundário
  unsigned qtd_registros_secundario;
 
  // Quantidade de registros de índice primário
  unsigned qtd_registros_primario;
 
  // Tamanho de uma chave secundária nesse índice
  unsigned tam_chave_secundaria;
 
  // Tamanho de uma chave primária nesse índice
  unsigned tam_chave_primaria;
 
  // Função utilizada para comparar as chaves do índice secundário.
  // Igual às funções de comparação do bsearch e qsort.
  int (*compar)(const void *key, const void *elem);
} inverted_list;
 
/* Struct para os parâmetros de uma Árvore-B */
typedef struct {
  // RRN da raiz
  int rrn_raiz;
 
  // Ponteiro para o arquivo de índice
  char *arquivo;
 
  // Quantidade de nós no arquivo de índice
  unsigned qtd_nos;
 
  // Tamanho de uma chave nesse índice
  unsigned tam_chave;
 
  // Função utilizada para comparar as chaves do índice.
  // Igual às funções de comparação do bsearch e qsort.
  int (*compar)(const void *key, const void *elem);
} btree;
typedef struct {
  char chave_promovida[TAM_CHAVE_TITULO_IDX +
                       1]; // TAM_CHAVE_TITULO_IDX é a maior chave possível
  int filho_direito;
} promovido_aux;
 
/* Arquivos de índices */
char ARQUIVO_USUARIOS_IDX[TAM_ARQUIVO_USUARIOS_IDX];
char ARQUIVO_CURSOS_IDX[TAM_ARQUIVO_CURSOS_IDX];
char ARQUIVO_INSCRICOES_IDX[TAM_ARQUIVO_INSCRICOES_IDX];
char ARQUIVO_TITULO_IDX[TAM_ARQUIVO_TITULO_IDX];
char ARQUIVO_DATA_CURSO_USUARIO_IDX[TAM_ARQUIVO_DATA_CURSO_USUARIO_IDX];
char ARQUIVO_CATEGORIAS_SECUNDARIO_IDX[TAM_ARQUIVO_CATEGORIAS_IDX];
char ARQUIVO_CATEGORIAS_PRIMARIO_IDX[TAM_ARQUIVO_CATEGORIAS_IDX];
 
/* Comparam a chave (key) com cada elemento do índice (elem).
 * Funções auxiliares para o buscar e inserir chaves em Árvores-B.
 * Note que, desta vez, as funções comparam chaves no formato de strings,
 * não structs. key é a chave do tipo string que está sendo buscada ou
 * inserida. elem é uma chave do tipo string da struct btree_node.
 *
 * Dica: busque sobre as funções strncmp e strnlen, muito provavelmente vai
 * querer utilizá-las no código.
 * */
int order_usuarios_idx(const void *key, const void *elem);
int order_cursos_idx(const void *key, const void *elem);
int order_inscricoes_idx(const void *key, const void *elem);
int order_titulo_idx(const void *key, const void *elem);
int order_data_curso_usuario_idx(const void *key, const void *elem);
int order_categorias_idx(const void *key, const void *elem);
btree usuarios_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_USUARIOS_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_USUARIOS_IDX,
    .compar = order_usuarios_idx,
};
btree cursos_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_CURSOS_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_CURSOS_IDX,
    .compar = order_cursos_idx,
};
btree inscricoes_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_INSCRICOES_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_INSCRICOES_IDX,
    .compar = order_inscricoes_idx,
};
btree titulo_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_TITULO_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_TITULO_IDX,
    .compar = order_titulo_idx,
};
btree data_curso_usuario_idx = {
    .rrn_raiz = -1,
    .arquivo = ARQUIVO_DATA_CURSO_USUARIO_IDX,
    .qtd_nos = 0,
    .tam_chave = TAM_CHAVE_DATA_CURSO_USUARIO_IDX,
    .compar = order_data_curso_usuario_idx,
};
inverted_list categorias_idx = {
    .arquivo_secundario = ARQUIVO_CATEGORIAS_SECUNDARIO_IDX,
    .arquivo_primario = ARQUIVO_CATEGORIAS_PRIMARIO_IDX,
    .qtd_registros_secundario = 0,
    .qtd_registros_primario = 0,
    .tam_chave_secundaria = TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX,
    .tam_chave_primaria = TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX,
    .compar = order_categorias_idx,
};
 
/* Contadores */
unsigned qtd_registros_usuarios = 0;
unsigned qtd_registros_cursos = 0;
unsigned qtd_registros_inscricoes = 0;
 
/* Funções de geração determinística de números pseudo-aleatórios */
uint64_t prng_seed;
void prng_srand(uint64_t value) { prng_seed = value; }
uint64_t prng_rand() {
  // https://en.wikipedia.org/wiki/Xorshift#xorshift*
  uint64_t x =
      prng_seed; // O estado deve ser iniciado com um valor diferente de 0
  x ^= x >> 12;  // a
  x ^= x << 25;  // b
  x ^= x >> 27;  // c
  prng_seed = x;
  return x * UINT64_C(0x2545F4914F6CDD1D);
}
 
/**
 * Gera um <a
 * href="https://en.wikipedia.org/wiki/Universally_unique_identifier">UUID
 * Version-4 Variant-1</a>
 * (<i>string</i> aleatória) de 36 caracteres utilizando o gerador de
 * números pseudo-aleatórios <a
 * href="https://en.wikipedia.org/wiki/Xorshift#xorshift*">xorshift*</a>. O
 * UUID é escrito na <i>string</i> fornecida como parâmetro.<br /> <br />
 * Exemplo de uso:<br /> <code> char chave_aleatoria[37];<br />
 * new_uuid(chave_aleatoria);<br />
 * printf("chave aleatória: %s&#92;n", chave_aleatoria);<br />
 * </code>
 *
 * @param buffer String de tamanho 37 no qual será escrito
 * o UUID. É terminado pelo caractere <code>\0</code>.
 */
void new_uuid(char buffer[37]) {
  uint64_t r1 = prng_rand();
  uint64_t r2 = prng_rand();
  sprintf(buffer, "%08x-%04x-%04lx-%04lx-%012lx", (uint32_t)(r1 >> 32),
          (uint16_t)(r1 >> 16), 0x4000 | (r1 & 0x0fff), 0x8000 | (r2 & 0x3fff),
          r2 >> 16);
}
 
/* Funções de manipulação de data */
time_t epoch;
 
#define YEAR0 1900
#define EPOCH_YR 1970
#define SECS_DAY (24L * 60L * 60L)
#define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)
 
#define TIME_MAX 2147483647L
long _dstbias = 0;  // Offset for Daylight Saving Time
long _timezone = 0; // Difference in seconds between GMT and local time
const int _ytab[2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                          {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
struct tm *gmtime_r(const time_t *timer, struct tm *tmbuf) {
  // based on http://www.jbox.dk/sanos/source/lib/time.c.html
  time_t time = *timer;
  unsigned long dayclock, dayno;
  int year = EPOCH_YR;
  dayclock = (unsigned long)time % SECS_DAY;
  dayno = (unsigned long)time / SECS_DAY;
  tmbuf->tm_sec = dayclock % 60;
  tmbuf->tm_min = (dayclock % 3600) / 60;
  tmbuf->tm_hour = dayclock / 3600;
  tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
  while (dayno >= (unsigned long)YEARSIZE(year)) {
    dayno -= YEARSIZE(year);
    year++;
  }
  tmbuf->tm_year = year - YEAR0;
  tmbuf->tm_yday = dayno;
  tmbuf->tm_mon = 0;
  while (dayno >= (unsigned long)_ytab[LEAPYEAR(year)][tmbuf->tm_mon]) {
    dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
    tmbuf->tm_mon++;
  }
  tmbuf->tm_mday = dayno + 1;
  tmbuf->tm_isdst = 0;
  return tmbuf;
}
time_t mktime(struct tm *tmbuf) {
  // based on http://www.jbox.dk/sanos/source/lib/time.c.html
  long day, year;
  int tm_year;
  int yday, month;
  /*unsigned*/ long seconds;
  int overflow;
  long dst;
  tmbuf->tm_min += tmbuf->tm_sec / 60;
  tmbuf->tm_sec %= 60;
  if (tmbuf->tm_sec < 0) {
    tmbuf->tm_sec += 60;
    tmbuf->tm_min--;
  }
  tmbuf->tm_hour += tmbuf->tm_min / 60;
  tmbuf->tm_min = tmbuf->tm_min % 60;
  if (tmbuf->tm_min < 0) {
    tmbuf->tm_min += 60;
    tmbuf->tm_hour--;
  }
  day = tmbuf->tm_hour / 24;
  tmbuf->tm_hour = tmbuf->tm_hour % 24;
  if (tmbuf->tm_hour < 0) {
    tmbuf->tm_hour += 24;
    day--;
  }
  tmbuf->tm_year += tmbuf->tm_mon / 12;
  tmbuf->tm_mon %= 12;
  if (tmbuf->tm_mon < 0) {
    tmbuf->tm_mon += 12;
    tmbuf->tm_year--;
  }
  day += (tmbuf->tm_mday - 1);
  while (day < 0) {
    if (--tmbuf->tm_mon < 0) {
      tmbuf->tm_year--;
      tmbuf->tm_mon = 11;
    }
    day += _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
  }
  while (day >= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon]) {
    day -= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
    if (++(tmbuf->tm_mon) == 12) {
      tmbuf->tm_mon = 0;
      tmbuf->tm_year++;
    }
  }
  tmbuf->tm_mday = day + 1;
  year = EPOCH_YR;
  if (tmbuf->tm_year < year - YEAR0)
    return (time_t)-1;
  seconds = 0;
  day = 0; // Means days since day 0 now
  overflow = 0;
 
  // Assume that when day becomes negative, there will certainly
  // be overflow on seconds.
  // The check for overflow needs not to be done for leapyears
  // divisible by 400.
  // The code only works when year (1970) is not a leapyear.
  tm_year = tmbuf->tm_year + YEAR0;
  if (TIME_MAX / 365 < tm_year - year)
    overflow++;
  day = (tm_year - year) * 365;
  if (TIME_MAX - day < (tm_year - year) / 4 + 1)
    overflow++;
  day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
  day -=
      (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
  day +=
      (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);
  yday = month = 0;
  while (month < tmbuf->tm_mon) {
    yday += _ytab[LEAPYEAR(tm_year)][month];
    month++;
  }
  yday += (tmbuf->tm_mday - 1);
  if (day + yday < 0)
    overflow++;
  day += yday;
  tmbuf->tm_yday = yday;
  tmbuf->tm_wday = (day + 4) % 7; // Day 0 was thursday (4)
  seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;
  if ((TIME_MAX - seconds) / SECS_DAY < day)
    overflow++;
  seconds += day * SECS_DAY;
 
  // Now adjust according to timezone and daylight saving time
  if (((_timezone > 0) && (TIME_MAX - _timezone < seconds)) ||
      ((_timezone < 0) && (seconds < -_timezone))) {
    overflow++;
  }
  seconds += _timezone;
  if (tmbuf->tm_isdst) {
    dst = _dstbias;
  } else {
    dst = 0;
  }
  if (dst > seconds)
    overflow++; // dst is always non-negative
  seconds -= dst;
  if (overflow)
    return (time_t)-1;
  if ((time_t)seconds != seconds)
    return (time_t)-1;
  return (time_t)seconds;
}
bool set_time(char *date) {
  // http://www.cplusplus.com/reference/ctime/mktime/
  struct tm tm_;
  if (strlen(date) == TAM_DATETIME - 1 &&
      sscanf(date, "%4d%2d%2d%2d%2d", &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday,
             &tm_.tm_hour, &tm_.tm_min) == 5) {
    tm_.tm_year -= 1900;
    tm_.tm_mon -= 1;
    tm_.tm_sec = 0;
    tm_.tm_isdst = -1;
    epoch = mktime(&tm_);
    return true;
  }
  return false;
}
void tick_time() {
  epoch += prng_rand() % 864000; // 10 dias
}
 
/**
 * Escreve a <i>data</i> atual no formato <code>AAAAMMDD</code> em uma
 * <i>string</i> fornecida como parâmetro.<br /> <br /> Exemplo de uso:<br
 * /> <code> char timestamp[TAM_DATE];<br /> current_date(timestamp);<br />
 * printf("data atual: %s&#92;n", timestamp);<br />
 * </code>
 *
 * @param buffer String de tamanho <code>TAM_DATE</code> no qual será
 * escrita a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_date(char buffer[TAM_DATE]) {
  // http://www.cplusplus.com/reference/ctime/strftime/
  // http://www.cplusplus.com/reference/ctime/gmtime/
  // AAAA MM DD
  // %Y   %m %d
  struct tm tm_;
  if (gmtime_r(&epoch, &tm_) != NULL)
    strftime(buffer, TAM_DATE, "%Y%m%d", &tm_);
}
 
/**
 * Escreve a <i>data</i> e a <i>hora</i> atual no formato
 * <code>AAAAMMDDHHMM</code> em uma <i>string</i> fornecida como
 * parâmetro.<br
 * /> <br /> Exemplo de uso:<br /> <code> char timestamp[TAM_DATETIME];<br
 * /> current_datetime(timestamp);<br /> printf("data e hora atual:
 * %s&#92;n", timestamp);<br />
 * </code>
 *
 * @param buffer String de tamanho <code>TAM_DATETIME</code> no qual será
 * escrita a <i>timestamp</i>. É terminado pelo caractere <code>\0</code>.
 */
void current_datetime(char buffer[TAM_DATETIME]) {
  // http://www.cplusplus.com/reference/ctime/strftime/
  // http://www.cplusplus.com/reference/ctime/gmtime/
  // AAAA MM DD HH MM
  // %Y   %m %d %H %M
  struct tm tm_;
  if (gmtime_r(&epoch, &tm_) != NULL)
    strftime(buffer, TAM_DATETIME, "%Y%m%d%H%M", &tm_);
}
 
/* Remove comentários (--) e caracteres whitespace do começo e fim de uma
 * string
 */
void clear_input(char *str) {
  char *ptr = str;
  int len = 0;
  for (; ptr[len]; ++len) {
    if (strncmp(&ptr[len], "--", 2) == 0) {
      ptr[len] = '\0';
      break;
    }
  }
  while (len - 1 > 0 && isspace(ptr[len - 1]))
    ptr[--len] = '\0';
  while (*ptr && isspace(*ptr))
    ++ptr, --len;
  memmove(str, ptr, len + 1);
}
 
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES
 * =========================
 * ==========================================================================
 */
 
/* Cria o índice respectivo */
void criar_usuarios_idx();
void criar_cursos_idx();
void criar_inscricoes_idx();
void criar_titulo_idx();
void criar_data_curso_usuario_idx();
void criar_categorias_idx();
 
/* Exibe um registro com base no RRN */
bool exibir_usuario(int rrn);
bool exibir_curso(int rrn);
bool exibir_inscricao(int rrn);
 
/* Exibe um registro com base na chave de um btree_node */
bool exibir_btree_usuario(char *chave);
bool exibir_btree_curso(char *chave);
bool exibir_btree_inscricao(char *chave);
bool exibir_btree_titulo(char *chave);
bool exibir_btree_data_curso_usuario(char *chave);
 
/* Recupera do arquivo o registro com o RRN informado
 * e retorna os dados nas structs Usuario, Curso e Inscricao */
Usuario recuperar_registro_usuario(int rrn);
Curso recuperar_registro_curso(int rrn);
Inscricao recuperar_registro_inscricao(int rrn);
 
/* Escreve em seu respectivo arquivo na posição informada (RRN) */
void escrever_registro_usuario(Usuario u, int rrn);
void escrever_registro_curso(Curso j, int rrn);
void escrever_registro_inscricao(Inscricao c, int rrn);
 
/* Funções principais */
void cadastrar_usuario_menu(char *id_usuario, char *nome, char *email,
                            char *telefone);
void cadastrar_telefone_menu(char *id_usuario, char *telefone);
void remover_usuario_menu(char *id_usuario);
void cadastrar_curso_menu(char *titulo, char *instituicao, char *ministrante,
                          char *lancamento, int carga, double valor);
void adicionar_saldo_menu(char *id_usuario, double valor);
void inscrever_menu(char *id_curso, char *id_usuario);
void cadastrar_categoria_menu(char *titulo, char *categoria);
void atualizar_status_inscricoes_menu(char *id_usuario, char *titulo,
                                      char status);
 
/* Busca */
void buscar_usuario_id_menu(char *id_usuario);
void buscar_curso_id_menu(char *id_curso);
void buscar_curso_titulo_menu(char *titulo);
 
/* Listagem */
void listar_usuarios_id_menu();
void listar_cursos_categorias_menu(char *categoria);
void listar_inscricoes_periodo_menu(char *data_inicio, char *data_fim);
 
/* Liberar espaço */
void liberar_espaco_menu();
 
/* Imprimir arquivos de dados */
void imprimir_arquivo_usuarios_menu();
void imprimir_arquivo_cursos_menu();
void imprimir_arquivo_inscricoes_menu();
 
/* Imprimir índices primários */
void imprimir_usuarios_idx_menu();
void imprimir_cursos_idx_menu();
void imprimir_inscricoes_idx_menu();
 
/* Imprimir índices secundários */
void imprimir_titulo_idx_menu();
void imprimir_data_curso_usuario_idx_menu();
void imprimir_categorias_secundario_idx_menu();
void imprimir_categorias_primario_idx_menu();
 
/* Funções de manipulação de Lista Invertida */
/**
 * Responsável por inserir duas chaves (chave_secundaria e chave_primaria)
 * em uma Lista Invertida (t).<br /> Atualiza os parâmetros dos índices
 * primário e secundário conforme necessário.<br /> As chaves a serem
 * inseridas devem estar no formato correto e com tamanho
 * t->tam_chave_primario e t->tam_chave_secundario.<br /> O funcionamento
 * deve ser genérico para qualquer Lista Invertida, adaptando-se para os
 * diferentes parâmetros presentes em seus structs.<br />
 *
 * @param chave_secundaria Chave a ser buscada (caso exista) ou inserida
 * (caso não exista) no registro secundário da Lista Invertida.
 * @param chave_primaria Chave a ser inserida no registro primário da Lista
 * Invertida.
 * @param t Ponteiro para a Lista Invertida na qual serão inseridas as
 * chaves.
 */
void inverted_list_insert(char *chave_secundaria, char *chave_primaria,
                          inverted_list *t);
 
/**
 * Responsável por buscar uma chave no índice secundário de uma Lista
 * invertida (T). O valor de retorno indica se a chave foi encontrada ou
 * não. O ponteiro para o int result pode ser fornecido opcionalmente, e
 * conterá o índice inicial das chaves no registro primário.<br /> <br />
 * Exemplos de uso:<br /> <code>
 * // Exemplo 1. A chave encontrada deverá ser retornada e o caminho não
 * deve ser informado.<br />
 * ...<br />
 * int result;<br />
 * bool found = inverted_list_secondary_search(&result, false, categoria,
 * &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse na chave encontrada, apenas se ela existe,
 * e o caminho não deve ser informado.<br />
 * ...<br />
 * bool found = inverted_list_secondary_search(NULL, false, categoria,
 * &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Há interesse no caminho feito para encontrar a chave.<br />
 * ...<br />
 * int result;<br />
 * bool found = inverted_list_secondary_search(&result, true, categoria,
 * &categorias_idx);<br />
 * </code>
 *
 * @param result Ponteiro para ser escrito o índice inicial (primeira
 * ocorrência) das chaves do registro primário. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave_secundaria Chave a ser buscada.
 * @param t Ponteiro para o índice do tipo Lista invertida no qual será
 * buscada a chave.
 * @return Indica se a chave foi encontrada.
 */
bool inverted_list_secondary_search(int *result, bool exibir_caminho,
                                    char *chave_secundaria, inverted_list *t);
 
/**
 * Responsável por percorrer o índice primário de uma Lista invertida (T). O
 * valor de retorno indica a quantidade de chaves encontradas. O ponteiro
 * para o vetor de strings result pode ser fornecido opcionalmente, e será
 * populado com a lista de todas as chaves encontradas. O ponteiro para o
 * inteiro indice_final também pode ser fornecido opcionalmente, e deve
 * conter o índice do último campo da lista encadeada da chave primaria
 * fornecida (isso é útil na inserção de um novo registro).<br /> <br />
 * Exemplos de uso:<br /> <code>
 * // Exemplo 1. As chaves encontradas deverão ser retornadas e tanto o
 * caminho quanto o indice_final não devem ser informados.<br />
 * ...<br />
 * char chaves[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX][MAX_REGISTROS];<br />
 * int qtd = inverted_list_primary_search(chaves, false, indice, NULL,
 * &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse nas chaves encontradas, apenas no
 * indice_final, e o caminho não deve ser informado.<br />
 * ...<br />
 * int indice_final;
 * int qtd = inverted_list_primary_search(NULL, false, indice,
 * &indice_final, &categorias_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Há interesse nas chaves encontradas e no caminho feito.<br
 * />
 * ...<br />
 * char chaves[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX][MAX_REGISTROS];<br />
 * int qtd = inverted_list_primary_search(chaves, true, indice, NULL,
 * &categorias_idx);<br />
 * ...<br />
 * <br />
 * </code>
 *
 * @param result Ponteiro para serem escritas as chaves encontradas. É
 * ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param indice Índice do primeiro registro da lista encadeada a ser
 * procurado.
 * @param indice_final Ponteiro para ser escrito o índice do último registro
 * encontrado (cujo campo indice é -1). É ignorado caso NULL.
 * @param t Ponteiro para o índice do tipo Lista invertida no qual será
 * buscada a chave.
 * @return Indica a quantidade de chaves encontradas.
 */
int inverted_list_primary_search(
    char result[][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX], bool exibir_caminho,
    int indice, int *indice_final, inverted_list *t);
 
/**
 * Responsável por buscar uma chave (k) dentre os registros secundários de
 * uma Lista Invertida de forma eficiente.<br /> O valor de retorno deve
 * indicar se a chave foi encontrada ou não. O ponteiro para o int result
 * pode ser fornecido opcionalmente, e conterá o índice no registro
 * secundário da chave encontrada.<br />
 *
 * @param result Ponteiro para ser escrito o índice nos registros
 * secundários da chave encontrada. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave Chave a ser buscada na Lista Invertida.
 * @param t Ponteiro para o índice da Lista Invertida no qual será buscada a
 * chave.
 * @return Indica se a chave foi encontrada.
 */
bool inverted_list_binary_search(int *result, bool exibir_caminho, char *chave,
                                 inverted_list *t);
 
/* Funções de manipulação de Árvores-B */
/**
 * Responsável por inserir uma chave (k) em uma Árvore-B (T). Atualiza os
 * parâmetros da Árvore-B conforme necessário.<br /> A chave a ser inserida
 * deve estar no formato correto e com tamanho t->tam_chave.<br /> O
 * funcionamento deve ser genérico para qualquer Árvore-B, considerando que
 * os únicos parâmetros que se alteram entre as árvores é o t->tam_chave.<br
 * /> <br /> Exemplo de uso:<br /> <code>
 * ...<br />
 * char usuario_str[TAM_CHAVE_USUARIOS_IDX + 1];<br />
 * sprintf(usuario_str, "%s%04d", id_usuario, rrn_usuario);<br />
 * btree_insert(usuario_str, &usuarios_idx);<br />
 * ...<br />
 * </code>
 *
 * @param chave Chave a ser inserida na Árvore-B.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será inserida a
 * chave.
 */
void btree_insert(char *chave, btree *t);
 
/**
 * Função auxiliar de inserção de uma chave (k) em uma Árvore-B (T).
 * Atualiza os parâmetros da Árvore-B conforme necessário.<br /> Esta é uma
 * função recursiva. Ela recebe o RRN do nó a ser trabalhado sobre.<br />
 *
 * @param chave Chave a ser inserida na Árvore-B.
 * @param rrn RRN do nó no qual deverá ser processado.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será inserida a
 * chave.
 * @return Retorna uma struct do tipo promovido_aux que contém a chave
 * promovida e o RRN do filho direito.
 */
promovido_aux btree_insert_aux(char *chave, int rrn, btree *t);
 
/**
 * Função auxiliar para dividir um nó de uma Árvore-B (T). Atualiza os
 * parâmetros conforme necessário.<br />
 *
 * @param promo Uma struct do tipo promovido_aux que contém a chave a ser
 * inserida e o RRN do seu filho direito.
 * @param node Ponteiro para nó que deve ser dividido.
 * @param i O índice da posição onde a chave a ser inserida deve estar.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será inserida a
 * chave.
 * @return Retorna uma struct do tipo promovido_aux que contém a chave
 * promovida e o RRN do filho direito.
 */
promovido_aux btree_divide(promovido_aux promo, btree_node *node, int i,
                           btree *t);
 
/**
 * Responsável por remover uma chave (k) de uma Árvore-B (T). Atualiza os
 * parâmetros da Árvore-B conforme necessário.<br /> A chave a ser removida
 * deve estar no formato correto e com tamanho t->tam_chave.<br /> O
 * funcionamento deve ser genérico para qualquer Árvore-B, considerando que
 * os únicos parâmetros que se alteram entre as árvores é o t->tam_chave.<br
 * /> <br /> Exemplo de uso:<br /> <code>
 * ...<br />
 * char usuario_str[TAM_CHAVE_USUARIOS_IDX + 1];<br />
 * sprintf(usuario_str, "%s%04d", id_usuario, rrn_usuario);<br />
 * btree_delete(usuario_str, &usuarios_idx);<br />
 * ...<br />
 * </code>
 *
 * @param chave Chave a ser removida da Árvore-B.
 * @param t Ponteiro para o índice do tipo Árvore-B do qual será removida a
 * chave.
 */
void btree_delete(char *chave, btree *t);
 
/**
 * Função auxiliar de remoção de uma chave (k) de uma Árvore-B (T). Atualiza
 * os parâmetros da Árvore-B conforme necessário.<br /> Esta é uma função
 * recursiva. Ela recebe o RRN do nó a ser trabalhado sobre.<br />
 *
 * @param chave Chave a ser removida da Árvore-B.
 * @param rrn RRN do nó no qual deverá ser processado.
 * @param t Ponteiro para o índice do tipo Árvore-B do qual será removida a
 * chave.
 * @return Indica se a remoção deixou o nó que foi processado com menos
 * chaves que o mínimo necessário.
 */
bool btree_delete_aux(char *chave, int rrn, btree *t);
 
/**
 * Função auxiliar para redistribuir ou concatenar nós irmãos adjacentes à
 * esquerda e à direita de um nó pai em uma Árvore-B (T). Atualiza os
 * parâmetros conforme necessário.<br />
 *
 * @param node Ponteiro para nó pai dos nós irmãos adjacentes que deve ser
 * redistribuidos ou concatenados.
 * @param i O índice da posição no nó pai onde se encontra a chave
 * separadora dos nós irmãos adjacentes.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual serão
 * redistribuídos ou concatenados os nós irmãos adjacentes.
 * @return Indica se a redistribuição ou concatenação deixou o nó pai com
 * menos chaves que o mínimo necessário.
 */
bool btree_borrow_or_merge(btree_node *node, int i, btree *t);
 
/**
 * Responsável por buscar uma chave (k) em uma Árvore-B (T). O valor de
 * retorno indica se a chave foi encontrada ou não. O ponteiro para a string
 * result pode ser fornecido opcionalmente, e conterá o resultado
 * encontrado.<br /> Esta é uma função recursiva. O parâmetro rrn recebe a
 * raíz da Árvore-B na primeira chamada, e nas chamadas subsequentes é o RRN
 * do filho de acordo com o algoritmo fornecido.<br /> Comportamento de
 * acordo com as especificações do PDF sobre Árvores-B e suas operações.<br
 * /> <br /> Exemplos de uso:<br /> <code>
 * // Exemplo 1. A chave encontrada deverá ser retornada e o caminho não
 * deve ser informado.<br />
 * ...<br />
 * char result[TAM_CHAVE_USUARIOS_IDX + 1];<br />
 * bool found = btree_search(result, false, id_usuario,
 * usuarios_idx.rrn_raiz, &usuarios_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 2. Não há interesse na chave encontrada, apenas se ela existe,
 * e o caminho não deve ser informado.<br />
 * ...<br />
 * bool found = btree_search(NULL, false, id_usuario, usuarios_idx.rrn_raiz,
 * &usuarios_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 3. Busca por uma chave de tamanho variável (específico para o
 * caso de buscas por chaves PIX).<br />
 * ...<br />
 * char titulo_str[TAM_MAX_TITULO];<br />
 * strcpy(titulo_str, titulo);<br />
 * strpadright(titulo_str, '#', TAM_MAX_TITULO - 1);<br />
 * bool found = btree_search(NULL, false, titulo_str, titulo_idx.rrn_raiz,
 * &titulo_idx);<br />
 * ...<br />
 * <br />
 * // Exemplo 4. Há interesse no caminho feito para encontrar a chave.<br />
 * ...<br />
 * char result[TAM_CHAVE_USUARIOS_IDX + 1];<br />
 * printf(RRN_NOS);<br />
 * bool found = btree_search(result, true, id_usuario,
 * usuarios_idx.rrn_raiz, &usuarios_idx);<br /> printf("\n");<br />
 * </code>
 *
 * @param result Ponteiro para ser escrita a chave encontrada. É ignorado
 * caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave Chave a ser buscada na Árvore-B.
 * @param rrn RRN do nó no qual deverá ser processado. É o RRN da raíz da
 * Árvore-B caso seja a primeira chamada.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será buscada a
 * chave.
 * @return Indica se a chave foi encontrada.
 */
bool btree_search(char *result, bool exibir_caminho, char *chave, int rrn,
                  btree *t);
 
/**
 * Responsável por buscar uma chave (k) dentro do nó de uma Árvore-B (T) de
 * forma eficiente. O valor de retorno indica se a chave foi encontrada ou
 * não. O ponteiro para o int result pode ser fornecido opcionalmente, e
 * indica o índice da chave encontrada (caso tenha sido encontrada) ou o
 * índice do filho onde esta chave deve estar (caso não tenha sido
 * encontrada).<br />
 *
 * @param result Ponteiro para ser escrito o índice da chave encontrada ou
 * do filho onde ela deve estar. É ignorado caso NULL.
 * @param exibir_caminho Indica se o caminho percorrido deve ser impresso.
 * @param chave Chave a ser buscada na Árvore-B.
 * @param node Ponteiro para o nó onde a busca deve ser feita.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será buscada a
 * chave.
 * @return Indica se a chave foi encontrada.
 */
bool btree_binary_search(int *result, bool exibir_caminho, char *chave,
                         btree_node *node, btree *t);
 
/**
 * Função para percorrer uma Árvore-B (T) em ordem.<br />
 * Os parâmetros chave_inicio e chave_fim podem ser fornecidos
 * opcionalmente, e contém o intervalo do percurso. Caso chave_inicio e
 * chave_fim sejam NULL, o índice inteiro é percorrido. Esta é uma função
 * recursiva. O parâmetro rrn recebe a raíz da Árvore-B na primeira chamada,
 * e nas chamadas subsequentes é o RRN do filho de acordo com o algoritmo de
 * percursão em ordem.<br /> <br /> Exemplo de uso:<br /> <code>
 * // Exemplo 1. Intervalo não especificado.
 * ...<br />
 * bool imprimiu = btree_print_in_order(NULL, NULL, exibir_btree_usuario,
 * usuarios_idx.rrn_raiz, &usuarios_idx);
 * ...<br />
 * <br />
 * // Exemplo 2. Imprime as transações contidas no intervalo especificado.
 * ...<br />
 * bool imprimiu = btree_print_in_order(data_inicio, data_fim,
 * exibir_btree_data_curso_usuario, data_curso_usuario_idx.rrn_raiz,
 * &data_curso_usuario_idx);
 * ...<br />
 * </code>
 *
 * @param chave_inicio Começo do intervalo. É ignorado caso NULL.
 * @param chave_fim Fim do intervalo. É ignorado caso NULL.
 * @param exibir Função utilizada para imprimir uma chave no índice. É
 * informada a chave para a função.
 * @param rrn RRN do nó no qual deverá ser processado.
 * @param t Ponteiro para o índice do tipo Árvore-B no qual será inserida a
 * chave.
 * @return Indica se alguma chave foi impressa.
 */
bool btree_print_in_order(char *chave_inicio, char *chave_fim,
                          bool (*exibir)(char *chave), int rrn, btree *t);
 
/**
 * Função interna para ler um nó em uma Árvore-B (T).<br />
 *
 * @param no No a ser lido da Árvore-B.
 * @param t Árvore-B na qual será feita a leitura do nó.
 */
btree_node btree_read(int rrn, btree *t);
 
/**
 * Função interna para escrever um nó em uma Árvore-B (T).<br />
 *
 * @param no No a ser escrito na Árvore-B.
 * @param t Árvore-B na qual será feita a escrita do nó.
 */
void btree_write(btree_node no, btree *t);
 
/**
 * Função interna para alocar o espaço necessário dos campos chaves (vetor
 * de strings) e filhos (vetor de inteiros) da struct btree_node.<br />
 *
 * @param t Árvore-B base para o qual será alocado um struct btree_node.
 */
btree_node btree_node_malloc(btree *t);
 
/**
 * Função interna para liberar o espaço alocado dos campos chaves (vetor de
 * strings) e filhos (vetor de inteiros) da struct btree_node.<br />
 *
 * @param t Árvore-B base para o qual será liberado o espaço alocado para um
 * struct btree_node.
 */
void btree_node_free(btree_node no);
 
/**
 * Função interna para calcular o tamanho em bytes de uma Árvore-B.<br />
 *
 * @param t Árvore-B base para o qual será calculado o tamanho.
 */
int btree_register_size(btree *t);
 
/**
 * Preenche uma string str com o caractere pad para completar o tamanho
 * size.<br
 * />
 *
 * @param str Ponteiro para a string a ser manipulada.
 * @param pad Caractere utilizado para fazer o preenchimento à direita.
 * @param size Tamanho desejado para a string.
 */
char *strpadright(char *str, char pad, unsigned size);
 
/**
 * Converte uma string str para letras maiúsculas.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 */
char *strupr(char *str);
 
/**
 * Converte uma string str para letras minúsculas.<br />
 *
 * @param str Ponteiro para a string a ser manipulada.
 */
char *strlower(char *str);
 
/* <<< COLOQUE AQUI OS DEMAIS PROTÓTIPOS DE FUNÇÕES, SE NECESSÁRIO >>> */
 
/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL
 * ============================
 * =============================== NÃO ALTERAR
 * ============================== */
int main() {
  // variáveis utilizadas pelo interpretador de comandos
  char input[500];
  uint64_t seed = 2;
  char datetime[TAM_DATETIME] = "202103181430"; // UTC 18/03/2021 14:30:00
  char id_usuario[TAM_ID_USUARIO];
  char nome[TAM_MAX_NOME];
  char email[TAM_MAX_EMAIL];
  char telefone[TAM_TELEFONE];
  char id_curso[TAM_ID_CURSO];
  char titulo[TAM_MAX_TITULO];
  char instituicao[TAM_MAX_INSTITUICAO];
  char ministrante[TAM_MAX_MINISTRANTE];
  char lancamento[TAM_DATE];
  char categoria[TAM_MAX_CATEGORIA];
  int carga;
  double valor;
  char data_inicio[TAM_DATETIME];
  char data_fim[TAM_DATETIME];
  char status;
  scanf("SET BTREE_ORDER %d;\n", &btree_order);
  scanf("SET ARQUIVO_USUARIOS TO '%[^']';\n", ARQUIVO_USUARIOS);
  int temp_len = strlen(ARQUIVO_USUARIOS);
  qtd_registros_usuarios = temp_len / TAM_REGISTRO_USUARIO;
  ARQUIVO_USUARIOS[temp_len] = '\0';
  scanf("SET ARQUIVO_CURSOS TO '%[^']';\n", ARQUIVO_CURSOS);
  temp_len = strlen(ARQUIVO_CURSOS);
  qtd_registros_cursos = temp_len / TAM_REGISTRO_CURSO;
  ARQUIVO_CURSOS[temp_len] = '\0';
  scanf("SET ARQUIVO_INSCRICOES TO '%[^']';\n", ARQUIVO_INSCRICOES);
  temp_len = strlen(ARQUIVO_INSCRICOES);
  qtd_registros_inscricoes = temp_len / TAM_REGISTRO_INSCRICAO;
  ARQUIVO_INSCRICOES[temp_len] = '\0';
 
  // inicialização do gerador de números aleatórios e função de datas
  prng_srand(seed);
  putenv("TZ=UTC");
  set_time(datetime);
  criar_usuarios_idx();
  criar_cursos_idx();
  criar_inscricoes_idx();
  criar_titulo_idx();
  criar_data_curso_usuario_idx();
  criar_categorias_idx();
  while (1) {
    fgets(input, 500, stdin);
    printf("%s", input);
    clear_input(input);
    if (strcmp("", input) == 0)
      continue; // não avança o tempo nem imprime o
                // comando este seja em branco
 
    /* Funções principais */
    if (sscanf(input,
               "INSERT INTO usuarios VALUES ('%[^']', '%[^']', '%[^']', "
               "'%[^']');",
               id_usuario, nome, email, telefone) == 4)
      cadastrar_usuario_menu(id_usuario, nome, email, telefone);
    else if (sscanf(input,
                    "INSERT INTO usuarios VALUES ('%[^']', '%[^']', '%[^']');",
                    id_usuario, nome, email) == 3) {
      strcpy(telefone, "");
      strpadright(telefone, '*', TAM_TELEFONE - 1);
      cadastrar_usuario_menu(id_usuario, nome, email, telefone);
    } else if (sscanf(input,
                      "UPDATE usuarios SET telefone = '%[^']' WHERE id_usuario "
                      "= '%[^']';",
                      telefone, id_usuario) == 2)
      cadastrar_telefone_menu(id_usuario, telefone);
    else if (sscanf(input, "DELETE FROM usuarios WHERE id_usuario = '%[^']';",
                    id_usuario) == 1)
      remover_usuario_menu(id_usuario);
    else if (sscanf(input,
                    "INSERT INTO cursos VALUES ('%[^']', '%[^']', '%[^']', "
                    "'%[^']', %d, %lf);",
                    titulo, instituicao, ministrante, lancamento, &carga,
                    &valor) == 6)
      cadastrar_curso_menu(titulo, instituicao, ministrante, lancamento, carga,
                           valor);
    else if (sscanf(input,
                    "UPDATE usuarios SET saldo = saldo + %lf WHERE id_usuario "
                    "= '%[^']';",
                    &valor, id_usuario) == 2)
      adicionar_saldo_menu(id_usuario, valor);
    else if (sscanf(input, "INSERT INTO inscricoes VALUES ('%[^']', '%[^']');",
                    id_curso, id_usuario) == 2)
      inscrever_menu(id_curso, id_usuario);
    else if (sscanf(input,
                    "UPDATE cursos SET categorias = array_append(categorias, "
                    "'%[^']') WHERE titulo = '%[^']';",
                    categoria, titulo) == 2)
      cadastrar_categoria_menu(titulo, categoria);
    else if (sscanf(input,
                    "UPDATE inscricoes SET status = '%c' WHERE id_curso = "
                    "(SELECT id_curso FROM cursos WHERE titulo = '%[^']') AND "
                    "id_usuario = '%[^']';",
                    &status, titulo, id_usuario) == 3)
      atualizar_status_inscricoes_menu(id_usuario, titulo, status);
 
    /* Busca */
    else if (sscanf(input, "SELECT * FROM usuarios WHERE id_usuario = '%[^']';",
                    id_usuario) == 1)
      buscar_usuario_id_menu(id_usuario);
    else if (sscanf(input, "SELECT * FROM cursos WHERE id_curso = '%[^']';",
                    id_curso) == 1)
      buscar_curso_id_menu(id_curso);
    else if (sscanf(input, "SELECT * FROM cursos WHERE titulo = '%[^']';",
                    titulo) == 1)
      buscar_curso_titulo_menu(titulo);
 
    /* Listagem */
    else if (strcmp("SELECT * FROM usuarios ORDER BY id_usuario ASC;", input) ==
             0)
      listar_usuarios_id_menu();
    else if (sscanf(input,
                    "SELECT * FROM cursos WHERE '%[^']' = ANY (categorias) "
                    "ORDER BY id_curso ASC;",
                    categoria) == 1)
      listar_cursos_categorias_menu(categoria);
    else if (sscanf(input,
                    "SELECT * FROM inscricoes WHERE data_inscricao BETWEEN "
                    "'%[^']' AND '%[^']' ORDER BY data_inscricao ASC;",
                    data_inicio, data_fim) == 2)
      listar_inscricoes_periodo_menu(data_inicio, data_fim);
 
    /* Liberar espaço */
    else if (strcmp("VACUUM usuarios;", input) == 0)
      liberar_espaco_menu();
 
    /* Imprimir arquivos de dados */
    else if (strcmp("\\echo file ARQUIVO_USUARIOS", input) == 0)
      imprimir_arquivo_usuarios_menu();
    else if (strcmp("\\echo file ARQUIVO_CURSOS", input) == 0)
      imprimir_arquivo_cursos_menu();
    else if (strcmp("\\echo file ARQUIVO_INSCRICOES", input) == 0)
      imprimir_arquivo_inscricoes_menu();
 
    /* Imprimir índices primários */
    else if (strcmp("\\echo index usuarios_idx", input) == 0)
      imprimir_usuarios_idx_menu();
    else if (strcmp("\\echo index cursos_idx", input) == 0)
      imprimir_cursos_idx_menu();
    else if (strcmp("\\echo index inscricoes_idx", input) == 0)
      imprimir_inscricoes_idx_menu();
 
    /* Imprimir índices secundários */
    else if (strcmp("\\echo index titulo_idx", input) == 0)
      imprimir_titulo_idx_menu();
    else if (strcmp("\\echo index data_curso_usuario_idx", input) == 0)
      imprimir_data_curso_usuario_idx_menu();
    else if (strcmp("\\echo index categorias_secundario_idx", input) == 0)
      imprimir_categorias_secundario_idx_menu();
    else if (strcmp("\\echo index categorias_primario_idx", input) == 0)
      imprimir_categorias_primario_idx_menu();
 
    /* Liberar memória eventualmente alocada e encerrar programa */
    else if (strcmp("\\q", input) == 0) {
      return 0;
    } else if (sscanf(input, "SET SRAND %lu;", &seed) == 1) {
      prng_srand(seed);
      printf(SUCESSO);
      continue;
    } else if (sscanf(input, "SET TIME '%[^']';", datetime) == 1) {
      if (set_time(datetime))
        printf(SUCESSO);
      else
        printf(ERRO_VALOR_INVALIDO);
      continue;
    } else
      printf(ERRO_OPCAO_INVALIDA);
    tick_time();
  }
}
 
/* ==========================================================================
 */
 
/* Cria o índice primário usuarios_idx */
void criar_usuarios_idx() {
  char usuario_str[TAM_CHAVE_USUARIOS_IDX + 1];
  for (unsigned i = 0; i < qtd_registros_usuarios; ++i) {
    Usuario u = recuperar_registro_usuario(i);
    sprintf(usuario_str, "%s%04d", u.id_usuario, i);
    btree_insert(usuario_str, &usuarios_idx);
  }
  printf(INDICE_CRIADO, "usuarios_idx");
}
 
/* Cria o índice primário cursos_idx */
void criar_cursos_idx() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char curso_str[TAM_CHAVE_CURSOS_IDX + 1];
  for (unsigned i = 0; i < qtd_registros_cursos; ++i) {
    Curso c = recuperar_registro_curso(i);
    sprintf(curso_str, "%s%04d", c.id_curso, i);
    btree_insert(curso_str, &cursos_idx);
  }
  printf(INDICE_CRIADO, "cursos_idx");
}
 
/* Cria o índice primário inscricoes_idx */
void criar_inscricoes_idx() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char inscricoes_str[TAM_CHAVE_INSCRICOES_IDX + 1];
  for (unsigned i = 0; i < qtd_registros_inscricoes; ++i) {
    Inscricao ins = recuperar_registro_inscricao(i);
    sprintf(inscricoes_str, "%s%s%04d", ins.id_curso, ins.id_usuario, i);
    btree_insert(inscricoes_str, &inscricoes_idx);
  }
  printf(INDICE_CRIADO, "inscricoes_idx");
}
 
/* Cria o índice secundário titulo_idx */
void criar_titulo_idx() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char titulo_str[TAM_CHAVE_CURSOS_IDX + 1];
  char title[TAM_MAX_TITULO];
  for (unsigned i = 0; i < qtd_registros_inscricoes; ++i) {
    Curso c = recuperar_registro_curso(i);
    strcpy(title, c.titulo);
    strpadright(title, '#', TAM_MAX_TITULO - 1);
    sprintf(titulo_str, "%s%s%04d", strupr(title), strupr(c.id_curso), i);
    btree_insert(titulo_str, &titulo_idx);
  }
  printf(INDICE_CRIADO, "titulo_idx");
}
 
/* Cria o índice secundário data_curso_usuario_idx */
void criar_data_curso_usuario_idx() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char data_curso_str[TAM_CHAVE_DATA_CURSO_USUARIO_IDX + 1];
  for (unsigned i = 0; i < qtd_registros_inscricoes; ++i) {
    Inscricao ins = recuperar_registro_inscricao(i);
    sprintf(data_curso_str, "%s%s%s", ins.data_inscricao, ins.id_curso,
            ins.id_usuario);
    btree_insert(data_curso_str, &inscricoes_idx);
  }
  printf(INDICE_CRIADO, "data_curso_usuario_idx");
}
 
/* Cria os índices (secundário e primário) de categorias_idx */
void criar_categorias_idx() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  for (unsigned i = 0; i < qtd_registros_cursos; ++i) {
    Curso c = recuperar_registro_curso(i);
    for (unsigned j = 0; j < 3; j++) {
      if (c.categorias[j] != NULL)
        inverted_list_insert(c.categorias[j], c.id_curso, &categorias_idx);
    }
  }
  printf(INDICE_CRIADO, "categorias_idx");
}
 
/* Exibe um usuario dado seu RRN */
bool exibir_usuario(int rrn) {
  if (rrn < 0)
    return false;
  Usuario u = recuperar_registro_usuario(rrn);
  printf("%s, %s, %s, %s, %.2lf\n", u.id_usuario, u.nome, u.email, u.telefone,
         u.saldo);
  return true;
}
 
/* Exibe um curso dado seu RRN */
bool exibir_curso(int rrn) {
  if (rrn < 0)
    return false;
  Curso j = recuperar_registro_curso(rrn);
  printf("%s, %s, %s, %s, %s, %d, %.2lf\n", j.id_curso, j.titulo, j.instituicao,
         j.ministrante, j.lancamento, j.carga, j.valor);
  return true;
}
 
/* Exibe uma inscricao dado seu RRN */
bool exibir_inscricao(int rrn) {
  if (rrn < 0)
    return false;
  Inscricao c = recuperar_registro_inscricao(rrn);
  printf("%s, %s, %s, %c, %s\n", c.id_curso, c.id_usuario, c.data_inscricao,
         c.status, c.data_atualizacao);
  return true;
}
bool exibir_btree_usuario(char *chave) {
  int rrn = atoi(chave + TAM_ID_USUARIO - 1);
  return exibir_usuario(rrn);
}
bool exibir_btree_curso(char *chave) {
  int rrn = atoi(chave + TAM_ID_CURSO - 1);
  return exibir_curso(rrn);
}
bool exibir_btree_inscricao(char *chave) {
  int rrn = atoi(chave + TAM_ID_CURSO + TAM_ID_USUARIO - 2);
  return exibir_inscricao(rrn);
}
bool exibir_btree_titulo(char *chave) {
  char id_curso[TAM_ID_CURSO];
  char result[TAM_CHAVE_CURSOS_IDX + 1];
  strncpy(id_curso, chave + TAM_MAX_TITULO - 1, TAM_ID_CURSO);
  btree_search(result, false, id_curso, cursos_idx.rrn_raiz, &cursos_idx);
  return exibir_btree_curso(result);
}
bool exibir_btree_data_curso_usuario(char *chave) {
  char inscription[TAM_ID_USUARIO + TAM_ID_CURSO - 1];
  char result[TAM_CHAVE_INSCRICOES_IDX + 1];
  strncpy(inscription, chave + TAM_DATETIME - 1,
          TAM_ID_CURSO + TAM_ID_USUARIO - 2);
  btree_search(result, false, inscription, inscricoes_idx.rrn_raiz,
               &inscricoes_idx);
  return exibir_btree_inscricao(result);
}
 
/* Recupera do arquivo de usuários o registro com o RRN
 * informado e retorna os dados na struct Usuario */
Usuario recuperar_registro_usuario(int rrn) {
  Usuario u;
  char temp[TAM_REGISTRO_USUARIO + 1], *p;
  strncpy(temp, ARQUIVO_USUARIOS + (rrn * TAM_REGISTRO_USUARIO),
          TAM_REGISTRO_USUARIO);
  temp[TAM_REGISTRO_USUARIO] = '\0';
  p = strtok(temp, ";");
  strcpy(u.id_usuario, p);
  p = strtok(NULL, ";");
  strcpy(u.nome, p);
  p = strtok(NULL, ";");
  strcpy(u.email, p);
  p = strtok(NULL, ";");
  strcpy(u.telefone, p);
  p = strtok(NULL, ";");
  u.saldo = atof(p);
  p = strtok(NULL, ";");
  return u;
}
 
/* Recupera do arquivo de cursos o registro com o RRN
 * informado e retorna os dados na struct Curso */
Curso recuperar_registro_curso(int rrn) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Curso c;
  char temp[TAM_REGISTRO_CURSO + 1], *p, *categorias;
  int contador_Num_Categorias = 0;
 
  /* Copia para a variavel temp o registro especifico, determinado pelo rrn
   * * TAM_REGISTRO */
  strncpy(temp, ARQUIVO_CURSOS + (rrn * TAM_REGISTRO_CURSO),
          TAM_REGISTRO_CURSO);
  temp[TAM_REGISTRO_CURSO] = '\0';
 
  /* Auxilia para conferir se está com as categorias ou não */
  char str[2];
  str[0] = '#';
  str[1] = '#';
 
  /* Passa os dados para a struct de inscrições */
  p = strtok(temp, ";");
  strcpy(c.id_curso, p);
  p = strtok(NULL, ";");
  strcpy(c.titulo, p);
  p = strtok(NULL, ";");
  strcpy(c.instituicao, p);
  p = strtok(NULL, ";");
  strcpy(c.ministrante, p);
  p = strtok(NULL, ";");
  strcpy(c.lancamento, p);
  p = strtok(NULL, ";");
  c.carga = atoi(p);
  p = strtok(NULL, ";");
  c.valor = atof(p);
  p = strtok(NULL, ";");
  categorias = strtok(p, "|");
 
  /* Percorre as categorias para adiciona-las ou não */
  while (strncmp(categorias, str, 1) != 0) {
    strncpy(c.categorias[contador_Num_Categorias], categorias,
            TAM_MAX_CATEGORIA);
    contador_Num_Categorias++;
    categorias = strtok(NULL, "|");
    if (categorias == NULL) {
      break;
    }
    if (contador_Num_Categorias == 3) {
      break;
    }
  }
  for (unsigned i = contador_Num_Categorias; i < 3; i++) {
    strncpy(c.categorias[i], "", TAM_MAX_CATEGORIA);
  }
  return c;
}
 
/* Recupera do arquivo de inscricoes o registro com o RRN
 * informado e retorna os dados na struct Inscricao */
Inscricao recuperar_registro_inscricao(int rrn) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Inscricao i;
  char temp[TAM_REGISTRO_INSCRICAO + 1], *p;
 
  /* Copia para a variavel temp o registro especifico, determinado pelo rrn
   * * TAM_REGISTRO */
  strncpy(temp, ARQUIVO_INSCRICOES + (rrn * TAM_REGISTRO_INSCRICAO),
          TAM_REGISTRO_INSCRICAO);
  temp[TAM_REGISTRO_INSCRICAO] = '\0';
 
  /* Como o registro de inscrições possui tamanho fixo pega de byte em byte
   * preciso */
  p = temp;
 
  /* Complementa com o \0 para o final da string */
  strncpy(p, temp, 8);
  strncpy(i.id_curso, p, 8);
  i.id_curso[TAM_ID_CURSO - 1] = '\0';
  strncpy(p, temp + 8, 11);
  strncpy(i.id_usuario, p, 11);
  i.id_usuario[TAM_ID_USUARIO - 1] = '\0';
  strncpy(p, temp + 19, 12);
  strncpy(i.data_inscricao, p, 12);
  i.data_inscricao[TAM_DATETIME - 1] = '\0';
  strncpy(p, temp + 31, 1);
  i.status = p[0];
  strncpy(p, temp + 32, 12);
  strncpy(i.data_atualizacao, p, 12);
  i.data_atualizacao[TAM_DATETIME - 1] = '\0';
  return i;
}
 
/* Escreve no arquivo de usuários na posição informada (RRN)
 * os dados na struct Usuario */
void escrever_registro_usuario(Usuario u, int rrn) {
  char temp[TAM_REGISTRO_USUARIO + 1], p[100];
  temp[0] = '\0';
  p[0] = '\0';
  strcpy(temp, u.id_usuario);
  strcat(temp, ";");
  strcat(temp, u.nome);
  strcat(temp, ";");
  strcat(temp, u.email);
  strcat(temp, ";");
  strcat(temp, u.telefone);
  strcat(temp, ";");
  sprintf(p, "%013.2lf", u.saldo);
  strcat(temp, p);
  strcat(temp, ";");
  strpadright(temp, '#', TAM_REGISTRO_USUARIO);
  strncpy(ARQUIVO_USUARIOS + rrn * TAM_REGISTRO_USUARIO, temp,
          TAM_REGISTRO_USUARIO);
}
 
/* Escreve no arquivo de cursos na posição informada (RRN)
 * os dados na struct Curso */
void escrever_registro_curso(Curso j, int rrn) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  /* Foi usado como material de estudo
   * https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm */
  char temp[TAM_REGISTRO_CURSO + 1], p[100];
  temp[0] = '\0';
  p[0] = '\0';
  strcpy(temp, j.id_curso);
  strcat(temp, ";");
  strcat(temp, j.titulo);
  strcat(temp, ";");
  strcat(temp, j.instituicao);
  strcat(temp, ";");
  strcat(temp, j.ministrante);
  strcat(temp, ";");
  strcat(temp, j.lancamento);
  strcat(temp, ";");
  sprintf(p, "%04d", j.carga); // Usado para manter a formatação correta
  strcat(temp, p);
  strcat(temp, ";");
  sprintf(p, "%013.2lf", j.valor); // Usado para manter a formatação correta
  strcat(temp, p);
  strcat(temp, ";");
 
  /* Define as categorias para o formato adequado do programa */
  if (strcmp(j.categorias[0], "") != 0 &&
      strncmp(j.categorias[0], "#", 1) != 0) {
    strcat(temp, j.categorias[0]);
  }
  if (strcmp(j.categorias[1], "") != 0 &&
      strncmp(j.categorias[1], "#", 1) != 0) {
    strcat(temp, "|");
    strcat(temp, j.categorias[1]);
  }
  if (strcmp(j.categorias[2], "") != 0 &&
      strncmp(j.categorias[2], "#", 1) != 0) {
    strcat(temp, "|");
    strcat(temp, j.categorias[2]);
    strcat(temp, "|");
  }
  strcat(temp, ";");
  strpadright(temp, '#', TAM_REGISTRO_CURSO);
  strncpy(ARQUIVO_CURSOS + rrn * TAM_REGISTRO_CURSO, temp, TAM_REGISTRO_CURSO);
}
 
/* Escreve no arquivo de inscricoes na posição informada (RRN)
 * os dados na struct Inscricao */
void escrever_registro_inscricao(Inscricao c, int rrn) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char temp[TAM_REGISTRO_INSCRICAO + 1], p[100];
  temp[0] = '\0';
  p[0] = '\0';
 
  /* Usado para inserir o status da categoria de maneira correta */
  char stringStatus[2] = {c.status, '\0'};
  strcpy(temp, c.id_curso);
  strcat(temp, c.id_usuario);
  strcat(temp, c.data_inscricao);
  strcat(temp, stringStatus);
  strcat(temp, c.data_atualizacao);
  strpadright(temp, '#', TAM_REGISTRO_INSCRICAO);
  strncpy(ARQUIVO_INSCRICOES + rrn * TAM_REGISTRO_INSCRICAO, temp,
          TAM_REGISTRO_INSCRICAO);
}
 
/* Funções principais */
void cadastrar_usuario_menu(char *id_usuario, char *nome, char *email,
                            char *telefone) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char key[TAM_CHAVE_USUARIOS_IDX + 1];
  Usuario user;
  strcpy(user.id_usuario, id_usuario);
  strcpy(user.nome, nome);
  strcpy(user.email, email);
  user.saldo = atof("00000000000.00"); // Usado para manter a formatação correta
  if (strcmp(telefone, "") == 0) {
    strcpy(user.telefone, "***********");
  } else
    strcpy(user.telefone, telefone);
 
  /* Busca para conferir se já possui esse usuário */
  bool exists = btree_search(NULL, false, id_usuario, usuarios_idx.rrn_raiz,
                             &usuarios_idx);
  if (!exists) {
    sprintf(key, "%s%04d", id_usuario, qtd_registros_usuarios);
    btree_insert(key, &usuarios_idx);
    escrever_registro_usuario(user, qtd_registros_usuarios);
    qtd_registros_usuarios += 1;
    printf(SUCESSO);
    return;
  } else {
    // registro já existe e não pode ser adicionado
    printf(ERRO_PK_REPETIDA, id_usuario);
    return;
  }
}
void cadastrar_telefone_menu(char *id_usuario, char *telefone) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char result[TAM_CHAVE_USUARIOS_IDX + 1];
  char rrn_aux[5];
  int rrn;
  rrn_aux[4] = '\0';
 
  /* Busca para ver se o usuário que ele quer inserir existe */
  bool exists = btree_search(result, false, id_usuario, usuarios_idx.rrn_raiz,
                             &usuarios_idx);
  if (exists) {
    strncpy(rrn_aux, result + TAM_ID_USUARIO - 1, 4);
    rrn = atoi(rrn_aux);
    Usuario user = recuperar_registro_usuario(rrn);
    strcpy(user.telefone, telefone);
    escrever_registro_usuario(user, rrn);
    printf(SUCESSO);
    return;
  } else {
    /* Caso não tenha achado o usuário */
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
}
void remover_usuario_menu(char *id_usuario) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char result[TAM_CHAVE_USUARIOS_IDX + 1];
  char rrn_aux[5];
  int rrn;
  rrn_aux[4] = '\0';
 
  /* Busca para ver se o usuário que ele quer inserir existe */
  bool exists = btree_search(result, false, id_usuario, usuarios_idx.rrn_raiz,
                             &usuarios_idx);
  if (exists) {
    strncpy(rrn_aux, result + TAM_ID_USUARIO - 1, 4);
    rrn = atoi(rrn_aux);
    Usuario user = recuperar_registro_usuario(rrn); // recupera registro correto
    strncpy(user.id_usuario, "*|", 2);
    escrever_registro_usuario(user, rrn);
    btree_delete(result, &usuarios_idx);
    // chama função de remover da arvore
    printf(SUCESSO);
    return;
  } else {
    /* Caso não tenha achado o usuário */
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
}
void cadastrar_curso_menu(char *titulo, char *instituicao, char *ministrante,
                          char *lancamento, int carga, double valor) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Curso c;
  char title[TAM_CHAVE_TITULO_IDX + 1];
  char id_course[TAM_CHAVE_CURSOS_IDX + 1];
  sprintf(c.id_curso, "%08d",
          qtd_registros_cursos); // Usado para manter a formatação correta
  strcpy(c.titulo, titulo);
  strcpy(c.instituicao, instituicao);
  strcpy(c.ministrante, ministrante);
  strcpy(c.lancamento, lancamento);
  c.carga = carga;
  c.valor = valor;
  strncpy(c.categorias[0], "", TAM_MAX_CATEGORIA);
  strncpy(c.categorias[1], "", TAM_MAX_CATEGORIA);
  strncpy(c.categorias[2], "", TAM_MAX_CATEGORIA);
  char p[TAM_MAX_TITULO]; // Usado como auxilio para lidar com a caixa alta
                              // do titulo
  strncpy(p, titulo, TAM_MAX_TITULO);
  p[TAM_MAX_TITULO - 1] = '\0';
  strpadright(p, '#', TAM_MAX_TITULO - 1);
  bool existsTitle =
      btree_search(NULL, false, strupr(p), titulo_idx.rrn_raiz, &titulo_idx);
 
  /* Já existe este titulo */
  if (existsTitle) {
    // registro já existe e não pode ser adicionado
    printf(ERRO_PK_REPETIDA, titulo);
    return;
  }
  sprintf(title, "%s%08d", p, qtd_registros_cursos);
  sprintf(id_course, "%08d%04d", qtd_registros_cursos, qtd_registros_cursos);
  // Inserir na arvore de usuarios_idx
  btree_insert(title, &titulo_idx);
  btree_insert(id_course, &cursos_idx);
  escrever_registro_curso(c, qtd_registros_cursos);
  qtd_registros_cursos++;
  printf(SUCESSO);
}
void adicionar_saldo_menu(char *id_usuario, double valor) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
 
  /* Busca pelo usuário para colocar saldo */
  char result[TAM_CHAVE_USUARIOS_IDX + 1];
  char rrn_aux[5];
  int rrn;
  rrn_aux[4] = '\0';
  /* Busca para ver se o usuário que ele quer inserir existe */
  bool exists = btree_search(result, false, id_usuario, usuarios_idx.rrn_raiz,
                             &usuarios_idx);
  if (exists) {
    strncpy(rrn_aux, result + TAM_ID_USUARIO - 1, 4);
    rrn = atoi(rrn_aux);
    Usuario user = recuperar_registro_usuario(rrn);
    if (valor <= 0) {
      printf(ERRO_VALOR_INVALIDO);
      return;
    } else {
      /* Caso o usuário exista ele adiciona o valor */
      user.saldo += valor;
      escrever_registro_usuario(user, rrn);
      printf(SUCESSO);
      return;
    }
  } else {
    /* Caso não tenha achado o usuário */
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
}
void inscrever_menu(char *id_curso, char *id_usuario) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Curso cursoI;
  Usuario user;
  Inscricao ins;
  char id_curso_id_user[TAM_CHAVE_INSCRICOES_IDX + 1];
  char dt[TAM_DATETIME]; // usado como auxilio para determinar a data correta
  char chave[TAM_CHAVE_CURSOS_IDX + 1];
  char resultUser[TAM_CHAVE_USUARIOS_IDX + 1];
  char resultCurso[TAM_CHAVE_CURSOS_IDX + 1];
  char rrn_aux[5];
  int rrnCurso;
  int rrnUsuario;
  char data_grid[TAM_CHAVE_DATA_CURSO_USUARIO_IDX + 1];
  rrn_aux[4] = '\0';
  /* Busca pelo usuario */
  bool existsUser = btree_search(resultUser, false, id_usuario,
                                 usuarios_idx.rrn_raiz, &usuarios_idx);
 
  /* Busca pelo curso */
  bool existsCurso = btree_search(resultCurso, false, id_curso,
                                  cursos_idx.rrn_raiz, &cursos_idx);
 
  /* Confere a existência do curso */
  if (existsCurso) {
    strncpy(rrn_aux, resultCurso + TAM_ID_CURSO - 1, 4);
    rrnCurso = atoi(rrn_aux);
    cursoI = recuperar_registro_curso(rrnCurso);
  }
 
  /* Confere a existência do usuario */
  if (existsUser) {
    strncpy(rrn_aux, resultUser + TAM_ID_USUARIO - 1, 4);
    rrnUsuario = atoi(rrn_aux);
    user = recuperar_registro_usuario(rrnUsuario);
  }
 
  /* Caso usuário não exista */
  if (!existsUser) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  /* Caso curso não exista */
  else if (!existsCurso) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  } else {
    /* Busca a inscrição com o auxilio do aux que contem todos os dados
     * necessários para      * serem inserido no qsort
     */
    sprintf(id_curso_id_user, "%s%s%04d", id_curso, id_usuario,
            qtd_registros_inscricoes);
    bool existsInscricao =
        btree_search(NULL, false, id_curso_id_user, inscricoes_idx.rrn_raiz,
                     &inscricoes_idx);
 
    /* Caso a inscrição não exista */
    if (!existsInscricao) {
      if (cursoI.valor > user.saldo) { // saldo insuficiente
        printf(ERRO_SALDO_NAO_SUFICIENTE);
        return;
      } else {
        user.saldo -= cursoI.valor;
        escrever_registro_usuario(user, rrnUsuario); // para atualizar o saldo
        current_datetime(dt); // insere a data no formato correto
        sprintf(data_grid, "%s%s%s", dt, id_curso, id_usuario);
        strcpy(ins.data_inscricao, dt);
        strcpy(ins.data_atualizacao, dt);
        strcpy(ins.id_curso, id_curso);
        strcpy(ins.id_usuario, id_usuario);
        // Insere na arvore
        btree_insert(id_curso_id_user, &inscricoes_idx);
        btree_insert(data_grid, &data_curso_usuario_idx);
        ins.status = 'A';
        escrever_registro_inscricao(
            ins, qtd_registros_inscricoes); // escreve no arquivo de dados
        qtd_registros_inscricoes++;
        /* Ordena o arquivo de indices */
        printf(SUCESSO);
        return;
      }
    } else {
      strncpy(chave, id_curso, TAM_ID_CURSO);
      chave[TAM_CHAVE_CURSOS_IDX] = '\0';
      strcat(chave, id_usuario);
      printf(ERRO_PK_REPETIDA, chave);
      return;
    }
  }
}
void cadastrar_categoria_menu(char *titulo, char *categoria) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Curso curso;
  char result[TAM_CHAVE_TITULO_IDX + 1];
  char rrn_aux[5];
  int rrn;
  rrn_aux[4] = '\0';
  char id_course[TAM_ID_CURSO];
  id_course[TAM_ID_CURSO - 1] = '\0';
  char resultCurso[TAM_CHAVE_CURSOS_IDX + 1];
  resultCurso[TAM_CHAVE_CURSOS_IDX] = '\0';
  bool existsCurso =
      btree_search(result, false, strupr(titulo), titulo_idx.rrn_raiz, &titulo_idx);
  if (!existsCurso) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
 
  strncpy(id_course, result + TAM_MAX_TITULO - 1, TAM_ID_CURSO - 1);
  btree_search(resultCurso, false, id_course, cursos_idx.rrn_raiz, &cursos_idx);
  
  strncpy(rrn_aux, resultCurso + TAM_ID_CURSO - 1, 4);
  rrn = atoi(rrn_aux);
  curso = recuperar_registro_curso(rrn);
 
  /* roda entre os cursos para conferir se a categoria não está sendo
   * repetida no curso */
  for (int i = 0; i < 3; i++) {
    if (strcmp(curso.categorias[i], categoria) == 0) {
      printf(ERRO_CATEGORIA_REPETIDA, curso.titulo,
             categoria); // categoria repetida
      return;
    } else {
      if (strcmp(curso.categorias[i], "") == 0) { // pode inserir a categoria
        strcpy(curso.categorias[i], categoria);
        break;
      }
    }
  }
  escrever_registro_curso(curso, rrn);
  inverted_list_insert(categoria, curso.id_curso,
                       &categorias_idx); // Insere na lista invertida para os
  // indices da categoria.
  printf(SUCESSO);
}
void atualizar_status_inscricoes_menu(char *id_usuario, char *titulo,
                                      char status) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  Inscricao ins;
  Curso c;
  char dt[TAM_DATETIME];
  char resultTitulo[TAM_CHAVE_TITULO_IDX + 1];
  char inscription[TAM_CHAVE_INSCRICOES_IDX + 1];
  char id_curso[TAM_ID_CURSO];
  char rrn_aux[5];
  int rrn;
  rrn_aux[4] = '\0';
  char resultInscription[TAM_CHAVE_INSCRICOES_IDX + 1];
  rrn_aux[4] = '\0';
  bool existsCurso = btree_search(resultTitulo, false, strupr(titulo),
                                  titulo_idx.rrn_raiz, &titulo_idx);
 
  /* Busca para conferir se a inscrição existe */
 
  /* Caso não ache o titulo */
  if (!existsCurso) {
    printf(ERRO_REGISTRO_NAO_ENCONTRADO);
    return;
  }
  strncpy(id_curso, resultTitulo + TAM_MAX_TITULO - 1, TAM_ID_CURSO - 1);
  id_curso[TAM_ID_CURSO - 1] = '\0';
  sprintf(inscription, "%s%s", id_curso, id_usuario);
  bool existsInscricao = btree_search(resultInscription, false, inscription,
                                      inscricoes_idx.rrn_raiz, &inscricoes_idx);
 
  /* Recupera a inscrição para escrever no registro de inscrições */
  strncpy(rrn_aux, resultInscription + TAM_ID_CURSO + TAM_ID_USUARIO - 2, 4);
  rrn = atoi(rrn_aux);
  ins = recuperar_registro_inscricao(rrn);
  current_datetime(dt);
  ins.status = status;
  strcpy(ins.data_atualizacao, dt);
  escrever_registro_inscricao(ins, rrn);
  printf(SUCESSO);
}
 
/* Busca */
void buscar_usuario_id_menu(char *id_usuario) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char result[TAM_CHAVE_USUARIOS_IDX + 1];
  char rrn_aux[5];
  int rrn;
  rrn_aux[4] = '\0';
  printf(RRN_NOS);
  /* Busca o usuario na arvore*/
  bool exist = btree_search(result, true, id_usuario, usuarios_idx.rrn_raiz,
                            &usuarios_idx);
  printf("\n");
  if (exist) {
    strncpy(rrn_aux, result + TAM_ID_USUARIO - 1, 4);
    rrn = atoi(rrn_aux);
    exibir_usuario(rrn);
    return;
  }
  printf(ERRO_REGISTRO_NAO_ENCONTRADO);
}
void buscar_curso_id_menu(char *id_curso) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char result[TAM_CHAVE_CURSOS_IDX + 1];
  char rrn_aux[5];
  int rrn;
  rrn_aux[4] = '\0';
  printf(RRN_NOS);
  /* busca o curso na arvore*/
  bool exist =
      btree_search(result, true, id_curso, cursos_idx.rrn_raiz, &cursos_idx);
  printf("\n");
  if (exist) {
    strncpy(rrn_aux, result + TAM_ID_CURSO - 1, 4);
    rrn = atoi(rrn_aux);
    exibir_curso(rrn);
    return;
  }
  printf(ERRO_REGISTRO_NAO_ENCONTRADO);
}
void buscar_curso_titulo_menu(char *titulo) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char result[TAM_CHAVE_CURSOS_IDX + 1];
  char id_course[TAM_ID_CURSO];
  char resultTitle[TAM_CHAVE_TITULO_IDX + 1];
  char rrn_aux[5];
  int rrn;
  rrn_aux[4] = '\0';
  /* Busca o titulo na arvore*/
  printf(RRN_NOS);
  bool exist = btree_search(resultTitle, true, strupr(titulo),
                            titulo_idx.rrn_raiz, &titulo_idx);
  printf("\n");
  if (exist) {
    strncpy(id_course, resultTitle + TAM_MAX_TITULO - 1, TAM_ID_CURSO);
    id_course[TAM_ID_CURSO - 1] = '\0';
    btree_search(result, false, id_course, cursos_idx.rrn_raiz, &cursos_idx);
    strncpy(rrn_aux, result + TAM_ID_CURSO - 1, 4);
    rrn = atoi(rrn_aux);
    exibir_curso(rrn);
    return;
  }
  printf(ERRO_REGISTRO_NAO_ENCONTRADO);
}
 
/* Listagem */
void listar_usuarios_id_menu() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  if (qtd_registros_usuarios == 0) {
    printf(ERRO_ARQUIVO_VAZIO);
    return;
  }
  btree_print_in_order(NULL, NULL, exibir_btree_usuario, usuarios_idx.rrn_raiz,
                       &usuarios_idx);
}
void listar_cursos_categorias_menu(char *categoria) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char ids[qtd_registros_cursos][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX];
  char key[TAM_CHAVE_CURSOS_IDX + 1];
  int result;
  int quantidade_cursos;
  bool existe; // auxiliar para ver se a categoria existe na lista de categorias
  // secundarias
  existe = inverted_list_secondary_search(&result, true, categoria,
                                          &categorias_idx);
  if (existe == false) { // caso não exista
    printf(AVISO_NENHUM_REGISTRO_ENCONTRADO);
    return;
  } else { // caso exista
    /* Passa o array ids que conterá todos os ids da lista de categorias
     * primarias */
    quantidade_cursos =
        inverted_list_primary_search(ids, true, result, NULL, &categorias_idx);
    for (int i = 0; i < quantidade_cursos; i++) {
      btree_search(key, false, ids[i], cursos_idx.rrn_raiz, &cursos_idx);
      /* Exibe o curso */
      exibir_btree_curso(key);
    }
  }
}
void listar_inscricoes_periodo_menu(char *data_inicio, char *data_fim) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  if (qtd_registros_inscricoes == 0) {
    printf(ERRO_ARQUIVO_VAZIO);
    return;
  }
  btree_print_in_order(data_inicio, data_fim, exibir_btree_data_curso_usuario,
                       data_curso_usuario_idx.rrn_raiz,
                       &data_curso_usuario_idx);
}
 
/* Liberar espaço */
void liberar_espaco_menu() {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  printf(ERRO_NAO_IMPLEMENTADO, "liberar_espaco_menu");
}
 
/* Imprimir arquivos de dados */
void imprimir_arquivo_usuarios_menu() {
  if (qtd_registros_usuarios == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_USUARIOS);
}
void imprimir_arquivo_cursos_menu() {
  if (qtd_registros_cursos == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_CURSOS);
}
void imprimir_arquivo_inscricoes_menu() {
  if (qtd_registros_inscricoes == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_INSCRICOES);
}
 
/* Imprimir índices primários */
void imprimir_usuarios_idx_menu() {
  if (usuarios_idx.qtd_nos == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_USUARIOS_IDX);
}
void imprimir_cursos_idx_menu() {
  if (cursos_idx.qtd_nos == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_CURSOS_IDX);
}
void imprimir_inscricoes_idx_menu() {
  if (inscricoes_idx.qtd_nos == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_INSCRICOES_IDX);
}
 
/* Imprimir índices secundários */
void imprimir_titulo_idx_menu() {
  if (titulo_idx.qtd_nos == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_TITULO_IDX);
}
void imprimir_data_curso_usuario_idx_menu() {
  if (data_curso_usuario_idx.qtd_nos == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_DATA_CURSO_USUARIO_IDX);
}
void imprimir_categorias_secundario_idx_menu() {
  if (categorias_idx.qtd_registros_secundario == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_CATEGORIAS_SECUNDARIO_IDX);
}
void imprimir_categorias_primario_idx_menu() {
  if (categorias_idx.qtd_registros_primario == 0)
    printf(ERRO_ARQUIVO_VAZIO);
  else
    printf("%s\n", ARQUIVO_CATEGORIAS_PRIMARIO_IDX);
}
 
/* Função de comparação entre chaves do índice usuarios_idx */
int order_usuarios_idx(const void *key, const void *elem) {
  return strncmp(key, elem, TAM_ID_USUARIO - 1);
}
 
/* Função de comparação entre chaves do índice cursos_idx */
int order_cursos_idx(const void *key, const void *elem) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  return strncmp(key, elem, TAM_ID_CURSO - 1);
}
 
/* Função de comparação entre chaves do índice inscricoes_idx */
int order_inscricoes_idx(const void *key, const void *elem) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  if (strncmp(key, elem, TAM_ID_CURSO - 1) == 0) {
    return strncmp(key, elem, TAM_ID_CURSO - 1);
  } else {
    return strncmp(key + TAM_ID_CURSO - 1, elem + TAM_ID_CURSO - 1,
                   TAM_ID_USUARIO - 1);
  }
}
 
/* Função de comparação entre chaves do índice titulo_idx */
int order_titulo_idx(const void *key, const void *elem) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  /* strnlen pelo fato de ter # no final do titulo */
  return strncmp(key, elem, strnlen(key, TAM_MAX_TITULO - 1));
}
 
/* Funções de comparação entre chaves do índice data_curso_usuario_idx */
int order_data_curso_usuario_idx(const void *key, const void *elem) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  if (strncmp(key, elem, TAM_DATE - 1) == 0) {
    if (strncmp(key + TAM_DATE - 1, elem + TAM_DATE - 1, TAM_ID_CURSO - 1) ==
        0) {
      return strncmp(key + TAM_DATE - 1 + TAM_ID_CURSO - 1,
                     elem + TAM_DATE - 1 + TAM_ID_CURSO - 1,
                     TAM_ID_USUARIO - 1);
    } else {
      return strncmp(key + TAM_DATE - 1, elem + TAM_DATE - 1, TAM_ID_CURSO - 1);
    }
  } else {
    return strncmp(key, elem, TAM_DATE - 1);
  }
}
 
/* Função de comparação entre chaves do índice secundário de categorias_idx
 */
int order_categorias_idx(const void *key, const void *elem) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  /* strnlen pelo fato dos # da categoria */
  return strncmp(key, elem,
                 strnlen(key, TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX - 1));
}
 
/* Funções de manipulação de Lista Invertida */
void inverted_list_insert(char *chave_secundaria, char *chave_primaria,
                          inverted_list *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  int firstIndice;
  int ultimoIndice;
  char prox_indice[5];
  int valor_negativo = -1;
  char key_aux[strlen(chave_secundaria) + 1];
  char key_aux_[5];
  /* True é retornado se a categoria já existe no arquivo de indices secundarios
   */
  /* UltimoIndice é onde está -1 */
  if (inverted_list_secondary_search(&firstIndice, false,
                                     strupr(chave_secundaria), t) == true) {
    inverted_list_primary_search(NULL, false, firstIndice, &ultimoIndice, t);
    strcpy(t->arquivo_primario +
                (t->qtd_registros_primario * (t->tam_chave_primaria + 4)),
            chave_primaria);
    sprintf(prox_indice, "%04d", valor_negativo);
    strncpy(t->arquivo_primario +
               (ultimoIndice * (t->tam_chave_primaria + 4)) +
               t->tam_chave_primaria,
           prox_indice, 4);
    sprintf(t->arquivo_primario +
                (t->qtd_registros_primario * (t->tam_chave_primaria + 4)) +
                t->tam_chave_primaria,
            "%04d", t->qtd_registros_primario);
    t->qtd_registros_primario++;
  }
  /* Retorna a quantidade de cursos */
  /* False é retornado quando a categoria não existe no arquivo de indices
   * secundarios */
  /* Caso a categoria não exista no arquivo de indices secundario */
  if (inverted_list_secondary_search(&firstIndice, false, chave_secundaria,
                                     t) == false) {
    strcpy(key_aux, strupr(chave_secundaria));
    strpadright(key_aux, '#', t->tam_chave_secundaria);
    sprintf(prox_indice, "%04d", valor_negativo);
    strcpy(t->arquivo_secundario +
                (t->qtd_registros_secundario * (t->tam_chave_secundaria + 4)),
            key_aux);
    sprintf(key_aux_, "%04d", t->qtd_registros_primario);
    strcpy(t->arquivo_secundario +
               (t->qtd_registros_secundario) * (t->tam_chave_secundaria + 4) +
               t->tam_chave_secundaria,
           key_aux_);
           /* adiciona o - para o indice */
    sprintf(prox_indice, "%04d", valor_negativo);
    strcpy(t->arquivo_primario +
                (t->qtd_registros_primario * (t->tam_chave_primaria + 4)),
            chave_primaria);
    strcpy(t->arquivo_primario +
               (t->qtd_registros_primario * (t->tam_chave_primaria + 4)) +
               t->tam_chave_primaria,
           prox_indice);
    t->qtd_registros_primario++;
    t->qtd_registros_secundario++;
    /* ordena as categorias */
    qsort(t->arquivo_secundario, t->qtd_registros_secundario,
          t->tam_chave_secundaria + 4, t->compar);
  }
}
bool inverted_list_secondary_search(int *result, bool exibir_caminho,
                                    char *chave_secundaria, inverted_list *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  char rrn[5];
  int indices;
  char registro[TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX + 4];
  registro[TAM_CHAVE_CATEGORIAS_SECUNDARIO_IDX] = '\0';
  rrn[4] = '\0';
 
  if (exibir_caminho) {
    printf(RRN_REGS_SECUNDARIOS);
  }
 /* Busca na lista invertida */
  if (inverted_list_binary_search(&indices, exibir_caminho, chave_secundaria,
                                  t) == true) {
    if(result != NULL) {
      strncpy(registro,
              t->arquivo_secundario + indices * (t->tam_chave_secundaria + 4),
              t->tam_chave_secundaria + 4);
      strncpy(rrn, registro + t->tam_chave_secundaria, 4);
      *result = atoi(rrn);
    }
    return true;
  }
  return false;
}
int inverted_list_primary_search(
    char result[][TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX], bool exibir_caminho,
    int indice, int *indice_final, inverted_list *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  int contador = 0;
  int caminho[t->qtd_registros_primario]; // armazena os indices
  char rrn[5];
  int indices;
  int indice_final_;
  char registro[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX + 5];
  registro[TAM_CHAVE_CATEGORIAS_PRIMARIO_IDX] = '\0';
  rrn[4] = '\0';
  if (exibir_caminho) {
    printf(RRN_REGS_PRIMARIOS);
  }
 
  /* Percorre por todos os indices das categorias */
  for (int i = indice; i != -1; i = indice) {
    strncpy(registro, t->arquivo_primario + i * (t->tam_chave_primaria + 4),
            t->tam_chave_primaria + 4);
    strncpy(rrn, registro + t->tam_chave_primaria, 4);
    caminho[contador] = i;
    if (result != NULL) {
      strncpy(result[contador], registro, t->tam_chave_primaria);
    }
    contador++; // aumenta a quantidade de chaves encontradas
    indice_final_ = indice;
    indice = atoi(rrn);
  }
   if (exibir_caminho) {
    for (unsigned j = 0; j < contador; j++) {
      printf(" %d", caminho[j]);
    }
    printf("\n");
  }
  /* Caso queira alterar o indice final */
  if (indice_final != NULL) {
    *indice_final = indice_final_;
  }
  /* Printa os registros percorridos */
  return contador;
}
 
bool inverted_list_binary_search(int *result, bool exibir_caminho, char *chave,
                                 inverted_list *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  int caminho[t->qtd_registros_secundario];
  int position = 0;
  int inicio = 0; // posição inicial (mais a esquerda)
  int fim = t->qtd_registros_secundario - 1;
  int m;
  char elementoMeio[t->tam_chave_secundaria + 1];
  elementoMeio[t->tam_chave_secundaria] = '\0';
  while (inicio <= fim) {
    m = inicio + ((fim - inicio + 1) / 2); // meio do array
    caminho[position++] = m;
    strncpy(elementoMeio,
            t->arquivo_secundario + ((m) * (t->tam_chave_secundaria + 4)), t->tam_chave_secundaria); // elemento do meio do array
    int comparacao = t->compar(strupr(chave), elementoMeio);
    if (comparacao == 0) {
      if (result != NULL) {
        *result = m;
      }
      if (exibir_caminho) {
        for (int i = 0; i < position; i++) {
          printf(" %d", caminho[i]);
        }
        printf("\n");
      }
      return true;
      } else if (comparacao < 0) {
        fim = m - 1;
      }
      /* Caso o elemento esteja para a direita */
      else {
        inicio = m + 1;
      }
  }
  if (exibir_caminho) {
    for (int i = 0; i < position; i++) {
      printf(" %d", caminho[i]);
    }
    printf("\n");
  }
  return false;
}
 
/* Funções de manipulação de Árvores-B */
void btree_insert(char *chave, btree *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  /* Primeira situação: arvore vazia */
  if (t->rrn_raiz == -1) {
    btree_node node = btree_node_malloc(t);
    t->qtd_nos = 0;
    t->qtd_nos++;
    t->rrn_raiz = 0;
    node.qtd_chaves = 0;
    node.qtd_chaves++;
    node.folha = true;
    node.this_rrn = 0;
    strncpy(node.chaves[0], chave, t->tam_chave);
    btree_write(node, t);
    btree_node_free(node);
    return;
  }
  promovido_aux promoted = btree_insert_aux(chave, t->rrn_raiz, t);
  /* dividiu o nó e ai tem que criar nó novo*/
  if (promoted.chave_promovida[0] != '\0') {
    /* cria nó novo */
    btree_node node = btree_node_malloc(t);
    node.qtd_chaves = 0;
    node.qtd_chaves++;
    node.folha = false;
    node.this_rrn = t->qtd_nos;
    node.filhos[1] = promoted.filho_direito;
    node.filhos[0] = t->rrn_raiz;
    t->rrn_raiz = t->qtd_nos;
    t->qtd_nos++;
    strcpy(node.chaves[0], promoted.chave_promovida);
    btree_write(node, t);
    btree_node_free(node);
    return;
  }
}
promovido_aux btree_insert_aux(char *chave, int rrn, btree *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  int position;
  char rrn_aux[5];
  rrn_aux[4] = '\0';
  char key[t->tam_chave + 1];
  promovido_aux node_promoted;
  node_promoted.chave_promovida[0] = '\0';
  if (rrn == -1) {
    node_promoted.chave_promovida[0] = '\0';
    return node_promoted;
  }
  btree_node node = btree_read(rrn, t);
  btree_binary_search(&position, false, chave, &node, t);
  node_promoted.filho_direito = -1;
  if (node.folha == false) {
    node_promoted = btree_insert_aux(chave, node.filhos[position], t);
    if (node_promoted.chave_promovida[0] != '\0') {
      if (node.qtd_chaves < btree_order - 1) {
        btree_binary_search(&position, false, node_promoted.chave_promovida,
                            &node, t);
        for (int i = node.qtd_chaves; i > position; i--) {
          strncpy(node.chaves[i], node.chaves[i - 1], t->tam_chave);
          node.filhos[i + 1] = node.filhos[i];
          node.chaves[i][t->tam_chave] = '\0';
        }
        strncpy(node.chaves[position], node_promoted.chave_promovida,
                t->tam_chave);
        node.chaves[position][t->tam_chave] = '\0';
        node.filhos[position + 1] = node_promoted.filho_direito;
        node.qtd_chaves++;
        btree_write(node, t);
        node_promoted.chave_promovida[0] = '\0';
      } else {
        node_promoted = btree_divide(node_promoted, &node, position, t);
      }
    } else {
      btree_node_free(node);
      return node_promoted;
    }
  } else {
    if (node.qtd_chaves == btree_order - 1) {
      strncpy(node_promoted.chave_promovida, chave, t->tam_chave);
      node_promoted.chave_promovida[t->tam_chave] = '\0';
      node_promoted = btree_divide(node_promoted, &node, position, t);
    } else {
      for (int i = node.qtd_chaves; i > position; i--) {
        strncpy(node.chaves[i], node.chaves[i - 1], t->tam_chave);
        node.chaves[i][t->tam_chave] = '\0';
      }
      strncpy(node.chaves[position], chave, t->tam_chave);
      node.chaves[position][t->tam_chave] = '\0';
      node.qtd_chaves++;
      btree_write(node, t);
      btree_node_free(node);
      return node_promoted;
    }
  }
  btree_node_free(node);
  return node_promoted;
}
promovido_aux btree_divide(promovido_aux promo, btree_node *node, int i,
                           btree *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  btree_node node_aux = btree_node_malloc(t);
  char elemento[t->tam_chave];
  int meio = btree_order / 2;
  int contador = 1;
  node_aux.qtd_chaves = 0;
  node_aux.this_rrn = t->qtd_nos;
  t->qtd_nos++;
  node_aux.folha = node->folha;
  /* se a posicao é menor que o meio*/
  if (i < meio) {
    /* pega a ultima chave e passa pro nó novo*/
    strncpy(node_aux.chaves[0], node->chaves[node->qtd_chaves - 1],
            t->tam_chave);
    node_aux.chaves[0][t->tam_chave] = '\0';
    node->chaves[node->qtd_chaves - 1][0] = '\0';
    node->qtd_chaves--;
    node_aux.qtd_chaves++;
    /* Esquema para abrir espaço no nó*/
    for (int j = node->qtd_chaves; j > i; j--) {
      strncpy(node->chaves[j], node->chaves[j - 1], t->tam_chave);
      node->chaves[j][t->tam_chave] = '\0';
    }
    strncpy(node->chaves[i], promo.chave_promovida, t->tam_chave);
    node->chaves[i][t->tam_chave] = '\0';
    node->qtd_chaves++;
    node_aux.filhos[0] = node->filhos[node->qtd_chaves - 1];
    node_aux.filhos[1] = node->filhos[node->qtd_chaves];
    node->filhos[node->qtd_chaves - 1] = -1;
    node->filhos[node->qtd_chaves] = -1;
    strncpy(promo.chave_promovida, node->chaves[node->qtd_chaves - 1],
            t->tam_chave);
    promo.chave_promovida[t->tam_chave] = '\0';
    node->chaves[node->qtd_chaves - 1][0] = '\0';
    node->qtd_chaves--;
    promo.filho_direito = node_aux.this_rrn;
    /* se a posicao é igual ao meio*/
  } else if (i == meio) {
 
    strncpy(node_aux.chaves[0], node->chaves[node->qtd_chaves - 1],
            t->tam_chave);
    node_aux.chaves[0][t->tam_chave] = '\0';
    node->chaves[node->qtd_chaves - 1][0] = '\0';
    node->qtd_chaves--;
    node_aux.qtd_chaves++;
 
    node_aux.filhos[1] = node->filhos[node->qtd_chaves + 1];
    node_aux.filhos[0] = promo.filho_direito;
    node->filhos[node->qtd_chaves + 1] = -1;
    promo.filho_direito = node_aux.this_rrn;
  }
    /* se a posicao é maior ao meio*/
   else {
    strncpy(node_aux.chaves[0], promo.chave_promovida, t->tam_chave);
    node_aux.chaves[0][t->tam_chave] = '\0';
    node_aux.qtd_chaves++;
    node_aux.filhos[0] = node->filhos[node->qtd_chaves];
    node_aux.filhos[1] = promo.filho_direito;
    strncpy(promo.chave_promovida, node->chaves[node->qtd_chaves - 1],
            t->tam_chave);
    promo.chave_promovida[t->tam_chave] = '\0';
    node->chaves[node->qtd_chaves - 1][0] = '\0';
    node->qtd_chaves--;
    node->filhos[node->qtd_chaves + 1] = -1;
    promo.filho_direito = node_aux.this_rrn;
  }
  btree_write(node_aux, t);
  btree_write(*node, t);
  btree_node_free(node_aux);
  return promo;
}
void btree_delete(char *chave, btree *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  printf(ERRO_NAO_IMPLEMENTADO, "btree_delete");
}
bool btree_delete_aux(char *chave, int rrn, btree *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  printf(ERRO_NAO_IMPLEMENTADO, "btree_delete_aux");
}
bool btree_borrow_or_merge(btree_node *node, int i, btree *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  printf(ERRO_NAO_IMPLEMENTADO, "btree_borrow_or_merge");
}
bool btree_search(char *result, bool exibir_caminho, char *chave, int rrn,
                  btree *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  bool valor;
  if (rrn == -1) {
    return false;
  }
  int position;
  btree_node node = btree_read(rrn, t);
  if (exibir_caminho) {
    printf(" %d", node.this_rrn);
  }
  if (btree_binary_search(&position, exibir_caminho, chave, &node, t) == true) {
    if (result != NULL) {
      strcpy(result, node.chaves[position]);
    }
    btree_node_free(node);
    return true;
  } else if (btree_binary_search(&position, false, chave, &node, t) == false) {
    valor =
        btree_search(result, exibir_caminho, chave, node.filhos[position], t);
    btree_node_free(node);
    return valor;
  }
}
bool btree_binary_search(int *result, bool exibir_caminho, char *chave,
                         btree_node *node, btree *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  int caminho[node->qtd_chaves];
  int position = 0;
  int inicio = 0; // posição inicial (mais a esquerda)
  int fim = node->qtd_chaves - 1;
  int m;
  if (exibir_caminho)
    printf(" (");
  while (inicio <= fim) {
    m = inicio + ((fim - inicio + 1) / 2); // meio do array
    caminho[position++] = m;
    int comparacao = t->compar(chave, node->chaves[m]);
    if (comparacao == 0) {
      if (result != NULL) {
        *result = m;
      }
      if (exibir_caminho) {
        for (int i = 0; i < position; i++) {
          printf(" %d", caminho[i]);
        }
        printf(")");
      }
      return true; // para o retorno do tipo genérico
    } else if (comparacao < 0) {
      fim = m - 1;
    }
    /* Caso o elemento esteja para a direita */
    else {
      inicio = m + 1;
    }
  }
  if (result != NULL) {
    if (t->compar(chave, node->chaves[m]) <= 0) {
      *result = m;
    } else {
      *result = m + 1;
    }
  }
  if (exibir_caminho) {
    for (int i = 0; i < position; i++) {
      printf(" %d", caminho[i]);
    }
    printf(")");
  }
  return false;
}
bool btree_print_in_order(char *chave_inicio, char *chave_fim,
                          bool (*exibir)(char *chave), int rrn, btree *t) {
  /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  if (rrn == -1)
    return true;
  btree_node node = btree_read(rrn, t);
  if (chave_inicio == NULL && chave_fim == NULL) {
    for (int i = 0; i <= node.qtd_chaves; i++) {
      btree_print_in_order(NULL, NULL, exibir, node.filhos[i], t);
      if (i < node.qtd_chaves)
        exibir(node.chaves[i]);
    }
  } else {
    for (int i = 0; i <= node.qtd_chaves; i++) {
      btree_print_in_order(NULL, NULL, exibir, node.filhos[i], t);
      if (i < node.qtd_chaves) {
        if (strncmp(node.chaves[i], chave_inicio, t->tam_chave) >= 0 &&
            strncmp(node.chaves[i], chave_fim, t->tam_chave) <= 0) {
          exibir(node.chaves[i]);
        }
      }
    }
  }
  btree_node_free(node);
  return true;
}
btree_node btree_read(int rrn, btree *t) {
   /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  // Declaração das variáveis locais
  int qtd_chaves;
  char kids[4];
  char keys[(btree_order - 1) * 3];
  char boole[2];
  boole[1] = '\0';
  kids[3] = '\0';
  char qtd[4];
  qtd[3] = '\0';
  char filhos;
  char aux[4];
  char chave[t->tam_chave + 1];

  // Alocação do novo nó da árvore e inicialização de algumas informações
  btree_node node = btree_node_malloc(t);
  char auxiliar[btree_register_size(t) + 1];

  // Leitura das informações do arquivo para a variável auxiliar
  strncpy(auxiliar, t->arquivo + rrn * btree_register_size(t),
          btree_register_size(t));
  auxiliar[btree_register_size(t)] = '\0';

  // Leitura da quantidade de chaves do nó a partir do início da variável auxiliar
  strncpy(qtd, auxiliar, 3);
  qtd_chaves = atoi(qtd);
  node.qtd_chaves = qtd_chaves;
  node.this_rrn = rrn;

  // Leitura da informação sobre o nó ser folha ou não
  strncpy(boole, auxiliar + 3 + ((btree_order - 1) * (t->tam_chave)), 1);
  if (strncmp(boole, "T", 1) == 0) {
    node.folha = true;
  } else {
    node.folha = false;
  }

  // Leitura dos filhos do nó
  for (int i = 0; i < btree_order; i++) {
    strncpy(kids, auxiliar + 3 + (btree_order - 1) * t->tam_chave + 1 + i * 3,
            3);
    if (kids[0] == '*') {
      node.filhos[i] = -1;
    } else {
      strncpy(aux, kids, 3);
      aux[3] = '\0';
      node.filhos[i] = atoi(aux);
    }
  }

  // Leitura das chaves do nó
  for (int i = 0; i < btree_order - 1; i++) {
    strncpy(chave, auxiliar + 3 + i * t->tam_chave, t->tam_chave);
    chave[t->tam_chave] = '\0';
    if (chave[0] == '#') {
      continue;
    } else {
      strncpy(node.chaves[i], chave, t->tam_chave);
      node.chaves[i][t->tam_chave] = '\0';
    }
  }

  // Retorna o nó lido do arquivo
  return node;
}

void btree_write(btree_node no, btree *t) {
   /* <<< COMPLETE AQUI A IMPLEMENTAÇÃO >>> */
  // Função para escrever os dados do nó em um arquivo
  // Recebe um nó e um ponteiro para a árvore B correspondente

  // Aloca espaço suficiente para a string que representará os dados do nó
  char *key = malloc(sizeof(char) * (btree_register_size(t) + 1));

  // Aloca espaço suficiente para a string auxiliar, usada para preencher com '#' as chaves vazias
  char *auxiliar_espaco = malloc(sizeof(char) * (t->tam_chave + 1));
  auxiliar_espaco[0] = '\0';

  // Preenche a string auxiliar com '#' para o tamanho correto
  strpadright(auxiliar_espaco, '#', t->tam_chave);
  auxiliar_espaco[t->tam_chave] = '\0';

  // Cria a string 'key' com as informações do nó, no formato específico da árvore B
  sprintf(key, "%03d", no.qtd_chaves); // insere a quantidade de chaves do nó
  for (int i = 0; i < btree_order - 1; i++) { // percorre todas as chaves do nó
    if (no.chaves[i][0] == '\0') { // se a chave estiver vazia, preenche com '#'
      strcat(key, auxiliar_espaco);
    } else { // caso contrário, adiciona a chave na string
      no.chaves[i][t->tam_chave] = '\0'; // garante que a chave tem o tamanho correto
      strcat(key, no.chaves[i]);
    }
  }
  if (no.folha == true) { // se o nó for folha, adiciona T na string
    strcat(key, "T");
  } else { // caso contrário, adiciona 'F'
    strcat(key, "F");
  }
  for (int i = 0; i < btree_order; i++) { // percorre todos os filhos do nó
    if (no.filhos[i] == -1) { // se o filho não existir, preenche com '***'
      strcat(key, "***");
    } else { // caso contrário, adiciona o RRN do filho na string
      sprintf(key + 3 + (btree_order - 1) * t->tam_chave + 1 + i * 3, "%03d",
              no.filhos[i]);
    }
  }

  strncpy(t->arquivo + no.this_rrn * btree_register_size(t), key,
          btree_register_size(t));

  // Libera a memória alocada para as strings auxiliares
  free(key);
  free(auxiliar_espaco);
}

int btree_register_size(btree *t) {
  int chaves_ordenadas = (btree_order - 1) * t->tam_chave;
  return 3 + chaves_ordenadas + 1 + (btree_order * 3);
}
btree_node btree_node_malloc(btree *t) {
  btree_node no;
  no.chaves = malloc((btree_order - 1) * sizeof(char *));
  for (int i = 0; i < btree_order - 1; ++i) {
    no.chaves[i] = malloc(t->tam_chave + 1);
    no.chaves[i][0] = '\0';
  }
  no.filhos = malloc(btree_order * sizeof(int));
  for (int i = 0; i < btree_order; ++i)
    no.filhos[i] = -1;
  return no;
}
void btree_node_free(btree_node no) {
  for (int i = 0; i < btree_order - 1; ++i)
    free(no.chaves[i]);
  free(no.chaves);
  free(no.filhos);
}
char *strpadright(char *str, char pad, unsigned size) {
  for (unsigned i = strlen(str); i < size; ++i)
    str[i] = pad;
  str[size] = '\0';
  return str;
}
char *strupr(char *str) {
  for (char *p = str; *p; ++p)
    *p = toupper(*p);
  return str;
}
char *strlower(char *str) {
  for (char *p = str; *p; ++p)
    *p = tolower(*p);
  return str;
}
