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

/* For use with thr_lock:s */
/*
 * 表锁管理器，无论存储引擎所支持的锁的水平如何，涉及所有存储引擎的表的所有查询都会通过表锁管理器
 * */
#ifndef _thr_lock_h
#define _thr_lock_h
#ifdef	__cplusplus
extern "C" {
#endif

#include <my_pthread.h>
#include <my_list.h>

struct st_thr_lock;
extern ulong locks_immediate, locks_waited;

/*
 * 所有可能用到的表类型都在下面进行定义
 * */
enum thr_lock_type {
    /*
     * 锁请求中使用的一种特殊数值，不得对于锁描述符接哦股执行任何的操作
     * */
    TL_IGNORE = -1,
    /*
     * 锁请求中使用的一种特殊数值，说明应该释放锁
     * */
    TL_UNLOCK,            /* UNLOCK ANY LOCK */
    /*
     * 常规 读 锁
     * */
    TL_READ,            /* Read lock */
    /*
     * InnoDB 为 select .... lock in share mode使用的优先级的读锁
     * */
    TL_READ_WITH_SHARED_LOCKS,
    /* High prior. than TL_WRITE. Allow concurrent insert */
    /*
     * SELECT HIGH_PRIORITY .. 锁使用的高优先级读锁
     * */
    TL_READ_HIGH_PRIORITY,
    /* READ, Don't allow concurrent insert */
    /*
     * 不允许并发插入的特殊读锁
     * */
    TL_READ_NO_INSERT,
    /*
   Write lock, but allow other threads to read / write.
   Used by BDB tables in MySQL to mark that someone is
   reading/writing to the table.
     */
    /*
     * 存储引擎使用的特殊锁，用于锁定本身
     * 当此锁暂停锁定时，允许其他线程取得读锁和写锁
     * */
    TL_WRITE_ALLOW_WRITE,
    /*
   Write lock, but allow other threads to read.
   Used by ALTER TABLE in MySQL to allow readers
   to use the table until ALTER TABLE is finished.
    */
    /*
     * 用于 alter table 的特殊锁。修改一个表包括使用新表结构创建一个临时表，在表中填写新行，然后重新以原来的名称命名
     * 在这个操作中，可以在修改时读取表内容
     * */
    TL_WRITE_ALLOW_READ,
    /*
      WRITE lock used by concurrent insert. Will allow
      READ, if one could use concurrent insert on table.
    */
    /*
     * 由同步插入所使用的写锁。如果表中已经放置了这一类型的锁，则除非向 TL_READ_NO_INSERT 提出请求，否则会立即将读锁交给其他线程
     * */
    TL_WRITE_CONCURRENT_INSERT,
    /* Write used by INSERT DELAYED.  Allows READ locks */
    /*
     * INSERT DELAYED ... 所使用的特殊锁
     * */
    TL_WRITE_DELAYED,
    /* WRITE lock that has lower priority than TL_READ */
    /*
     * UPDATE LOW_PRIORITY .... 及其他具有 LOW_PRRORITY 属性的查询所使用的低优先级的锁
     * */
    TL_WRITE_LOW_PRIORITY,
    /* Normal WRITE lock */
    /*
     * 常规写锁
     * */
    TL_WRITE,
    /* Abort new lock request with an error */
    /*
     * 在请求关闭表时的操作执行期间，中止旧锁时所使用的内部数值
     * */
    TL_WRITE_ONLY
};

enum enum_thr_lock_result {
    THR_LOCK_SUCCESS = 0, THR_LOCK_ABORTED = 1,
    THR_LOCK_WAIT_TIMEOUT = 2, THR_LOCK_DEADLOCK = 3
};


extern ulong max_write_lock_count;
extern ulong table_lock_wait_timeout;
extern my_bool thr_lock_inited;
extern enum thr_lock_type thr_upgraded_concurrent_insert_lock;

/*
  A description of the thread which owns the lock. The address
  of an instance of this structure is used to uniquely identify the thread.
*/

typedef struct st_thr_lock_info {
    pthread_t thread;
    ulong thread_id;
    ulong n_cursors;
} THR_LOCK_INFO;

/*
  Lock owner identifier. Globally identifies the lock owner within the
  thread and among all the threads. The address of an instance of this
  structure is used as id.
*/

typedef struct st_thr_lock_owner {
    THR_LOCK_INFO *info;
} THR_LOCK_OWNER;


typedef struct st_thr_lock_data {
    THR_LOCK_OWNER *owner;
    struct st_thr_lock_data *next, **prev;
    struct st_thr_lock *lock;
    pthread_cond_t *cond;
    enum thr_lock_type type;
    void *status_param;            /* Param to status functions */
    void *debug_print_param;
} THR_LOCK_DATA;

struct st_lock_list {
    THR_LOCK_DATA *data, **last;
};

typedef struct st_thr_lock {
    LIST list;
    pthread_mutex_t mutex;
    struct st_lock_list read_wait;
    struct st_lock_list read;
    struct st_lock_list write_wait;
    struct st_lock_list write;
    /* write_lock_count is incremented for write locks and reset on read locks */
    ulong write_lock_count;
    uint read_no_write_count;

    void (*get_status)(void *, int);    /* When one gets a lock */
    void (*copy_status)(void *, void *);

    void (*update_status)(void *);        /* Before release of write */
    my_bool (*check_status)(void *);
} THR_LOCK;


extern LIST *thr_lock_thread_list;
extern pthread_mutex_t THR_LOCK_lock;

my_bool init_thr_lock(void);        /* Must be called once/thread */
#define thr_lock_owner_init(owner, info_arg) (owner)->info= (info_arg)
void thr_lock_info_init(THR_LOCK_INFO *info);
void thr_lock_init(THR_LOCK *lock);
void thr_lock_delete(THR_LOCK *lock);
void thr_lock_data_init(THR_LOCK *lock, THR_LOCK_DATA *data,
                        void *status_param);
enum enum_thr_lock_result thr_lock(THR_LOCK_DATA *data,
                                   THR_LOCK_OWNER *owner,
                                   enum thr_lock_type lock_type);
void thr_unlock(THR_LOCK_DATA *data);
enum enum_thr_lock_result thr_multi_lock(THR_LOCK_DATA **data,
                                         uint count, THR_LOCK_OWNER *owner);
void thr_multi_unlock(THR_LOCK_DATA **data, uint count);
void thr_abort_locks(THR_LOCK *lock);
my_bool thr_abort_locks_for_thread(THR_LOCK *lock, pthread_t thread);
void thr_print_locks(void);        /* For debugging */
my_bool thr_upgrade_write_delay_lock(THR_LOCK_DATA *data);
my_bool thr_reschedule_write_lock(THR_LOCK_DATA *data);
#ifdef    __cplusplus
}
#endif
#endif /* _thr_lock_h */
