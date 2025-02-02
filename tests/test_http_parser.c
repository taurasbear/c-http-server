#include "unity.h"
#include "http_parser.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_parse_request_line_valid(void)
{
    char buffer[] = "GET /index.html HTTP/1.1\r\n";
    size_t length = strlen(buffer);
    char temp_buffer[1024] = {0};
    size_t temp_buf_len = 0;
    HttpRequest httpRequest;

    int result = parse_request_line(buffer, &length, temp_buffer, &temp_buf_len, &httpRequest);

    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL_STRING("GET", httpRequest.method);
    TEST_ASSERT_EQUAL_STRING("/index.html", httpRequest.path);
    TEST_ASSERT_EQUAL_STRING("HTTP/1.1", httpRequest.protocol);
}

void test_simple(void)
{
    TEST_ASSERT_EQUAL(1, 1);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_simple);
    //RUN_TEST(test_parse_request_line_valid);

    return UNITY_END();
}