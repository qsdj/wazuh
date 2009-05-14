/* @(#) $Id$ */

/* Copyright (C) 2008 Third Brigade, Inc.
 * All rights reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 3) as published by the FSF - Free Software
 * Foundation
 */



#include "manage_agents.h"


int OS_IsValidID(char *id)
{
    int id_len = 0;
    int i = 0;
    
    /* ID must not be null */ 
    if(!id)
      return(0);

    id_len = strlen(id);

    /* Check ID length, it should contain max. 5 characters */
    if (id_len > 8)
      return(0);

    /* Check ID if it contains only numeric characters [0-9] */
    for(i = 0; i < id_len; i++)
    {
      if(!(isdigit((int)id[i])))
        return(0);
    }
    
    return(1);
}


/* Get full agent name (name + ip) of ID.
 */
char *getFullnameById(char *id)
{
    FILE *fp;
    char line_read[FILE_SIZE +1];
    line_read[FILE_SIZE] = '\0';

    /* ID must not be null */
    if(!id)
        return(NULL);

    fp = fopen(AUTH_FILE, "r");
    if(!fp)
        return(NULL);


    while(fgets(line_read, FILE_SIZE -1, fp) != NULL)
    {
        char *name;
        char *ip;
        char *tmp_str;

        if(line_read[0] == '#')
        {
            continue;
        }

        name = strchr(line_read, ' ');
        if(name)
        {
            *name = '\0';
            /* Didn't match */
            if(strcmp(line_read,id) != 0)
            {
                continue;
            }

            name++;

            /* Removed entry */
            if(*name == '#')
            {
                continue;
            }
            
            ip = strchr(name, ' ');
            if(ip)
            {
                *ip = '\0';
                ip++;

                /* Cleaning up ip */
                tmp_str = strchr(ip, ' ');
                if(tmp_str)
                {
                    char *final_str;
                    *tmp_str = '\0';
                    tmp_str = strchr(ip, '/');
                    if(tmp_str)
                        *tmp_str = '\0';

                    /* If we reached here, we found the IP and name */
                    os_calloc(1, FILE_SIZE, final_str);
                    snprintf(final_str, FILE_SIZE -1, "%s-%s", name, ip);

                    fclose(fp);
                    return(final_str);        
                }
            }
        }
    }

    fclose(fp);
    return(NULL);
}


/* ID Search (is valid ID) */
int IDExist(char *id)
{
    FILE *fp;
    char line_read[FILE_SIZE +1];
    line_read[FILE_SIZE] = '\0';
   
    /* ID must not be null */ 
    if(!id)
        return(0);

    fp = fopen(AUTH_FILE, "r");
    if(!fp)
        return(0);
        
    fseek(fp, 0, SEEK_SET);
    fgetpos(fp, &fp_pos);
    
    while(fgets(line_read,FILE_SIZE -1, fp) != NULL)
    {
        char *name;

        if(line_read[0] == '#')
        {
            fgetpos(fp, &fp_pos);
            continue;
        }
        
        name = strchr(line_read, ' ');
        if(name)
        {
            *name = '\0';
            name++;

            if(strcmp(line_read,id) == 0)
            {
                fclose(fp);
                return (1); /*(fp_pos);*/
            }
        }

        fgetpos(fp, &fp_pos);
    }

    fclose(fp);
    return(0);
}


/* Validate agent name.
 */
int OS_IsValidName(char *u_name)
{
    int i = 0;

    /* We must have something in the name */
    if(strlen(u_name) < 2 || strlen(u_name) > 128)
      return(0);

    /* check if it contains any non-alphanumeric characters */
    for(i = 0; i < strlen(u_name); i++)
    {
      if(!isalnum((int)u_name[i]) && (u_name[i] != '-') && 
         (u_name[i] != '_') && (u_name[i] != '.'))
        return(0);
    }

    return(1);
}


