/* Copyright (C) 2000,2004 MySQL AB & MySQL Finland AB & TCX DataKonsult AB

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
 * Definitions for parameters to do with handler-routines
 * 存储引擎接口模块；接口实现了一些常见的操作：打开和关闭表、连续扫描记录、按照键值检索记录，存储与删除记录
 *
 * */

#ifdef USE_PRAGMA_INTERFACE
#pragma interface			/* gcc class implementation */
#endif

#include <ft_global.h>
#include <keycache.h>

#ifndef NO_HASH
#define NO_HASH				/* Not yet implemented */
#endif

#if defined(HAVE_BERKELEY_DB) || defined(HAVE_INNOBASE_DB) || \
    defined(HAVE_NDBCLUSTER_DB)
#define USING_TRANSACTIONS
#endif

// the following is for checking tables

#define HA_ADMIN_ALREADY_DONE	  1
#define HA_ADMIN_OK               0
#define HA_ADMIN_NOT_IMPLEMENTED -1
#define HA_ADMIN_FAILED		 -2
#define HA_ADMIN_CORRUPT         -3
#define HA_ADMIN_INTERNAL_ERROR  -4
#define HA_ADMIN_INVALID         -5
#define HA_ADMIN_REJECT          -6
#define HA_ADMIN_TRY_ALTER       -7
#define HA_ADMIN_WRONG_CHECKSUM  -8

/* Bits in table_flags() to show what database can do */

/*
  Can switch index during the scan with ::rnd_same() - not used yet.
  see mi_rsame/heap_rsame/myrg_rsame
*/
#define HA_READ_RND_SAME       (1 << 0)
#define HA_TABLE_SCAN_ON_INDEX (1 << 2) /* No separate data/index file */
#define HA_REC_NOT_IN_SEQ      (1 << 3) /* ha_info don't return recnumber;
                                           It returns a position to ha_r_rnd */
#define HA_CAN_GEOMETRY        (1 << 4)
/*
  Reading keys in random order is as fast as reading keys in sort order
  (Used in records.cc to decide if we should use a record cache and by
  filesort to decide if we should sort key + data or key + pointer-to-row
*/
#define HA_FAST_KEY_READ       (1 << 5)
#define HA_NULL_IN_KEY         (1 << 7) /* One can have keys with NULL */
#define HA_DUPP_POS            (1 << 8) /* ha_position() gives dup row */
#define HA_NO_BLOBS            (1 << 9) /* Doesn't support blobs */
#define HA_CAN_INDEX_BLOBS     (1 << 10)
#define HA_AUTO_PART_KEY       (1 << 11) /* auto-increment in multi-part key */
#define HA_REQUIRE_PRIMARY_KEY (1 << 12) /* .. and can't create a hidden one */
#define HA_NOT_EXACT_COUNT     (1 << 13)
/*
  INSERT_DELAYED only works with handlers that uses MySQL internal table
  level locks
*/
#define HA_CAN_INSERT_DELAYED  (1 << 14)
#define HA_PRIMARY_KEY_IN_READ_INDEX (1 << 15)
#define HA_NOT_DELETE_WITH_CACHE (1 << 18)
#define HA_NO_PREFIX_CHAR_KEYS (1 << 20)
#define HA_CAN_FULLTEXT        (1 << 21)
#define HA_CAN_SQL_HANDLER     (1 << 22)
#define HA_NO_AUTO_INCREMENT   (1 << 23)
#define HA_HAS_CHECKSUM        (1 << 24)
/* Table data are stored in separate files (for lower_case_table_names) */
#define HA_FILE_BASED	       (1 << 26)
#define HA_NO_VARCHAR	       (1 << 27)
#define HA_CAN_BIT_FIELD       (1 << 28) /* supports bit fields */
#define HA_NEED_READ_RANGE_BUFFER (1 << 29) /* for read_multi_range */
#define HA_ANY_INDEX_MAY_BE_UNIQUE (1 << 30)


/* bits in index_flags(index_number) for what you can do with index */
#define HA_READ_NEXT            1       /* TODO really use this flag */
#define HA_READ_PREV            2       /* supports ::index_prev */
#define HA_READ_ORDER           4       /* index_next/prev follow sort order */
#define HA_READ_RANGE           8       /* can find all records in a range */
#define HA_ONLY_WHOLE_INDEX	16	/* Can't use part key searches */
#define HA_KEYREAD_ONLY         64	/* Support HA_EXTRA_KEYREAD */

/*
  Index scan will not return records in rowid order. Not guaranteed to be
  set for unordered (e.g. HASH) indexes.
*/
#define HA_KEY_SCAN_NOT_ROR     128 


/* operations for disable/enable indexes */
#define HA_KEY_SWITCH_NONUNIQ      0
#define HA_KEY_SWITCH_ALL          1
#define HA_KEY_SWITCH_NONUNIQ_SAVE 2
#define HA_KEY_SWITCH_ALL_SAVE     3

/*
  Note: the following includes binlog and closing 0.
  so: innodb + bdb + ndb + binlog + myisam + myisammrg + archive +
      example + csv + heap + blackhole + federated + 0
  (yes, the sum is deliberately inaccurate)
*/
#define MAX_HA 14

/*
  Bits in index_ddl_flags(KEY *wanted_index)
  for what ddl you can do with index
  If none is set, the wanted type of index is not supported
  by the handler at all. See WorkLog 1563.
*/
#define HA_DDL_SUPPORT   1 /* Supported by handler */
#define HA_DDL_WITH_LOCK 2 /* Can create/drop with locked table */
#define HA_DDL_ONLINE    4 /* Can create/drop without lock */

/*
  Parameters for open() (in register form->filestat)
  HA_GET_INFO does an implicit HA_ABORT_IF_LOCKED
*/

#define HA_OPEN_KEYFILE		1
#define HA_OPEN_RNDFILE		2
#define HA_GET_INDEX		4
#define HA_GET_INFO		8	/* do a ha_info() after open */
#define HA_READ_ONLY		16	/* File opened as readonly */
/* Try readonly if can't open with read and write */
#define HA_TRY_READ_ONLY	32
#define HA_WAIT_IF_LOCKED	64	/* Wait if locked on open */
#define HA_ABORT_IF_LOCKED	128	/* skip if locked on open.*/
#define HA_BLOCK_LOCK		256	/* unlock when reading some records */
#define HA_OPEN_TEMPORARY	512

	/* Errors on write which is recoverable  (Key exist) */
#define HA_WRITE_SKIP 121		/* Duplicate key on write */
#define HA_READ_CHECK 123		/* Update with is recoverable */
#define HA_CANT_DO_THAT 131		/* Databasehandler can't do it */

	/* Some key definitions */
#define HA_KEY_NULL_LENGTH	1
#define HA_KEY_BLOB_LENGTH	2

#define HA_LEX_CREATE_TMP_TABLE	1
#define HA_LEX_CREATE_IF_NOT_EXISTS 2
#define HA_OPTION_NO_CHECKSUM	(1L << 17)
#define HA_OPTION_NO_DELAY_KEY_WRITE (1L << 18)
#define HA_MAX_REC_LENGTH	65535

/* Table caching type */
#define HA_CACHE_TBL_NONTRANSACT 0
#define HA_CACHE_TBL_NOCACHE     1
#define HA_CACHE_TBL_ASKTRANSACT 2
#define HA_CACHE_TBL_TRANSACT    4

/* Options of START TRANSACTION statement (and later of SET TRANSACTION stmt) */
#define MYSQL_START_TRANS_OPT_WITH_CONS_SNAPSHOT 1

