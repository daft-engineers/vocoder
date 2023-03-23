#include <gtest/gtest.h>
#include "../include/amp/amp.hh"


TEST(AmpTest, NonEmptyReturn) {
    int scale = 1;
    Audio onevalue;
    onevalue.push_back(0);
    Amplifer a;

    Audio out = a.amplify(onevalue, scale);

    Audio empty;
    EXPECT_NE(empty,out);
}

TEST(AmpTest, EmptyReturn) {
    int scale = 1;
    Audio novalue;
    Amplifer a;

    Audio out = a.amplify(novalue, scale);

    Audio empty;
    EXPECT_EQ(empty,out);
}

TEST(AmpTest, NoAmplification) {
    int scale = 1;
    Audio somevalues;
    for (int i = 0; i<100; i++){
        somevalues.push_back(i);
    }
    Amplifer a;

    Audio out = a.amplify(somevalues, scale);

    EXPECT_EQ(somevalues.size(), out.size());
    EXPECT_EQ(somevalues,out);

}

TEST(AmpTest, HalfAmplification) {
    float scale = 0.5;
    Audio somevalues;
    Audio somehalfvalues;
    for (int i = 0; i<100; i++){
        somevalues.push_back(i);
        somehalfvalues.push_back(i/2);
    }
    Amplifer a;

    Audio out = a.amplify(somevalues, scale);

    EXPECT_EQ(somevalues.size(), out.size());
    EXPECT_EQ(somehalfvalues,out);

}

TEST(AmpTest, DoubleAmplification) {
    float scale = 2;
    Audio somevalues;
    Audio somedoublevalues;
    for (int i = 0; i<100; i++){
        somevalues.push_back(i);
        somedoublevalues.push_back(i*2);
    }
    Amplifer a;

    Audio out = a.amplify(somevalues, scale);

    EXPECT_EQ(somevalues.size(), out.size());
    EXPECT_EQ(somedoublevalues,out);

}