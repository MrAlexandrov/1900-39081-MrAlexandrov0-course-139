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

    bookmarker::DbQuery expected_query{"SELECT * FROM bookmarker.bookmarks WHERE owner_id = $1 ORDER BY id LIMIT 10",
                                       std::move(userver::storages::postgres::ParameterStore{}
                                       .PushBack("test_user")
                                       )
    };
    bookmarker::DbQuery actual_query = bookmarker::BuildDbRequest(session, filters);

    auto actual_buffer = actual_query.params.GetInternalData().ParamBuffers();
    auto expected_buffer = expected_query.params.GetInternalData().ParamBuffers();

    EXPECT_EQ(actual_query.query, expected_query.query);
    EXPECT_EQ(actual_query.params.Size(), expected_query.params.Size());
    EXPECT_EQ(actual_query.params.Size(), 1);
    EXPECT_EQ(*actual_buffer[0], *expected_buffer[0]);
}

UTEST_F(BuildDbRequestTest, WithTagFilter) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(10, "work");

    bookmarker::DbQuery expected_query = {"SELECT * FROM bookmarker.bookmarks WHERE owner_id = $1 AND tag = $2 ORDER BY id LIMIT 10",
                                          std::move(userver::storages::postgres::ParameterStore{}
                                          .PushBack("test_user")
                                          .PushBack("work")
                                          )
    };
    bookmarker::DbQuery actual_query = bookmarker::BuildDbRequest(session, filters);

    auto actual_buffer = actual_query.params.GetInternalData().ParamBuffers();
    auto expected_buffer = expected_query.params.GetInternalData().ParamBuffers();

    EXPECT_EQ(actual_query.query, expected_query.query);
    EXPECT_EQ(actual_query.params.Size(), expected_query.params.Size());
    EXPECT_EQ(actual_query.params.Size(), 2);
    EXPECT_EQ(*actual_buffer[0], *expected_buffer[0]);
    EXPECT_EQ(*actual_buffer[1], *expected_buffer[1]);
}

UTEST_F(BuildDbRequestTest, WithDifferentLimit) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(20);

    bookmarker::DbQuery expected_query{"SELECT * FROM bookmarker.bookmarks WHERE owner_id = $1 ORDER BY id LIMIT 20",
                                       std::move(userver::storages::postgres::ParameterStore{}
                                       .PushBack("test_user")
                                       )
    };
    bookmarker::DbQuery actual_query = bookmarker::BuildDbRequest(session, filters);

    auto actual_buffer = actual_query.params.GetInternalData().ParamBuffers();
    auto expected_buffer = expected_query.params.GetInternalData().ParamBuffers();

    EXPECT_EQ(actual_query.query, expected_query.query);
    EXPECT_EQ(actual_query.params.Size(), expected_query.params.Size());
    EXPECT_EQ(actual_query.params.Size(), 1);
    EXPECT_EQ(*actual_buffer[0], *expected_buffer[0]);
}

UTEST_F(BuildDbRequestTest, WithTitleOrder) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(10, std::nullopt, bookmarker::TFilters::ESortOrder::TITLE);

    bookmarker::DbQuery expected_query{"SELECT * FROM bookmarker.bookmarks WHERE owner_id = $1 ORDER BY title LIMIT 10",
                                       std::move(userver::storages::postgres::ParameterStore{}
                                       .PushBack("test_user")
                                       )
    };
    bookmarker::DbQuery actual_query = bookmarker::BuildDbRequest(session, filters);

    auto actual_buffer = actual_query.params.GetInternalData().ParamBuffers();
    auto expected_buffer = expected_query.params.GetInternalData().ParamBuffers();

    EXPECT_EQ(actual_query.query, expected_query.query);
    EXPECT_EQ(actual_query.params.Size(), expected_query.params.Size());
    EXPECT_EQ(actual_query.params.Size(), 1);
    EXPECT_EQ(*actual_buffer[0], *expected_buffer[0]);
}

UTEST_F(BuildDbRequestTest, WithTagAndUrlOrder) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(10, "personal", bookmarker::TFilters::ESortOrder::URL);

    bookmarker::DbQuery expected_query{"SELECT * FROM bookmarker.bookmarks WHERE owner_id = $1 AND tag = $2 ORDER BY url LIMIT 10",
                                       std::move(userver::storages::postgres::ParameterStore{}
                                       .PushBack("test_user")
                                       .PushBack("personal")
                                       )
    };
    bookmarker::DbQuery actual_query = bookmarker::BuildDbRequest(session, filters);

    auto actual_buffer = actual_query.params.GetInternalData().ParamBuffers();
    auto expected_buffer = expected_query.params.GetInternalData().ParamBuffers();

    EXPECT_EQ(actual_query.query, expected_query.query);
    EXPECT_EQ(actual_query.params.Size(), expected_query.params.Size());
    EXPECT_EQ(actual_query.params.Size(), 2);
    EXPECT_EQ(*actual_buffer[0], *expected_buffer[0]);
    EXPECT_EQ(*actual_buffer[1], *expected_buffer[1]);
}

UTEST_F(BuildDbRequestTest, WithCreatedTsOrder) {
    bookmarker::TSession session = CreateSession("test_user");
    bookmarker::TFilters filters = CreateFilters(10, std::nullopt, bookmarker::TFilters::ESortOrder::CREATED_TS);

    bookmarker::DbQuery expected_query{"SELECT * FROM bookmarker.bookmarks WHERE owner_id = $1 ORDER BY created_ts LIMIT 10",
                                       std::move(userver::storages::postgres::ParameterStore{}
                                       .PushBack("test_user")
                                       )
    };
    bookmarker::DbQuery actual_query = bookmarker::BuildDbRequest(session, filters);

    auto actual_buffer = actual_query.params.GetInternalData().ParamBuffers();
    auto expected_buffer = expected_query.params.GetInternalData().ParamBuffers();

    EXPECT_EQ(actual_query.query, expected_query.query);
    EXPECT_EQ(actual_query.params.Size(), expected_query.params.Size());
    EXPECT_EQ(actual_query.params.Size(), 1);
    EXPECT_EQ(*actual_buffer[0], *expected_buffer[0]);
}

} // namespace