enum db_type
{
  DB_TYPE_UNKNOWN=0,DB_TYPE_DIAB_ISAM=1,
  DB_TYPE_HASH,DB_TYPE_MISAM,DB_TYPE_PISAM,
  DB_TYPE_RMS_ISAM, DB_TYPE_HEAP, DB_TYPE_ISAM,
  DB_TYPE_MRG_ISAM, DB_TYPE_MYISAM, DB_TYPE_MRG_MYISAM,
  DB_TYPE_BERKELEY_DB, DB_TYPE_INNODB,
  DB_TYPE_GEMINI, DB_TYPE_NDBCLUSTER,
  DB_TYPE_EXAMPLE_DB, DB_TYPE_ARCHIVE_DB, DB_TYPE_CSV_DB,
  DB_TYPE_FEDERATED_DB,
  DB_TYPE_BLACKHOLE_DB,
  DB_TYPE_DEFAULT // Must be last
};

enum row_type { ROW_TYPE_NOT_USED=-1, ROW_TYPE_DEFAULT, ROW_TYPE_FIXED,
		ROW_TYPE_DYNAMIC, ROW_TYPE_COMPRESSED,
		ROW_TYPE_REDUNDANT, ROW_TYPE_COMPACT };

/* struct to hold information about the table that should be created */

/* Bits in used_fields */
#define HA_CREATE_USED_AUTO             (1L << 0)
#define HA_CREATE_USED_RAID             (1L << 1)
#define HA_CREATE_USED_UNION            (1L << 2)
#define HA_CREATE_USED_INSERT_METHOD    (1L << 3)
#define HA_CREATE_USED_MIN_ROWS         (1L << 4)
#define HA_CREATE_USED_MAX_ROWS         (1L << 5)
#define HA_CREATE_USED_AVG_ROW_LENGTH   (1L << 6)
#define HA_CREATE_USED_PACK_KEYS        (1L << 7)
#define HA_CREATE_USED_CHARSET          (1L << 8)
#define HA_CREATE_USED_DEFAULT_CHARSET  (1L << 9)
#define HA_CREATE_USED_DATADIR          (1L << 10)
#define HA_CREATE_USED_INDEXDIR         (1L << 11)
#define HA_CREATE_USED_ENGINE           (1L << 12)
#define HA_CREATE_USED_CHECKSUM         (1L << 13)
#define HA_CREATE_USED_DELAY_KEY_WRITE  (1L << 14)
#define HA_CREATE_USED_ROW_FORMAT       (1L << 15)
#define HA_CREATE_USED_COMMENT          (1L << 16)
#define HA_CREATE_USED_PASSWORD         (1L << 17)
#define HA_CREATE_USED_CONNECTION       (1L << 18)

typedef ulonglong my_xid; // this line is the same as in log_event.h
#define MYSQL_XID_PREFIX "MySQLXid"
#define MYSQL_XID_PREFIX_LEN 8 // must be a multiple of 8
#define MYSQL_XID_OFFSET (MYSQL_XID_PREFIX_LEN+sizeof(server_id))
#define MYSQL_XID_GTRID_LEN (MYSQL_XID_OFFSET+sizeof(my_xid))

#define XIDDATASIZE 128
#define MAXGTRIDSIZE 64
#define MAXBQUALSIZE 64

struct xid_t {
  long formatID;
  long gtrid_length;
  long bqual_length;
  char data[XIDDATASIZE];  // not \0-terminated !

  bool eq(struct xid_t *xid)
  { return eq(xid->gtrid_length, xid->bqual_length, xid->data); }
  bool eq(long g, long b, const char *d)
  { return g == gtrid_length && b == bqual_length && !memcmp(d, data, g+b); }
  void set(struct xid_t *xid)
  { memcpy(this, xid, xid->length()); }
  void set(long f, const char *g, long gl, const char *b, long bl)
  {
    formatID= f;
    memcpy(data, g, gtrid_length= gl);
    memcpy(data+gl, b, bqual_length= bl);
  }
  void set(ulonglong xid)
  {
    my_xid tmp;
    formatID= 1;
    set(MYSQL_XID_PREFIX_LEN, 0, MYSQL_XID_PREFIX);
    memcpy(data+MYSQL_XID_PREFIX_LEN, &server_id, sizeof(server_id));
    tmp= xid;
    memcpy(data+MYSQL_XID_OFFSET, &tmp, sizeof(tmp));
    gtrid_length=MYSQL_XID_GTRID_LEN;
  }
  void set(long g, long b, const char *d)
  {
    formatID= 1;
    gtrid_length= g;
    bqual_length= b;
    memcpy(data, d, g+b);
  }
  bool is_null() { return formatID == -1; }
  void null() { formatID= -1; }
  my_xid quick_get_my_xid()
  {
    my_xid tmp;
    memcpy(&tmp, data+MYSQL_XID_OFFSET, sizeof(tmp));
    return tmp;
  }
  my_xid get_my_xid()
  {
    return gtrid_length == MYSQL_XID_GTRID_LEN && bqual_length == 0 &&
           !memcmp(data+MYSQL_XID_PREFIX_LEN, &server_id, sizeof(server_id)) &&
           !memcmp(data, MYSQL_XID_PREFIX, MYSQL_XID_PREFIX_LEN) ?
           quick_get_my_xid() : 0;
  }
  uint length()
  {
    return sizeof(formatID)+sizeof(gtrid_length)+sizeof(bqual_length)+
           gtrid_length+bqual_length;
  }
  byte *key()
  {
    return (byte *)&gtrid_length;
  }
  uint key_length()
  {
    return sizeof(gtrid_length)+sizeof(bqual_length)+gtrid_length+bqual_length;
  }
};
typedef struct xid_t XID;

/* for recover() handlerton call */
#define MIN_XID_LIST_SIZE  128
#ifdef SAFEMALLOC
#define MAX_XID_LIST_SIZE  256
#else
#define MAX_XID_LIST_SIZE  (1024*128)
#endif

