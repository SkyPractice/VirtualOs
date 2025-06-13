#pragma once
#include "../Interpreter/Interpreter.h"
#include <chrono>
#include <mutex>
#include "../Kernel/Interrupt.h"
#include "ProcessWindow.h"
#include <cstdlib>
#include "OutputLabel.h"
#include <thread>
#include "../IO/IoOperations.h"
#include <condition_variable>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>	
#include <boost/beast/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <fstream>

namespace net = boost::asio;
namespace ssl = net::ssl;
namespace tcp = net::ip;
namespace beast = boost::beast;
namespace http = beast::http;

class BoostBundler {
public:
	static boost::asio::io_context io_ctx;
	static boost::asio::ssl::context ssl_ctx;
};

struct RequestData {
	std::string scheme;
	std::string host;
	std::string path;
};

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
	std::thread io_thread;
	std::shared_ptr<IoOperation> io_operation = nullptr;
	std::condition_variable io_condition_var;

	Process(std::vector<shared_ptr<StatementObj>>& statements,
		std::vector<Interrupt>* interrupts,
		std::mutex& m, ProcessWindow* win) : interpreter(statements),
		interrupt_vec(interrupts), kernel_mutex(m),
	window(win), io_thread([&](){
		while(true){
			{
				std::unique_lock<std::mutex> locker(process_mutex);
				io_condition_var.wait(locker);
				if(!io_operation)
					break;
				switch(io_operation->type){
					case HttpRequestType:{
						std::shared_ptr<HttpRequest> req = std::dynamic_pointer_cast<HttpRequest>(io_operation);
						switch(req->httpType){
							case GetRequestType:
								processGetRequest(std::dynamic_pointer_cast<GetRequest>(req));
								break;
							case PostRequestType:
								processPostRequest(std::dynamic_pointer_cast<PostRequest>(req));
								break;
							case PutRequestType:
								processPutRequest(std::dynamic_pointer_cast<PutRequest>(req));
								break;
							case DeleteRequestType:
								processDeleteRequest(std::dynamic_pointer_cast<DeleteRequest>(req));
								break;
							case FileDownloadRequestType:
								processFileDownloadRequest(std::dynamic_pointer_cast<FileDownloadRequest>(req));
								break;
						}
					}
						break;
					default:
						break;
				}
				io_operation = nullptr;
				suspended = false;

			}

		}
	}){
		interpreter.proc = this;
		random_iden = rand();
	};

	Process(const Process& other);
	void wakeUpIoThread() {
		io_condition_var.notify_all();
	};
	void processGetRequest(std::shared_ptr<GetRequest> req);
	void processPostRequest(std::shared_ptr<PostRequest> req);
	void processPutRequest(std::shared_ptr<PutRequest> req);
	void processDeleteRequest(std::shared_ptr<DeleteRequest> req);
	void processFileDownloadRequest(std::shared_ptr<FileDownloadRequest> req);

	std::string doHttpRequest(std::string scheme, std::string host, std::string path, 
		boost::beast::http::verb method, std::string body = "", std::string body_type = "",
			bool is_file_download = false, std::string file_NAME = "");
	
	RequestData parseUrl(std::string url);
	

	Process& operator=(const Process& other);

	shared_ptr<RunTimeVal> continueExecution();

	~Process();
};