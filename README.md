# sysrepo-examples
Some examples highlighting the core concepts in sysrepo

## Preparations
Make sure you have installed sysrepo. On Ubuntu, run
```sh
$ sudo apt install sysrepo libsysrepo-dev
```

For other Linux distributions, please have a look with your preferred distribution's package manager.

## set-get-config
Illustrates how to retrieve and update a configuration value. Typically, an application would get configuration values from the running data store. A configuration tool, or some other process, would set the configuration value.

The example has a very simple YANG module definition that defines a single configuration value of type uint32. The application simply increments it for each run, and then iterates through all values in the module and prints them to stdout.

### Running
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

## observer
Illustrates how a service can observe any YANG module in sysrepo (entire modules in this case, but can be narrowed done with XPath).

The example has a very simple YANG module definition that defines a single configuration value of type uint32. The `service` (the observer) registers a subscription callback for the entire module `observer`, and no XPath (`NULL`). A change to the module is done using `simulate-change` (typically, the change would've come from a NETCONF client, but to keep the example short and to the point, this is what you get!). `simulate-change` will write a random value between `0` and `100`. `service` will see the changes as events, and print values to the terminal.

### Running
Install the YANG module by
```sh
$ make install-yang
```

Build by
```sh
$ make
```

Run service as
```sh
$ ./service
```

Simulate a change in the sysrepo values with
```sh
$ ./simulate-change
```

### Demonstrated aspects
What is demonstrated in this example is the following aspects:
- Any change to a subscribe module (or part of module) can be observed
- Any service can listen to any module - two or more services can depend on the same configuration, e.g. _hostname_
- Changes are delivered as they are changed AND as they are committed (or aborted) (`SR_EV_CHANGE` vs `SR_EV_DONE` and `SR_EV_ABORT`). Allows for two phase commit transactions within the system.

N.B. The original source code in https://github.com/sysrepo/sysrepo/blob/master/examples/application_changes_example.c is for a more recent version of sysrepo than what is available on Ubuntu 21.10, and does not compile as is! Notably, the subscription callback function has changed.

## state
Illustrates how a service can provide state in a "pull" manner via a callback to provide state to the operational data store.

The example has a very simple YANG module definition that defines a single configuration value of type uint32. The `service` (the state provider) registers a callback for the entire module `state`. Any read for this module from the _operational datastore_ will trigger a call to this callback. The callback's job is to provide data for the state the service maintains. The service will update a random value every 100 ms, and send it off to the requester in the callback.

The state value is fetched via a separate client application (typically netconf/netopeer as a response to a network request or CLI request).

### Running
Install the YANG module by
```sh
$ make install-yang
```

Build by
```sh
$ make
```

Run service as
```sh
$ ./service
```

Run the client to get the state value
```sh
$ ./client
```

### Demonstrated aspects
What is demonstrated in this example is the following aspects:
- State can be pulled from a service via a callback
- The difference between running and operational data store - the service provides state to the _running_ datastore, while the client pulls it from the _operational_ datastore.

N.B. The original source code in https://github.com/sysrepo/sysrepo/blob/master/examples/application_changes_example.c is for a more recent version of sysrepo and libyang than what is available on Ubuntu 21.10, and does not compile as is! Notably, the callback function and the libyang API for creating configuration nodes have changed.
