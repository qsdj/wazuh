/* Copyright (C) 2009 Trend Micro Inc.
 * All right reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */

#ifndef __SYSCHECK_H
#define __SYSCHECK_H

#include "config/syscheck-config.h"
#define MAX_LINE PATH_MAX+256

/* Notify list size */
#define NOTIFY_LIST_SIZE    32

// Number of attributes in the hash table
#define SK_DB_NATTR 10

#define WDATA_DEFAULT_INTERVAL_SCAN 300

/* Global config */
extern syscheck_config syscheck;

/** Function Prototypes **/

/* Check the integrity of the files against the saved database */
void run_check(void);

/* Run run_check periodically */
void start_daemon(void) __attribute__((noreturn));

/* Read the XML config */
int Read_Syscheck_Config(const char *cfgfile) __attribute__((nonnull));

/* Create the database */
int create_db(void);

/* Check database for changes */
int run_dbcheck(void);

/* Scan directory */
int read_dir(const char *dir_name, int dir_position, whodata_evt *evt, int max_depth);

/* Check the registry for changes */
void os_winreg_check(void);

/* Start real time */
int realtime_start(void);

/* Add a directory to real time monitoring */
int realtime_adddir(const char *dir, int whodata) __attribute__((nonnull(1)));

/* Initializes the whodata scan mode */
int run_whodata_scan(void);

/* Process real time queue */
int realtime_process(void);

/* Process the content of the file changes */
char *seechanges_addfile(const char *filename) __attribute__((nonnull));

/* Generate the whodata csum */
int extract_whodata_sum(whodata_evt *evt, char *wd_sum, int size);
void init_whodata_event(whodata_evt *w_evt);
void free_whodata_event(whodata_evt *w_evt);

/* Get checksum changes */
int c_read_file(const char *file_name, const char *oldsum, char *newsum, whodata_evt * evt) __attribute__((nonnull(1,2,3)));

int send_syscheck_msg(const char *msg) __attribute__((nonnull));
int send_rootcheck_msg(const char *msg) __attribute__((nonnull));


int realtime_checksumfile(const char *file_name, whodata_evt *evt) __attribute__((nonnull(1)));

/* Find container directory */
int find_dir_pos(const char *filename, int full_compare, int check_find, int deep_search) __attribute__((nonnull(1)));

#ifdef __linux__
int audit_init(void);
int check_auditd_enabled(void);
int set_auditd_config(void);
int init_auditd_socket(void);
int audit_add_rule(const char *path, const char *key);
int audit_delete_rule(const char *path, const char *key);
void *audit_main(int *audit_sock);
void clean_rules(void);
extern W_Vector *audit_added_dirs;
extern volatile int audit_thread_active;
extern pthread_mutex_t audit_mutex;
extern pthread_cond_t audit_thread_started;
#elif WIN32
int whodata_audit_start();
int set_winsacl(const char *dir, int position);
long unsigned int WINAPI state_checker(__attribute__((unused)) void *_void);
#endif

extern pthread_mutex_t lastcheck_mutex;
int fim_initialize();

/* Check for restricts and ignored files */
int fim_check_ignore(const char *file_name);
int fim_check_restrict(const char *file_name, OSMatch *restriction);

#endif
