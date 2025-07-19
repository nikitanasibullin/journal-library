
#include "myJournal.hpp"
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <iostream>

namespace journal{
	struct logItem{
		MessageLevel lvl;
		std::string data;
	};
	
	class JournalThread{ //class to write into file and add log. items into queue.
		std::queue<logItem> logItemsQueue;
		std::mutex queueMutex_;  //mutex to make logItemsQueue safe
		Journal journal_;
		std::thread worker_;     //thread to log from queue
		bool running_ = true;
		std::condition_variable queueCond_;  //for loop(if queue are not empty)
		
	private:
		void loopLogs(){
			while(true){
				logItem item;
				{
					std::unique_lock<std::mutex> lock(queueMutex_);
					if(logItemsQueue.empty() and running_){ queueCond_.wait(lock);} 
					if(logItemsQueue.empty() and !running_) {break;}  
					item =logItemsQueue.front();
					logItemsQueue.pop();                               
				}
				if(journal_.isValid()){
					journal_.log(item.data,item.lvl);   
				}
			}
		}
	public:
		JournalThread(const std::string &filename,MessageLevel lvlDefault): journal_(filename,lvlDefault){
			worker_=std::thread([this] {loopLogs();}); //start running loopLogs into thread
			
		}
		~JournalThread(){
			{
				running_=false;  
				std::lock_guard<std::mutex> lock(queueMutex_);
				queueCond_.notify_one();                       //notifying mutex that we stopped write into console app 
			}
			worker_.join();                                    
		}
		
		void inQueueLog(MessageLevel lvl, std::string& data){  //pushing data from console app into queue
			std::lock_guard<std::mutex> lock(queueMutex_);
			logItemsQueue.push({lvl,data});
			queueCond_.notify_one();                           //notifying mutex that now queue isn't empty
		}
		
		bool isValid(){           
			return journal_.isValid();		                   
		}
	};
	
	bool stringToMsgLevel(MessageLevel& lvlDefault,const std::string& level){ //function recieve MessageLvl& and string
																			  // if string is express level, this function return true
																			  // and write this messagelevel into variable
																			  // otherwise it returns false
		if(level=="INFO" || level =="info" || level=="Info"){
			lvlDefault=journal::MessageLevel::INFO;
			return true;}
		else if(level=="WARNING" || level =="warning" || level=="Warning"){
			lvlDefault=journal::MessageLevel::WARNING;
			return true;}
		else if(level=="ERROR" || level =="error" || level=="Error"){
			lvlDefault=journal::MessageLevel::ERROR;
			return true;}
		else{
			return false;
		}
	}
		
}//namespace journal

int main(int argc, char* argv[]){
	if(argc <3){
		std::cerr<<"There is no enough arguments"<<std::endl;
		return 1;
	}
	std::string filename = argv[1];
	std::string levelString = argv[2];
	journal::MessageLevel lvlDefault;                   //we are going to determine it in function "stringToMsgLevel"
	if(!journal::stringToMsgLevel(lvlDefault,levelString) ){
		std::cerr<<"invalid default level value: "<<levelString<<std::endl;
		return 1;
	}
	journal::JournalThread myJournal(filename,lvlDefault);
	if(!myJournal.isValid()){
		std::cerr<<"Something wrong with journal file."<<std::endl;
		return 1;
	}
	std::cout<<"Now you can start journaling into " <<filename<<". Your default level is "<<levelString<< std::endl;
	std::cout<<"If you want to manually choose level of message, type it before message and then type \";\" "<<std::endl;
	std::cout<<"If you want to finish journaling session, type \"quit\""<<std::endl;
	while(true){
		std::string input;
		std::cout<<">";
		std::getline(std::cin,input);
		if(input=="quit"){
			break;
		}
		if(input.empty()){
			std::cout<<"Your message is empty"<<std::endl;
			continue;
		}
		std::string data = input;                           //structure of input `messageLevel`;`message` or `message`
		size_t pos = input.rfind(";");						//try to find ";" 
		if(pos!=std::string::npos){							//if we have messageLevel in input:
			
			std::string levelStrInput = input.substr(0,pos);         //"messageLevel"
			std::string message = input.substr(pos+1);			     //"message"
			if(message.empty()){
				std::cout<<"Your message is empty"<<std::endl;
				continue;
			}
			journal::MessageLevel inputLevel;
			if(stringToMsgLevel(inputLevel,levelStrInput )){
				myJournal.inQueueLog(inputLevel,message);
			}
			else{
				std::cerr<<"invalid level in input: "<<levelStrInput <<std::endl;
				continue;
			}
		}
		else{											   //if we dont have messageLevel in input(level by default(
			myJournal.inQueueLog(lvlDefault,input);
		}
		
	}
	
	
	return 0;
}
