#include "common.h"
#include <aos/kernel.h>
#include <fs/vfs.h>
#include <network/network.h>
#include <ulog/ulog.h>

#define BUF_SIZE 8192
#define FTP_ALLPERMS 0x01ff

/* Valid usernames for anonymous ftp */
static const char *usernames[] = 
{
  "ftp","anonymous","public","anon","test","foo","siim"
};

ssize_t sendfile_to_network(int out_fd, int in_fd, off_t * offset, size_t count)
{
  off_t orig = 0;
  char buf[BUF_SIZE];
  size_t toRead, numRead, numSent, totSent;

  if (offset != NULL) {
    /* Save current file offset and set offset to value in '*offset' */
    orig = aos_lseek(in_fd, 0, SEEK_CUR);
    if (orig == -1)
      return -1;
    if (aos_lseek(in_fd, *offset, SEEK_SET) == -1)
      return -1;
  }

  totSent = 0;
  while (count > 0) {
    toRead = count < BUF_SIZE ? count : BUF_SIZE;

    numRead = aos_read(in_fd, buf, toRead);
    if (numRead == -1)
      return -1;
    if (numRead == 0) {
      LOG("%s read end of file", __func__);
      break; /* EOF */
    }

    numSent = write(out_fd, buf, numRead);
    if (numSent == -1)
      return -1;
    if (numSent == 0) { /* Should never happen */
      LOG("sendfile: write() transferred 0 bytes");
      return -1;
    }

    count -= numSent;
    totSent += numSent;
  }

  if (offset != NULL) {
    /* Return updated file offset in '*offset', and reset the file offset
       to the value it had when we were called. */
    *offset = aos_lseek(in_fd, 0, SEEK_CUR);
    if (*offset == -1)
      return -1;
    if (aos_lseek(in_fd, orig, SEEK_SET) == -1)
      return -1;
  }

  return totSent;
}

ssize_t recvfile_from_network(int out_fd, int in_fd)
{
  uint8_t buff[BUF_SIZE];
  size_t toSend, numRead, numSent, totSent = 0;

  while ((numRead = recv(in_fd, buff, sizeof(buff) - 1, 0)) != 0) {
    if (numRead == -1) {
      LOGE(TAG, "Error to recv from network, errno: %d", errno);
      return -1;
    }

    toSend = numRead;
    while (toSend > 0) {
      numSent = aos_write(out_fd, buff, toSend);
      if (numSent == -1) {
        LOGE(TAG, "Error to write");
        return -1;
      }

      toSend -= numSent;
    }

    totSent += numRead;
  }

  return totSent;
}

/**
 * Generates response message for client
 * @param cmd Current command
 * @param state Current connection state
 */
void response(Command *cmd, State *state)
{
  switch (lookup_cmd(cmd->command)) {
    case USER: ftp_user(cmd,state); break;
    case PASS: ftp_pass(cmd,state); break;
    case PASV: ftp_pasv(cmd,state); break;
    case LIST: ftp_list(cmd,state); break;
    case CWD:  ftp_cwd(cmd,state); break;
    case PWD:  ftp_pwd(cmd,state); break;
    case MKD:  ftp_mkd(cmd,state); break;
    case RMD:  ftp_rmd(cmd,state); break;
    case RETR: ftp_retr(cmd,state); break;
    case STOR: ftp_stor(cmd,state); break;
    case DELE: ftp_dele(cmd,state); break;
    case SIZE: ftp_size(cmd,state); break;
    case ABOR: ftp_abor(state); break;
    case QUIT: ftp_quit(cmd, state); break;
    case TYPE: ftp_type(cmd,state); break;
    case NOOP:
      if (state->logged_in) {
        state->message = "200 Nice to NOOP you!\n";
      } else {
        state->message = "530 NOOB hehe.\n";
      }
      write_state(state);
      break;
    default:
      state->message = "500 Unknown command\n";
      write_state(state);
      break;
  }
}

/**
 * Handle USER command
 * @param cmd Command with args
 * @param state Current client connection state
 */
void ftp_user(Command *cmd, State *state)
{
  const int total_usernames = sizeof(usernames) / sizeof(char *);

  if (lookup(cmd->arg, usernames, total_usernames) >= 0) {
    state->username = malloc(32);
    memset(state->username,0,32);
    strcpy(state->username,cmd->arg);
    state->username_ok = 1;
    state->message = "331 User name okay, need password\n";
  } else {
    state->message = "530 Invalid username\n";
  }

  write_state(state);
}

