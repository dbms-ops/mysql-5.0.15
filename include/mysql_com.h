/* Copyright (C) 2000 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/*
** Common definition between mysql server & client
*/

#ifndef _mysql_com_h
#define _mysql_com_h

#define NAME_LEN	64		/* Field/table name length */
#define HOSTNAME_LENGTH 60
#define USERNAME_LENGTH 16
#define SERVER_VERSION_LENGTH 60
#define SQLSTATE_LENGTH 5

#define LOCAL_HOST	"localhost"
#define LOCAL_HOST_NAMEDPIPE "."


#if defined(__WIN__) && !defined( _CUSTOMCONFIG_)
#define MYSQL_NAMEDPIPE "MySQL"
#define MYSQL_SERVICENAME "MySQL"
#endif /* __WIN__ */

/*
  You should add new commands to the end of this list, otherwise old
  servers won't be able to handle them as 'unsupported'.
*/

enum enum_server_command
{
  COM_SLEEP, COM_QUIT, COM_INIT_DB, COM_QUERY, COM_FIELD_LIST,
  COM_CREATE_DB, COM_DROP_DB, COM_REFRESH, COM_SHUTDOWN, COM_STATISTICS,
  COM_PROCESS_INFO, COM_CONNECT, COM_PROCESS_KILL, COM_DEBUG, COM_PING,
  COM_TIME, COM_DELAYED_INSERT, COM_CHANGE_USER, COM_BINLOG_DUMP,
  COM_TABLE_DUMP, COM_CONNECT_OUT, COM_REGISTER_SLAVE,
  COM_STMT_PREPARE, COM_STMT_EXECUTE, COM_STMT_SEND_LONG_DATA, COM_STMT_CLOSE,
  COM_STMT_RESET, COM_SET_OPTION, COM_STMT_FETCH,
  /* don't forget to update const char *command_name[] in sql_parse.cc */

  /* Must be last */
  COM_END
};


/*
  Length of random string sent by server on handshake; this is also length of
  obfuscated password, recieved from client
*/
#define SCRAMBLE_LENGTH 20
#define SCRAMBLE_LENGTH_323 8
/* length of password stored in the db: new passwords are preceeded with '*' */
#define SCRAMBLED_PASSWORD_CHAR_LENGTH (SCRAMBLE_LENGTH*2+1)
#define SCRAMBLED_PASSWORD_CHAR_LENGTH_323 (SCRAMBLE_LENGTH_323*2)


#define NOT_NULL_FLAG	1		/* Field can't be NULL */
#define PRI_KEY_FLAG	2		/* Field is part of a primary key */
#define UNIQUE_KEY_FLAG 4		/* Field is part of a unique key */
#define MULTIPLE_KEY_FLAG 8		/* Field is part of a key */
#define BLOB_FLAG	16		/* Field is a blob */
#define UNSIGNED_FLAG	32		/* Field is unsigned */
#define ZEROFILL_FLAG	64		/* Field is zerofill */
#define BINARY_FLAG	128		/* Field is binary   */

/* The following are only sent to new clients */
#define ENUM_FLAG	256		/* field is an enum */
#define AUTO_INCREMENT_FLAG 512		/* field is a autoincrement field */
#define TIMESTAMP_FLAG	1024		/* Field is a timestamp */
#define SET_FLAG	2048		/* field is a set */
#define NO_DEFAULT_VALUE_FLAG 4096	/* Field doesn't have default value */
#define NUM_FLAG	32768		/* Field is num (for clients) */
#define PART_KEY_FLAG	16384		/* Intern; Part of some key */
#define GROUP_FLAG	32768		/* Intern: Group field */
#define UNIQUE_FLAG	65536		/* Intern: Used by sql_yacc */
#define BINCMP_FLAG	131072		/* Intern: Used by sql_yacc */

