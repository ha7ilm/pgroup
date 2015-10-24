pgroup
======

While writing scripts that start subprocesses that start other subprocesses, etc., it was always hard to make sure all of these are terminated properly. 

    pgroup [-9] <command>

**pgroup** executes a command given as its parameter. If **pgroup** is sent SIGTERM/SIGQUIT, it will send it to all of its subprocesses (children, grandchildren, etc.), and then terminate.

The parameter `-9` tells pgroup to send SIGKILL to all subprocesses instead of sending them the signal originally caught, so we can terminate even the most unwilling processes.

For example, if you use `-9` and send SIGTERM to **pgroup**, it will send SIGKILL to all of its child processes.

## Installing pgroup

**pgroup** requires Linux to run. You can build it with *gcc* by:

    make && sudo make install
