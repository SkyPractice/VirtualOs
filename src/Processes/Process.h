#pragma once
#include "../Interpreter/Interpreter.h"
#include <chrono>
#include <mutex>
#include "../Kernel/Interrupt.h"
#include "ProcessWindow.h"
#include <cstdlib>
#include "OutputLabel.h"

#define TIMEOUT 10
class Process final {
public:
	Interpreter interpreter;
	std::vector<shared_ptr<RunTimeVal>> sys_call_args;
	std::vector<Interrupt>* interrupt_vec;
	std::mutex& kernel_mutex;
	std::chrono::steady_clock::time_point last_start_time;
	bool should_stop = false; // by a hardware interrupt or a timer
	std::mutex process_mutex;
	ProcessWindow* window;
	int random_iden;
	OutputLabel* output_label;
	bool suspended = true;
	std::chrono::high_resolution_clock::time_point sleep_time;
	int sleep_duration;
	bool isSleeping = false;

	Process(std::vector<shared_ptr<StatementObj>>& statements,
		std::vector<Interrupt>* interrupts,
		std::mutex& m, ProcessWindow* win) : interpreter(statements),
		interrupt_vec(interrupts), kernel_mutex(m),
	window(win){
		random_iden = rand();
	};

	Process(const Process& other);

	Process& operator=(const Process& other);

	shared_ptr<RunTimeVal> continueExecution();

	~Process();
};