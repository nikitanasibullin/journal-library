#ifndef MY_JOURNAL_HPP
#define MY_JOURNAL_HPP
#include <string>
#include <stdexcept>
#include <fstream>
#include <chrono>

namespace journal {

  enum class MessageLevel { INFO, WARNING, ERROR, FATAL_ERROR }; //4 levels in gradation

  class Journal {
	MessageLevel msgLevelDefault_;
	std::string journalName_;
    std::ofstream journalFile_;
    
  public:
    Journal(const std::string & journalName, MessageLevel msgLevelDefault);
    ~Journal();
    
    //setters and getters
    void setMsgLevelDefault(MessageLevel msgLevelDefaultNew);
    MessageLevel getMsgLevelDefault();
    std::string getJournalName();
    
    //our logging function
    void log(std::string data,MessageLevel level,const std::chrono::system_clock::time_point &time);
    void log(const std::string data,const std::chrono::system_clock::time_point &time);
    
	//we are not going to use this methods
    Journal(const Journal&) = delete;
    Journal& operator=(const Journal&) = delete;
    Journal(Journal&&) = delete;
    Journal& operator=(Journal&&) = delete;

  };
}

#endif //MY_JOURNAL_HPP
