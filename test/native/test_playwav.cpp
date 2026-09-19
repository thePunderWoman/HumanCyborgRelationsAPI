// Host-side test for HCRVocalizer::PlayWAV's per-channel debounce.
//
//   ./test/native/run.sh
//
// Commands are captured through the HCRTransport hook, so no serial/I2C
// hardware is involved; time is driven by mock_millis.

#include <Arduino.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "hcr.h"

unsigned long mock_millis = 0;
HardwareSerial Serial;

namespace {

struct CaptureTransport : HCRTransport {
    std::vector<std::string> sent;
    bool send(const char *command) override {
        sent.push_back(command);
        return true;
    }
};

int failures = 0;

void check(bool ok, const char *what) {
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) failures++;
}

struct Fixture {
    HardwareSerial port;
    CaptureTransport transport;
    HCRVocalizer hcr;
    Fixture() : hcr(&port, 9600) { hcr.setExternalTransport(&transport); }
};

// The debounce state is file-scope in hcr.cpp, so each test moves the clock
// well past the previous one's window instead of resetting it.
void advance(unsigned long ms) { mock_millis += ms; }

void test_different_channels_both_go_out() {
    Fixture f;
    advance(10000);
    f.hcr.PlayWAV(0, 1);
    f.hcr.PlayWAV(1, 2);
    f.hcr.PlayWAV(2, 3);
    check(f.transport.sent.size() == 3, "rapid plays on V, A, B all go out");
    check(f.transport.sent[0] == "<CV0001,QPV>", "V frame is <CV0001,QPV>");
    check(f.transport.sent[1] == "<CA0002,QPA>", "A frame is <CA0002,QPA>");
    check(f.transport.sent[2] == "<CB0003,QPB>", "B frame is <CB0003,QPB>");
}

void test_same_channel_replay_is_debounced() {
    Fixture f;
    advance(10000);
    f.hcr.PlayWAV(1, 5);
    advance(100);
    f.hcr.PlayWAV(1, 6);
    f.hcr.PlayWAV(1, String("0007"));
    check(f.transport.sent.size() == 1, "replays on A inside 150 ms are dropped");
}

void test_same_channel_replay_after_window_goes_out() {
    Fixture f;
    advance(10000);
    f.hcr.PlayWAV(1, 5);
    advance(150);
    f.hcr.PlayWAV(1, 6);
    check(f.transport.sent.size() == 2, "replay on A at 150 ms goes out");
    check(f.transport.sent[1] == "<CA0006,QPA>", "second frame is <CA0006,QPA>");
}

void test_debounce_on_one_channel_does_not_block_another() {
    Fixture f;
    advance(10000);
    f.hcr.PlayWAV(0, 1);
    advance(50);
    f.hcr.PlayWAV(0, 2);   // V still inside its window: dropped
    f.hcr.PlayWAV(2, 3);   // B is independent: goes out
    check(f.transport.sent.size() == 2, "V replay dropped, B play sent");
    check(f.transport.sent[1] == "<CB0003,QPB>", "the frame that went out is B's");
}

void test_out_of_range_channel_is_ignored() {
    Fixture f;
    advance(10000);
    f.hcr.PlayWAV(-1, 1);
    f.hcr.PlayWAV(3, 1);
    f.hcr.PlayWAV(3, String("0001"));
    check(f.transport.sent.empty(), "channels outside 0..2 send nothing");
}

}  // namespace

int main() {
    test_different_channels_both_go_out();
    test_same_channel_replay_is_debounced();
    test_same_channel_replay_after_window_goes_out();
    test_debounce_on_one_channel_does_not_block_another();
    test_out_of_range_channel_is_ignored();
    printf(failures ? "\n%d FAILED\n" : "\nall passed\n", failures);
    return failures ? 1 : 0;
}
