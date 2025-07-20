#ifndef MY_JOURNAL_HPP
#define MY_JOURNAL_HPP
#include <string>
#include <stdexcept>
#include <fstream>
#include <chrono>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace journal {

  //3 levels in gradation
  enum class MessageLevel { INFO, WARNING, ERROR }; 
  
   //virtual class for writing in some outstream(file/socket)
   class JournalStream{
   public:
	   virtual bool write(const std::string&) = 0;
	   virtual bool is_open() const = 0;
	   virtual ~JournalStream() = default;
   };
   
    //class for writing in filestream
   class JournalFileStream :public JournalStream {
	   std::ofstream journalFile_;
	   bool valid_;
   public:
	   JournalFileStream(const std::string& filename);
	   ~JournalFileStream();
	   bool write(const std::string& data) override;
	   bool is_open() const override;
   };
   
      //class for writing in socketstream
   class JournalSocketStream : public JournalStream{
	   int socketFd_; //socket descriptor
	   bool valid_;
   public:
	   JournalSocketStream(const std::string& host, int port);
	   ~JournalSocketStream() override;
	   bool write(const std::string& data) override;
	   bool is_open() const override;
	   
   };

  class Journal {
	MessageLevel msgLevelDefault_; 
	std::string journalName_;
	bool valid_; 				   //correctness of Journal
    std::unique_ptr<JournalStream> stream;
    
  public:
    Journal(const std::string & journalName, MessageLevel msgLevelDefault, bool useSocket = false, const std::string& host = "",int port = 0);
    ~Journal();
    
    bool isValid();
    
    //setters and getters
    void setMsgLevelDefault(MessageLevel msgLevelDefaultNew);
    MessageLevel getMsgLevelDefault();
    std::string getJournalName();
    
    //our logging function
    bool log(std::string data,MessageLevel level,const std::chrono::system_clock::time_point &time = std::chrono::system_clock::now());
    bool log(const std::string data,const std::chrono::system_clock::time_point &time = std::chrono::system_clock::now());
    
	//we are not going to use this methods
    Journal(const Journal&) = delete;
    Journal& operator=(const Journal&) = delete;
    Journal(Journal&&) = delete;
    Journal& operator=(Journal&&) = delete;

  };
}//namespace journal

#endif //MY_JOURNAL_HPP