#define REFRESH_GRANT		1	/* Refresh grant tables */
#define REFRESH_LOG		2	/* Start on new log file */
#define REFRESH_TABLES		4	/* close all tables */
#define REFRESH_HOSTS		8	/* Flush host cache */
#define REFRESH_STATUS		16	/* Flush status variables */
#define REFRESH_THREADS		32	/* Flush thread cache */
#define REFRESH_SLAVE           64      /* Reset master info and restart slave
					   thread */
#define REFRESH_MASTER          128     /* Remove all bin logs in the index
					   and truncate the index */

/* The following can't be set with mysql_refresh() */
#define REFRESH_READ_LOCK	16384	/* Lock tables for read */
#define REFRESH_FAST		32768	/* Intern flag */

/* RESET (remove all queries) from query cache */
#define REFRESH_QUERY_CACHE	65536
#define REFRESH_QUERY_CACHE_FREE 0x20000L /* pack query cache */
#define REFRESH_DES_KEY_FILE	0x40000L
#define REFRESH_USER_RESOURCES	0x80000L

#define CLIENT_LONG_PASSWORD	1	/* new more secure passwords */
#define CLIENT_FOUND_ROWS	2	/* Found instead of affected rows */
#define CLIENT_LONG_FLAG	4	/* Get all column flags */
#define CLIENT_CONNECT_WITH_DB	8	/* One can specify db on connect */
#define CLIENT_NO_SCHEMA	16	/* Don't allow database.table.column */
#define CLIENT_COMPRESS		32	/* Can use compression protocol */
#define CLIENT_ODBC		64	/* Odbc client */
#define CLIENT_LOCAL_FILES	128	/* Can use LOAD DATA LOCAL */
#define CLIENT_IGNORE_SPACE	256	/* Ignore spaces before '(' */
#define CLIENT_PROTOCOL_41	512	/* New 4.1 protocol */
#define CLIENT_INTERACTIVE	1024	/* This is an interactive client */
#define CLIENT_SSL              2048	/* Switch to SSL after handshake */
#define CLIENT_IGNORE_SIGPIPE   4096    /* IGNORE sigpipes */
#define CLIENT_TRANSACTIONS	8192	/* Client knows about transactions */
#define CLIENT_RESERVED         16384   /* Old flag for 4.1 protocol  */
#define CLIENT_SECURE_CONNECTION 32768  /* New 4.1 authentication */
#define CLIENT_MULTI_STATEMENTS 65536   /* Enable/disable multi-stmt support */
#define CLIENT_MULTI_RESULTS    131072  /* Enable/disable multi-results */
#define CLIENT_REMEMBER_OPTIONS	(((ulong) 1) << 31)

#define SERVER_STATUS_IN_TRANS     1	/* Transaction has started */
#define SERVER_STATUS_AUTOCOMMIT   2	/* Server in auto_commit mode */
#define SERVER_STATUS_MORE_RESULTS 4	/* More results on server */
#define SERVER_MORE_RESULTS_EXISTS 8    /* Multi query - next query exists */
#define SERVER_QUERY_NO_GOOD_INDEX_USED 16
#define SERVER_QUERY_NO_INDEX_USED      32
/*
  The server was able to fulfill the clients request and opened a
  read-only non-scrollable cursor for a query. This flag comes
  in reply to COM_STMT_EXECUTE and COM_STMT_FETCH commands.
*/
#define SERVER_STATUS_CURSOR_EXISTS 64
/*
  This flag is sent when a read-only cursor is exhausted, in reply to
  COM_STMT_FETCH command.
*/
#define SERVER_STATUS_LAST_ROW_SENT 128
#define SERVER_STATUS_DB_DROPPED        256 /* A database was dropped */
#define SERVER_STATUS_NO_BACKSLASH_ESCAPES 512

#define MYSQL_ERRMSG_SIZE	512
#define NET_READ_TIMEOUT	30		/* Timeout on read */
#define NET_WRITE_TIMEOUT	60		/* Timeout on write */
#define NET_WAIT_TIMEOUT	8*60*60		/* Wait for new query */

#define ONLY_KILL_QUERY         1

