#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <algorithm>
#include <iterator>


using namespace std;



struct ProcInfo {
  string name;
  string cmd;
};

vector<string> readProcessFile() {
  vector<string> lines;

  // Create a text string, which is used to output the text file
  string line;

  // Read from the text file
  ifstream ProcFile("process.proc");

  // Use a while loop together with the getline() function to read the file line by line
  while (getline (ProcFile, line)) {
    // Output the text from the file
    lines.push_back(line);
  }
  // Close the file
  ProcFile.close();

  return lines;
}

vector<ProcInfo> getProcs() {
  vector<string> procFile = readProcessFile();

  vector<ProcInfo> procs;

  for (auto& line : procFile) {
    ProcInfo proc;
    proc.name = line.substr(0, line.find(":"));
    proc.cmd = line.substr(line.find(":") + 1, line.length());
    procs.push_back(proc);
  }

  return procs;
}

void runProcess(ProcInfo proc) {
  char buffer[128];
  string result = "";

  cout << "Running: " << proc.cmd << endl;


  int pid = fork();

  if (pid == 0) { // we are the child

    // convert command to something that execvp understands
    istringstream iss(proc.cmd);
    vector<string> argsVector{istream_iterator<string>{iss},
                istream_iterator<string>{}};

    const char **argv = new const char* [argsVector.size()+1];   // extra room for sentinel
    for (int i = 0; i < argsVector.size(); i++) {
      argv[i] = argsVector[i].c_str();
    }
    argv[argsVector.size()] = NULL;


    // pipe the stdout/stderr of the command to a file
    int fd = open("proc.log", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    dup2(fd, 1);   // make stdout go to file
    dup2(fd, 2);   // make stderr go to file - you may choose to not do this

    close(fd);     // fd no longer needed - the dup'ed handles are sufficient

    execvp(argv[0], (char *const *)argv);

  } else {
     cout << "PID: " << pid << endl;
  }
}


int main(int argc, char* argv[]) {
  if (argc < 2) {
    cout << "Please specify at a process to run" << endl;
    return 0;
  }

  string procName = argv[1];

  vector<ProcInfo> procs = getProcs();

  bool found = false;
  for (auto& proc : procs) {
    // we found the process, now lets run it
    if (proc.name == procName) {
      found = true;
      runProcess(proc);
    }
  }

  if (!found) {
    cout << "could not find process with that name" << endl;
  }

  return 0;
}