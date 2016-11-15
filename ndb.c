#include <sys/ptrace.h>
#include <stdio.h>
#include <sys/user.h>

int main(int argc, char **argv) {

	struct user_regs_struct regs;
	pid_t child;
	unsigned long next, cfa, so_offset;
	int i;

	// pid of the process to inspect
	child = strtol(argv[1], NULL, 10);

	// attach to the process and wait to avoid a race condition (ptrace uses an async signal)
	ptrace(PTRACE_ATTACH, child, NULL, NULL);
	wait(NULL);

	// set this to the offset in memory where your shared object is loaded
	// cat /proc/PID/maps for this info
	so_offset = 0x7f88701ac000;

	// get initial register state and print useful registers
	ptrace(PTRACE_GETREGS, child, NULL, &regs);
	printf("rip: %lx rsp: %lx rdi: %ld\n", regs.rip, regs.rsp, regs.rdi);
	printf("func: %lx\n", regs.rip - so_offset);

	// fill this in with frame offsets as you uncover them
	// readelf -w /path/to/obj.so | grep -A10 ADDR
	int frame_offsets[] = {0, 528, 480, 1344};
	next = regs.rsp;

	// step up the stack, printing function addresses
	for (i = 0; i < sizeof(frame_offsets) / sizeof(int); i++) {
		next += frame_offsets[i];
		cfa = ptrace(PTRACE_PEEKDATA, child, next, NULL);
		printf("func: %lx\n", cfa - so_offset);
	}

	// disconnect
	ptrace(PTRACE_DETACH, child);

}
