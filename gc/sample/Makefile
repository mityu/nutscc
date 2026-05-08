SRCS := main.c ../gc.c ../hashmap.c

.PHONY: all
all: nogc withgc

nogc: $(SRCS)
	gcc -O0 -g -o nogc $(SRCS)

withgc: $(SRCS)
	gcc -O0 -g -DENABLE_GC -o withgc $(SRCS)

# Note that the "pidstat" command can be installed via `nix-shell -p sysstat`.
.PHONY: perf-nogc
perf-nogc: nogc
	pidstat -r 1 -e ./nogc

.PHONY: perf-withgc
perf-withgc: withgc
	pidstat -r 1 -e ./withgc
