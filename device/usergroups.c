/*
 * Copyright (C) 2024-11-01 Johannes Intke
 * Licensed under the GNU General Public License v2.0 (GPLv2.0)
 * Email: <root@openjohannes.org>
*/

#include <include/usergroups.h>

char* process_file(const char *filetxt, const char *filemode, char *content)
{
    char path[256];
    snprintf(
        path,
        sizeof(
            path
        ),
        "/storage/%s",
        filetxt
    );

    FILE *txt = fopen(path, filemode);

    if (txt == NULL) {
       return "ERR";
    }

    if (strcmp(content, "NULL") != 0 && strcmp(filemode, "w") == 0) {
        fprintf(txt, content);
        fclose(txt);

        txt = fopen(path, "r");
        char fread[100];
        while (fgets(fread, sizeof(fread), txt) != NULL) {
            if (strstr(content, fread) != NULL) {
                return "WRITE_SUCCESS";
            }
        }

        fclose(txt);
        return "WRITE";
    }

    if (strcmp(content, "NULL") == 0 && strcmp(filemode, "r") == 0) {
        static char buffer[256];
        if (fgets(buffer, 12, txt) != NULL) {
            fclose(txt);
            return buffer;
        }
    }

    fclose(txt);
    return "NONE";
}

char* format_filename(const char *name) 
{
    char *filename = malloc(strlen(name) + strlen(".txt") + 1);
    if (filename == NULL) {
        fprintf(stderr, "mallocerr");
        exit(1);
    }

    strcpy(filename, name);
    strcat(filename, ".txt");
    return filename;
}

char* search_user(const char *all, const char *name) {
    struct dirent *entry;
    DIR *directory_search = opendir("/storage");

    if (directory_search == NULL) {
        return "ERR";
    }

    if (strcmp(all, "YES") == 0 && strcmp(name, "NULL") == 0) {
        while ((entry = readdir(directory_search)) != NULL) {
            static char users[256];
            snprintf(
                 users,
                 sizeof(
                     users
                 ),
                 "%s",
                 entry->d_name
            );
            closedir(directory_search);
            return users;
        }
    }

    if (strcmp(all, "YES") != 0 && strcmp(name, "NULL") != 0) {
        while ((entry = readdir(directory_search)) != NULL) {
            if (strcmp(name, entry->d_name) == 0) {
                static char found_users[256];
                snprintf(
                     found_users,
                     sizeof(
                         found_users
                     ),
                     "%s",
                     entry->d_name
                );
                closedir(directory_search);
                return found_users;
            } else {
                return "USER_NOT_FOUND";
            }
        }
    }

    closedir(directory_search);
    return "NOTHING";
}

char* delete_user(const char *name)
{
    char *filename = format_filename(name);
    char *is_valid_user = search_user("YES", "NULL");

    if (strcmp(is_valid_user, filename) == 0) {

         char path[256];
         snprintf(
             path,
             sizeof(
                path
             ),
             "/storage/%s",
             filename
         );

         if (remove(path) == 0) {
             free(filename);
             return "SUCCESS";
         } else {
            free(filename);
            return "U_F_B_F_C_D";
         }
    } else {
        free(filename);
        return "USER_NOT_FOUND";
    }

    free(filename);
    return "NOTHING";
}

char *create_user(const char *name, int phone_number, int is_admin)
{
    char *users = search_user("YES", "NULL");
    if (strcmp(users, name) != 0) {
         char content[256];
         char *filename = format_filename(name);

         snprintf(
             content,
             sizeof(
                content
             ),
             "%d\n%d",
             phone_number,
             is_admin
         );

         char *process_state = process_file(filename, "w", content);
         if ((process_state = "WRITE_SUCCESS") != NULL) {
            free(filename);
            return "USER_CREATED";
         } else {
             free(filename);
             return "COULD_NOT_CREATE_USER";
         }
     } else {
         return "USER_ALREADY_EXISTS";
     }

   return "NOTHING";
}

int fs_init(void)
{
    esp_vfs_spiffs_conf_t config = {
         .base_path = "/storage",
         .partition_label = "storage",
         .max_files = 5,
         .format_if_mount_failed = true,
    };
    esp_vfs_spiffs_register(&config);

    if (esp_vfs_spiffs_register(&config)) {
        return 0;
    } else {
        return 1;
    }
}
