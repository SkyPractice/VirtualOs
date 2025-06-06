#pragma once
class Process;
class OsAppIcon;


enum InterruptSource {
	HardWareInterrupt, UserModeInterrupt
};

enum InterruptType {
	SysCallInterrupt, TimerInterrupt, ProcessCreation,
	ProcessTerminationInterrupt
};

struct Interrupt {
	InterruptSource source;
	InterruptType type;
	Process* caller; // from a process
	OsAppIcon* icon_caller; // clicking on a desktop icon
};