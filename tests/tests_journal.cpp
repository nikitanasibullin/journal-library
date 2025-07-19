#include <gtest/gtest.h>
#include "myJournal.hpp"
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>
#include <vector>
#include <cstdlib>


// ----------------------------------library tests---------------------------------------------------
TEST(JournalLibTest,InitializationSuccess){
	EXPECT_NO_THROW(journal::Journal("test.log",journal::MessageLevel::INFO));
}

TEST(JournalLibTest,SetAndGetLevel){      //testing level's setter and getter
	journal::Journal logger("test.txt",journal::MessageLevel::ERROR);
	EXPECT_EQ(logger.getMsgLevelDefault(),journal::MessageLevel::ERROR);
	logger.setMsgLevelDefault(journal::MessageLevel::INFO);
	EXPECT_EQ(logger.getMsgLevelDefault(),journal::MessageLevel::INFO);
}

TEST(JournalLibTest,GetName){   		//testing filename getter
	journal::Journal logger("test.txt",journal::MessageLevel::ERROR);
	EXPECT_EQ(logger.getJournalName(),"test.txt");
}

TEST(JournalLibTest,LoggingSuccess){   //testing success journal.log() 
	std::filesystem::remove("test_log.txt");
	journal::Journal journal("test_log.txt",journal::MessageLevel::INFO);
	EXPECT_TRUE(journal.isValid());
	EXPECT_TRUE(journal.log("test log",journal::MessageLevel::INFO));
	std::filesystem::remove("test_log.txt");
}

TEST(JournalLibTest,LoggingCorrectness1){  //testing correctness of journal.log() 
	auto fixedTime = std::chrono::system_clock::from_time_t(1745134260);
	std::filesystem::remove("test_log.txt");
	journal::Journal journal("test_log.txt",journal::MessageLevel::INFO);
	EXPECT_TRUE(journal.isValid());
	EXPECT_TRUE(journal.log("test log",journal::MessageLevel::INFO,fixedTime));
	
	std::ifstream inputFile("test_log.txt");
	std::stringstream buf;
	buf<<inputFile.rdbuf();
	std::string content = buf.str();
	std::string expected = "2025-04-20 10:31:00;INFO;test log\n";
	inputFile.close();
	
	EXPECT_EQ(content,expected);             //compare determined string and out journal file
	std::filesystem::remove("test_log.txt");
}

TEST(JournalLibTest,LoggingCorrectness2){  //testing correctness of journal.log() 
	auto fixedTime = std::chrono::system_clock::from_time_t(1745134260);
	std::filesystem::remove("test_log.txt");
	journal::Journal journal("test_log.txt",journal::MessageLevel::WARNING);
	EXPECT_TRUE(journal.isValid());
	EXPECT_TRUE(journal.log("test log",journal::MessageLevel::INFO,fixedTime));
	
	std::ifstream inputFile("test_log.txt");
	std::stringstream buf;
	buf<<inputFile.rdbuf();
	std::string content = buf.str();
	std::string expected = "";
	inputFile.close();
	EXPECT_EQ(content,expected);          //compare determined string and out journal file
	std::filesystem::remove("test_log.txt");
}

TEST(JournalLibTest,LoggingCorrectness3){   //testing correctness of journal.log(). 
	auto fixedTime1 = std::chrono::system_clock::from_time_t(1745134260);
	auto fixedTime2= std::chrono::system_clock::from_time_t(1745134261);
	auto fixedTime3 = std::chrono::system_clock::from_time_t(1745134262);
	std::filesystem::remove("test_log.txt");
	journal::Journal journal("test_log.txt",journal::MessageLevel::WARNING);
	EXPECT_TRUE(journal.isValid());
	EXPECT_TRUE(journal.log("test log1",journal::MessageLevel::WARNING,fixedTime1));
	EXPECT_TRUE(journal.log("test log2",journal::MessageLevel::INFO,fixedTime2));
	EXPECT_TRUE(journal.log("test log3",journal::MessageLevel::ERROR,fixedTime3));
	
	std::ifstream inputFile("test_log.txt");
	std::stringstream buf;
	buf<<inputFile.rdbuf();
	std::string content = buf.str();
	std::string expected = "2025-04-20 10:31:00;WARNING;test log1\n2025-04-20 10:31:02;ERROR;test log3\n";
	inputFile.close();
	EXPECT_EQ(content,expected);                  //compare determined string and out journal file
	std::filesystem::remove("test_log.txt");
}

TEST(JournalLibTest,LoggingCorrectness4){    //testing correctness of journal.log() 
	auto fixedTime1 = std::chrono::system_clock::from_time_t(1745134260);
	auto fixedTime2= std::chrono::system_clock::from_time_t(1745134261);
	auto fixedTime3 = std::chrono::system_clock::from_time_t(1745134262);
	std::filesystem::remove("test_log.txt");
	journal::Journal journal("test_log.txt",journal::MessageLevel::WARNING);
	EXPECT_TRUE(journal.isValid());
	EXPECT_TRUE(journal.log("test log1",journal::MessageLevel::WARNING,fixedTime1));
	journal.setMsgLevelDefault(journal::MessageLevel::INFO);
	EXPECT_TRUE(journal.log("test log2",journal::MessageLevel::INFO,fixedTime2));
	EXPECT_TRUE(journal.log("test log3",journal::MessageLevel::ERROR,fixedTime3));
	
	std::ifstream inputFile("test_log.txt");
	std::stringstream buf;
	buf<<inputFile.rdbuf();
	std::string content = buf.str();
	std::string expected = "2025-04-20 10:31:00;WARNING;test log1\n2025-04-20 10:31:01;INFO;test log2\n2025-04-20 10:31:02;ERROR;test log3\n";
	inputFile.close();
	EXPECT_EQ(content,expected);          //compare determined string and out journal file
	std::filesystem::remove("test_log.txt");
}


