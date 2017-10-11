/**
 * CS3013
 * Assignment 1
 * Yuan Wang- ywang19@wpi.edu
 */

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <iomanip>
#include <time.h>
#include <string.h>
#include <sstream>
#include <pwd.h>
#include <vector>

using namespace std;

/**
 * This struct is about a running process.
 * the pid of the process, the command that was run, and the time
 * (wall clock time) that the process was started at.
 */
typedef struct
{
    int pid;
    string command;
    long startTime;
} process;

/**
 * Print information which returned from the getrusage() function.
 * arrange the format about the output information
 */
void printStatus(char const *stat, long val)
{
    cout << left <<setw(30) << stat << right << setw(10) << val << endl;
}

/**
 * Print out process stats as requirements from the instructions.
 * Some formats and structs are from the GNU C Library,
 * ex: getrusage, rusage, timeval
 */
void printProcessStats(long start_ms)
{
    
    struct rusage childUsage;
    struct timeval end;
    gettimeofday(&end, NULL);
    long end_ms = end.tv_sec * 1000 + end.tv_usec / 1000;//end_ms
    getrusage(RUSAGE_CHILDREN, &childUsage);
    
    printStatus("Wall Clock Time: ", end_ms - start_ms);
    printStatus("User CPU Time:", childUsage.ru_utime.tv_sec * 1000 + childUsage.ru_utime.tv_usec / 1000);
    printStatus("System CPU Time:", childUsage.ru_stime.tv_sec * 1000 + childUsage.ru_stime.tv_usec / 1000);
    printStatus("Max RSS:", childUsage.ru_maxrss);
    printStatus("Integral Shared Memory Size:", childUsage.ru_ixrss);
    printStatus("Integral Unshared Data Size:", childUsage.ru_idrss);
    printStatus("Integral Unshared Stack Size:", childUsage.ru_isrss);
    printStatus("Page Reclaims:", childUsage.ru_minflt);
    printStatus("Page Faults:", childUsage.ru_majflt);
    printStatus("Swaps:", childUsage.ru_nswap);
    printStatus("Block Input Operations:", childUsage.ru_inblock);
    printStatus("Block Output Operations:", childUsage.ru_oublock);
    printStatus("IPC Messages Sent:", childUsage.ru_msgsnd);
    printStatus("IPC Messages Received:", childUsage.ru_msgrcv);
    printStatus("Signals Received:", childUsage.ru_nsignals);
    printStatus("Voluntary Context Switches:", childUsage.ru_nvcsw);
    printStatus("Involuntary Context Switches:", childUsage.ru_nivcsw);
}

/**
 * This help function about shutting down the shell . It will wait for all
 * background tasks to complete and then exits.
 * using pid_t waitpid (pid_t pid, int *status-ptr, int options) to check the
 */

void terminate(vector<process> *childProcess)
{
    if (childProcess -> size() > 0) //if there are any children left, wait for them to complete.
    {
        cout << "It is waiting for background processes to complete..." << endl;
        for (unsigned long i = 0; i < childProcess -> size(); i++) //loop through children => wait each of them
        {
            int processStatus;
            pid_t result = waitpid(childProcess -> at(i).pid, &processStatus, 0);
            if (result == 0) //ChildProcess is running
            {
            }
            else if (result < 0) //Error
            {
            }
            else //childProcess quit, print statistics
            {
                cout << "[" << i + 1 << "] " << childProcess->at(i).pid << " " << childProcess->at(i).command << " [Finished]" << endl;
                printProcessStats(childProcess->at(i).startTime);
            }
        }
    }
    exit(0); //terminate program
}

/**
 * Main function of the doit program which determines
 * which mode to run the shell in, and then executes that mode. It does this
 * by determining how many arguments the program has. If there is only one
 * argument, then the user is attempting to run the program in shell mode. If
 * there are more than one, then the user wants the program to simply run the
 * input command, print the stats, and then exit.
 * argc => The argument count.
 * argv => Program arguments as a string.
 * pid_t waitpid (pid_t pid, int *status-ptr, int options) => check any child processes, find if any are dead
 *
 */