struct st_vio;					/* Only C */
typedef struct st_vio Vio;

#define MAX_TINYINT_WIDTH       3       /* Max width for a TINY w.o. sign */
#define MAX_SMALLINT_WIDTH      5       /* Max width for a SHORT w.o. sign */
#define MAX_MEDIUMINT_WIDTH     8       /* Max width for a INT24 w.o. sign */
#define MAX_INT_WIDTH           10      /* Max width for a LONG w.o. sign */
#define MAX_BIGINT_WIDTH        20      /* Max width for a LONGLONG */
#define MAX_CHAR_WIDTH		255	/* Max length for a CHAR colum */
#define MAX_BLOB_WIDTH		8192	/* Default width for blob */

typedef struct st_net {
#if !defined(CHECK_EMBEDDED_DIFFERENCES) || !defined(EMBEDDED_LIBRARY)
    Vio *vio; /*低层次网络IO描述符 V 虚拟*/
    /*
     * buff: 数据缓冲区起点
     * buff_end：数据缓冲区终点
     * write_pos：指向数据缓冲区下一次写操作将取出数据的位置
     * read_pos：指向缓冲区下一次读操作将放入数据的位置
     * */
  unsigned char *buff,*buff_end,*write_pos,*read_pos;
    /*
     * 操作系统套接字描述符编号
     * */
    my_socket fd;					/* For Perl DBI/dbd */

    /* max_packet：网络包缓冲区的当前大小。
     * max_packet_size：本连接上允许的包的大小范围；即：max-allowed-packet 配置的变量的值
     * */
    unsigned long max_packet,max_packet_size;
    /*
     * pkt_nr: 非压缩协议的当前包序号。包序号主要用于协议中的健全性检查
     * compress_pkt_nr：压缩协议的当前包序号
     * write_timeout：允许网络写操作在超时出错前占用的最长时间；net-write-timeout 变量配置的值
     * read_timeout：允许网络读操作在超时出错前占用的最长时间：net-read-timeout 变量配置的值
     * retry_count：在认定失败的网络 IO 操作为失败操作前应该重新尝试操作的次数。 即：net-retry-count 配置变量的数值
     * */
    unsigned int pkt_nr,compress_pkt_nr;
  unsigned int write_timeout, read_timeout, retry_count;
  int fcntl;
    /* 使用数据压缩时，设置为 1
     * */
    my_bool compress;
    /*
      The following variable is set if we are doing several queries in one
      command ( as in LOAD TABLE ... FROM MASTER ),
      and do not want to confuse the client with OK at the wrong time

      remain_in_buf: 使用压缩协议时，读对等点【reading peer】可能会尝试从可能超过包压缩长度的套接口中读取数据。
      可能读取下一个包的下一部分，本变量对于超额读取的字节数量进行追踪
      length：当前包的长度，不包含包头
      buf_length：包含当前的包的长度。
      where_b: read_pos - buff: 的数值，为缓冲区中当前读取未知的偏移量

    */
  unsigned long remain_in_buf,length, buf_length, where_b;
    /*
     * return_status: 执行与链接有关的THD线程描述符中的 server_status 变量
     * */
    unsigned int *return_status;
    /*
     * reading_or_writing: 在没有进行 IO操作时设置为 0 ，在读取时设置为 1，在写操作时，设置为 2；
     * 在 show processlist 查询时使用
     * */
    unsigned char reading_or_writing;
    /*
     *
     * */
    char save_char;
    /* 在大多数的时候，服务器上的成功操作是以 OK 包向客户端报告的
     * */
    my_bool no_send_ok;  /* For SPs and other things that do multiple stmts */
  my_bool no_send_eof; /* For SPs' first version read-only cursors */
  /*
    Set if OK packet is already sent, and we do not need to send error
    messages
  */
  my_bool no_send_error;
    /*
      Pointer to query object in query cache, do not equal NULL (0) for
      queries in cache that have not stored its results yet
    sqlstate[SQLSTATE_LENGTH+1]：缓冲区，包含 ODBC 和 JDBC驱动程序所使用SQL状态的数值
    */
#endif
    /*
     * last_error：发送给客户端的最后一个错误消息中的MySQL错误代码的数值，如果没有错误，则设置为 0
     *
     * */
    char last_error[MYSQL_ERRMSG_SIZE], sqlstate[SQLSTATE_LENGTH + 1];
  unsigned int last_errno;
    /*
     * 如果 IO 操作成功，设置为 0，协议上层逻辑错误设置为1，系统调用或者标准库错误设置为 2
     * */
    unsigned char error;
    /*
     * query_cache_query: 用于网络IO代码和查询高速缓存之间的恰当同步
     * */
    gptr query_cache_query;
    /*
     * report_error：向客户端报告错误设置为 1
     * */
    my_bool report_error; /* We should report error (we have unreported error) */
    /* return_errno: 如果向客户端报告MySQL错误代码值则应该设置为 1
     * */
    my_bool return_errno;
} NET;

