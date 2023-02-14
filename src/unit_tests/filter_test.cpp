#include "filter.h"
#include <gtest/gtest.h>

namespace{

	TEST(FilterTest, InvalidInputs){
		    EXPECT_EQ(nullptr, BPFilter(4));
		    EXPECT_EQ(nullptr, BPFilter(std::vector dddddd));
	}
	TEST(FilterTest, HigherFreq){
		    // Output should be attenuated
		    EXPECT_EQ(AudioSnippet out, BPFilter(AudioSnippet in));
		    EXPECT_EQ(AudioSnippet out, BPFilter(AudioSnippet in));
	}
	TEST(FilterTest, LowerFreq){
	            // Output should be attenuated
	            EXPECT_EQ(AudioSnippet out, BPFilter(AudioSnippet in));
	            EXPECT_EQ(AudioSnippet out, BPFilter(AudioSnippet in));
	}
	TEST(FilterTest, PassFreq){
	            // Output should stay the same
	            EXPECT_EQ(AudioSnippet out, BPFilter(AudioSnippet in));
	            EXPECT_EQ(AudioSnippet out, BPFilter(AudioSnippet in));
	}
}
