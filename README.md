pgroup
======

While writing scripts that start subprocesses that start other subprocesses, etc., it was always hard to make sure all of these are terminated properly. 

    pgroup [-9] <command>

**pgroup** executes a command given as its parameter. If **pgroup** is sent SIGTERM/SIGQUIT, it will send it to all of its subprocesses (children, grandchildren, etc.), and then terminate.

The parameter `-9` tells pgroup to send SIGKILL to all subprocesses instead of sending them the signal originally caught, so we can terminate even the most unwilling processes.

For example, if you use `-9` and send SIGTERM to **pgroup**, it will send SIGKILL to all of its child processes.

**pgroup** also terminates all of its subprocesses if its parent process ends, or any of its child processes end.

## Installing pgroup

**pgroup** works on Linux. You can build it with *gcc* by:

    make && sudo make install

## How to use in shell scripts

```bash
#!/bin/bash
#We create some processes
pgroup -9 bash -c "cat /dev/urandom | cat | cat >/dev/null" &
CHILDPID=$!
sleep 0.2
pstree $$ #We see 3*[cat]

kill $CHILDPID #Send SIGTERM to pgroup, which kills all subprocesses
echo
pstree $$ #Now we don't see pgroup and the [cat]s anymore.
```

This would be the output of this script:

    ha7ilm@linux ~ $ bash test.sh
    bash─┬─pgroup───bash───3*[cat]
         └─pstree
    
    bash───pstree

## How to use in Qt

```C++
#include <sys/types.h>
#include <signal.h>
#include <QProcess>

void MainWindow::startAndStopSomeProcesses()
{
	QProcess proc;
	proc.start("pgroup -9 bash -c \"cat /dev/urandom | cat | cat >/dev/null\"");
	kill(proc.pid(), SIGTERM); //Now we can make sure that all subprocesses were killed properly.
}
```

## How to use in python

```python
import subprocess, os, time
myproc=subprocess.Popen(["pgroup","-9","bash","-c", "cat /dev/urandom | cat | cat >/dev/null" ])
time.sleep(0.3)
os.system("pstree "+str(os.getpid()))
myproc.terminate()
myproc.wait() #to end the zombie process that would remain
print
os.system("pstree "+str(os.getpid()))
```

In fact, executing a subprocess with *setpgrp()* and terminating the whole group has already been possible in python:

```python
import subprocess, os, signal, time
myproc = subprocess.Popen("cat /dev/urandom | cat | cat >/dev/null", shell=True, preexec_fn=os.setpgrp) #<== python will fork(), setpgrp() and then exec(), the same that pgroup does
time.sleep(0.3)
os.system("pstree "+str(os.getpid()))
os.killpg(os.getpgid(myproc.pid), signal.SIGKILL)
myproc.wait()
os.system("pstree "+str(os.getpid()))
```

However, the first version using `pgroup -9` is safer than doing this!

What happens if we SIGKILL our python script? The subprocesses will remain... 

If we use **pgroup**, it will get notified by the OS that it became orphaned, and it terminates itself and all of its subprocesses.

## How does it work

When the **pgroup** process is started, it calls <a href="http://www.unix.com/man-page/linux/3/setpgrp/">setpgrp()</a>, so it becomes a session leader. 

Later it uses <a href="http://www.unix.com/man-page/linux/3/killpg">killpg()</a> to send a signal to its process group.

It does not check if the signal is handled by all the processes in the group. 

If you want to be absolutely sure that all subprocesses are terminated, you should use the `-9` switch. If you do this:
* the subprocesses cannot handle the SIGKILL signal, they are just stopped by the OS anyway;
* on the other hand, they cannot properly close files and things, so only use it if necessary.