#define packet_error (~(unsigned long) 0)

enum enum_field_types { MYSQL_TYPE_DECIMAL, MYSQL_TYPE_TINY,
			MYSQL_TYPE_SHORT,  MYSQL_TYPE_LONG,
			MYSQL_TYPE_FLOAT,  MYSQL_TYPE_DOUBLE,
			MYSQL_TYPE_NULL,   MYSQL_TYPE_TIMESTAMP,
			MYSQL_TYPE_LONGLONG,MYSQL_TYPE_INT24,
			MYSQL_TYPE_DATE,   MYSQL_TYPE_TIME,
			MYSQL_TYPE_DATETIME, MYSQL_TYPE_YEAR,
			MYSQL_TYPE_NEWDATE, MYSQL_TYPE_VARCHAR,
			MYSQL_TYPE_BIT,
                        MYSQL_TYPE_NEWDECIMAL=246,
			MYSQL_TYPE_ENUM=247,
			MYSQL_TYPE_SET=248,
			MYSQL_TYPE_TINY_BLOB=249,
			MYSQL_TYPE_MEDIUM_BLOB=250,
			MYSQL_TYPE_LONG_BLOB=251,
			MYSQL_TYPE_BLOB=252,
			MYSQL_TYPE_VAR_STRING=253,
			MYSQL_TYPE_STRING=254,
			MYSQL_TYPE_GEOMETRY=255

};

/* For backward compatibility */
#define CLIENT_MULTI_QUERIES    CLIENT_MULTI_STATEMENTS    
#define FIELD_TYPE_DECIMAL     MYSQL_TYPE_DECIMAL
#define FIELD_TYPE_NEWDECIMAL  MYSQL_TYPE_NEWDECIMAL
#define FIELD_TYPE_TINY        MYSQL_TYPE_TINY
#define FIELD_TYPE_SHORT       MYSQL_TYPE_SHORT
#define FIELD_TYPE_LONG        MYSQL_TYPE_LONG
#define FIELD_TYPE_FLOAT       MYSQL_TYPE_FLOAT
#define FIELD_TYPE_DOUBLE      MYSQL_TYPE_DOUBLE
#define FIELD_TYPE_NULL        MYSQL_TYPE_NULL
#define FIELD_TYPE_TIMESTAMP   MYSQL_TYPE_TIMESTAMP
#define FIELD_TYPE_LONGLONG    MYSQL_TYPE_LONGLONG
#define FIELD_TYPE_INT24       MYSQL_TYPE_INT24
#define FIELD_TYPE_DATE        MYSQL_TYPE_DATE
#define FIELD_TYPE_TIME        MYSQL_TYPE_TIME
#define FIELD_TYPE_DATETIME    MYSQL_TYPE_DATETIME
#define FIELD_TYPE_YEAR        MYSQL_TYPE_YEAR
#define FIELD_TYPE_NEWDATE     MYSQL_TYPE_NEWDATE
#define FIELD_TYPE_ENUM        MYSQL_TYPE_ENUM
#define FIELD_TYPE_SET         MYSQL_TYPE_SET
#define FIELD_TYPE_TINY_BLOB   MYSQL_TYPE_TINY_BLOB
#define FIELD_TYPE_MEDIUM_BLOB MYSQL_TYPE_MEDIUM_BLOB
#define FIELD_TYPE_LONG_BLOB   MYSQL_TYPE_LONG_BLOB
#define FIELD_TYPE_BLOB        MYSQL_TYPE_BLOB
#define FIELD_TYPE_VAR_STRING  MYSQL_TYPE_VAR_STRING
#define FIELD_TYPE_STRING      MYSQL_TYPE_STRING
#define FIELD_TYPE_CHAR        MYSQL_TYPE_TINY
#define FIELD_TYPE_INTERVAL    MYSQL_TYPE_ENUM
#define FIELD_TYPE_GEOMETRY    MYSQL_TYPE_GEOMETRY
#define FIELD_TYPE_BIT         MYSQL_TYPE_BIT