/** PASS command */
void ftp_pass(Command *cmd, State *state)
{
  if (state->username_ok == 1) {
    state->logged_in = 1;
    state->message = "230 Login successful\n";
  } else {
    state->message = "500 Invalid username or password\n";
  }

  write_state(state);
}

/** PASV command */
void ftp_pasv(Command *cmd, State *state)
{
  if (state->logged_in) {
    int ip[4];
    char buff[255];
    char *response = "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\n";
    Port *port = malloc(sizeof(Port));
    gen_port(port);
    getip(state->connection,ip);

    /* Close previous passive socket? */
    close(state->sock_pasv);

    /* Start listening here, but don't accept the connection */
    state->sock_pasv = create_socket((256 * port->p1) + port->p2);
    LOG("port: %d", 256*port->p1 + port->p2);
    sprintf(buff, response,ip[0], ip[1], ip[2], ip[3], port->p1, port->p2);
    state->message = buff;
    state->mode = SERVER;
    LOG(state->message);
  } else {
    state->message = "530 Please login with USER and PASS.\n";
    LOG("%s", state->message);
  }

  write_state(state);
}

/** LIST command */
void ftp_list(Command *cmd, State *state)
{
  if (state->logged_in == 1) {
    aos_dirent_t *entry;
    struct aos_stat statbuf;
    //struct tm *time;
    char timebuff[80], to_send[BSIZE] = {0};
    int connection;
    //time_t rawtime;

    /* TODO: dynamic buffering maybe? */
    char cwd[BSIZE], cwd_orig[BSIZE];
    memset(cwd, 0, BSIZE);
    memset(cwd_orig, 0, BSIZE);
    
    /* Later we want to go to the original path */
    aos_getcwd(cwd_orig, BSIZE);
    
    /* Just chdir to specified path */
    if (strlen(cmd->arg) > 0 && cmd->arg[0] != '-') {
      aos_chdir(cmd->arg);
    }

    aos_getcwd(cwd, BSIZE);
    aos_dir_t *dp = aos_opendir(cwd);

    LOGD(TAG, "%s cwd %s", __func__, cwd);

    if (!dp) {
      state->message = "550 Failed to open directory.\n";
    } else {
      if (state->mode == SERVER) {
        connection = accept_connection(state->sock_pasv);
        state->message = "150 Here comes the directory listing.\n";
        //LOG(state->message);

        char fpath[128];
        while ((entry = aos_readdir(dp)) != NULL) {
          LOGD(TAG, "%s entry %s", __func__, entry->d_name);
          memset(fpath, 0, sizeof(fpath));
          if (cwd[strlen(cwd) - 1] == '/') {
            snprintf(fpath, sizeof(fpath), "%s%s", cwd, entry->d_name);
          } else {
            snprintf(fpath, sizeof(fpath), "%s/%s", cwd, entry->d_name);
          }
          LOGD(TAG, "operating on path %s", fpath);
          if (aos_stat(fpath, &statbuf) != 0) {
            LOGE(TAG, "FTP: Error reading file stats...");
          }else{
            char *perms = malloc(9);
            memset(perms, 0, 9);

            /* Convert time_t to tm struct */
            /* TODO: time, hardcode to Epoch time for now */
            //rawtime = statbuf.st_modtime;
            //time = localtime(&rawtime);
            //strftime(timebuff, sizeof timebuff, "%b %d %H:%M", time);
            snprintf(timebuff, sizeof(timebuff), "%s %s %s:%s", "Jan", "1" , "00", "00");
            str_perm((statbuf.st_mode & FTP_ALLPERMS), perms);
            snprintf(to_send, sizeof(to_send) - 1,
                "%c%s %5d %4d %4d %8d %s %s\r\n", 
                (entry->d_type == DT_DIR) ? 'd' : '-',
                perms, /*TODO: statbuf.st_nlink*/0,
                /*TODO: statbuf.st_uid*/0, 
                /*TODO: statbuf.st_gid*/0,
                statbuf.st_size,
                timebuff,
                entry->d_name);
            /* TODO: write may be partial */
            write(connection, to_send, strlen(to_send) + 1);
          }
        }
        write_state(state);
        state->message = "226 Directory send OK.\n";
        state->mode = NORMAL;
        close(connection);
        close(state->sock_pasv);
      } else if (state->mode == CLIENT) {
        state->message = "502 Command not implemented.\n";
      } else {
        state->message = "425 Use PASV or PORT first.\n";
      }
      aos_closedir(dp);
    }
    aos_chdir(cwd_orig);
  } else {
    state->message = "530 Please login with USER and PASS.\n";
  }
  state->mode = NORMAL;
  write_state(state);
}

