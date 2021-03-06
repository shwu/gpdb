#include "s3bucket_reader.cpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mock_classes.h"

using ::testing::AtLeast;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::Throw;
using ::testing::_;

class MockS3Reader : public Reader {
   public:
    MOCK_METHOD1(open, void(const S3Params& params));
    MOCK_METHOD2(read, uint64_t(char*, uint64_t));
    void close() {
    }
};

// ================== S3BucketReaderTest ===================

class S3BucketReaderTest : public testing::Test {
   protected:
    // Remember that SetUp() is run immediately before a test starts.
    virtual void SetUp() {
        memset(buf, 0, sizeof(buf));

        bucketReader = new S3BucketReader();
        bucketReader->setS3InterfaceService(&s3Interface);
    }

    // TearDown() is invoked immediately after a test finishes.
    virtual void TearDown() {
        eolString[0] = '\n';
        eolString[1] = '\0';

        delete bucketReader;

        s3ext_segid = 0;
        s3ext_segnum = 1;
    }

    S3BucketReader* bucketReader;
    S3Params params;
    char buf[64];

    MockS3Interface s3Interface;
    MockS3Reader s3Reader;
};

TEST_F(S3BucketReaderTest, OpenInvalidURL) {
    string url = "https://s3-us-east-2.amazon.com/s3test.pivotal.io/whatever";
    params.setBaseUrl(url);
    EXPECT_THROW(bucketReader->open(params), S3ConfigError);
}

TEST_F(S3BucketReaderTest, OpenURL) {
    EXPECT_CALL(s3Interface, listBucket(_, _, _, _)).Times(1).WillOnce(Return(ListBucketResult()));

    string url = "https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever";
    params.setBaseUrl(url);

    EXPECT_NO_THROW(bucketReader->open(params));
}

TEST_F(S3BucketReaderTest, OpenThrowExceptionWhenS3InterfaceIsNULL) {
    bucketReader->setS3InterfaceService(NULL);

    string url = "https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever";
    params.setBaseUrl(url);
    EXPECT_THROW(bucketReader->open(params), S3RuntimeError);
}

TEST_F(S3BucketReaderTest, ParseURL_normal) {
    EXPECT_NO_THROW(this->bucketReader->parseURL(
        "s3://s3-us-west-2.amazonaws.com/s3test.pivotal.io/dataset1/normal"));
    EXPECT_EQ("us-west-2", this->bucketReader->getRegion());
    EXPECT_EQ("s3test.pivotal.io", this->bucketReader->getBucket());
    EXPECT_EQ("dataset1/normal", this->bucketReader->getPrefix());
}

// cannot find '://', so return url itself
TEST_F(S3BucketReaderTest, ParseURL_noSchema) {
    string url = "abcd";
    EXPECT_EQ(url, S3UrlUtility::replaceSchemaFromURL(url));
}

TEST_F(S3BucketReaderTest, ParseURL_NoPrefixAndSlash) {
    EXPECT_NO_THROW(
        this->bucketReader->parseURL("s3://s3-us-west-2.amazonaws.com/s3test.pivotal.io"));
    EXPECT_EQ("s3test.pivotal.io", this->bucketReader->getBucket());
    EXPECT_EQ("", this->bucketReader->getPrefix());
}

TEST_F(S3BucketReaderTest, ParseURL_NoPrefix) {
    EXPECT_NO_THROW(
        this->bucketReader->parseURL("s3://s3-us-west-2.amazonaws.com/s3test.pivotal.io/"));
    EXPECT_EQ("s3test.pivotal.io", this->bucketReader->getBucket());
    EXPECT_EQ("", this->bucketReader->getPrefix());
}

TEST_F(S3BucketReaderTest, ParseURL_default) {
    EXPECT_NO_THROW(
        this->bucketReader->parseURL("s3://s3.amazonaws.com/s3test.pivotal.io/dataset1/normal"));
    EXPECT_EQ("external-1", this->bucketReader->getRegion());
    EXPECT_EQ("s3test.pivotal.io", this->bucketReader->getBucket());
    EXPECT_EQ("dataset1/normal", this->bucketReader->getPrefix());
}

TEST_F(S3BucketReaderTest, ParseURL_useast1) {
    EXPECT_NO_THROW(this->bucketReader->parseURL(
        "s3://s3-us-east-1.amazonaws.com/s3test.pivotal.io/dataset1/normal"));
    EXPECT_EQ("external-1", this->bucketReader->getRegion());
    EXPECT_EQ("s3test.pivotal.io", this->bucketReader->getBucket());
    EXPECT_EQ("dataset1/normal", this->bucketReader->getPrefix());
}

TEST_F(S3BucketReaderTest, ParseURL_eucentral1) {
    EXPECT_NO_THROW(this->bucketReader->parseURL(
        "s3://s3.eu-central-1.amazonaws.com/s3test.pivotal.io/dataset1/normal"));
    EXPECT_EQ("eu-central-1", this->bucketReader->getRegion());
    EXPECT_EQ("s3test.pivotal.io", this->bucketReader->getBucket());
    EXPECT_EQ("dataset1/normal", this->bucketReader->getPrefix());
}