/* Shutdown/kill enums and constants */ 

/* Bits for THD::killable. */
#define MYSQL_SHUTDOWN_KILLABLE_CONNECT    (unsigned char)(1 << 0)
#define MYSQL_SHUTDOWN_KILLABLE_TRANS      (unsigned char)(1 << 1)
#define MYSQL_SHUTDOWN_KILLABLE_LOCK_TABLE (unsigned char)(1 << 2)
#define MYSQL_SHUTDOWN_KILLABLE_UPDATE     (unsigned char)(1 << 3)

enum mysql_enum_shutdown_level {
  /*
    We want levels to be in growing order of hardness (because we use number
    comparisons). Note that DEFAULT does not respect the growing property, but
    it's ok.
  */
  SHUTDOWN_DEFAULT = 0,
  /* wait for existing connections to finish */
  SHUTDOWN_WAIT_CONNECTIONS= MYSQL_SHUTDOWN_KILLABLE_CONNECT,
  /* wait for existing trans to finish */
  SHUTDOWN_WAIT_TRANSACTIONS= MYSQL_SHUTDOWN_KILLABLE_TRANS,
  /* wait for existing updates to finish (=> no partial MyISAM update) */
  SHUTDOWN_WAIT_UPDATES= MYSQL_SHUTDOWN_KILLABLE_UPDATE,
  /* flush InnoDB buffers and other storage engines' buffers*/
  SHUTDOWN_WAIT_ALL_BUFFERS= (MYSQL_SHUTDOWN_KILLABLE_UPDATE << 1),
  /* don't flush InnoDB buffers, flush other storage engines' buffers*/
  SHUTDOWN_WAIT_CRITICAL_BUFFERS= (MYSQL_SHUTDOWN_KILLABLE_UPDATE << 1) + 1,
  /* Now the 2 levels of the KILL command */
#if MYSQL_VERSION_ID >= 50000
  KILL_QUERY= 254,
#endif
  KILL_CONNECTION= 255
};


enum enum_cursor_type
{
  CURSOR_TYPE_NO_CURSOR= 0,
  CURSOR_TYPE_READ_ONLY= 1,
  CURSOR_TYPE_FOR_UPDATE= 2,
  CURSOR_TYPE_SCROLLABLE= 4
};


/* options for mysql_set_option */
enum enum_mysql_set_option
{
  MYSQL_OPTION_MULTI_STATEMENTS_ON,
  MYSQL_OPTION_MULTI_STATEMENTS_OFF
};

#define net_new_transaction(net) ((net)->pkt_nr=0)