/*
  handlerton is a singleton structure - one instance per storage engine -
  to provide access to storage engine functionality that works on the
  "global" level (unlike handler class that works on a per-table basis)

  usually handlerton instance is defined statically in ha_xxx.cc as

  static handlerton { ... } xxx_hton;

  savepoint_*, prepare, recover, and *_by_xid pointers can be 0.
*/
typedef struct
{
  /*
    storage engine name as it should be printed to a user
    存储引擎的名称
  */
  const char *name;

  /*
    Historical marker for if the engine is available of not
    在 SHOW STORAGE ENGINES输出结果中恰当的输出Support列时需要用到
  */
  SHOW_COMP_OPTION state;

  /*
    A comment used by SHOW to describe an engine.
    SHOW ENGINES 输出结构中的 Comment 列的数值
  */
  const char *comment;

  /*
    Historical number used for frm file to determine the correct storage engine.
    This is going away and new engines will just use "name" for this.
    在.frm文件中使用，以确定相关表的存储引擎类型
  */
  enum db_type db_type;
  /* 
    Method that initizlizes a storage engine
    初始化存储引擎的函数
  */
  bool (*init)();

  /*
    each storage engine has it's own memory area (actually a pointer)
    in the thd, for storing per-connection information.
    It is accessed as

      thd->ha_data[xxx_hton.slot]

   slot number is initialized by MySQL after xxx_init() is called.
   由 MyISAM 内部使用，开始时应该设置为 0
   */
   uint slot;
   /*
     to store per-savepoint data storage engine is provided with an area
     of a requested size (0 is ok here).
     savepoint_offset must be initialized statically to the size of
     the needed memory to store per-savepoint information.
     After xxx_init it is changed to be an offset to savepoint storage
     area and need not be used by storage engine.
     see binlog_hton and binlog_savepoint_set/rollback for an example.
     包含到保存点存储区域的偏移量。开始时应设置位保存点结构的大小
   */
   uint savepoint_offset;
   /*
     handlerton methods:

     close_connection is only called if
     thd->ha_data[xxx_hton.slot] is non-zero, so even if you don't need
     this storage area - set it to something, so that MySQL would know
     this storage engine was accessed in this connection
     在连接关闭时执行特定存储引擎 清 0 的函数
   */
   int  (*close_connection)(THD *thd);
   /*
     sv points to an uninitialized storage area of requested size
     (see savepoint_offset description)
     处理保存点的函数
   */
   int  (*savepoint_set)(THD *thd, void *sv);
   /*
     sv points to a storage area, that was earlier passed
     to the savepoint_set call
     处理 ROLLBACK TO SAVEPOINT的函数
   */
   int  (*savepoint_rollback)(THD *thd, void *sv);
   /*
    * 处理 RELEASE SAVEPOINT 函数
    * */
   int  (*savepoint_release)(THD *thd, void *sv);
   /*
     'all' is true if it's a real commit, that makes persistent changes
     'all' is false if it's not in fact a commit but an end of the
     statement that is part of the transaction.
     NOTE 'all' is also false in auto-commit mode where 'end of statement'
     and 'real commit' mean the same event.
   */
   /*
    * 处理 COMMIT 函数
    * */
   int  (*commit)(THD *thd, bool all);
   /*
    * 处理 ROLLBACK 函数
    * */
   int  (*rollback)(THD *thd, bool all);
   /*
    * 处理 XA PREPARE 函数
    * */
   int  (*prepare)(THD *thd, bool all);
   /*
    * 处理 XA RECOVER 函数
    * */
   int  (*recover)(XID *xid_list, uint len);
   /*
    * 处理 XA COMMIT 函数
    * */
   int  (*commit_by_xid)(XID *xid);
   /*
    * 处理 XA COMMIT 函数
    * */
   int  (*rollback_by_xid)(XID *xid);
   /*
    * 打开一个光标的函数
    * */
   void *(*create_cursor_read_view)();
   /*
    * 从光标读取的函数
    * */
   void (*set_cursor_read_view)(void *);
   /*
    * 关闭一个光标的函数
    * */
   void (*close_cursor_read_view)(void *);
   uint32 flags;                                /* global handler flags */
} handlerton;

struct show_table_alias_st {
  const char *alias;
  const char *type;
};

/* Possible flags of a handlerton */
#define HTON_NO_FLAGS                 0
#define HTON_CLOSE_CURSORS_AT_COMMIT (1 << 0)
#define HTON_ALTER_NOT_SUPPORTED     (1 << 1)
#define HTON_CAN_RECREATE            (1 << 2)

typedef struct st_thd_trans
{
  /* number of entries in the ht[] */
  uint        nht;
  /* true is not all entries in the ht[] support 2pc */
  bool        no_2pc;
  /* storage engines that registered themselves for this transaction */
  handlerton *ht[MAX_HA];
} THD_TRANS;

enum enum_tx_isolation { ISO_READ_UNCOMMITTED, ISO_READ_COMMITTED,
			 ISO_REPEATABLE_READ, ISO_SERIALIZABLE};

typedef struct st_ha_create_information
{
  CHARSET_INFO *table_charset, *default_table_charset;
  LEX_STRING connect_string;
  const char *comment,*password;
  const char *data_file_name, *index_file_name;
  const char *alias;
  ulonglong max_rows,min_rows;
  ulonglong auto_increment_value;
  ulong table_options;
  ulong avg_row_length;
  ulong raid_chunksize;
  ulong used_fields;
  SQL_LIST merge_list;
  enum db_type db_type;
  enum row_type row_type;
  uint null_bits;                       /* NULL bits at start of record */
  uint options;				/* OR of HA_CREATE_ options */
  uint raid_type,raid_chunks;
  uint merge_insert_method;
  uint extra_size;                      /* length of extra data segment */
  bool table_existed;			/* 1 in create if table existed */
  bool frm_only;                        /* 1 if no ha_create_table() */
  bool varchar;                         /* 1 if table has a VARCHAR */
} HA_CREATE_INFO;


/* The handler for a table type.  Will be included in the TABLE structure */

struct st_table;
typedef struct st_table TABLE;
struct st_foreign_key_info;
typedef struct st_foreign_key_info FOREIGN_KEY_INFO;

typedef struct st_savepoint SAVEPOINT;
extern ulong savepoint_alloc_size;

/* Forward declaration for condition pushdown to storage engine */
typedef class Item COND;

typedef struct st_ha_check_opt
{
  ulong sort_buffer_size;
  uint flags;       /* isam layer flags (e.g. for myisamchk) */
  uint sql_flags;   /* sql layer flags - for something myisamchk cannot do */
  KEY_CACHE *key_cache;	/* new key cache when changing key cache */
  void init();
} HA_CHECK_OPT;


/*
  This is a buffer area that the handler can use to store rows.
  'end_of_used_area' should be kept updated after calls to
  read-functions so that other parts of the code can use the
  remaining area (until next read calls is issued).
*/

typedef struct st_handler_buffer
{
  const byte *buffer;         /* Buffer one can start using */
  const byte *buffer_end;     /* End of buffer */
  byte *end_of_used_area;     /* End of area that was used by handler */
} HANDLER_BUFFER;


class handler :public Sql_alloc
{
 protected:
    /*
     * 与给定 handler 实例相关的表的描述符
     * */
  struct st_table *table;		/* The table definition */

  virtual int index_init(uint idx) { active_index=idx; return 0; }
  virtual int index_end() { active_index=MAX_KEY; return 0; }
  /*
    rnd_init() can be called two times without rnd_end() in between
    (it only makes sense if scan=1).
    then the second call should prepare for the new table scan (e.g
    if rnd_init allocates the cursor, second call should position it
    to the start of the table, no need to deallocate and allocate it again
  */
  virtual int rnd_init(bool scan) =0;
  virtual int rnd_end() { return 0; }

public:
  const handlerton *ht;                 /* storage engine of this handler */
  /*
   * Pointer to current row
   * 存储当前记录参照的数值。记录值是给定的表的内部专用的记录描述符；
   * MyISAM：在数据文件中为本域使用记录偏移量
   * InnoDB：使用以特殊方式进行格式化的逐渐的数值
   * Memory：使用一个指向记录起点指针，数值长度为：ref_length
   *
   * */
  byte *ref;
    /*
     * Pointer to dupp row
     * 用于存储在插入新纪录时导致唯一键冲突的记录的参照的附加寄存器
     * */
  byte *dupp_ref;
  /*
   * data_file_length：使用数据文件的存储引擎的数据文件的长度。不使用数据文件的引擎，
   * 则通过在本变量中存储所有记录加上能插入新纪录的位置的总长度来临时适应结果；显示在 SHOW TABLE STATUS 的输出结果中
   *
   * */
  ulonglong data_file_length;
  /*
   * 数据文件的最大可能长度；通过 data_file_length成员引用。出现在 show table status 的输出结果中
   * */
  ulonglong max_data_file_length;
  /* 使用索引的引擎：索引文件的长度；
   * 不使用索引的引擎：本表存储索引的内存或者磁盘的近似值
   * */
  ulonglong index_file_length;
  /*
   * 索引文件的最大可能长度；仅支持 MyISAM
   * */
  ulonglong max_index_file_length;
  /*
   * 已经分配，但是没有使用的字节的数量。
   * MyISAM：被标记已经删除的记录占据的空间的大小
   * */
  ulonglong delete_length;
  /*
   * 在下一次插入时将分配给自动累加列的数值【insert id】
   * 可以在表创建时使用 AUTO_INCREMENT 从句设置该数值，或者使用 alter table
   * */
  ulonglong auto_increment_value;
  /*
   * 表中记录的数目；InnoDB 仅仅提供预估值
   * */
  ha_rows records;
  /*
   * 表中被标记为已删除的记录的数目
   * */
  ha_rows deleted;
  /*
   * 与MyISAM表的 RAID 特性相关
   * */
  ulong raid_chunksize;
  /*
   * 记录的平均长度：出现在 show table status 中
   * */
  ulong mean_rec_length;
  /*
   * 表的创建时间
   * */
  time_t create_time;
  /*
   * 上次使用 CHECK TABLE 检查表的时间；出现在 SHOW TABLE STATUS 中
   * */
  time_t check_time;
  /*
   * 上次更新表的时间；出现在 show tables status 中
   * */
  time_t update_time;