int main(int argc, char **argv)
{
    if (argc == 1)
    {
        cout << "The program is executing as shell..." << endl;
        vector<process> childProcess;
        
        while (1)
        {
            for (unsigned long i = 0; i < childProcess.size(); i++)
            {
                int processStatus;
                pid_t result = waitpid(childProcess.at(i).pid, &processStatus, WNOHANG);
                if (result == 0) //Child running
                {
                }
                else if (result < 0) //Error
                {
                }
                else //Child quit
                {
                    cout << "[" << i + 1 << "] " << childProcess.at(i).pid << " " << childProcess.at(i).command << " [Finished]" << endl;
                    printProcessStats(childProcess.at(i).startTime);
                    childProcess.erase(childProcess.begin() + i);
                }
               
            }
            /* Instead of using a default string of "==>"
             * I wanted the program to look like a standard shell prompt, so this extracts the
             * username, the hostname, and the currently running path and uses those to construct the prompt.
             */
            
            struct passwd *passwd;
            passwd = getpwuid(getuid());
            char hostname[128];
            gethostname(hostname, sizeof hostname);
            char currentpath[256];
            getcwd(currentpath, sizeof currentpath);
            cout << passwd -> pw_name << "@" << hostname << ":" << currentpath << "$ ";

            //an array of 32 commands to fit assignment spec
            char *argvNew[32];
            string cmd;
            getline(cin, cmd); //Get a line of input from user
            if (!cin)
            {
                if (cin.eof())
                {
                    cout << "Got EOF, exiting..." << endl;
                    terminate(&childProcess);
                }
            }
            istringstream is(cmd); //Create stringstream for parsing into arguments
            string part;
            int arg = 0;
            int bg = 0;
            if (cmd.length() >= 1)
            {
                while (getline(is, part, ' ')) //Using getline to parse string, delimited by space characters
                {
                    char *cstr = strdup(part.c_str());
                    if (strncmp(cstr, "&", 1) == 0)
                    {
                        cout << "Background requested." << endl;
                        bg = 1;
                    }
                    else
                    {
                        argvNew[arg] = cstr;
                        arg++;
                    }
                }
                argvNew[arg] = NULL;
                if (strncmp(argvNew[0], "exit", 4) == 0) //parse out exit command
                {
                    terminate(&childProcess);
                }
                else if (strncmp(argvNew[0], "cd", 2) == 0) //Parse out cd command
                {
                    int result = chdir(argvNew[1]); //Change directory to first argument of cd command
                    if (result < 0)
                    {
                        cerr << "Error changing directory!" << endl;
                    }
                }
                else if (strncmp(argvNew[0], "jobs", 4) == 0) //Parse out jobs command
                {
                    if (childProcess.size() == 0)
                    {
                        cout << "No jobs running." << endl;
                    }
                    else
                    {
                        for (unsigned long i = 0; i < childProcess.size(); i++) //If there are any children, print out their names and information.
                        {
                            cout << "[" << i + 1 << "] " << childProcess[i].pid << " " << childProcess[i].command << endl;
                        }
                    }
                }
                else
                {
                    int pid;
                    struct timeval start;
                    gettimeofday(&start, NULL);
                    long start_ms = start.tv_sec * 1000 + start.tv_usec / 1000; //Calculate start time
                    if ((pid = fork()) < 0) //fork failed
                    {
                        cerr << "Fork error!" << endl;
                    }
                    else if (pid == 0) //is child
                    {
                        if (execvp(argvNew[0], argvNew) < 0)
                        {
                            cerr << "Execvp error!" << endl;
                            exit(1);
                        }
                    }
                    else //is parent
                    {
                        if (bg != 1) //if not requested to be backgrounded, wait for process and then print stats.
                        {
                            wait(0);
                            printProcessStats(start_ms);
                        }
                        else
                        {
                            process proc = {pid, cmd, start_ms};
                            childProcess.push_back(proc); //Add new process to list of jobs.
                            cout << "[" << childProcess.size() << "] " << childProcess.back().pid << " " << childProcess.back().command << endl; //Print that child has started.
                        }
                    }
                }
            }
        }
    }
    else if (argc > 1) //Execute input command and quit
    {
        int pid;
        struct timeval start;
        gettimeofday(&start, NULL);
        long start_ms = start.tv_sec * 1000 + start.tv_usec / 1000; //calculate start time of child
        //Exec given command
        if ((pid = fork()) < 0) //fork failed
        {
            cerr << "Fork error!" << endl;
        }
        else if (pid == 0)   //is child
        {
            char *argvNew[argc];
            for (int i = 1; i < argc; i++) //Copies argv into a new array without ./doit to feed into exec
            {
                argvNew[i - 1] = argv[i];
            }
            argvNew[argc - 1] = NULL; //argv must be null terminated
            if (execvp(argvNew[0], argvNew) < 0)
            {
                cerr << "Execvp error!" << endl;
                exit(1);
            }
        }
        else //is parent
        {
            wait(0); //wait for child to finish before printing stats
            printProcessStats(start_ms);
        }
    }
}

