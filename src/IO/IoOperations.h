#include <string>

enum IoOperationType{
	FileReadType, FileWriteType, HttpRequestType
};

enum HttpRequestTypeEnum {
	GetRequestType, PostRequestType, PutRequestType, DeleteRequestType, FileDownloadRequestType
};

class IoOperation {
public:
    IoOperationType type;

    IoOperation(IoOperationType t): type(t) {};
    virtual ~IoOperation() {}
};

class HttpRequest : public IoOperation {
public:
    HttpRequestTypeEnum httpType;
    std::string address;

    HttpRequest(std::string addr, HttpRequestTypeEnum t): IoOperation(HttpRequestType), address(addr), httpType(t) {};
};

class GetRequest : public HttpRequest {
public:
    GetRequest(std::string addr): HttpRequest(addr, GetRequestType) {};
};

class PostRequest : public HttpRequest {
public:
    std::string body;

    PostRequest(std::string addr, std::string body_arg): HttpRequest(addr, PostRequestType), body(body_arg) {};

};

class PutRequest : public HttpRequest {
public:
    std::string body;

    PutRequest(std::string addr, std::string body_arg): HttpRequest(addr, PutRequestType), body(body_arg) {};

};

class DeleteRequest : public HttpRequest {
public:
    std::string body;

    DeleteRequest(std::string addr, std::string body_arg): HttpRequest(addr, DeleteRequestType), body(body_arg) {};

};

class FileDownloadRequest : public HttpRequest {
public:
    std::string file_name;
    FileDownloadRequest(std::string addr, std::string file_Name): HttpRequest(addr, FileDownloadRequestType),
        file_name(file_Name) {};

};