TEST_F(S3BucketReaderTest, ParseURL_eucentral11) {
    EXPECT_NO_THROW(this->bucketReader->parseURL(
        "s3://s3-eu-central-1.amazonaws.com/s3test.pivotal.io/dataset1/normal"));
    EXPECT_EQ("eu-central-1", this->bucketReader->getRegion());
    EXPECT_EQ("s3test.pivotal.io", this->bucketReader->getBucket());
    EXPECT_EQ("dataset1/normal", this->bucketReader->getPrefix());
}

TEST_F(S3BucketReaderTest, ParseURL_apnortheast2) {
    EXPECT_NO_THROW(this->bucketReader->parseURL(
        "s3://s3.ap-northeast-2.amazonaws.com/s3test.pivotal.io/dataset1/normal"));
    EXPECT_EQ("ap-northeast-2", this->bucketReader->getRegion());
    EXPECT_EQ("s3test.pivotal.io", this->bucketReader->getBucket());
    EXPECT_EQ("dataset1/normal", this->bucketReader->getPrefix());
}

TEST_F(S3BucketReaderTest, ParseURL_apnortheast21) {
    EXPECT_NO_THROW(this->bucketReader->parseURL(
        "s3://s3-ap-northeast-2.amazonaws.com/s3test.pivotal.io/dataset1/normal"));
    EXPECT_EQ("ap-northeast-2", this->bucketReader->getRegion());
    EXPECT_EQ("s3test.pivotal.io", this->bucketReader->getBucket());
    EXPECT_EQ("dataset1/normal", this->bucketReader->getPrefix());
}

TEST_F(S3BucketReaderTest, ReaderThrowExceptionWhenUpstreamReaderIsNULL) {
    EXPECT_THROW(bucketReader->read(buf, sizeof(buf)), S3RuntimeError);
}

TEST_F(S3BucketReaderTest, ReaderReturnZeroForEmptyBucket) {
    EXPECT_CALL(s3Interface, listBucket(_, _, _, _)).Times(1).WillOnce(Return(ListBucketResult()));

    params.setBaseUrl("https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever");
    bucketReader->open(params);
    bucketReader->setUpstreamReader(&s3Reader);
    EXPECT_EQ((uint64_t)0, bucketReader->read(buf, sizeof(buf)));
}

TEST_F(S3BucketReaderTest, ReadBucketWithSingleFile) {
    ListBucketResult result;
    result.contents.emplace_back("foo", 456);

    EXPECT_CALL(s3Interface, listBucket(_, _, _, _)).Times(1).WillOnce(Return(result));

    EXPECT_CALL(s3Reader, read(_, _))
        .Times(3)
        .WillOnce(Return(256))
        .WillOnce(Return(200))
        .WillOnce(Return(0));

    EXPECT_CALL(s3Reader, open(_)).Times(1);

    s3ext_segid = 0;
    s3ext_segnum = 1;
    params.setBaseUrl("https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever");
    bucketReader->open(params);
    bucketReader->setUpstreamReader(&s3Reader);

    EXPECT_EQ((uint64_t)256, bucketReader->read(buf, sizeof(buf)));
    EXPECT_EQ((uint64_t)200, bucketReader->read(buf, sizeof(buf)));
    EXPECT_EQ((uint64_t)0, bucketReader->read(buf, sizeof(buf)));
}

TEST_F(S3BucketReaderTest, ReadBuckeWithOneEmptyFileOneNonEmptyFile) {
    ListBucketResult result;
    result.contents.emplace_back("foo", 0);
    result.contents.emplace_back("bar", 456);

    EXPECT_CALL(s3Interface, listBucket(_, _, _, _)).Times(1).WillOnce(Return(result));

    EXPECT_CALL(s3Reader, read(_, _))
        .Times(3)
        .WillOnce(Return(0))
        .WillOnce(Return(256))
        .WillOnce(Return(0));

    EXPECT_CALL(s3Reader, open(_)).Times(2);

    s3ext_segid = 0;
    s3ext_segnum = 1;
    params.setBaseUrl("https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever");
    bucketReader->open(params);
    bucketReader->setUpstreamReader(&s3Reader);

    EXPECT_EQ((uint64_t)256, bucketReader->read(buf, sizeof(buf)));
    EXPECT_EQ((uint64_t)0, bucketReader->read(buf, sizeof(buf)));
}

TEST_F(S3BucketReaderTest, ReaderShouldSkipIfFileIsNotForThisSegment) {
    ListBucketResult result;
    result.contents.emplace_back("foo", 456);

    EXPECT_CALL(s3Interface, listBucket(_, _, _, _)).Times(1).WillOnce(Return(result));

    s3ext_segid = 10;
    s3ext_segnum = 16;

    params.setBaseUrl("https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever");
    bucketReader->setUpstreamReader(&s3Reader);
    bucketReader->setS3InterfaceService(&s3Interface);
    bucketReader->open(params);

    EXPECT_EQ((uint64_t)0, bucketReader->read(buf, sizeof(buf)));
}

