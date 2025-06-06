#pragma once
#include "../Processes/Process.h"
#include "../DeskTop/DeskTop.h"
#include <mutex>
#include <thread>
#include <chrono>
#include "../Lexer/Lexer.h"
#include "../CustomMsgs/WindowCreationEvent.h"
#include "../CustomMsgs/WindowTerminationEvent.h"
#include <wx/wx.h>
#include <wx/event.h>
#include <algorithm>


class Kernel;

class VirtualOs {

	Kernel* kernel;
	std::vector<Process*> processes;

public:
	DeskTop* desktop;

	VirtualOs(DeskTop* d_top);
};

constexpr int clock_frequency = 10; // every 5ms

class Kernel {
public:
	std::vector<Process*>& processes;
	DeskTop** desktop;
	std::vector<Interrupt> interrupts;
	std::mutex mutex;
	Process* current_process;
	std::thread time_out_manager;
	VirtualOs* os;

	Kernel(std::vector<Process*>& processes_arg, DeskTop** desktop_arg, VirtualOs* os_arg) : processes(processes_arg),
		desktop(desktop_arg), time_out_manager([&]() {
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(clock_frequency));
			if (current_process != nullptr) {
				std::lock_guard<std::mutex> locker(current_process->process_mutex);
				if (
					std::chrono::duration_cast<std::chrono::milliseconds>(
						std::chrono::high_resolution_clock::now() - current_process->last_start_time).count() > TIMEOUT) {
					current_process->should_stop = true;

				}
			}
		}
			}), os(os_arg) {
	
		time_out_manager.detach();

	};

	void doLifeCycle();
	bool processInterrupts();

};