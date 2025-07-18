#include "myJournal.hpp"
#include <stdexcept>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>


namespace journal{
	Journal::Journal(const std::string & journalName, MessageLevel msgLevelDefault) :
										journalName_(journalName), msgLevelDefault_(msgLevelDefault) 
    {
      msgLevelDefault_ = msgLevelDefault;
      journalName_ = journalName;
      journalFile_.open(journalName_);
      if (!journalFile_.is_open()) {
        std::runtime_error("Failed to open journal file: " + journalName_);
      }
    }
    
    Journal::~Journal(){
		if(journalFile_.is_open()){
			journalFile_.close();
		}
	}
    
    void Journal::setMsgLevelDefault(MessageLevel msgLevelDefaultNew) {
		msgLevelDefault_ = msgLevelDefaultNew;
    }
    MessageLevel Journal::getMsgLevelDefault() {
		return msgLevelDefault_;
    }
    std::string Journal::getJournalName() {
		return journalName_;
    }
    
    //our main logging function
    void Journal::log(const std::string data,
					const MessageLevel level,
					const std::chrono::system_clock::time_point &time = std::chrono::system_clock::now())
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
			case MessageLevel::ERROR:
				levelString = "ERROR";
				break;
			default:
				levelString = "FATAL ERROR";
			}
		
		auto timeT = std::chrono::system_clock::to_time_t(time);
		if(journalFile_.is_open())
		{
			journalFile_ << std::put_time(std::localtime(&timeT),"%Y-%m-%d %H:%M:%S")<<";"<<levelString<<";"<<data<<std::endl;;
		}
		else
		{
			std::runtime_error("Failed to open journal file: " + journalName_);
		}
	}
	
	//if we dont have lvl as arg, then using private class field (we cant just use nonstatic field as default arg)
	void Journal::log(const std::string data,
					const std::chrono::system_clock::time_point &time = std::chrono::system_clock::now())
	{
		log(data, msgLevelDefault_, time);
	}
}
