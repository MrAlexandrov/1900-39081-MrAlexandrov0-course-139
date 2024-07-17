#include <userver/utest/utest.hpp>
#include "../src/models/validate.hpp"

namespace bookmarker {

class ValidateTest : public ::testing::Test {};

UTEST_F(ValidateTest, ValidEmails) {
    std::vector<std::string> valid_emails = {
        "user@example.com",
        "username@example.com",
        "user_name@example.com",
        "username123@example.com"
    };

    for (const auto& email : valid_emails) {
        EXPECT_TRUE(userver::utils::regex_match(email, email_pattern)) << "Expected valid email: " << email;
    }
}

UTEST_F(ValidateTest, InvalidEmails) {
    std::vector<std::string> invalid_emails = {
        "userexample.com",
        "user@.com",
        "@example.com",
        "user@com",
        "user@example.c",
        "user@example.toolongextension",
        "user@%example.com",
        "user+name@example.com"
    };

    for (const auto& email : invalid_emails) {
        EXPECT_FALSE(userver::utils::regex_match(email, email_pattern)) << "Expected invalid email: " << email;
    }
}

UTEST_F(ValidateTest, ValidPasswords) {
    std::vector<std::string> valid_passwords = {
        "password123",
        "12345678",
        "!@#$%^&*()",
        "Password!",
        "ValidPass1"
    };

    for (const auto& password : valid_passwords) {
        EXPECT_TRUE(userver::utils::regex_match(password, password_pattern)) << "Expected valid password: " << password;
    }
}

UTEST_F(ValidateTest, InvalidPasswords) {
    std::vector<std::string> invalid_passwords = {
        "short",
        "1234567",
        "pass",
        "!@#$%^",
        "small"
    };

    for (const auto& password : invalid_passwords) {
        EXPECT_FALSE(userver::utils::regex_match(password, password_pattern)) << "Expected invalid password: " << password;
    }
}

} // namespace bookmarker
