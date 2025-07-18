#include "myJournal.hpp"
#include <stdexcept>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <memory>

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
		return journalFile_.good();
	}
	
	
	Journal::Journal(const std::string & journalName, MessageLevel msgLevelDefault, bool useSocket, const std::string & host, int port) :
										journalName_(journalName), msgLevelDefault_(msgLevelDefault)
    {
	  if(useSocket){
		  //TODO
	  }	
	  else{
		  stream = std::make_unique<journal::JournalFileStream>(journalName);
	  }
      valid_ = (*stream).is_open();
    }
    
    Journal::~Journal() = default;
	
    
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
    
    //our main logging function
    bool Journal::log(const std::string data,
					const MessageLevel level,
					const std::chrono::system_clock::time_point &time)
					//determined time - for tests
					
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
	
	//if we dont have lvl as arg, then using private class field (we cant just use nonstatic field as default arg)
	bool Journal::log(const std::string data,
					const std::chrono::system_clock::time_point &time)
	{
		return log(data, msgLevelDefault_, time);
	}
}//namespace journal
