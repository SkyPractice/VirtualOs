#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#include "Process.h"

boost::asio::io_context BoostBundler::io_ctx;
boost::asio::ssl::context BoostBundler::ssl_ctx{boost::asio::ssl::context::tls_client};

struct BoostInitializer {
	BoostInitializer(){
		BoostBundler::ssl_ctx.set_verify_mode(boost::asio::ssl::context_base::verify_none);
	}
};

static BoostInitializer initializer;

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
};

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

};

RequestData Process::parseUrl(std::string url){
	std::string scheme;
	if(url.starts_with("https")){
		scheme = "https";
		url = url.substr(8);
	} else {
		scheme = "http";
		url = url.substr(7);
	}
	size_t host_name_end = url.find('/');
	std::string host;
	std::string path;
	if(host_name_end != std::string::npos){
		host = url.substr(0, host_name_end);
		url = url.substr(host_name_end);
		path = url;
	} else {
		host = url;
		path = "/";
	}
	return { scheme, host, path };
}

std::string Process::doHttpRequest(std::string scheme, std::string host, std::string path, 
	boost::beast::http::verb method, std::string body, std::string body_type, bool is_file_download,
		std::string file_NAME){
	boost::asio::ip::tcp::resolver resolver(BoostBundler::io_ctx);
	if (scheme == "https")
	{

		boost::asio::ssl::stream<boost::asio::ip::tcp::socket> sock(BoostBundler::io_ctx, BoostBundler::ssl_ctx);
		SSL_set_tlsext_host_name(sock.native_handle(), host.c_str());
		auto result = resolver.resolve(host, "https");

		boost::asio::connect(sock.lowest_layer(), result);
		sock.handshake(boost::asio::ssl::stream_base::client);

		boost::beast::http::request<boost::beast::http::string_body> req(method, path, 11);
		req.set(boost::beast::http::field::host, host);
		req.set(boost::beast::http::field::user_agent, "myuseragent");
		if(!body_type.empty())
			req.set(boost::beast::http::field::content_type, body_type);
		if(!body.empty())
			req.body() = body;

		boost::beast::http::write(sock, req);	

		boost::beast::flat_buffer buff;
		if(!is_file_download){
			boost::beast::http::response<boost::beast::http::string_body> res;

			boost::beast::http::read(sock, buff, res);

			boost::system::error_code error;
			std::string bodd = res.body();
			sock.lowest_layer().close(error);

			return bodd;
		}
		else {
			boost::beast::http::response<boost::beast::http::file_body> res;
			boost::system::error_code err;
			res.body().open(file_NAME.c_str(), boost::beast::file_mode::write, err);
			boost::beast::http::read(sock, buff, res);
			res.body().close();
			boost::system::error_code error;
			sock.lowest_layer().close(error);
		}

	}
	else if (scheme == "http")
	{

		boost::asio::ip::tcp::socket sock(BoostBundler::io_ctx);
		auto result = resolver.resolve(host, "http");

		boost::asio::connect(sock, result);

		boost::beast::http::request<boost::beast::http::string_body> req(boost::beast::http::verb::get, path, 11);
		req.set(boost::beast::http::field::host, host);
		req.set(boost::beast::http::field::user_agent, "myuseragent");
		if(!body_type.empty())
			req.set(boost::beast::http::field::content_type, body_type);
		if(!body.empty())
			req.body() = body;

		boost::beast::http::write(sock, req);

		boost::beast::flat_buffer buff;
		if(!is_file_download){
			boost::beast::http::response<boost::beast::http::string_body> res;

			boost::beast::http::read(sock, buff, res);

			boost::system::error_code error;
			std::string bodd = res.body();
			sock.lowest_layer().close(error);

			return bodd;
		}
		else {
			boost::beast::http::response<boost::beast::http::file_body> res;
			boost::system::error_code err;
			res.body().open(file_NAME.c_str(), boost::beast::file_mode::write, err);
			boost::beast::http::read(sock, buff, res);
			res.body().close();
			boost::system::error_code error;
			sock.lowest_layer().close(error);
		}
	}
	return "The Available Schemes are http and https only!";
	};

void Process::processGetRequest(std::shared_ptr<GetRequest> request){
	RequestData url_data = parseUrl(request->address);

	interpreter.current_scope->variables["eax"] = std::make_shared<StringVal>(doHttpRequest(url_data.scheme, 
		url_data.host, url_data.path, boost::beast::http::verb::get));
};

void Process::processPostRequest(std::shared_ptr<PostRequest> req){
	RequestData url_data = parseUrl(req->address);

	interpreter.current_scope->variables["eax"] = std::make_shared<StringVal>(doHttpRequest(url_data.scheme, 
		url_data.host, url_data.path, boost::beast::http::verb::post, req->body, "application/json"));

};
void Process::processPutRequest(std::shared_ptr<PutRequest> req){
	RequestData url_data = parseUrl(req->address);

	interpreter.current_scope->variables["eax"] = std::make_shared<StringVal>(doHttpRequest(url_data.scheme, 
		url_data.host, url_data.path, boost::beast::http::verb::put, req->body, "application/json"));

};
void Process::processDeleteRequest(std::shared_ptr<DeleteRequest> req){
	RequestData url_data = parseUrl(req->address);

	interpreter.current_scope->variables["eax"] = std::make_shared<StringVal>(doHttpRequest(url_data.scheme, 
		url_data.host, url_data.path, boost::beast::http::verb::delete_, req->body, "application/json"));

};

void Process::processFileDownloadRequest(std::shared_ptr<FileDownloadRequest> req){
	RequestData url_data = parseUrl(req->address);

	doHttpRequest(url_data.scheme, 
		url_data.host, url_data.path, boost::beast::http::verb::get, "", "", true, req->file_name);
};


Process::~Process() {
	io_operation = nullptr;
	wakeUpIoThread();
};