  /* The following are for read_multi_range */
  bool multi_range_sorted;
  KEY_MULTI_RANGE *multi_range_curr;
  KEY_MULTI_RANGE *multi_range_end;
  HANDLER_BUFFER *multi_range_buffer;

  /* The following are for read_range() */
  /*
   * save_end_range：handler::read_range_first(): 成员方法中使用的成员变量
   * end_range: 在大量与键范围的数值读取记录有关的成员方法中使用的存储变量
   * */
  key_range save_end_range, *end_range;
  /*
   * range_key_part：在 read_range_first()成员方法中使用的存储变量
   * */
  KEY_PART_INFO *range_key_part;
  /*
   * read_range_first()与compare_key()成员方法中使用的存储变量。如果键的实际值被证明等于与之进行比较的数值，
   * 则包含compare_key()应返回的结果
   * */
  int key_compare_result_on_equal;
  /* 在 read_range_first() 和 read_range_next() 成方法中使用的一个存储变量
   * */
  bool eq_range;

  /*
   * errkey: 包含发生错误的最后一个键的数目。错误往往是尝试创建一个唯一键的重复键的数值
   *
   * */
  uint errkey;				/* Last dup key */
  /*
   * sortkey: 键编号，根据此编号对记录进行物理排序。如果不存在这样的键，则设置为 255
   * key_used_on_scan：最后用于扫描记录的键编号
   *
   * */
  uint sortkey, key_used_on_scan;
  /*
   * active_index：当前所选择的键编号，如果没有选择任何键，则设置为 MAX_KEY
   * */
  uint active_index;
  /*
   * 存储在 ref 成员中的数值的长度
   * */
  uint ref_length;
  /* 用于该表的键块的尺寸
   * */
  uint block_size;
  /*
   * raid_type：与 MyISAM 表的 RAID 特性有关
   * raid_chunks：与MyISAM 表的 RAID 特性有关
   * */
  uint raid_type,raid_chunks;
  /*
   * 纯文本键操作描述符
   * */
  FT_INFO *ft_handler;
  /*
   * 将 handler 对象初始化为读取一个键，还是扫描表，或者什么都不做；
   * 调用 ha_init_index()将本数值设置为INDEX，调用ha_init_rnd()将数值设置为RAND()
   * 将数值清0，并且复位为NONE，则分别由ha_end_index()和ha_end_rnd()完成
   * */
  enum {NONE=0, INDEX, RND} inited;
  /*
   * 由update_auto_increment()设置，指出上次操作是否导致自动递增列的数值发生了变化
   * */
  bool  auto_increment_column_changed;
  /*
   * 由 MEMORY handler 设置，指出在服务器启动期间内存表已清空。为了正确进行某些复制记录需要本信息
   * */
  bool implicit_emptied;                /* Can be !=0 only if HEAP */
  const COND *pushed_cond;

  handler(const handlerton *ht_arg, TABLE *table_arg) :table(table_arg),
    ht(ht_arg),
    ref(0), data_file_length(0), max_data_file_length(0), index_file_length(0),
    delete_length(0), auto_increment_value(0),
    records(0), deleted(0), mean_rec_length(0),
    create_time(0), check_time(0), update_time(0),
    key_used_on_scan(MAX_KEY), active_index(MAX_KEY),
    ref_length(sizeof(my_off_t)), block_size(0),
    raid_type(0), ft_handler(0), inited(NONE), implicit_emptied(0),
    pushed_cond(NULL)
    {}
  virtual ~handler(void) { /* TODO: DBUG_ASSERT(inited == NONE); */ }
  int ha_open(const char *name, int mode, int test_if_locked);
  bool update_auto_increment();
  virtual void print_error(int error, myf errflag);
  virtual bool get_error_message(int error, String *buf);
  uint get_dup_key(int error);
  /*
   * change_table_ptr：将 table 成员设置为有参数提供的数值
   * */
  void change_table_ptr(TABLE *table_arg) { table=table_arg; }
  virtual double scan_time()
    { return ulonglong2double(data_file_length) / IO_SIZE + 2; }
    /*
     * 返回一个块读取操作的数目，该操作将使用键编号index从范围的ranges数目中读取行的 rows 数目
     * */
  virtual double read_time(uint index, uint ranges, ha_rows rows)
 { return rows2double(ranges+rows); }
 /*
  * 通常 MySQL 优化器扫描表时不使用键，因为一个纯文本数据文件的全扫描要比遍历一个B树索引更快。
  * 但是有时存储引擎可能会按照下列方式组织数据：
  * 在全表扫描时，遍历一个键更有利。这种方法返回一个键映射，对于可以扫描表的键其位被置位
  *
  * */
  virtual const key_map *keys_to_use_for_scanning() { return &key_map_empty; }
  /*
   * 如果存储引擎支持事务返回 True。默认的方法是返回 False
   * */
  virtual bool has_transactions(){ return 0;}
  /*
   * sql/table.cc 中的openfrm()分配一个临时记录缓冲区，在表描述符中存放当前记录。缓冲区的大小是记录的逻辑长度，
   * 在加上因存储引擎的特定原因而可能加上的一些额外保留的长度
   * */
  virtual uint extra_rec_buf_length() { return 0; }
  
  /*
    Return upper bound of current number of records in the table
    (max. of how many records one will retrieve when doing a full table scan)
    If upper bound is not known, HA_POS_ERROR should be returned as a max
    possible upper bound.

    返回可以在扫描表时检查的记录的最大数目
  */
  virtual ha_rows estimate_rows_upper_bound()
  { return records+EXTRA_RECORDS; }

  /*
    Get the row type from the storage engine.  If this method returns
    ROW_TYPE_NOT_USED, the information in HA_CREATE_INFO should be used.
  */
  virtual enum row_type get_row_type() const { return ROW_TYPE_NOT_USED; }

  /*
   * index_type：返回一个由参数指定的索引的文本说明的指针
   * */
  virtual const char *index_type(uint key_number) { DBUG_ASSERT(0); return "";}

