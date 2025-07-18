#include <gtest/gtest.h>
#include "myJournal.hpp"
#include <fstream>
#include <filesystem>
#include <string>
#include <chrono>

TEST(JournalLibTest,InitializationSuccess){
	EXPECT_NO_THROW(journal::Journal("test.log",journal::MessageLevel::INFO));
}

TEST(JournalLibTest,SetAndGetLevel){
	journal::Journal logger("test.txt",journal::MessageLevel::ERROR);
	EXPECT_EQ(logger.getMsgLevelDefault(),journal::MessageLevel::ERROR);
	logger.setMsgLevelDefault(journal::MessageLevel::INFO);
	EXPECT_EQ(logger.getMsgLevelDefault(),journal::MessageLevel::INFO);
}

TEST(JournalLibTest,Logging){
	std::filesystem::remove("test_log.txt");
	journal::Journal journal("test_log.txt",journal::MessageLevel::INFO);
	EXPECT_TRUE(journal.isValid());
	EXPECT_TRUE(journal.log("test log",journal::MessageLevel::INFO));
}
