#include <userver/utest/utest.hpp>
#include "../src/handlers/v1/get-bookmarks/view.hpp"

namespace {

class BuildDbRequestTest : public ::testing::Test {
protected:
    bookmarker::TSession CreateSession(const std::string& user_id) {
        bookmarker::TSession session;
        session.id = user_id;
        session.user_id = user_id;
        return session;
    }

    bookmarker::TFilters CreateFilters(size_t limit = 10,
                           std::optional<std::string> tag = std::nullopt,
                           bookmarker::TFilters::ESortOrder order_by = bookmarker::TFilters::ESortOrder::ID) {
        bookmarker::TFilters filters;
        filters.limit = limit;
        filters.tag = tag;
        filters.order_by = order_by;
        return filters;
    }
};

UTEST_F(BuildDbRequestTest, NoFilters) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters();

    std::string expected_query = "SELECT * FROM bookmarker.bookmarks WHERE owner_id = 'test_user' ORDER BY id LIMIT 10";
    std::string actual_query = bookmarker::BuildDbRequest(session, filters);

    EXPECT_EQ(expected_query, actual_query);
}

UTEST_F(BuildDbRequestTest, WithTagFilter) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(10, "work");

    std::string expected_query = "SELECT * FROM bookmarker.bookmarks WHERE owner_id = 'test_user' AND tag = 'work' ORDER BY id LIMIT 10";
    std::string actual_query = bookmarker::BuildDbRequest(session, filters);

    EXPECT_EQ(expected_query, actual_query);
}

UTEST_F(BuildDbRequestTest, WithDifferentLimit) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(20);

    std::string expected_query = "SELECT * FROM bookmarker.bookmarks WHERE owner_id = 'test_user' ORDER BY id LIMIT 20";
    std::string actual_query = bookmarker::BuildDbRequest(session, filters);

    EXPECT_EQ(expected_query, actual_query);
}

UTEST_F(BuildDbRequestTest, WithTitleOrder) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(10, std::nullopt, bookmarker::TFilters::ESortOrder::TITLE);

    std::string expected_query = "SELECT * FROM bookmarker.bookmarks WHERE owner_id = 'test_user' ORDER BY title LIMIT 10";
    std::string actual_query = bookmarker::BuildDbRequest(session, filters);

    EXPECT_EQ(expected_query, actual_query);
}

UTEST_F(BuildDbRequestTest, WithTagAndUrlOrder) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(10, "personal", bookmarker::TFilters::ESortOrder::URL);

    std::string expected_query = "SELECT * FROM bookmarker.bookmarks WHERE owner_id = 'test_user' AND tag = 'personal' ORDER BY url LIMIT 10";
    std::string actual_query = bookmarker::BuildDbRequest(session, filters);

    EXPECT_EQ(expected_query, actual_query);
}

UTEST_F(BuildDbRequestTest, WithCreatedTsOrder) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(10, std::nullopt, bookmarker::TFilters::ESortOrder::CREATED_TS);

    std::string expected_query = "SELECT * FROM bookmarker.bookmarks WHERE owner_id = 'test_user' ORDER BY created_ts LIMIT 10";
    std::string actual_query = bookmarker::BuildDbRequest(session, filters);

    EXPECT_EQ(expected_query, actual_query);
}


} // namespace