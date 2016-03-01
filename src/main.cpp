//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include "server.hpp"
#include <getopt.h>

namespace po = boost::program_options;

static void skeleton_daemon()
{
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    //TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    //chdir("/");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>0; x--)
    {
        close (x);
    }

    /* Open the log file */
    openlog ("httpserver", LOG_PID, LOG_DAEMON);
}

bool parse_parameters(int argc, char** argv, std::string& host, std::string& port, std::string& doc_root, std::string& log_file) {
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("host,h", po::value<std::string>(), "host or ip")
            ("port,p", po::value<std::string>(), "port to listen")
            ("docroot,d", po::value<std::string>(), "server document's root")
            ("log,l", po::value<std::string>(), "log file path")
            ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return false;
    }

    if (vm.count("host")) {
        host = vm["host"].as<std::string>();
    } else {
        std::cout << "Using localhost as default host.\n";
        host = "localhost";
    }

    if (vm.count("port")) {
        port = vm["port"].as<std::string>();
    } else {
        std::cout << "Using 6666 as default port.\n";
        port = "6666";
    }

    if (vm.count("docroot")) {
        doc_root = vm["docroot"].as<std::string>();
    } else {
        std::cout << "Using current directory as default documents root.\n";
        doc_root = "./";
    }

    if (vm.count("log")) {
        log_file = vm["log"].as<std::string>();
    }

    return true;
}

bool parse_parameters_getopt(int argc, char** argv, std::string& host, std::string& port, std::string& doc_root, std::string& log_file) {
    int c;

    int opterr = 0;
    while ((c = getopt (argc, argv, "h:p:d:l:")) != -1)
        switch (c)
        {
        case 'h':
            host = std::string(optarg);
            break;
        case 'p':
            port = std::string(optarg);
            break;
        case 'd':
            doc_root = std::string(optarg);
            break;
        case 'l':
            log_file = std::string(optarg);
            break;
        default:
            break;
        }

    return true;
}

int main(int argc, char* argv[])
{
    try {
        std::string host, port, doc_root, log_file;
        if (!parse_parameters_getopt(argc, argv, host, port, doc_root, log_file))
            return 0;

        skeleton_daemon();
        // Initialise the server.
        http::server::server s(host, port, doc_root, log_file);

        while (1)
        {
            // Run the server until stopped.
            syslog (LOG_NOTICE, "Http server started.");
            s.run();
            break;
        }

    }
    catch (std::exception& e)
    {
        std::cerr << "exception: " << e.what() << "\n";
    }

    syslog (LOG_NOTICE, "Http server terminated.");
    closelog();

    return 0;
}
