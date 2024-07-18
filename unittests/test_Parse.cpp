#include <userver/utest/utest.hpp>
#include "../src/models/filter.hpp"

UTEST(TFiltersTest, ParseValidLimit) {
    auto result = bookmarker::TFilters::Parse("5", "example_tag", "title");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->limit, 5);
    EXPECT_EQ(result->tag, "example_tag");
    EXPECT_EQ(result->order_by, bookmarker::TFilters::ESortOrder::TITLE);
}

UTEST(TFiltersTest, ParseNegativeLimit) {
    auto result = bookmarker::TFilters::Parse("-5", std::nullopt, std::nullopt);
    EXPECT_FALSE(result.has_value());
}

UTEST(TFiltersTest, ParseInvalidLimit) {
    auto result = bookmarker::TFilters::Parse("invalid", std::nullopt, std::nullopt);
    EXPECT_FALSE(result.has_value());
}

UTEST(TFiltersTest, ParseValidOrderBy) {
    auto result = bookmarker::TFilters::Parse(std::nullopt, std::nullopt, "title");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->order_by, bookmarker::TFilters::ESortOrder::TITLE);
}

UTEST(TFiltersTest, ParseInvalidOrderBy) {
    auto result = bookmarker::TFilters::Parse(std::nullopt, std::nullopt, "invalid_order");
    EXPECT_FALSE(result.has_value());
}

UTEST(TFiltersTest, ParseAllValid) {
    auto result = bookmarker::TFilters::Parse("10", "example_tag", "created_ts");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->limit, 10);
    EXPECT_EQ(result->tag, "example_tag");
    EXPECT_EQ(result->order_by, bookmarker::TFilters::ESortOrder::CREATED_TS);
}

UTEST(TFiltersTest, ParseMissingLimit) {
    auto result = bookmarker::TFilters::Parse(std::nullopt, "example_tag", "title");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->tag, "example_tag");
    EXPECT_EQ(result->order_by, bookmarker::TFilters::ESortOrder::TITLE);
}

UTEST(TFiltersTest, ParseMissingTag) {
    auto result = bookmarker::TFilters::Parse("5", std::nullopt, "url");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->limit, 5);
    EXPECT_EQ(result->order_by, bookmarker::TFilters::ESortOrder::URL);
}

UTEST(TFiltersTest, ParseMissingOrderBy) {
    auto result = bookmarker::TFilters::Parse("5", "example_tag", std::nullopt);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->limit, 5);
    EXPECT_EQ(result->tag, "example_tag");
}

UTEST(TFiltersTest, ParseAllMissing) {
    auto result = bookmarker::TFilters::Parse(std::nullopt, std::nullopt, std::nullopt);
    ASSERT_TRUE(result.has_value());
}

UTEST(TFiltersTest, ParseEmptyLimit) {
    auto result = bookmarker::TFilters::Parse("", "example_tag", "id");
    EXPECT_FALSE(result.has_value());
}

UTEST(TFiltersTest, ParseEmptyTag) {
    auto result = bookmarker::TFilters::Parse("5", "", "id");
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->limit, 5);
    EXPECT_EQ(result->tag, "");
    EXPECT_EQ(result->order_by, bookmarker::TFilters::ESortOrder::ID);
}

UTEST(TFiltersTest, ParseEmptyOrderBy) {
    auto result = bookmarker::TFilters::Parse("5", "example_tag", "");
    EXPECT_FALSE(result.has_value());
}