TEST_F(S3BucketReaderTest, UpstreamReaderThrowException) {
    ListBucketResult result;
    result.contents.emplace_back("foo", 0);
    result.contents.emplace_back("bar", 456);

    EXPECT_CALL(s3Interface, listBucket(_, _, _, _)).Times(1).WillOnce(Return(result));

    EXPECT_CALL(s3Reader, read(_, _)).Times(AtLeast(1)).WillRepeatedly(Throw(S3RuntimeError("")));

    EXPECT_CALL(s3Reader, open(_)).Times(1);

    s3ext_segid = 0;
    s3ext_segnum = 1;
    params.setBaseUrl("https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever");
    bucketReader->open(params);
    bucketReader->setUpstreamReader(&s3Reader);

    EXPECT_THROW(bucketReader->read(buf, sizeof(buf)), S3RuntimeError);
    EXPECT_THROW(bucketReader->read(buf, sizeof(buf)), S3RuntimeError);
}

class MockRead {
   public:
    MockRead(const char* ptr) : p(ptr) {
        size = strlen(p);
    }
    uint64_t operator()(char* buf, uint64_t len) {
        memcpy(buf, p, size);
        return size;
    }

   private:
    const char* p;
    uint64_t size;
};

TEST_F(S3BucketReaderTest, ReadBucketFileWithHeader) {
    hasHeader = true;

    ListBucketResult result;
    result.contents.emplace_back("foo", 4);
    result.contents.emplace_back("bar", 8);

    EXPECT_CALL(s3Interface, listBucket(_, _, _, _)).Times(1).WillOnce(Return(result));

    EXPECT_CALL(s3Reader, read(_, _))
        .WillOnce(Return(4))
        .WillOnce(Return(0))
        .WillOnce(Invoke(MockRead("abc\ndef\n")))
        .WillOnce(Return(0));

    EXPECT_CALL(s3Reader, open(_)).Times(2);

    s3ext_segid = 0;
    s3ext_segnum = 1;
    params.setBaseUrl("https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever");
    bucketReader->open(params);
    bucketReader->setUpstreamReader(&s3Reader);

    EXPECT_EQ((uint64_t)4, bucketReader->read(buf, sizeof(buf)));
    EXPECT_EQ((uint64_t)4, bucketReader->read(buf, sizeof(buf)));
    EXPECT_EQ((uint64_t)0, bucketReader->read(buf, sizeof(buf)));

    // reset to test following tests
    hasHeader = false;
}

TEST_F(S3BucketReaderTest, ReadBucketFileWithHeaderAndWrongEOL) {
    hasHeader = true;
    eolString[0] = '\r';
    eolString[1] = '\n';
    eolString[2] = '\0';

    ListBucketResult result;
    result.contents.emplace_back("foo", 4);
    result.contents.emplace_back("bar", 8);

    EXPECT_CALL(s3Interface, listBucket(_, _, _, _)).Times(1).WillOnce(Return(result));

    EXPECT_CALL(s3Reader, read(_, _))
        .WillOnce(Return(4))
        .WillOnce(Return(0))
        .WillOnce(Invoke(MockRead("abc\ndef\n")))
        .WillOnce(Return(0))
        .WillOnce(Return(0));

    EXPECT_CALL(s3Reader, open(_)).Times(2);

    s3ext_segid = 0;
    s3ext_segnum = 1;
    params.setBaseUrl("https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever");
    bucketReader->open(params);
    bucketReader->setUpstreamReader(&s3Reader);

    EXPECT_EQ((uint64_t)4, bucketReader->read(buf, sizeof(buf)));
    EXPECT_EQ((uint64_t)0, bucketReader->read(buf, sizeof(buf)));

    // reset to test following tests
    hasHeader = false;
    eolString[0] = '\n';
    eolString[1] = '\0';
}

TEST_F(S3BucketReaderTest, ReadBucketFileWithHeaderAndFuzyMatch) {
    hasHeader = true;
    eolString[0] = '\r';
    eolString[1] = '\n';
    eolString[2] = '\0';

    ListBucketResult result;
    result.contents.emplace_back("foo", 4);
    result.contents.emplace_back("bar", 8);

    EXPECT_CALL(s3Interface, listBucket(_, _, _, _)).Times(1).WillOnce(Return(result));

    EXPECT_CALL(s3Reader, read(_, _))
        .WillOnce(Return(4))
        .WillOnce(Return(0))
        .WillOnce(Invoke(MockRead("\ra\ndef\r\n")))
        .WillOnce(Return(0));

    EXPECT_CALL(s3Reader, open(_)).Times(2);

    s3ext_segid = 0;
    s3ext_segnum = 1;
    params.setBaseUrl("https://s3-us-east-2.amazonaws.com/s3test.pivotal.io/whatever");
    bucketReader->open(params);
    bucketReader->setUpstreamReader(&s3Reader);

    EXPECT_EQ((uint64_t)4, bucketReader->read(buf, sizeof(buf)));
    EXPECT_EQ((uint64_t)0, bucketReader->read(buf, sizeof(buf)));

    // reset to test following tests
    hasHeader = false;
    eolString[0] = '\n';
    eolString[1] = '\0';
}