// ----------------------------------Console app tests---------------------------------------------------

std::vector<std::string> parsingLogFile(const std::string& filename){        //structure of our line is 'date';'level';'message'
																			 //and we want to ignore date in the tests
	std::vector<std::string> logs;
	std::ifstream fin(filename);
	std::string line;
	while(std::getline(fin,line)){                     //finding first ";"
		size_t pos = line.find(";");
		if(pos!=std::string::npos){
			logs.push_back(line.substr(pos+1));
		}
	}
	fin.close();
	return logs;
	
}

TEST(JournalConsoleAppTest,consoleOneLineCorectness){
	std::string commandsFilename = "test_commands.txt";
	std::string logFilename = "test_log.txt";
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");
	
	std::ofstream commands(commandsFilename);        //making file with commands that we are wanted to use in cmd
	commands<<"Hello world\n";
	commands<<"quit\n";
	commands.close();
	
	std::string startCommand = "../../consoleapp/build/consoleapp "+logFilename+" INFO < "+commandsFilename + " >/dev/null";  //making start command to run consoleapp and hiding output into ">/dev/null""
	int result = std::system(startCommand.c_str());                                                                           //running consoleapp
	EXPECT_EQ(result,0);
	
	auto logs = parsingLogFile(logFilename);
	EXPECT_EQ(logs.size(),1);
	EXPECT_EQ(logs[0],"INFO;Hello world");                                      //comapring determined line and out in test_log;
	
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");
	
	
}

//the same logic in the other tests

TEST(JournalConsoleAppTest,consoleMoreLineCorectness){                        //testing more than one line
	std::string commandsFilename = "test_commands.txt";
	std::string logFilename = "test_log.txt";
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");
	
	std::ofstream commands(commandsFilename);
	commands<<"Hello world\n";
	commands<<"I like this world\n";
	commands<<"Sometimes\n";
	commands<<"Especially if it is sunny\n";
	commands<<"quit\n";
	commands.close();
	
	std::string startCommand = "../../consoleapp/build/consoleapp "+logFilename+" INFO < "+commandsFilename + " >/dev/null";
	int result = std::system(startCommand.c_str());
	EXPECT_EQ(result,0);
	
	auto logs = parsingLogFile(logFilename);
	EXPECT_EQ(logs.size(),4);
	EXPECT_EQ(logs[0],"INFO;Hello world");
	EXPECT_EQ(logs[1],"INFO;I like this world");
	EXPECT_EQ(logs[2],"INFO;Sometimes");
	EXPECT_EQ(logs[3],"INFO;Especially if it is sunny");
	
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");
	
	
}


TEST(JournalConsoleAppTest,PriotityBlindnessCorectness){        //testing more then one line with less than default priotity (null output)
	std::string commandsFilename = "test_commands.txt";
	std::string logFilename = "test_log.txt";
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");
	
	std::ofstream commands(commandsFilename);
	commands<<"info;Hello world\n";
	commands<<"info;I like this world\n";
	commands<<"info;Sometimes\n";
	commands<<"info;Especially if it is sunny\n";
	commands<<"quit\n";
	commands.close();
	
	std::string startCommand = "../../consoleapp/build/consoleapp "+logFilename+" WARNING < "+commandsFilename + " >/dev/null";
	int result = std::system(startCommand.c_str());
	EXPECT_EQ(result,0);
	
	auto logs = parsingLogFile(logFilename);
	EXPECT_EQ(logs.size(),0);
	
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");
	
}

TEST(JournalConsoleAppTest,withPriotityBlindnessCorectness){               //mixed test
	std::string commandsFilename = "test_commands.txt";
	std::string logFilename = "test_log.txt";
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");
	
	std::ofstream commands(commandsFilename);
	commands<<"info;Hello world\n";
	commands<<"info;I like this world\n";
	commands<<"info;Sometimes\n";
	commands<<"error;Your PC is drowning in the sea!\n";
	commands<<"info;Especially if it is sunny\n";
	commands<<"quit\n";
	commands.close();
	
	std::string startCommand = "../../consoleapp/build/consoleapp "+logFilename+" WARNING < "+commandsFilename + " >/dev/null";
	int result = std::system(startCommand.c_str());
	EXPECT_EQ(result,0);
	
	auto logs = parsingLogFile(logFilename);
	EXPECT_EQ(logs.size(),1);
	EXPECT_EQ(logs[0],"ERROR;Your PC is drowning in the sea!");
	
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");	
}

TEST(JournalConsoleAppTest,SerialConsoleLinesCorectness){           //test with 2 times running consoleapp into one .txt file
	std::string commandsFilename = "test_commands.txt";
	std::string logFilename = "test_log.txt";
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");
	
	std::ofstream commands(commandsFilename);
	commands<<"Hello world\n";
	commands<<"quit\n";
	commands.close();
	
	std::string startCommand = "../../consoleapp/build/consoleapp "+logFilename+" INFO < "+commandsFilename + " >/dev/null";
	int result = std::system(startCommand.c_str());
	EXPECT_EQ(result,0);
	result = std::system(startCommand.c_str());
	EXPECT_EQ(result,0);
	
	auto logs = parsingLogFile(logFilename);
	EXPECT_EQ(logs.size(),2);
	EXPECT_EQ(logs[0],"INFO;Hello world");
	EXPECT_EQ(logs[0],"INFO;Hello world");
	
	std::filesystem::remove("test_log.txt");
	std::filesystem::remove("test_commands.txt");
	
	
}