#ifdef __cplusplus
extern "C" {
#endif

my_bool	my_net_init(NET *net, Vio* vio);
void	my_net_local_init(NET *net);
void	net_end(NET *net);
void	net_clear(NET *net);
my_bool net_realloc(NET *net, unsigned long length);
my_bool	net_flush(NET *net);
my_bool	my_net_write(NET *net,const char *packet,unsigned long len);
my_bool	net_write_command(NET *net,unsigned char command,
			  const char *header, unsigned long head_len,
			  const char *packet, unsigned long len);
int	net_real_write(NET *net,const char *packet,unsigned long len);
unsigned long my_net_read(NET *net);

/*
  The following function is not meant for normal usage
  Currently it's used internally by manager.c
*/
struct sockaddr;
int my_connect(my_socket s, const struct sockaddr *name, unsigned int namelen,
	       unsigned int timeout);

struct rand_struct {
  unsigned long seed1,seed2,max_value;
  double max_value_dbl;
};

#ifdef __cplusplus
}
#endif

  /* The following is for user defined functions */

enum Item_result {STRING_RESULT=0, REAL_RESULT, INT_RESULT, ROW_RESULT,
                  DECIMAL_RESULT};

typedef struct st_udf_args
{
  unsigned int arg_count;		/* Number of arguments */
  enum Item_result *arg_type;		/* Pointer to item_results */
  char **args;				/* Pointer to argument */
  unsigned long *lengths;		/* Length of string arguments */
  char *maybe_null;			/* Set to 1 for all maybe_null args */
  char **attributes;                    /* Pointer to attribute name */
  unsigned long *attribute_lengths;     /* Length of attribute arguments */
} UDF_ARGS;

  /* This holds information about the result */

typedef struct st_udf_init
{
  my_bool maybe_null;			/* 1 if function can return NULL */
  unsigned int decimals;		/* for real functions */
  unsigned long max_length;		/* For string functions */
  char	  *ptr;				/* free pointer for function data */
  my_bool const_item;			/* 0 if result is independent of arguments */
} UDF_INIT;

  /* Constants when using compression */
#define NET_HEADER_SIZE 4		/* standard header size */
#define COMP_HEADER_SIZE 3		/* compression header extra size */

  /* Prototypes to password functions */

#ifdef __cplusplus
extern "C" {
#endif

/*
  These functions are used for authentication by client and server and
  implemented in sql/password.c
*/

void randominit(struct rand_struct *, unsigned long seed1,
                unsigned long seed2);
double my_rnd(struct rand_struct *);
void create_random_string(char *to, unsigned int length, struct rand_struct *rand_st);

void hash_password(unsigned long *to, const char *password, unsigned int password_len);
void make_scrambled_password_323(char *to, const char *password);
void scramble_323(char *to, const char *message, const char *password);
my_bool check_scramble_323(const char *, const char *message,
                           unsigned long *salt);
void get_salt_from_password_323(unsigned long *res, const char *password);
void make_password_from_salt_323(char *to, const unsigned long *salt);

void make_scrambled_password(char *to, const char *password);
void scramble(char *to, const char *message, const char *password);
my_bool check_scramble(const char *reply, const char *message,
                       const unsigned char *hash_stage2);
void get_salt_from_password(unsigned char *res, const char *password);
void make_password_from_salt(char *to, const unsigned char *hash_stage2);
char *octet2hex(char *to, const char *str, unsigned int len);

/* end of password.c */

char *get_tty_password(char *opt_message);
const char *mysql_errno_to_sqlstate(unsigned int mysql_errno);

/* Some other useful functions */

my_bool my_init(void);
extern int modify_defaults_file(const char *file_location, const char *option,
                                const char *option_value,
                                const char *section_name, int remove_option);
int load_defaults(const char *conf_file, const char **groups,
		  int *argc, char ***argv);
my_bool my_thread_init(void);
void my_thread_end(void);

#ifdef _global_h
ulong STDCALL net_field_length(uchar **packet);
my_ulonglong net_field_length_ll(uchar **packet);
char *net_store_length(char *pkg, ulonglong length);
#endif

#ifdef __cplusplus
}
#endif

#define NULL_LENGTH ((unsigned long) ~0) /* For net_store_length */
#define MYSQL_STMT_HEADER       4
#define MYSQL_LONG_DATA_HEADER  6

#endif
