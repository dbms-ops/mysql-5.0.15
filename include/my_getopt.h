/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB

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

#ifndef _my_getopt_h
#define _my_getopt_h

C_MODE_START

#define GET_NO_ARG     1        /* 本选项不接受任何参数，无需考虑变量问题 */
#define GET_BOOL       2        /* 变量类型为 my_bool */
#define GET_INT        3        /* 变量类型为 int  */
#define GET_UINT       4        /* 变量类型为 uint */
#define GET_LONG       5        /* 变量类型为 long */
#define GET_ULONG      6        /* 变量类型为 mulong */
#define GET_LL         7        /* 变量类型为 LLong  */
#define GET_ULL        8        /* 变量类型为 ullong*/
#define GET_STR        9        /* 变量类型为 char *， 在执行解析时，变量被指向包含选项值的位置 */
/*
 * 变量类型为 char *， 如果初始数值不为 0，则选项解析代码假定 先前已经使用 my_alloc()分配过指针， 并且将使用 my_free() 释放指针；
 * 选项的数值既可以调用函数分配的与定位位置结束，也可以以选项分析器分配的位置结束；
 *
 * */
#define GET_STR_ALLOC 10
#define GET_DISABLED  11        /* 选项解析器解析了的选项，但改选项被禁用。如果使用，则解析中止，同时返回一个错误代码 */

/*
 * 该数值与其他数值相或。
 * 如果启用则由 sql/mysqld.cc 中的特殊函数 mysql_getopt_value() 给出变量地址
*/

#define GET_ASK_ADDR	 128
#define GET_TYPE_MASK	 127

/*
 * NO_ARG： 本选项不接受任何参数，提供参数将出错
 * OPT_ARG： 本选项可以解析参数，不提供也不会出错【将使用默认值】。本类型通常告诉 MySQL 在日志中记录某些事项的选项使用，
 * 并且可以选择性的指定记录的位置
 * REQUIRED_ARG： 要求用户提供参数，如果不提供参数，将会出错
 * */
enum get_opt_arg_type { NO_ARG, OPT_ARG, REQUIRED_ARG };

/* 配置选项的成员定义
 * */
struct my_option
{
  const char *name;                     /* 配置选项的名称，命令行中增加两个前缀字符：-- */
  int        id;                        /* 选项的唯一整型 ID代码。如果在 ASCII 字符集的范围之内，支持简短形式，例如：b, 支持 -b */
  const char *comment;                  /* mysqld --help 的简要文档说明 */
  gptr       *value;                    /* The variable value. 变量的值 */
  gptr       *u_max_value;              /* The user def. max variable value 用户声明变量的最大值 */
  const char **str_values;              /* Pointer to possible values 指向可能值得指针*/
  ulong     var_type;                   /* 变量类型代码，参考 本文 22--> 35  */
  enum get_opt_arg_type arg_type;       /* 变量类型代码， */
  longlong   def_value;                 /* Default value 参数的默认值 */
  longlong   min_value;                 /* Min allowed value 最小值，如果指定了一个小于最小值的数值，实际值为最小值 */
  longlong   max_value;                 /* Max allowed value 最大值，如果指定了一个大于最大值的数值，实际值为最大值 */
  longlong   sub_size;                  /* Subtract this from given value 将选项存储在与选项相关的变量中之前要减去的数值 */
  long       block_size;                /* Value should be a mult. of this 选项数值调整为本参数的倍数 */
  int        app_type;                  /* To be used by an application */
};

typedef my_bool (* my_get_one_option) (int, const struct my_option *, char * );
typedef void (* my_error_reporter) (enum loglevel level, const char *format, ... );

extern char *disabled_my_option;
extern my_bool my_getopt_print_errors;
extern my_error_reporter my_getopt_error_reporter;

extern int handle_options (int *argc, char ***argv, 
			   const struct my_option *longopts, my_get_one_option);
extern void my_print_help(const struct my_option *options);
extern void my_print_variables(const struct my_option *options);
extern void my_getopt_register_get_addr(gptr* (*func_addr)(const char *, uint,
							   const struct my_option *));

ulonglong getopt_ull_limit_value(ulonglong num, const struct my_option *optp);
my_bool getopt_compare_strings(const char *s, const char *t, uint length);

C_MODE_END

#endif /* _my_getopt_h */

