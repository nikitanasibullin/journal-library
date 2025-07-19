#include <gtest/gtest.h>
#include "myJournal.hpp"
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>

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

