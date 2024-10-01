#include <gtest/gtest.h>
#include <lib/HamArc/HamArc.h>

#include <fstream>
#include <sstream>

// Path to original and result files
const std::string kPathOriginal = "/Users/mhgffqwoer/Desktop/Basics_of_programming/labwork6-mhgffqwoer/tests/temp/original/";
const std::string kPathResult = "/Users/mhgffqwoer/Desktop/Basics_of_programming/labwork6-mhgffqwoer/tests/temp/result/";
const std::string kPathArchive = "/Users/mhgffqwoer/Desktop/Basics_of_programming/labwork6-mhgffqwoer/tests/temp/";

const std::string kFile1 = "file1.txt";
const std::string kFile2 = "file2.txt";

const std::string kArchive1 = "archive1.haf";
const std::string kArchive2 = "archive2.haf";
const std::string kArchiveMerge = "archiveMerge.haf";

bool CompareFiles(const std::string& file1, const std::string& file2) {
  std::ifstream file1Stream(file1);
  std::ifstream file2Stream(file2);

  for (int i = 0; !file1Stream.eof() && !file2Stream.eof(); ++i) {
    auto temp1 = file1Stream.get();
    auto temp2 = file2Stream.get();

    if (temp1 != temp2) {
      file1Stream.close();
      file2Stream.close();
      return false;
    }
  }

  return true;
}

TEST(ArchiveTest, CreateArchive) {
  bool create = true;
  HamArc::HamArc archive = HamArc::HamArc(create, kPathArchive + kArchive1);

  bool result = true;
  std::ifstream file(kPathOriginal + kArchive1);
  for (int i = 0; !file.eof(); ++i) {
    auto temp = file.get();
    if (temp == EOF) break;
    if (temp != '\0') result = false;
  }
  file.close();

  EXPECT_EQ(result, true);
}

TEST(ArchiveTest, ReadArchive) {
  bool create = false;
  HamArc::HamArc archive = HamArc::HamArc(create, kPathArchive + kArchive1);

  bool result = true;
  std::ifstream file(kPathOriginal + kArchive1);
  for (int i = 0; !file.eof(); ++i) {
    auto temp = file.get();
    if (temp == EOF) break;
    if (temp != '\0') result = false;
  }
  file.close();

  EXPECT_EQ(result, true);
}

TEST(ArchiveTest, AppendFile) {
  bool create = true;
  HamArc::HamArc archive = HamArc::HamArc(create, kPathArchive + kArchive1);

  int count = archive.GetCountFiles();
  EXPECT_EQ(count, 0);

  archive.Append(kPathOriginal + kFile1);

  count = archive.GetCountFiles();
  EXPECT_EQ(count, 1);

  archive.Append(kPathOriginal + kFile2);

  count = archive.GetCountFiles();
  EXPECT_EQ(count, 2);
}

TEST(ArchiveTest, ArchiveList) {
  bool create = true;
  HamArc::HamArc archive = HamArc::HamArc(create, kPathArchive + kArchive1);
  archive.Append(kPathOriginal + kFile1);
  archive.Append(kPathOriginal + kFile2);

  std::vector<std::string> list = archive.GetFilesList();
  if (list.size() != 2) FAIL();
  for (auto file : list) {
    if (file != kFile1 && file != kFile2) FAIL();
  }
}

TEST(ArchiveTest, ExtractFile) {
  bool create = true;
  HamArc::HamArc archive = HamArc::HamArc(create, kPathArchive + kArchive1);
  archive.Append(kPathOriginal + kFile1);
  archive.Append(kPathOriginal + kFile2);

  archive.Extract(kPathResult, kFile1);
  archive.Extract(kPathResult, kFile2);

  EXPECT_EQ(CompareFiles(kPathResult + kFile1, kPathOriginal + kFile1), true);
  EXPECT_EQ(CompareFiles(kPathResult + kFile2, kPathOriginal + kFile2), true);
}

TEST(ArchiveTest, ExtractAllFiles) {
  bool create = true;
  HamArc::HamArc archive = HamArc::HamArc(create, kPathArchive + kArchive1);
  archive.Append(kPathOriginal + kFile1);
  archive.Append(kPathOriginal + kFile2);

  archive.Extract(kPathResult);

  EXPECT_EQ(CompareFiles(kPathResult + kFile1, kPathOriginal + kFile1), true);
  EXPECT_EQ(CompareFiles(kPathResult + kFile2, kPathOriginal + kFile2), true);
}

TEST(ArchiveTest, DeleteFile) {
  bool create = true;
  HamArc::HamArc archive1 = HamArc::HamArc(create, kPathArchive + kArchive1);
  HamArc::HamArc archive2 = HamArc::HamArc(create, kPathArchive + kArchive2);

  archive1.Append(kPathOriginal + kFile1);
  archive1.Append(kPathOriginal + kFile2);

  archive2.Append(kPathOriginal + kFile1);

  EXPECT_EQ(archive1.GetCountFiles(), 2);
  EXPECT_EQ(archive2.GetCountFiles(), 1);

  archive1.Delete(kFile2);

  EXPECT_EQ(archive1.GetCountFiles(), 1);

  EXPECT_EQ(CompareFiles(kPathArchive + kArchive1, kPathArchive + kArchive2),
            true);
}

TEST(ArchiveTest, MergeArchives) {
  bool create = true;
  HamArc::HamArc archive1 = HamArc::HamArc(create, kPathArchive + kArchive1);
  HamArc::HamArc archive2 = HamArc::HamArc(create, kPathArchive + kArchive2);
  HamArc::HamArc merge = HamArc::HamArc(create, kPathArchive + kArchiveMerge);

  archive1.Append(kPathOriginal + kFile1);
  archive2.Append(kPathOriginal + kFile2);

  EXPECT_EQ(archive1.GetCountFiles(), 1);
  EXPECT_EQ(archive2.GetCountFiles(), 1);

  merge.Concatenate(kPathArchive + kArchive1, kPathArchive + kArchive2);
  EXPECT_EQ(merge.GetCountFiles(), 2);

  std::vector<std::string> list = merge.GetFilesList();
  if (list.size() != 2) FAIL();
  for (auto file : list) {
    if (file != kFile1 && file != kFile2) FAIL();
  }

  merge.Extract(kPathResult);
  EXPECT_EQ(CompareFiles(kPathResult + kFile1, kPathOriginal + kFile1), true);
  EXPECT_EQ(CompareFiles(kPathResult + kFile2, kPathOriginal + kFile2), true);
}