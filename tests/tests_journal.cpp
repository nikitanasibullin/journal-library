#include <gtest/gtest.h>
#include "myJournal.hpp"
#include <fstream>
#include <string>
#include <chrono>

TEST(JournalLibTest,InitializationSuccess){
	EXPECT_NO_THROW(journal::Journal("test.log",journal::MessageLevel::INFO));
}

TEST(JournalLibTest,SetAndGetLevel){
	journal::Journal logger("test.txt",journal::MessageLevel::FATAL_ERROR);
	EXPECT_EQ(logger.getMsgLevelDefault(),journal::MessageLevel::FATAL_ERROR);
	logger.setMsgLevelDefault(journal::MessageLevel::ERROR);
	EXPECT_EQ(logger.getMsgLevelDefault(),journal::MessageLevel::ERROR);
}

	

