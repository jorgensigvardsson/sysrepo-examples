# sysrepo-examples
Some examples highlighting the core concepts in sysrepo

Make sure you have installed sysrepo. On Ubuntu, run
```sh
$ sudo apt install sysrepo libsysrepo-dev
```

## set-get-config
Illustrates how to retrieve and update a configuration value. Typically, an application would get configuration values from the running data store. A configuration tool, or some other process, would set the configuration value.

The example has a very simple YANG module definition that defines a single configuration value of type uint32. The application simply increments it for each run, and then iterates through all values in the module and prints them to stdout.

Install the YANG module by
```sh
$ make install-yang
```

Build by
```sh
$ make
```

And run as
```sh
$ ./set-get-config
```

