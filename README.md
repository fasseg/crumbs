# Crumbs

Crumbs is a command line utility for the shell, for storing commands under a meaningful name in a hierarchy

## Motivation

For web service tests I am used to sending a lot of HTTP Requests via `curl`. The command line invocation can be quite complex and long and I wanted to be able to organize them in a directory like structure. Also I wanted to have a context sensitive auto completion for the different command invocations.

## Why not us aliases?

Aliases do not allow a user to use foward slashes in the alias name in order to create complex hierarchies and therefore no context sensitive auto completion.

## Installation

### Prerequisites

In order to compile and install crumbs on your local system you the following software must be available on the system:
1. [gcc](https://gcc.gnu.org/)
2. [make](https://www.gnu.org/software/make/)
3. [gzip](https://www.gnu.org/software/gzip/)
4. [git](https://git-scm.com/)

### Installation procedure

1. Checkout the sources from github:
```bash
git clone https://github.com/fasseg/crumbs.git
```
2. Compile the source code
```bash
cd crumbs
make
```
3. Install the binaries and the man pages
```bash
sudo make install
```

### Auto Completion

An auto completion file for the bash shell is included and installed in `/usr/local/share/crumbs/crumbs-completion.bash`. Inm order to enable the completion it can e.g. be added to the user's `.bashrc` file:
```bash
echo "source /usr/local/share/crumbs/crumbs-completion.bash" >> ~/.bashrc

```


### Usage

You can check the man page for usage documentation and some examples:
```bash
man crumbs
```


 