  /*
   * 初始化存储引擎，一边在参数指定的键上执行操作。成功返回 0，失败返回一个非 0 值
   * */
  int ha_index_init(uint idx)
  {
    DBUG_ENTER("ha_index_init");
    DBUG_ASSERT(inited==NONE);
    inited=INDEX;
    DBUG_RETURN(index_init(idx));
  }
  /*
   * 在存储引擎中执行键操作后清 0
   * */
  int ha_index_end()
  {
    DBUG_ENTER("ha_index_end");
    DBUG_ASSERT(inited==INDEX);
    inited=NONE;
    DBUG_RETURN(index_end());
  }
  int ha_rnd_init(bool scan)
  {
    DBUG_ENTER("ha_rnd_init");
    DBUG_ASSERT(inited==NONE || (inited==RND && scan));
    inited=RND;
    DBUG_RETURN(rnd_init(scan));
  }
  int ha_rnd_end()
  {
    DBUG_ENTER("ha_rnd_end");
    DBUG_ASSERT(inited==RND);
    inited=NONE;
    DBUG_RETURN(rnd_end());
  }
  /*
   * this is necessary in many places, e.g. in HANDLER command
   * 根据先前执行的初始化调用ha_index_end()或者ha_rnd_end(),成功发挥0，失败返回一个 非 0 值
   *
   * */
  int ha_index_or_rnd_end()
  {
    return inited == INDEX ? ha_index_end() : inited == RND ? ha_rnd_end() : 0;
  }
  /*
   * 返回当前所选择的索引的编号
   * */
  uint get_index(void) const { return active_index; }
  /*
   * 执行打开表的真正操作
   * name：frm 文件的路径；
   *
   * */
  virtual int open(const char *name, int mode, uint test_if_locked)=0;
  /*
   * 关闭表，执行必要的清0；
   * 必须在 open()之后调用。成功时返回0，失败时返回一个非0的错误代码
   * 本方法为纯虚拟方法，必须在一个子类中实现
   * */
  virtual int close(void)=0;
  /*
   * 在表中插入参数所指的记录。
   * 在处理 INSERT 查询时，本地调用是所有存储引擎共享的执行堆栈的底部
   * 本方法的默认实现方法是 HA_ERR_WRONG_COMMAND,执行失败，会导致所有的查询返回一个错误
   *
   * */
  virtual int write_row(byte * buf) { return  HA_ERR_WRONG_COMMAND; }
  /*
   * 更新由 old_data所指的记录，让 new_data指向内容。在处理 UPDATE 查询时，本调用是所有存储引擎共享的执行堆栈的底部
   * 本方法的默认实现方法是返回 HA_WRONG_COMMAND
   * 执行失败将导致所有的 UPDATE 查询返回一个错误
   * */
  virtual int update_row(const byte * old_data, byte * new_data)
   { return  HA_ERR_WRONG_COMMAND; }
   /*
    * 删除表中由参数所指的记录。在处理 DELETE 查询时，本调用是所有存储引擎共享的执行堆栈的底部
    * */
  virtual int delete_row(const byte * buf)
   { return  HA_ERR_WRONG_COMMAND; }
   /*
    * 根据 key 和 key_len的值，将光标放在第一个键上，如果找到匹配则将记录读入 buf。根据由 find_flag 指定的查找方法进行匹配。
    * 操作由当前激活的索引完成
    * */
  virtual int index_read(byte * buf, const byte * key,
			 uint key_len, enum ha_rkey_function find_flag)
   { return  HA_ERR_WRONG_COMMAND; }
   /*
    * 与 index_read() 相同，但首先激活由索引变量指定的键
    * */
  virtual int index_read_idx(byte * buf, uint index, const byte * key,
			     uint key_len, enum ha_rkey_function find_flag);
  /*
   * 从激活索引中将下一个记录读取到由参数指定的缓冲区中，然后激活的键光标前进一步。成功则返回0，失败返回非0 的错误代码
   * */
  virtual int index_next(byte * buf)
   { return  HA_ERR_WRONG_COMMAND; }
   /*
    * 从激活索引中将前一个记录读取到由参数指定的缓冲区中，然后激活的键光标后退一步。成功返回0，失败返回非0值
    * */
  virtual int index_prev(byte * buf)
   { return  HA_ERR_WRONG_COMMAND; }
   /*
    * 从激活的索引中将第一个记录读取到由参数指定的缓冲区中，然后将激活的键光标紧接着放在后面。成功返回0，失败返回非0的错误代码
    * */
  virtual int index_first(byte * buf)
   { return  HA_ERR_WRONG_COMMAND; }
   /*
    * 从激活的索引中将最后一个记录读到的参数指定的缓冲区中，并在前面放入激活键指标。成功返回0，失败返回非0的错误代码
    * */
  virtual int index_last(byte * buf)
   { return  HA_ERR_WRONG_COMMAND; }
   /*
    * 从当前激活的记录开始，读取与前一个读取记录具有相同键的记录，并放入由buf指向的缓冲区中
    * 由于有些存储引擎不存储最后读取的键数值，则使用 key 和 key len参数提醒他们。成功时激活键光标前进，同时返回 0
    * */
  virtual int index_next_same(byte *buf, const byte *key, uint keylen);
  /*
   * 将在上次键值与 key 和 key len 数值匹配过程中找到的记录读入 buf。然后将光标直接放在该记录的前面
   * 成功返回 0，失败返回非0的错误代码
   *
   * */
  virtual int index_read_last(byte * buf, const byte * key, uint key_len)
   { return (my_errno=HA_ERR_WRONG_COMMAND); }
  virtual int read_multi_range_first(KEY_MULTI_RANGE **found_range_p,
                                     KEY_MULTI_RANGE *ranges, uint range_count,
                                     bool sorted, HANDLER_BUFFER *buffer);
  virtual int read_multi_range_next(KEY_MULTI_RANGE **found_range_p);
  virtual int read_range_first(const key_range *start_key,
                               const key_range *end_key,
                               bool eq_range, bool sorted);

  virtual int read_range_next();

  int compare_key(key_range *range);
  /*
   * 重新初始化纯文本键操作的存储引擎
   * 当 MySQL 需要重复多次进行纯文本搜索时可以调用，目前仅对于 MyISAM 有意义
   * */
  virtual int ft_init() { return HA_ERR_WRONG_COMMAND; }
  void ft_end() { ft_handler=NULL; }
  /*
   * 为一个搜索初始化纯文本引擎。目前仅仅对于MyISAM 有意义
   * flags：指定搜索模式
   * inx：索引编号
   * key：用于提供搜索的键
   * */
  virtual FT_INFO *ft_init_ext(uint flags, uint inx,String *key)
    { return NULL; }
    /*
     * 将有关当前激活纯文本键的下一个记录读入由变量指定的缓冲区中，仅仅对于MyISAM有效
     *
     * */
  virtual int ft_read(byte *buf) { return HA_ERR_WRONG_COMMAND; }
  /*
   * rnd_next：在连续扫描期间，将下一条记录读入由 buf 指向的缓冲区中，连续扫描光标前进
   * 成功发挥 0，失败返回非 0 的错误代码
   * */
  virtual int rnd_next(byte *buf)=0;
  /*
   * 将由 pos 指定的记录读取 buf
   * pos：pos 的解释取决于存储引擎；MyISAM使用记录的数据文件偏移量；InnoDB 使用主键值
   * 成功返回一个非 0 错误代码，纯虚拟方法，需要在具体的子类中实现
   * */
  virtual int rnd_pos(byte * buf, byte *pos)=0;
  /*
   * 从表中检索一个任意选取的记录，并将它放入由 buf 参数指向的缓冲区
   * primary_key：参数影响记录的选择方法
   * 选择记录的两种方式：
   *    1、扫描表并返回第一个未被标记为已删除的记录
   *    2、选取键中带有 primary_key 参数数目的第一个记录
   *
   * 如果 primary_key 参数大于或等于 MAX_KEY，使用第一种方法；
   * 否则使用第二种方法
   * */
  virtual int read_first_row(byte *buf, uint primary_key);
  /*
    The following function is only needed for tables that may be temporary
    tables during joins

  */

