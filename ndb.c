#include <sys/ptrace.h>
#include <stdio.h>
#include <sys/user.h>

int main(int argc, char **argv) {
	struct user_regs_struct regs;
	pid_t child;
	unsigned long parent;
	child = strtoul(argv[1], NULL, 10);
	ptrace(PTRACE_ATTACH, child, NULL, NULL);
	wait(NULL);
	ptrace(PTRACE_GETREGS, child, NULL, &regs);
	printf("rip: %lx rsp: %lx rdi: %ld\n", regs.rip, regs.rsp, regs.rdi);
	parent = ptrace(PTRACE_PEEKDATA, child, regs.rsp, NULL);
	printf("parent rip: %lx\n", parent);
	ptrace(PTRACE_DETACH, child);
}
