////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include <iostream>
#include <memory>
#include "shared/shared_ptr.hpp"
#include <chrono>
#include <mutex>
#include <thread>

using namespace X17;

struct MediaAsset {
    virtual ~MediaAsset() = default;  // make it polymorphic
};

struct Song : public MediaAsset {
    std::wstring artist;
    std::wstring title;
    Song(const std::wstring& artist_, const std::wstring& title_)
        : artist{artist_}, title{title_} {}
};

struct Photo : public MediaAsset {
    std::wstring date;
    std::wstring location;
    std::wstring subject;
    Photo(const std::wstring& date_,
          const std::wstring& location_,
          const std::wstring& subject_)
        : date{date_}, location{location_}, subject{subject_} {}
};

int main() {
    auto sp1 =
        make_shared<Song>(L"The Beatles", L"Im Happy Just to Dance With You");
    
}