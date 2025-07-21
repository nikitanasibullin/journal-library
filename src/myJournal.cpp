#include "myJournal.hpp"
#include <stdexcept>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace journal{
	
	JournalFileStream::JournalFileStream(const std::string& filename): valid_(false){
		journalFile_.open(filename, std::ios::app);
		valid_ = journalFile_.is_open();
	}
	
	bool JournalFileStream::is_open() const{
		return valid_;
	}
	
	JournalFileStream::~JournalFileStream(){
		journalFile_.close();
	}
	
	bool JournalFileStream::write(const std::string& data){
		if(!valid_){
			return false;
		}
		journalFile_<<data<<std::endl;
		return journalFile_.good();    //if writing in ofstream was succesful
	}
	
	
	
	 JournalSocketStream::JournalSocketStream(const std::string& host, int port) : socketFd_(-1),valid_(false){
		 //create socket TCP
		socketFd_ = socket(AF_INET,SOCK_STREAM,0);
		if(socketFd_==-1){
			return;		//error with socket creation
		}
		sockaddr_in server_addr{};          //struct by default
		server_addr.sin_family = AF_INET;   //IPv4
		server_addr.sin_port = htons(port); //convert port into correct struct form
		
		// ip address to binary
		if(inet_pton(AF_INET,host.c_str(),&server_addr.sin_addr)<=0){  
			close(socketFd_);
			socketFd_=-1;
			return;  //error in adress
		}
		
		if(connect(socketFd_, (struct sockaddr*)&server_addr,sizeof(server_addr)) ==-1){
			close(socketFd_);
			socketFd_=-1;
			return;  //error in connection
		}
		
		valid_=true;    //success
		
	}
    
    JournalSocketStream::~JournalSocketStream(){
		if(socketFd_!=-1){
			socketFd_=-1;
			close(socketFd_);
		}
	}
	
	bool JournalSocketStream::is_open() const {
		return valid_;
	}
	
	//sending data into socket
	bool JournalSocketStream::write(const std::string &data)
	{
		
		std::string copy = data+"\n";
		
		if(!valid_){
			return false;
		}
		int bytes_sent = send(socketFd_, copy.c_str(), copy.size(), 0);
		return (bytes_sent == copy.size()); //if all bytes have sent
  }
  
  
	
	//constructor
	Journal::Journal(const std::string & journalName, MessageLevel msgLevelDefault, bool useSocket, const std::string & host, int port) :
										journalName_(journalName), msgLevelDefault_(msgLevelDefault), valid_(false)
										
	//mcreating socket or file stream
    {
	  if(useSocket){
		  stream = std::make_unique<JournalSocketStream>(host, port);
	  }	
	  else{
		  stream = std::make_unique<JournalFileStream>(journalName);
	  }
      valid_ = (*stream).is_open();  //if creating was succesful
    }
    
    Journal::~Journal() = default;
	
    //setter and getters
    
    void Journal::setMsgLevelDefault(MessageLevel msgLevelDefaultNew) {
		msgLevelDefault_ = msgLevelDefaultNew;
    }
    MessageLevel Journal::getMsgLevelDefault() {
		return msgLevelDefault_;
    }
    std::string Journal::getJournalName() {
		return journalName_;
    }
    
    bool Journal::isValid(){
		return valid_;
	}
    
   
   
   
    
    //our main logging function into stream
    bool Journal::log(const std::string data,const MessageLevel level, const std::chrono::system_clock::time_point &time) //determined time - for tests			
	{
		std::string levelString;
		switch(level){
			case MessageLevel::INFO:
				levelString = "INFO";
				break;
			case MessageLevel::WARNING:
				levelString = "WARNING";
				break;
			default:
				levelString = "ERROR";
			}
		if(static_cast<int>(level) < static_cast<int>(msgLevelDefault_)){
			return true;
		}
		auto timeT = std::chrono::system_clock::to_time_t(time);
		std::ostringstream oss;
		
		
		oss << std::put_time(std::localtime(&timeT),"%Y-%m-%d %H:%M:%S")<<";"<<levelString<<";"<<data;
		
		return stream->write(oss.str());
		
	}
	
	//if we dont have lvl as arg, then using private class field by default;
	bool Journal::log(const std::string data,
					const std::chrono::system_clock::time_point &time)
	{
		return log(data, msgLevelDefault_, time);
	}
}//namespace journal