  /*
   *  仅仅对于MyISAM 有意义，本方法是 rnd_pos 的别名
   *  目前仅调用这个方法一次，执行调用的是在临时表上处理SELECT DISTINCT 时，删除结果集中的重复的代码
   * */
  virtual int restart_rnd_next(byte *buf, byte *pos)
    { return HA_ERR_WRONG_COMMAND; }
    /*
     * 将当前记录重新读取 buf，如果改值大于或者等于0，则可能使用键编号 idx，成功返回0，失败返回非0错误代码
     * 该方法没有被实现，也不会被调用
     * */
  virtual int rnd_same(byte *buf, uint inx)
    { return HA_ERR_WRONG_COMMAND; }
    /*
     * 返回与键编号 idx 中的 min_key 和 max_key 锁限定的键值相匹配的记录的估计的数目
     * 默认的实现方法是返回10。
     * 如果返回一个伪值，则最坏的结果就是则优化器倾向于一个优化最差的数值，或根本不是用键
     *
     * */
  virtual ha_rows records_in_range(uint inx, key_range *min_key,
                                   key_range *max_key)
    { return (ha_rows) 10; }
    /*
     * 将唯一的参考值存储在 ref 成员的当前记录中。在 MyISAM 表中，该数值是记录在数据文件的位置
     *
     * */
  virtual void position(const byte *record)=0;
  /*
   * 根据参数的数值更新本对象的各种统计变量的数值
   * 纯虚拟方法，必须在一个子类中实现
   * */
  virtual void info(uint)=0;
  /*
   * 提示存储引擎使用一些特殊的优化
   * */
  virtual int extra(enum ha_extra_function operation)
  { return 0; }
  /*
   * 类似于 extra()，但是允许调用函数将参数传送给锁请求的操作。
   * 主要用于控制各种类型 IO的高速缓存的大小
   * */
  virtual int extra_opt(enum ha_extra_function operation, ulong cache_size)
  { return extra(operation); }
  /*
   * handler::extra(HA_EXTRA_RESET)的封装器。释放由较早的extra()调用分配的资源，将存储引擎的操作模式重新设置为默认值
   * */
  virtual int reset() { return extra(HA_EXTRA_RESET); }
  /*
   * MySQL 为语句中使用的每个表在语句起始处调用本方法一次
   * 如果启用了外部锁定徐安祥，因而启用了选项的历史名称，MyISAM 仅通过操作系统锁定键文件
   * 事务存储引擎将其作为挂钩，用于启动事务，并在必要时执行其他初始化
   *
   * 纯虚拟方法，需要在子类中进行实现
   * */
  virtual int external_lock(THD *thd, int lock_type) { return 0; }
  /*
   * 在 upadte 或者 delete 期间为与 WHERE从句不匹配的各行而调用，以便于删除不必要的行锁定
   * InnoDB 用于清除在半相同读模式下读取的行锁
   *
   * */
  virtual void unlock_row() {}
  /*
   * 在由 LOCK TABLES 发起的事务起始时的调用，让事务存储引擎有机会登记事务起点
   * 成功返回0，失败返回非 0 错误代码
   * */
  virtual int start_stmt(THD *thd, thr_lock_type lock_type) {return 0;}
  /*
    This is called to delete all rows in a table
    If the handler don't support this, then this function will
    return HA_ERR_WRONG_COMMAND and MySQL will delete the rows one
    by one.
  */
  /*
   * 立即从表中删除所有行。是一种可选的优化。如果不支持，则通过多次调用 delete_row() 来清除表
   *
   * */
  virtual int delete_all_rows()
  { return (my_errno=HA_ERR_WRONG_COMMAND); }

  virtual ulonglong get_auto_increment();
  virtual void restore_auto_increment();

  /*
    Reset the auto-increment counter to the given value, i.e. the next row
    inserted will get the given value. This is called e.g. after TRUNCATE
    is emulated by doing a 'DELETE FROM t'. HA_ERR_WRONG_COMMAND is
    returned by storage engines that don't support this operation.
  */
  virtual int reset_auto_increment(ulonglong value)
  { return HA_ERR_WRONG_COMMAND; }

  virtual void update_create_info(HA_CREATE_INFO *create_info) {}

  /* admin commands - called from mysql_admin_table */
  /*
   * 检查表是否由结构性错误。在发出check table 时调用
   * THD：当前线程描述符
   * check_opt：指向描述符操作选项结构
   * */
  virtual int check(THD* thd, HA_CHECK_OPT* check_opt)
  { return HA_ADMIN_NOT_IMPLEMENTED; }
  virtual int backup(THD* thd, HA_CHECK_OPT* check_opt)
  { return HA_ADMIN_NOT_IMPLEMENTED; }
  /*
    restore assumes .frm file must exist, and that generate_table() has been
    called; It will just copy the data file and run repair.
  */
  /*
   * 重新从 frm 和 数据文件创建索引文件，仅仅适用于 MyISAM
   * 成功返回 0，失败返回非 0 值
   * */
  virtual int restore(THD* thd, HA_CHECK_OPT* check_opt)
  { return HA_ADMIN_NOT_IMPLEMENTED; }
  /*
   * 修复破坏的表。在发出 REPAIR TABLE时调用。成功则返回 0，失败则返回返回一个非 0 错误代码
   * */
  virtual int repair(THD* thd, HA_CHECK_OPT* check_opt)
  { return HA_ADMIN_NOT_IMPLEMENTED; }
  /*
   * 是一个典型的查询重新以优化的形式构造表结构，在发出 OPTIMIZE TABLE时调用，成功时则返回 0
   *
   * */
  virtual int optimize(THD* thd, HA_CHECK_OPT* check_opt)
  { return HA_ADMIN_NOT_IMPLEMENTED; }
  /*
   * 更有有优化器使用的索引统计。在发出 ANALYZE TABLE时调用
   * 成功则返回0，失败返回一个非 0 错误代码
   * */
  virtual int analyze(THD* thd, HA_CHECK_OPT* check_opt)
  { return HA_ADMIN_NOT_IMPLEMENTED; }
  /*
   * 将该表的键分配给在 check_opt 结构中指定的键高速缓存
   * 在发出 CACHE INDEX 命令时调用
   * 成功则返回 0，失败则返回一个非 0 错误代码
   * */
  virtual int assign_to_keycache(THD* thd, HA_CHECK_OPT* check_opt)
  { return HA_ADMIN_NOT_IMPLEMENTED; }
  /*
   * 将本表的键装载到在check_opt结构中指定的高速缓存中
   * 成功时返回 0，失败时则返回一个非 0 的错误的代码
   * */
  virtual int preload_keys(THD* thd, HA_CHECK_OPT* check_opt)
  { return HA_ADMIN_NOT_IMPLEMENTED; }
  /* end of the list of admin commands */

