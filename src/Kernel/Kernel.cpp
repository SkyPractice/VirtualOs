#define _CRT_SECURE_NO_WARNINGS
#include "Kernel.h"

void Kernel::doLifeCycle() {
	(*desktop)->task_bar->processes = &processes;
	(*desktop)->task_bar->Refresh();
	std::thread([&](){
		while (true) {
			if (!processes.empty()) {

				for (auto process = processes.begin(); process != processes.end(); process++) {
					current_process = &(**process);

					if (current_process->suspended && !current_process->isSleeping) {
						std::this_thread::sleep_for(std::chrono::milliseconds(TIMEOUT));
						if (processInterrupts()) {
							(*desktop)->task_bar->Refresh();
							break;
						};
						continue;
					}
					else if (current_process->suspended && current_process->isSleeping) {
						if (std::chrono::duration_cast<std::chrono::milliseconds>(
							std::chrono::high_resolution_clock::now() - current_process->sleep_time
						).count() >= current_process->sleep_duration) {
							current_process->isSleeping = false;
							current_process->suspended = false;
						}
						else {
							if (processInterrupts()) {
								(*desktop)->task_bar->Refresh();
								break;
							}
							continue;
						};
					}
					try
					{
						shared_ptr<RunTimeVal> result = (**process).continueExecution();
						if (result->type == BoolType)
						{
							shared_ptr<BoolVal> val = std::dynamic_pointer_cast<BoolVal>(result);
							if (!val->val)
							{
								SystemCalls::sys_calls["print"]({make_shared<StringVal>("Program Successfully Ended . . . ")},
																current_process);
								current_process->suspended = true;
								current_process->window->Refresh();
							}
							// close the program gracefully if its false
						}
						else if (result->type == SysCallType)
						{
							shared_ptr<SysCallVal> val = std::dynamic_pointer_cast<SysCallVal>(result);
							// do the syscall
							SystemCalls::sys_calls[val->name](val->args, current_process);
						}
					} catch(std::exception& exp){
						std::string str = "Error From Process: " + current_process->window->application_name + "\n" + exp.what();
						wxLogError(str.c_str());
						processes.erase(process);
						wxQueueEvent(os->desktop, new WindowTerminationEvent((current_process)->random_iden));
						break;
					}
					if (processInterrupts()) {
						(*desktop)->task_bar->Refresh();
						break;
					};

					current_process = nullptr;
				}

			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				processInterrupts();
			}
		}
		
		}).detach();
	
}
// Returns true if the process vec was edited
// syscalls should be here but i forgot and im not moving it cause it will kill me
bool Kernel::processInterrupts() {
	std::lock_guard<std::mutex> locker(mutex);
	bool edited = false;
	for (auto& interrupt : interrupts) {
		try
		{
			if (interrupt.type == ProcessCreation)
			{
				std::vector<Token> tokens = Lexer::tokenize(interrupt.icon_caller->source_path);

				const Token mode = tokens[0];
				tokens.erase(tokens.begin());
				shared_ptr<ProgramObj> program = Parser::produceAst(tokens);

				const WindowType window_type = (mode.symbol == "console" ? ConsoleWindow : GUIWindow);

				Process *process = new Process(program->stmts, &interrupts, mutex, nullptr);

				processes.push_back(process);
				wxQueueEvent(os->desktop, new WindowCreationEvent(interrupt.icon_caller->app_name,
																  process, &interrupts, &mutex, interrupt.icon_caller->icon, window_type));

				edited = true;
			}
			else if (interrupt.type == ProcessTerminationInterrupt)
			{
				auto itr = std::find_if(processes.begin(), processes.end(), [&](Process *proc)
										{ return proc->random_iden == interrupt.caller->random_iden; });

				if (itr != processes.end())
				{
					wxQueueEvent(os->desktop, new WindowTerminationEvent((*itr)->random_iden));
					processes.erase(itr);
					current_process = nullptr;
				}
				edited = true;
			}
		} catch(std::exception& exp){
			wxLogError((std::string("Error While Executing An Interrupt: ") + exp.what()).c_str());
		} 
	}

	interrupts.clear();
	return edited;
}