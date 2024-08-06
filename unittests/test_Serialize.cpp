#include <userver/utest/utest.hpp>
#include "../src/models/bookmark.hpp"

namespace {

class SerializeTest : public ::testing::Test {
protected:
    bookmarker::TBookmark CreateBookmark(const std::string& id,
                                         const std::string& owner_id,
                                         const std::string& url,
                                         const std::string& title,
                                         const std::optional<std::string>& tag) {
        bookmarker::TBookmark bookmark;
        bookmark.id = id;
        bookmark.owner_id = owner_id;
        bookmark.url = url;
        bookmark.title = title;
        bookmark.tag = tag;
        bookmark.created_ts = std::chrono::system_clock::now();
        return bookmark;
    }
};

UTEST_F(SerializeTest, BasicSerialization) {
    bookmarker::TBookmark bookmark = CreateBookmark(
        "123", "owner123", "https://example.com", "Example Title", "example_tag"
    );

    auto serialized = bookmarker::Serialize(bookmark, userver::formats::serialize::To<userver::formats::json::Value>{});

    EXPECT_EQ(serialized["id"].As<std::string>(), bookmark.id);
    EXPECT_EQ(serialized["url"].As<std::string>(), bookmark.url);
    EXPECT_EQ(serialized["title"].As<std::string>(), bookmark.title);
    EXPECT_TRUE(serialized.HasMember("tag"));
    EXPECT_EQ(serialized["tag"].As<std::string>(), bookmark.tag.value());
    EXPECT_TRUE(serialized.HasMember("created_ts"));
}

UTEST_F(SerializeTest, SerializationWithoutTag) {
    bookmarker::TBookmark bookmark = CreateBookmark(
        "123", "owner123", "https://example.com", "Example Title", std::nullopt
    );

    auto serialized = bookmarker::Serialize(bookmark, userver::formats::serialize::To<userver::formats::json::Value>{});

    EXPECT_EQ(serialized["id"].As<std::string>(), bookmark.id);
    EXPECT_EQ(serialized["url"].As<std::string>(), bookmark.url);
    EXPECT_EQ(serialized["title"].As<std::string>(), bookmark.title);
    EXPECT_FALSE(serialized.HasMember("tag"));
    EXPECT_TRUE(serialized.HasMember("created_ts"));
}

} // namespace