  /*
   * 检查表是否被破坏及在必要时进行修复
   * 成功时返回 0，出错时返回 1。默认实现方法为仅 返回 1
   * */
  virtual bool check_and_repair(THD *thd) { return HA_ERR_WRONG_COMMAND; }
  /*
   * 以存储引擎专用的格式将表写入到 由 fd 指定的文件柄
   * 如果fd 小于 0，则数据写入与 thd 相关的网络连接
   * LOAD DATA FROM MASTER
   * */
  virtual int dump(THD* thd, int fd = -1) { return HA_ERR_WRONG_COMMAND; }
  /*
   * 禁止在表中使用键
   * 在发出 DISABLE KEYS 命令时调用，常常在一系列大型更新前使用【表锁】
   * */
  virtual int disable_indexes(uint mode) { return HA_ERR_WRONG_COMMAND; }
  /*
   * 重新允许在表中使用键。在发出 ENABLE KEYS命令时调用
   * */
  virtual int enable_indexes(uint mode) { return HA_ERR_WRONG_COMMAND; }
  /*
   * 如果已经禁用表中的索引，则返回 1，否则返回 0
   * */
  virtual int indexes_are_disabled(void) {return 0;}
  /*
   * 提示存储引擎启用批量插入优化
   * MySQL在表中插入大量行前调用
   * MyISAM 优化批量插入的方法是：在内存中高速缓存键值，并且将他们按照键顺序插入到 B-树索引中。默认实现是不执行任何操作
   * */
  virtual void start_bulk_insert(ha_rows rows) {}
  /*
   * 批量插入结束时调用
   * 成功返回0，否则返回一个非0错误代码
   * */
  virtual int end_bulk_insert() {return 0; }
  /*
   * InnoDB使用的方法是：用于执行为本表分配的表空间上的效率
   * discard：准备从备份中导入表空间
   * import：在将要存储的表空间文件复制到指定位置后，从备份中恢复数据
   * ALTER TABLE ... DISCARD TABLESPACE 或者 ALTER TABLE ... IMPORT TABLESPACE时调用；
   * 成功时返回0，失败则返回一个非0的错误代码
   * */
  virtual int discard_or_import_tablespace(my_bool discard)
  {return HA_ERR_WRONG_COMMAND;}
  /*
   * 从参数指定的网络连接中读取表数据，并且按照下列的方式进行存储：
   * 1、调用 repair() 足够使表进入一致状态
   * */
  virtual int net_read_dump(NET* net) { return HA_ERR_WRONG_COMMAND; }
  /*
   * 在 SHOW TABLES中使用，用于在 comment 中列中显示有关表的额外的信息
   * 返回一个指向包含经过更新的注释值的指针，如果返回一个与参数不同的数值，则调用函数会假定使用my_alloc()分配了新指针，并将在使用后通过my_free()释放
   * InnoDB 是提供自实现的唯一引擎
   * */
  virtual char *update_table_comment(const char * comment)
  { return (char*) comment;}
  /*
   * 将额外的特定存储引擎信息添加到由参数指定的 String 对象中
   * 用于生成 SHOW CREATE TABLE 的输出结果，默认的实现方式是返回 0
   * */
  virtual void append_create_info(String *packet) {}
  /*
   * 返回一个指向包含创建外键的 CREATE TABLE 语句部分的指针
   * 用于SHOW CREATE TABLE的输出结果
   * */
  virtual char* get_foreign_key_create_info()
  { return(NULL);}  /* gets foreign key create string from InnoDB */
  /* used in ALTER TABLE; 1 if changing storage engine is allowed */
  virtual bool can_switch_engines() { return 1; }
  /* used in REPLACE; is > 0 if table is referred by a FOREIGN KEY */
  virtual int get_foreign_key_list(THD *thd, List<FOREIGN_KEY_INFO> *f_key_list)
  { return 0; }
  /*
   * 如果某些外键引用了与本对象有关的表则返回1，否则返回 0
   * */
  virtual uint referenced_by_foreign_key() { return 0;}
  /*
   * 为后续HANLDER命令准备表。HANDLER命令提供了一个低层次接口
   * 可通过 SQL 连接某些存储引擎操作，默认实现方法是不执行任何操作
   *
   * */
  virtual void init_table_handle_for_HANDLER()
  { return; }       /* prepare InnoDB for HANDLER */
  /*
   * 必要时释放由 get_foreign_key_create_info() 返回的指针
   * 默认方法是不执行任何的操作
   * */
  virtual void free_foreign_key_create_info(char* str) {}
  /* The following can be called without an open handler */
  /*
   * 返回一个指向包含存储引擎名称的字符串的指针。本方法是纯虚拟方法，必须在子类中实现
   * */
  virtual const char *table_type() const =0;
  /*
   * 返回一个字符串指针数组，这些指针指向本存储引擎数据和键的文件的扩展名
   * 数组的最后元素为 0
   * */
  virtual const char **bas_ext() const =0;
  /*
   * 返回这些存储引擎能力的位掩码 在sql/handler.h 中进行定义
   * */
  virtual ulong table_flags(void) const =0;
  /*
   * 返回由参数指定的键或者键组件的能力的位掩码
   * */
  virtual ulong index_flags(uint idx, uint part, bool all_parts) const =0;
  /*
   * 返回一个给定键在创建或移除该键的位掩码
   * 默认的实现方法是返回 DDL_SUPPORT，这意味着存储引擎支持给定定义的索引，但是无法将其添加到现有表中
   * */
  virtual ulong index_ddl_flags(KEY *wanted_index) const
  { return (HA_DDL_SUPPORT); }
  /*
   * 在表中添加键组合
   * table_arg：
   * key_info：键定义数组的起点
   * num_of_keys：表示数组的大小
   * */
  virtual int add_index(TABLE *table_arg, KEY *key_info, uint num_of_keys)
  { return (HA_ERR_WRONG_COMMAND); }
  /*
   * 由参数指定的表中移除键。成功返回0，失败返回一个 非 0 错误代码
   * */
  virtual int drop_index(TABLE *table_arg, uint *key_num, uint num_of_keys)
  { return (HA_ERR_WRONG_COMMAND); }
  /*
   * 返回最大可能的记录的长度
   * 该限制要么是存储引擎支持，要么是核心代码限定
   * */
  uint max_record_length() const
  { return min(HA_MAX_REC_LENGTH, max_supported_record_length()); }
  /*
   * 返回每个表最大可能的键的数目
   * */
  uint max_keys() const
  { return min(MAX_KEY, max_supported_keys()); }
  /*
   * 返回一个键能包含的列后者列前缀的可能数目的最大值
   * */
  uint max_key_parts() const
  { return min(MAX_REF_PARTS, max_supported_key_parts()); }
  /*
   * 返回可能键长度的最大值
   * */
  uint max_key_length() const
  { return min(MAX_KEY_LENGTH, max_supported_key_length()); }
  /*
   * 返回可能的键组成部分长度的最大值
   * */
  uint max_key_part_length() const
  { return min(MAX_KEY_LENGTH, max_supported_key_part_length()); }

  /*
   * 返回本存储引擎设置的记录长度限制值
   * */
  virtual uint max_supported_record_length() const { return HA_MAX_REC_LENGTH; }
  /*
   * 返回本存储引擎设定的键数目的限制值
   * */
  virtual uint max_supported_keys() const { return 0; }
  /*
   * 返回本存储引擎设定的键组成部分数目的限制值
   * */
  virtual uint max_supported_key_parts() const { return MAX_REF_PARTS; }
  /*
   * 返回本存储引擎设定的键长度的限制值
   * */
  virtual uint max_supported_key_length() const { return MAX_KEY_LENGTH; }
  /*
   * 返回本存储引擎设定的键组成部分长度限制值
   * */
  virtual uint max_supported_key_part_length() const { return 255; }
  /*
   * 返回本存储引擎设定的键记录长度的下限值
   *
   * */
  virtual uint min_record_length(uint options) const { return 1; }

  /*
   * 本存储引擎固有的字节顺序
   * 小尾返回为1；否则返回为 0
   *
   * */
  virtual bool low_byte_first() const { return 1; }
  /*
   * 返回本表的校验和；
   * 默认返回值为 0
   * */
  virtual uint checksum() const { return 0; }
  /*
   * 表崩溃则返回为 1
   * 在CHECK TABLE 或者常规读/写操作发现问题时会出现这种情况，会将表标记为 已崩溃
   * 成功执行 repair table 会删除此标记
   *
   * */
  virtual bool is_crashed() const  { return 0; }
  /*
   * 如果存储引擎支持自动修复损毁的表，返回1；
   * 目前仅仅支持 MyISAM
   * */
  virtual bool auto_repair() const { return 0; }

  /*
    default rename_table() and delete_table() rename/delete files with a
    given name and extensions from bas_ext()
  */
  /*
   * 将 from 指定路径的表，移动到 to 的路径中
   * 参数是到达表定义文件的路径
   * 默认实现方法是对所有由 bas_ext() 返沪的可能扩展名进行迭代，然后返回重新命名匹配文件
   *
   * */
  virtual int rename_table(const char *from, const char *to);
  /*
   * 删除由 name 指定的表
   * 参数是到达表定义文件的路径
   * 默认的实现方法是对所有由 bas_ext() 返回的可能扩展名进行迭代，然后删除匹配文件
   * */
  virtual int delete_table(const char *name);
  /*
   * 使用表描述符form和创建信息描述符info 创建由name指定的表
   *
   * */
  virtual int create(const char *name, TABLE *form, HA_CREATE_INFO *info)=0;

