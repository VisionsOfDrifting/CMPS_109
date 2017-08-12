// $Id: cix.cpp,v 1.4 2016-05-09 16:01:56-07 - - $

#include <iostream>
#INCLUDE <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>
using namespace std;

#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>

#include "protocol.h"
#include "logstream.h"
#include "sockets.h"

logstream log (cout);
struct cix_exit: public exception {};

unordered_map<string,cix_command> command_map {
   {"exit", cix_command::EXIT},
   {"help", cix_command::HELP},
   {"ls"  , cix_command::LS },
   {"get" , cix_command::GET },
   {"rm"  , cix_command::RM },
   {"put" , cix_command::PUT },
};

void cix_help() {
   static const vector<string> help = {
      "exit         - Exit the program.  Equivalent to EOF.",
      "get filename - Copy remote file to local host.",
      "help         - Print help summary.",
      "ls           - List names of files on remote server.",
      "put filename - Copy local file to remote host.",
      "rm filename  - Remove file from remote server.",
   };
   for (const auto& line: help) cout << line << endl;
}

void cix_ls (client_socket& server) {
   cix_header header;
   header.command = cix_command::LS;
   log << "sending header " << header << endl;
   send_packet (server, &header, sizeof header);
   recv_packet (server, &header, sizeof header);
   log << "received header " << header << endl;
   if (header.command != cix_command::LSOUT) {
      log << "sent LS, server did not return LSOUT" << endl;
      log << "server returned " << header << endl;
   }else {
      char buffer[header.nbytes + 1];
      recv_packet (server, buffer, header.nbytes);
      log << "received " << header.nbytes << " bytes" << endl;
      buffer[header.nbytes] = '\0';
      cout << buffer;
   }
}

void cix_get(client_socket& server, string filename) {
    cix_header header;
    memset(&header, 0, sizeof header);
    header.command = cix_command::GET;
    strcpy(header.filename, filename.c_str());
    log << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);
    recv_packet(server, &header, sizeof header);
    log << "received header " << header << endl;
    if (header.command != cix_command::FILE) {
        log << "sent cix_command::GET, server did \
        not return cix_command::FILE" << endl;
        log << "server returned " << header << endl;
        return;
    }
    char buffer[header.nbytes + 1];
    recv_packet(server, buffer, header.nbytes);
    log << "received " << header.nbytes << " bytes" << endl;
    buffer[header.nbytes] = '\0';
    cout << buffer;
    write_to_file(filename, buffer, header.nbytes);
}

void cix_put(client_socket& server, string filename) {
    cix_header header;
    memset(&header, 0, sizeof header);
    ifstream file (filename);
    if (file == NULL) {
        log << "invalid filename (" << filename << ")" << endl;
        return;
    }
    uint32_t nbytes = header.nbytes = file_size(filename);
    char output[nbytes];
    load_from_file(filename, output, nbytes);
    // send put cmd
    header.command = cix_command::PUT;
    strcpy(header.filename, filename.c_str());

    log << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);

    log << "sending: " << output << endl
        << "===" << endl;
    send_packet(server, output, sizeof output);

    // get ack or fail
    recv_packet(server, &header, sizeof header);
    log << "received header " << header << endl;
    if (header.command != cix_command::ACK) {
        log << "sent cix_command::PUT, server did \
        not return cix_command::ACK" << endl;
        log << "server returned " << header << endl;
        log << "with err msg: " << strerror(header.nbytes) << endl;
        return;
    }
    log << "successfully sent file (" << filename << ")" << endl;
}

void cix_rm(client_socket& server, string filename) {
    cix_header header;
    memset(&header, 0, sizeof header);
    header.command = cix_command::RM;
    strcpy(header.filename, filename.c_str());
    header.nbytes = 0;
    log << "sending header " << header << endl;
    send_packet(server, &header, sizeof header);

    recv_packet(server, &header, sizeof header);
    log << "received header " << header << endl;
    if (header.command != cix_command::ACK) {
        log << "sent cix_command::RM, server did \
        not return cix_command::ACK" << endl;
        log << "server returned " << header << endl;
        log << "with err msg: " << strerror(header.nbytes) << endl;
        return;
    }
    log << "successfully rm'ed file (" << filename << ")" << endl;
}

void usage() {
   cerr << "Usage: " << log.execname() << " [host] [port]" << endl;
   throw cix_exit();
}

int main (int argc, char** argv) {
   log.execname (basename (argv[0]));
   log << "starting" << endl;
   vector<string> args (&argv[1], &argv[argc]);
   if (args.size() > 2) usage();
   string host = get_cix_server_host (args, 0);
   in_port_t port = get_cix_server_port (args, 1);
   log << to_string (hostinfo()) << endl;
   try {
      log << "connecting to " << host << " port " << port << endl;
      client_socket server (host, port);
      log << "connected to " << to_string (server) << endl;
      for (;;) {
         string line;
         getline (cin, line);
         if (cin.eof()) throw cix_exit();
         log << "command " << line << endl;
         size_t fst_space_pos = line.find(" ");
         log << "fst_space_pos " << fst_space_pos << endl;
         string cmd_str = line.substr(0, fst_space_pos);
         string args = (fst_space_pos != string::npos \
                  ? line.substr(fst_space_pos+1, line.size()) : "");
         log << "cmd_str: " << cmd_str << endl;
         log << "args: " << args << endl;
         if (cmd_str.size() == 0) continue;
         
         const auto& itor = command_map.find (line);
         cix_command cmd = itor == command_map.end()
                         ? cix_command::ERROR : itor->second;
         switch (cmd) {
            case cix_command::EXIT:
               throw cix_exit();
               break;
            case cix_command::HELP:
               cix_help();
               break;
            case cix_command::LS:
               cix_ls (server);
               break;
            case cix_command::GET:
                 cix_get(server, args);
                 break;
            case cix_command::PUT:
                 cix_put(server, args);
                 break;
            case cix_command::RM:
                 cix_rm(server, args);
                 break;
            default:
               log << line << ": invalid command" << endl;
               break;
         }
      }
   }catch (socket_error& error) {
      log << error.what() << endl;
   }catch (cix_exit& error) {
      log << "caught cix_exit" << endl;
   }
   log << "finishing" << endl;
   return 0;
}

