#define _CRT_SECURE_NO_WARNINGS
#include "Process.h"

shared_ptr<RunTimeVal> Process::continueExecution() {
	last_start_time = std::chrono::high_resolution_clock::now();
	std::shared_ptr<RunTimeVal> last_result = interpreter.nextStatement();
	while (last_result->type == BoolType && 
		std::dynamic_pointer_cast<BoolVal>(last_result)->val) {
		{
			std::lock_guard<std::mutex> lock(process_mutex);
			if (should_stop) {
				//					   For Simulation Purposses
				interrupt_vec->push_back({ HardWareInterrupt, TimerInterrupt, this, nullptr });
				should_stop = false;
				return make_shared<BoolVal>(true);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(2));
		{
			std::lock_guard<std::mutex> kernel_locker(kernel_mutex);
			for (const auto& interruptt : (*interrupt_vec)) {
				// simulating hardware interrupt immediate switch
				if (interruptt.type == HardWareInterrupt) return  make_shared<BoolVal>(true);
			}
		}
		last_result = interpreter.nextStatement();
	}
	if (last_result->type == SysCallType)
		return last_result;
	return make_shared<BoolVal>(false);
}

Process& Process::operator=(const Process& other){

	if (this == &other) {
		return *this;
	}

	interpreter = other.interpreter;
	sys_call_args = other.sys_call_args;
	interrupt_vec = other.interrupt_vec;
	last_start_time = other.last_start_time;
	should_stop = other.should_stop;

	return *this;
};

Process::Process(const Process& other) : interpreter(other.interpreter), interrupt_vec(other.interrupt_vec), kernel_mutex(other.kernel_mutex) {

}

Process::~Process() {
	if(window)
		delete window;
}
