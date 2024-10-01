#pragma < once>
#include <iostream>

namespace HamArc {
static const int kFileNameSize = 28;
static const int kIntInfoSize = 4;
static const int kFUllInfoSize = kFileNameSize + kIntInfoSize;

std::string pathFromDirectory(const std::string& directory);

std::string fileFromDirectory(const std::string& directory);

class HamArc {
 private:
  std::string archive_dir_;

  uint8_t header_info_[4];
  std::vector<uint8_t> header_;
  std::vector<uint8_t> data_;

  int GetHeaderInfo() const {
    int result = 0;
    for (int i = 0; i < kIntInfoSize; i++) {
      result <<= 8;
      result += header_info_[i];
    }
    return result;
  }

  void ChangeHeaderInfo(int value) {
    for (int i = kIntInfoSize - 1; i >= 0; --i) {
      header_info_[i] = value & 0xFF;
      value >>= 8;
    }
  }

  void WriteFileName(std::string name, int idx) {
    for (int i = 0; i < kFileNameSize; ++i) {
      if (i < kFileNameSize - name.size())
        header_.push_back(0);
      else
        header_.push_back(name[i - kFileNameSize + name.size()]);
    }
    for (int i = 24; i >= 0; i -= 8) {
      header_.push_back((idx >> i) & 0xFF);
    }
  }

  std::string encode16(const std::string& input);

  std::string decode17(std::string& encoded);

  std::vector<uint8_t> encodeHamming(const std::vector<uint8_t>& input);

  std::vector<uint8_t> decodeHamming(const std::vector<uint8_t>& encoded);

  std::pair<int, int> FindFile(const std::string& name) const;

  std::vector<std::string> ListFiles() const;

  void ExtractFile(std::string path, std::string name);

 public:
  HamArc(bool create, std::string archive_name);

  ~HamArc() = default;

  HamArc(const HamArc& other);

  HamArc& operator=(const HamArc& other) = default;

  void Read();

  void Write();

  void List() const;

  void Append(std::string path);

  void Extract(std::string path, std::string name) { ExtractFile(path, name); };

  void Extract(std::string arg) {
    if (arg.find("/") != std::string::npos) {
      std::vector<std::string> files = ListFiles();
      for (auto file : files) {
        ExtractFile(arg, file);
      }
    } else {
      Extract(pathFromDirectory(archive_dir_), arg);
    }
  }

  void Extract() {
    std::vector<std::string> files = ListFiles();
    for (auto file : files) {
      ExtractFile(pathFromDirectory(archive_dir_), file);
    }
  }

  HamArc& operator+(const HamArc& other);

  void Concatenate(const std::pair<std::string, std::string>& files);

  void Concatenate(std::string path1, std::string path2) {
    Concatenate(std::pair<std::string, std::string>(path1, path2));
  }

  void Delete(const std::string& name);

  // for testing
  int GetCountFiles() const { return GetHeaderInfo(); }

  std::vector<std::string> GetFilesList() const { return ListFiles(); }
};

}  // namespace HamArc