/* Is_Name (is valid name) */
int NameExist(char *u_name)
{
    FILE *fp;
    char line_read[FILE_SIZE +1];
    line_read[FILE_SIZE] = '\0';

    if((!u_name)||
       (*u_name == '\0')||
       (*u_name == '\r')||
       (*u_name == '\n'))
        return(0);

    fp = fopen(AUTH_FILE, "r");
    if(!fp)
        return(0);


    fseek(fp, 0, SEEK_SET);
    fgetpos(fp, &fp_pos);


    while(fgets(line_read, FILE_SIZE-1, fp) != NULL)
    {
        char *name;

        if(line_read[0] == '#')
            continue;

        name = strchr(line_read, ' ');
        if(name)
        {
            char *ip;
            name++;

            if(*name == '#')
            {
                continue;
            }
            
            ip = strchr(name, ' ');
            if(ip)
            {
                *ip = '\0';
                if(strcmp(u_name, name) == 0)
                {
                    fclose(fp);
                    return(1);
                }
            }
        }
        fgetpos(fp, &fp_pos);
    }

    fclose(fp);
    return(0);
}


/* print available agents */
int print_agents(int print_status, int active_only, int csv_output)
{
    int total = 0;
    FILE *fp;
    char line_read[FILE_SIZE +1];
    line_read[FILE_SIZE] = '\0';

    fp = fopen(AUTH_FILE, "r");
    if(!fp)
        return(0);

    fseek(fp, 0, SEEK_SET);
    
    memset(line_read,'\0',FILE_SIZE);
    
    while(fgets(line_read, FILE_SIZE -1, fp) != NULL)
    {
        char *name;

        if(line_read[0] == '#')
            continue;
            
        name = strchr(line_read, ' ');
        if(name)
        {
            char *ip;
            *name = '\0';
            name++;


            /* Removed agent. */
            if(*name == '#')
            {
                continue;
            }
            
            ip = strchr(name, ' ');
            if(ip)
            {
                char *key;
                *ip = '\0';
                ip++;
                key = strchr(ip, ' ');
                if(key)
                {
                    *key = '\0';
                    if(!total && !print_status)
                        printf(PRINT_AVAILABLE);
                    total++;

                    
                    if(print_status)
                    {
                        int agt_status = get_agent_status(name, ip);
                        if(active_only && (agt_status != GA_STATUS_ACTIVE))
                        {
                            continue;
                        }
            
                        if(csv_output)
                        {
                            printf("%s,%s,%s,%s,\n", line_read, name, ip, 
                                                  print_agent_status(agt_status));  
                        }
                        else
                        {
                            printf(PRINT_AGENT_STATUS, line_read, name, ip, 
                                   print_agent_status(agt_status));
                        }
                    }
                    else
                    {
                        printf(PRINT_AGENT, line_read, name, ip);
                    }
                }
                
            }
        }
    }


    /* Only print agentless for non-active only searches */
    if(!active_only && print_status)
    {
        char *aip = NULL;
        DIR *dirp;
        struct dirent *dp;
        
        if(!csv_output)
        {
            printf("\nList of agentless devices:\n");
        }

        dirp = opendir(AGENTLESS_ENTRYDIR);
        if(dirp)
        {
            while ((dp = readdir(dirp)) != NULL)
            {
                if(strncmp(dp->d_name, ".", 1) == 0)
                {
                    continue;
                }

                aip = strchr(dp->d_name, '@');
                if(aip)
                {
                    aip++;
                }
                else
                {
                    aip = "<na>";
                }

                if(csv_output)
                {
                    printf("na,%s,%s,agentless,\n", dp->d_name, aip);
                }
                else
                {
                    printf("   ID: na, Name: %s, IP: %s, agentless\n",
                           dp->d_name, aip);
                }
            }
            closedir(dirp);
        }
    }

    fclose(fp);
    if(total)
        return(1);
    
    return(0);    
}


/* EOF */