  /* lock_count() can be more than one if the table is a MERGE */
  /*
   * 返回存储本表的锁描述符时所需要的常规所得描述符块的数量
   * 大多数情况下只需要一个表描述符块，但是 MERGE 表除外，MERGE要求每个原件表有一个块
   * */
  virtual uint lock_count(void) const { return 1; }
  /*
   * 将与本表有关的所描述符的位置存储在 to 指定的地址中
   * 其余参数则提供当前线程描述符的数值，并在存储引擎处于内部目的想法下，提供表类型
   * 本方法主要目的是允许存储引擎在存储所之前修改锁
   * 行层次锁定存储疫情用它来防止表锁管理其在表中放入过于多的锁
   * */
  virtual THR_LOCK_DATA **store_lock(THD *thd,
				     THR_LOCK_DATA **to,
				     enum thr_lock_type lock_type)=0;

  /* Type of table for caching query */
  /*
   * 返回与查询高速缓存有关的位掩码，默认的实现方法是返回 HA_CREATE_TBL_NONTRANSACT，无论是否正在执行事务，都允许高速缓存
   * HA_CREATE_TBL_NONTRANSACT 意味着查询高速缓存将就每个表询问存储引擎是否可进行高速缓存。存储引擎可以使用事务可见性规则进行决定
   *
   * */
  virtual uint8 table_cache_type() { return HA_CACHE_TBL_NONTRANSACT; }
  /* ask handler about permission to cache table when query is to be cached */
  virtual my_bool register_query_cache_table(THD *thd, char *table_key,
					     uint key_length,
					     qc_engine_callback 
					     *engine_callback,
					     ulonglong *engine_data)
  {
    *engine_callback= 0;
    return 1;
  }
 /*
  RETURN
    true  Primary key (if there is one) is clustered key covering all fields
    false otherwise
 */
 virtual bool primary_key_is_clustered() { return FALSE; }

 virtual int cmp_ref(const byte *ref1, const byte *ref2)
 {
   return memcmp(ref1, ref2, ref_length);
 }
 
 /*
   Condition pushdown to storage engines
 */

 /*
   Push condition down to the table handler.
   SYNOPSIS
     cond_push()
     cond   Condition to be pushed. The condition tree must not be            
     modified by the by the caller.
   RETURN
     The 'remainder' condition that caller must use to filter out records.
     NULL means the handler will not return rows that do not match the
     passed condition.
   NOTES
   The pushed conditions form a stack (from which one can remove the
   last pushed condition using cond_pop).
   The table handler filters out rows using (pushed_cond1 AND pushed_cond2 
   AND ... AND pushed_condN)
   or less restrictive condition, depending on handler's capabilities.
   
   handler->extra(HA_EXTRA_RESET) call empties the condition stack.
   Calls to rnd_init/rnd_end, index_init/index_end etc do not affect the
   condition stack.
 */
 /*
  * 由能够过滤与 WHERE 从句的一个部分不匹配的记录的存储引擎使用
  * */
 virtual const COND *cond_push(const COND *cond) { return cond; };
 /*
   Pop the top condition from the condition stack of the handler instance.
   SYNOPSIS
     cond_pop()
     Pops the top if condition stack, if stack is not empty
 */
 /*
  * 将顶部条件从存储引擎条件对战的顶部删除
  * */
 virtual void cond_pop() { return; };
};

	/* Some extern variables used with handlers */

extern handlerton *sys_table_types[];
extern const char *ha_row_type[];
extern TYPELIB tx_isolation_typelib;
extern TYPELIB myisam_stats_method_typelib;
extern ulong total_ha, total_ha_2pc;

	/* Wrapper functions */
#define ha_commit_stmt(thd) (ha_commit_trans((thd), FALSE))
#define ha_rollback_stmt(thd) (ha_rollback_trans((thd), FALSE))
#define ha_commit(thd) (ha_commit_trans((thd), TRUE))
#define ha_rollback(thd) (ha_rollback_trans((thd), TRUE))

/* lookups */
enum db_type ha_resolve_by_name(const char *name, uint namelen);
const char *ha_get_storage_engine(enum db_type db_type);
handler *get_new_handler(TABLE *table, enum db_type db_type);
enum db_type ha_checktype(THD *thd, enum db_type database_type,
                          bool no_substitute, bool report_error);
bool ha_check_storage_engine_flag(enum db_type db_type, uint32 flag);

/* basic stuff */
int ha_init(void);
TYPELIB *ha_known_exts(void);
int ha_panic(enum ha_panic_function flag);
int ha_update_statistics();
void ha_close_connection(THD* thd);
my_bool ha_storage_engine_is_enabled(enum db_type database_type);
bool ha_flush_logs(void);
void ha_drop_database(char* path);
int ha_create_table(const char *name, HA_CREATE_INFO *create_info,
		    bool update_create_info);
int ha_delete_table(THD *thd, enum db_type db_type, const char *path,
                    const char *alias, bool generate_warning);

/* discovery */
int ha_create_table_from_engine(THD* thd, const char *db, const char *name);
int ha_discover(THD* thd, const char* dbname, const char* name,
                const void** frmblob, uint* frmlen);
int ha_find_files(THD *thd,const char *db,const char *path,
                  const char *wild, bool dir,List<char>* files);
int ha_table_exists_in_engine(THD* thd, const char* db, const char* name);

/* key cache */
int ha_init_key_cache(const char *name, KEY_CACHE *key_cache);
int ha_resize_key_cache(KEY_CACHE *key_cache);
int ha_change_key_cache_param(KEY_CACHE *key_cache);
int ha_change_key_cache(KEY_CACHE *old_key_cache, KEY_CACHE *new_key_cache);
int ha_end_key_cache(KEY_CACHE *key_cache);

/* report to InnoDB that control passes to the client */
int ha_release_temporary_latches(THD *thd);

/* transactions: interface to handlerton functions */
int ha_start_consistent_snapshot(THD *thd);
int ha_commit_or_rollback_by_xid(XID *xid, bool commit);
int ha_commit_one_phase(THD *thd, bool all);
int ha_rollback_trans(THD *thd, bool all);
int ha_prepare(THD *thd);
int ha_recover(HASH *commit_list);

/* transactions: these functions never call handlerton functions directly */
int ha_commit_trans(THD *thd, bool all);
int ha_autocommit_or_rollback(THD *thd, int error);
int ha_enable_transaction(THD *thd, bool on);

/* savepoints */
int ha_rollback_to_savepoint(THD *thd, SAVEPOINT *sv);
int ha_savepoint(THD *thd, SAVEPOINT *sv);
int ha_release_savepoint(THD *thd, SAVEPOINT *sv);

/* these are called by storage engines */
void trans_register_ha(THD *thd, bool all, handlerton *ht);

/*
  Storage engine has to assume the transaction will end up with 2pc if
   - there is more than one 2pc-capable storage engine available
   - in the current transaction 2pc was not disabled yet
*/
#define trans_need_2pc(thd, all)                   ((total_ha_2pc > 1) && \
        !((all ? &thd->transaction.all : &thd->transaction.stmt)->no_2pc))

/* semi-synchronous replication */
int ha_repl_report_sent_binlog(THD *thd, char *log_file_name,
                               my_off_t end_offset);
int ha_repl_report_replication_stop(THD *thd);
