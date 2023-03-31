#include "../include/amp/amp.hh"
#include <gtest/gtest.h>

// This is a unit test, magic numbers are called test data here
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(AmpTest, NonEmptyReturn) {
    const int scale = 1;
    Audio onevalue;
    onevalue.push_back(0);
    Pipe<Audio> in_pipe;
    Pipe<double> scale_pipe;
    Pipe<Audio> out_pipe;

    Amplifier a(in_pipe, scale_pipe, out_pipe);
    Audio out = a.amplify(onevalue, scale);

    Audio empty;
    EXPECT_NE(empty, out);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(AmpTest, EmptyReturn) {
    const int scale = 1;
    Audio novalue;
    Pipe<Audio> in_pipe;
    Pipe<double> scale_pipe;
    Pipe<Audio> out_pipe;

    Amplifier a(in_pipe, scale_pipe, out_pipe);

    Audio out = a.amplify(novalue, scale);

    Audio empty;
    EXPECT_EQ(empty, out);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(AmpTest, NoAmplification) {
    const int scale = 1;
    Audio somevalues;
    for (int i = 0; i < 100; i++) {
        somevalues.push_back(i);
    }
    Pipe<Audio> in_pipe;
    Pipe<double> scale_pipe;
    Pipe<Audio> out_pipe;

    Amplifier a(in_pipe, scale_pipe, out_pipe);
    Audio out = a.amplify(somevalues, scale);

    EXPECT_EQ(somevalues.size(), out.size());
    EXPECT_EQ(somevalues, out);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(AmpTest, HalfAmplification) {
    const double scale = 0.5;
    Audio somevalues;
    Audio somehalfvalues;
    for (int i = 0; i < 100; i++) {
        somevalues.push_back(i);
        somehalfvalues.push_back(i / 2);
    }
    Pipe<Audio> in_pipe;
    Pipe<double> scale_pipe;
    Pipe<Audio> out_pipe;

    Amplifier a(in_pipe, scale_pipe, out_pipe);
    Audio out = a.amplify(somevalues, scale);

    EXPECT_EQ(somevalues.size(), out.size());
    EXPECT_EQ(somehalfvalues, out);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(AmpTest, DoubleAmplification) {
    double scale = 2;
    Audio somevalues;
    Audio somedoublevalues;
    for (int i = 0; i < 100; i++) {
        somevalues.push_back(i);
        somedoublevalues.push_back(i * 2);
    }
    Pipe<Audio> in_pipe;
    Pipe<double> scale_pipe;
    Pipe<Audio> out_pipe;

    Amplifier a(in_pipe, scale_pipe, out_pipe);
    Audio out = a.amplify(somevalues, scale);

    EXPECT_EQ(somevalues.size(), out.size());
    EXPECT_EQ(somedoublevalues, out);
}

// TEST macro violates guidelines
// NOLINTNEXTLINE(cppcoreguidelines-owning-memory, cppcoreguidelines-avoid-non-const-global-variables)
TEST(AmpTest, ThreadAndMessaging) {
    Pipe<Audio> in_pipe;
    Pipe<double> scale_pipe;
    Pipe<Audio> out_pipe;

    Amplifier a(in_pipe, scale_pipe, out_pipe);

    int16_t single_sample = 2;
    Audio amp_in;
    amp_in.push_back(single_sample);
    double scale_factor = 0.5;
    Audio amp_out;
    amp_out.push_back(single_sample * scale_factor);

    a.run();

    std::thread input_thread{[&in_pipe, &amp_in] {
        auto *in = &in_pipe;
        {
            std::lock_guard<std::mutex> lk(in->cond_m);
            in->queue.push(amp_in);
        }
        in->cond.notify_all();
    }};

    std::thread scale_thread{[&scale_pipe, scale_factor] {
        auto *scale = &scale_pipe;
        {
            std::lock_guard<std::mutex> lk(scale->cond_m);
            scale->queue.push(scale_factor);
        }
        scale->cond.notify_all();
    }};

    // listen for output
    std::thread output_thread{[&out_pipe, &amp_out] {
        std::unique_lock<std::mutex> lk(out_pipe.cond_m);
        out_pipe.cond.wait(lk, [&out_pipe] { return out_pipe.queue.empty() == false; });
        Audio amp_processed_out;
        amp_processed_out = out_pipe.queue.front();
        EXPECT_EQ(amp_out, amp_processed_out) << "Amp scales correctly";
    }};

    std::cerr << "Amp stopped" << std::endl;
    input_thread.join();
    std::cerr << "Input stopped" << std::endl;
    scale_thread.join();
    std::cerr << "Scale stopped" << std::endl;
    output_thread.join();
    std::cerr << "Output stopped" << std::endl;
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)