/** QUIT command */
void ftp_quit(Command *cmd, State *state)
{
  state->message = "221 Goodbye, friend. I never thought I'd die like this.\n";
  write_state(state);
  close(state->connection);
  free(state);
  free(cmd);
  aos_task_exit(0);
}

/** PWD command */
void ftp_pwd(Command *cmd, State *state)
{
  if (state->logged_in) {
    char cwd[BSIZE];
    char result[BSIZE];
    memset(result, 0, BSIZE);
    if (aos_getcwd(cwd,BSIZE) != NULL) {
      strcat(result,"257 \"");
      strcat(result,cwd);
      strcat(result,"\"\n");
      state->message = result;
    } else {
      state->message = "550 Failed to get pwd.\n";
    }
    write_state(state);
  }
}

/** CWD command */
void ftp_cwd(Command *cmd, State *state)
{
  if (state->logged_in) {
    if (aos_chdir(cmd->arg)==0) {
      state->message = "250 Directory successfully changed.\n";
    } else {
      state->message = "550 Failed to change directory.\n";
    }
  } else {
    state->message = "500 Login with USER and PASS.\n";
  }
  write_state(state);
}

/** 
 * MKD command 
 * TODO: full path directory creation
 */
void ftp_mkd(Command *cmd, State *state)
{
  if (state->logged_in) {
    char cwd[BSIZE];
    char res[BSIZE];
    memset(cwd, 0, BSIZE);
    memset(res, 0, BSIZE);
    aos_getcwd(cwd, BSIZE);

    /* TODO: check if directory already exists with chdir? */

    /* Absolute path */
    if (cmd->arg[0] == '/') {
      if (aos_mkdir(cmd->arg) == 0) {
        strcat(res,"257 \"");
        strcat(res,cmd->arg);
        strcat(res,"\" new directory created.\n");
        state->message = res;
      } else {
        state->message = "550 Failed to create directory. Check path or permissions.\n";
      }
    }
    /* Relative path */
    else {
      if (aos_mkdir(cmd->arg) == 0) {
        snprintf(res, sizeof(res) - 1, "257 \"%s/%s\" new directory created.\n", cwd, cmd->arg);
        state->message = res;
      } else {
        state->message = "550 Failed to create directory.\n";
      }
    }
  } else {
    state->message = "500 Good news, everyone! There's a report on TV with some very bad news!\n";
  }
  write_state(state);
}

/** RETR command */
void ftp_retr(Command *cmd, State *state)
{
  int connection;
  int fd;
  struct aos_stat stat_buf;
  off_t offset = 0;
  int sent_total = 0;
  if (state->logged_in) {
    /* Passive mode */
    if (state->mode == SERVER) {
      if (/*(aos_access(cmd->arg, R_OK) == 0) && */(fd = aos_open(cmd->arg, O_RDONLY))) {
        aos_fstat(fd, &stat_buf);
        state->message = "150 Opening BINARY mode data connection.\n";
        write_state(state);
        connection = accept_connection(state->sock_pasv);
        close(state->sock_pasv);
        if ((sent_total = sendfile_to_network(connection, fd, &offset, stat_buf.st_size)) < 0) {
          if (sent_total != stat_buf.st_size) {
            LOG("ftp_retr:sendfile");
          }

          state->message = "226 File send OK.\n";
        } else {
          state->message = "550 Failed to read file.\n";
        }

        aos_close(fd);
        close(connection);
      } else {
        state->message = "550 Failed to get file\n";
      }
    } else {
      state->message = "550 Please use PASV instead of PORT.\n";
    }
  } else {
    state->message = "530 Please login with USER and PASS.\n";
  }

  write_state(state);
  state->mode = NORMAL;
  close(state->sock_pasv);
}

