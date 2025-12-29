# Getting Started

## Preparation

First, you have to compile scalc, in most cases you should already have all the tools needed for this, but here is the list again:
- gcc
- make
- libc (POSIX-compliant)

Also to clone the repository you would need `git`

Below is a list of commands that will help you install all the tools:

### Ubuntu/Debian
```
# apt install git gcc make
```

### Arch
```
# pacman -S git gcc make
```

### Gentoo
If you are running Gentoo you should know better, but you must already have these.
```
# emerge --ask --noreplace dev-vcs/git sys-devel/gcc dev-build/make
```

### Fedora/RHEL/CentOS
```
# dnf install git gcc make
```

### MacOS
If you have Homebrew:
```
$ brew install git gcc make
```

If you don't have Homebrew, install Xcode Command Line Tools:
```
$ xcode-select --install
```

### FreeBSD
```
# pkg install git gcc gmake
```

### OpenSUSE
```
# zypper install git gcc make
```

### Alpine
```
# apk add gcc make git musl-dev
```

### Void Linux
```
# xbps-install -S git gcc make
```

## Compilation

First you will need to clone the repository:
```
$ git clone https://gitlab.com/baka.senpai.lts/scalc
```

Then compile your scalc:
```
$ cd scalc
$ make
```

If you want to optimize your build, feel free to tweak CFLAGS however you want, here is how I build it generally, but note that I'm using gcc:
```
$ CFLAGS='-std=c99 -Wall -Wextra -Werror -Wpedantic -Oz -flto -march=native' make
```

## Next steps

Once you have your binary ready, you can proceed to reading the [basics](./basics.md)