/** Handle STOR command. TODO: check permissions. */
void ftp_stor(Command *cmd, State *state)
{
  int connection, fd;
  int res = 1;

  fd = aos_open(cmd->arg, O_RDWR | O_CREAT | O_TRUNC);

  if (fd < 0) {
    /* TODO: is this write status message expected? */
    state->message = "452 Requested action not taken.\n";
    LOG("ftp_stor:aos_open (fd: %d)", fd);
  } else if (state->logged_in) {
    if (!(state->mode == SERVER)) {
      state->message = "550 Please use PASV instead of PORT.\n";
    }
    /* Passive mode */
    else {
      connection = accept_connection(state->sock_pasv);

      if (connection == -1) {
        LOGE(TAG, "ftp_stor:accept");
        state->message = "452 Requested action not taken.\n";
      } else {
        close(state->sock_pasv);
        state->message = "125 Data connection already open; transfer starting.\n";
        write_state(state);

        res = recvfile_from_network(fd, connection);

        /* TODO: signal with ABOR command to exit */

        /* Internal error */
        if (res == -1) {
          LOGE(TAG, "ftp_stor:recvfile_from_network");
          state->message = "426 Connection closed; transfer aborted.\n";
        } else {
          state->message = "226 File send OK.\n";
        }

        write_state(state);
        close(connection);
        state->mode = NORMAL;
        return;
      }
    }
  } else {
    state->message = "530 Please login with USER and PASS.\n";
  }

  if (fd >= 0) aos_close(fd);
  write_state(state);
  close(state->sock_pasv);
  state->mode = NORMAL;
}

/** ABOR command */
void ftp_abor(State *state)
{
  if (state->logged_in) {
    state->message = "226 Closing data connection.\n";
    state->message = "225 Data connection open; no transfer in progress.\n";
  } else {
    state->message = "530 Please login with USER and PASS.\n";
  }

  write_state(state);
}

/** 
 * Handle TYPE command.
 * BINARY only at the moment.
 */
void ftp_type(Command *cmd,State *state)
{
  if (state->logged_in) {
    if (cmd->arg[0] == 'I') {
      state->message = "200 Switching to Binary mode.\n";
    } else if (cmd->arg[0] == 'A') {
      /* Type A must be always accepted according to RFC */
      state->message = "200 Switching to ASCII mode.\n";
    }else{
      state->message = "504 Command not implemented for that parameter.\n";
    }
  } else {
    state->message = "530 Please login with USER and PASS.\n";
  }

  write_state(state);
}

/** Handle DELE command */
void ftp_dele(Command *cmd,State *state)
{
  if (state->logged_in) {
    if (aos_unlink(cmd->arg) != 0) {
      state->message = "550 File unavailable.\n";
    } else {
      state->message = "250 Requested file action okay, completed.\n";
    }
  } else {
    state->message = "530 Please login with USER and PASS.\n";
  }

  write_state(state);
}

/** Handle RMD */
void ftp_rmd(Command *cmd, State *state)
{
  if (!state->logged_in) {
    state->message = "530 Please login first.\n";
  } else {
    if (aos_rmdir(cmd->arg) == 0) {
      state->message = "250 Requested file action okay, completed.\n";
    }else{
      state->message = "550 Cannot delete directory.\n";
    }
  }

  write_state(state);
}

/** Handle SIZE (RFC 3659) */
void ftp_size(Command *cmd, State *state)
{
  if (state->logged_in) {
    struct aos_stat statbuf;
    char filesize[128];
    memset(filesize, 0, 128);
    /* Success */
    if (aos_stat(cmd->arg, &statbuf) == 0) {
      sprintf(filesize, "213 %d\n", statbuf.st_size);
      state->message = filesize;
    } else {
      state->message = "550 Could not get file size.\n";
    }
  } else {
    state->message = "530 Please login with USER and PASS.\n";
  }

  write_state(state);
}

/** 
 * Converts permissions to string. e.g. rwxrwxrwx 
 * @param perm Permissions mask
 * @param str_perm Pointer to string representation of permissions
 */
void str_perm(int perm, char *str_perm)
{
  int curperm = 0;
  int read, write, exec;
  
  /* Flags buffer */
  char fbuff[4] = {0};

  read = write = exec = 0;

  int i;
  for (i = 6; i>=0; i-=3) {
    /* Explode permissions of user, group, others; starting with users */
    curperm = ((perm & FTP_ALLPERMS) >> i ) & 0x7;

    memset(fbuff, 0, 3);
    /* Check rwx flags for each*/
    read = (curperm >> 2) & 0x1;
    write = (curperm >> 1) & 0x1;
    exec = (curperm >> 0) & 0x1;

    sprintf(fbuff, "%c%c%c", read ? 'r' : '-', write ? 'w' : '-', exec ? 'x' : '-');
    strcat(str_perm, fbuff);
  }
}
