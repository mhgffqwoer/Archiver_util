#include "HamArc.h"

#include <filesystem>
#include <fstream>

namespace HamArc {

std::string pathFromDirectory(const std::string& directory) {
  if (directory == "") return "";

  return directory.substr(0, directory.find_last_of('/') + 1);
}

std::string fileFromDirectory(const std::string& directory) {
  if (directory == "") return "";

  return directory.substr(directory.find_last_of('/') + 1);
}

std::string HamArc::encode16(const std::string& input) {
  std::string encoded;

  size_t m = input.length();
  size_t r = 0;
  while ((1 << r) < (m + r + 1)) {
    ++r;
  }

  for (size_t i = 0, j = 0, k = 0; i < m + r; ++i) {
    if (i + 1 == (1 << k)) {
      encoded.push_back('0');
      ++k;
    } else {
      encoded.push_back(input[j]);
      ++j;
    }
  }

  for (size_t i = 0; i < r; ++i) {
    size_t parityBitPosition = (1 << i) - 1;
    int parity = 0;
    for (size_t j = parityBitPosition; j < m + r; j += (1 << (i + 1))) {
      parity ^= (encoded[j] - '0');
    }
    encoded[parityBitPosition] = (parity + '0');
  }

  return encoded;
}

std::string HamArc::decode17(std::string& encoded) {
  int cnt = 0;

  for (int i = 1; i < encoded.size(); i <<= 1) {
    int cntr = 0;
    for (int j = i - 1; j < encoded.size(); j += (i + i)) {
      for (int k = 0; k < i && (j + k) < encoded.size(); ++k) cntr ^= (encoded[j + k] - '0');
    }
    if (cntr == 1) cnt += i;
  }

  if (cnt != 0) {
    if (encoded[cnt - 1] == '0') encoded[cnt - 1] = '1';
    else encoded[cnt - 1] = '0';
  }
  std::cout << "Error: " << cnt << std::endl;

  return encoded;
}

std::vector<uint8_t> HamArc::encodeHamming(const std::vector<uint8_t>& input) {
  std::vector<uint8_t> encoded;
  std::string temp = "";
  for (auto i : input) {
    temp += std::bitset<8>(i).to_string();
    if (temp.size() == 128) {
      temp = encode16(temp);
      std::string temp2;
      for (auto j : temp) {
        temp2 += j;
        if (temp2.size() == 8) {
          encoded.push_back(static_cast<uint8_t>(std::stoi(temp2, nullptr, 2)));
          temp2 = "";
        }
      }
      temp = "";
    }
  }
  if (temp.size() > 0) {
    temp.append(128 - temp.size(), '0');
    temp = encode16(temp);
    std::string temp2;
    for (auto j : temp) {
      temp2 += j;
      if (temp2.size() == 8) {
        encoded.push_back(static_cast<uint8_t>(std::stoi(temp2, nullptr, 2)));
        temp2 = "";
      }
    }
  }

  return encoded;
}

std::vector<uint8_t> HamArc::decodeHamming(
    const std::vector<uint8_t>& encoded) {
  std::vector<uint8_t> decoded;
  std::string temp = "";
  for (auto i : encoded) {
    temp += std::bitset<8>(i).to_string();
    if (temp.size() == 136) {
      temp = decode17(temp);
      std::string temp2;
      for (auto j : temp) {
        temp2 += j;
        if (temp2.size() == 8) {
          decoded.push_back(static_cast<uint8_t>(std::stoi(temp2, nullptr, 2)));
          temp2 = "";
        }
      }
      temp = "";
    }
  }
  return decoded;
}

std::pair<int, int> HamArc::FindFile(const std::string& name) const {
  int file_count = GetHeaderInfo();
  std::pair<int, int> result = {0, 0};

  std::string temp;
  for (int i = 0; i < file_count; ++i) {
    for (int j = 0; j < kFileNameSize; ++j) {
      if (header_[i * kFUllInfoSize + j] != 0)
        temp += header_[i * kFUllInfoSize + j];
    }
    for (int j = 0; j < kIntInfoSize; ++j) {
      if (i != file_count - 1) {
        result.first <<= 8;
        result.first += header_[i * kFUllInfoSize + kFileNameSize + j];
        result.second <<= 8;
        result.second += header_[(i + 1) * kFUllInfoSize + kFileNameSize + j];
      } else {
        result.first <<= 8;
        result.first += header_[i * kFUllInfoSize + kFileNameSize + j];
        result.second = data_.size();
      }
    }
    if (temp == name) {
      return result;
    }
    temp = "";
    result = {0, 0};
  }
  std::cout << "File not found: " << name << std::endl;
  exit(1);
}

std::vector<std::string> HamArc::ListFiles() const {
  std::vector<std::string> result;
  int file_count = GetHeaderInfo();
  for (int i = 0; i < file_count; ++i) {
    std::string temp;
    for (int j = 0; j < kFileNameSize; ++j) {
      if (header_[i * kFUllInfoSize + j] != 0)
        temp += header_[i * kFUllInfoSize + j];
    }
    result.push_back(temp);
  }
  return result;
}

void HamArc::ExtractFile(std::string path, std::string name) {
  std::pair<int, int> idxs = FindFile(name);
  std::filesystem::create_directory(path);
  std::ofstream file(path + "/" + name);
  std::cout << "Extracting: " << path + "/" + name << " .\n\n";
  for (int i = idxs.first; i < idxs.second; ++i) {
    if (data_[i] != 0) file << data_[i];
  }
  // delete file from archive
}

HamArc::HamArc(bool create, std::string archive_dir) {
  if (create) {
    archive_dir_ = archive_dir;
    std::cout << "Archive created: " << fileFromDirectory(archive_dir_) << ".\n"
              << std::endl;
    ChangeHeaderInfo(0);
    std::filesystem::create_directory(pathFromDirectory(archive_dir));
    Write();
  } else {
    archive_dir_ = archive_dir;
    Read();
  }
}

HamArc::HamArc(const HamArc& other) {
  archive_dir_ = other.archive_dir_;
  header_info_[0] = other.header_info_[0];
  header_info_[1] = other.header_info_[1];
  header_info_[2] = other.header_info_[2];
  header_info_[3] = other.header_info_[3];
  header_ = other.header_;
  data_ = other.data_;
};

void HamArc::Read() {
  std::ifstream file(archive_dir_);
  if (!file) {
    std::cerr << "Can't open file: " << archive_dir_ << std::endl;
    exit(1);
  }
  for (int i = 0; i < kIntInfoSize; ++i) header_info_[i] = file.get();

  int file_count = GetHeaderInfo();

  std::vector<uint8_t> temp_data;
  for (int i = 0; !file.eof(); ++i) {
    auto temp = file.get();
    if (temp == EOF) break;
    temp_data.push_back(temp);
  }

  temp_data = decodeHamming(temp_data);

  for (int i = 0; i < temp_data.size(); ++i) {
    if (i < file_count * kFUllInfoSize)
      header_.push_back(temp_data[i]);
    else
      data_.push_back(temp_data[i]);
  }

  Write();

  file.close();
}

void HamArc::Write() {
  std::ofstream file(archive_dir_);
  for (int i = 0; i < kIntInfoSize; ++i) file << header_info_[i];

  std::vector<uint8_t> temp_vector = encodeHamming(header_);
  for (int i = 0; i < temp_vector.size(); ++i) {
    file << temp_vector[i];
  }

  temp_vector = encodeHamming(data_);
  for (int i = 0; i < temp_vector.size(); ++i) {
    file << temp_vector[i];
  }

  file.close();
}

void HamArc::List() const {
  std::cout << "List " << fileFromDirectory(archive_dir_) << ":\n";
  std::vector<std::string> files = ListFiles();
  for (auto file : files) {
    std::cout << "\t->" << file << "\n";
  }
  std::cout << "\n";
}

void HamArc::Append(std::string path) {
  std::ifstream file(path);
  ChangeHeaderInfo(GetHeaderInfo() + 1);
  int dot = data_.size();
  WriteFileName(fileFromDirectory(path), data_.size());
  for (int i = 0; !file.eof(); ++i) {
    auto temp = file.get();
    if (temp == EOF) break;
    data_.push_back(temp);
  }
  Write();
  file.close();
  // std::remove(path.c_str());
}

HamArc& HamArc::operator+(const HamArc& other) {
  ChangeHeaderInfo(other.GetHeaderInfo() + GetHeaderInfo());
  int length_data1 = data_.size();
  int file_count = other.GetHeaderInfo();
  for (int i = 0; i < file_count; ++i) {
    for (int j = 0; j < kFileNameSize; ++j) {
      header_.push_back(other.header_[i * kFUllInfoSize + j]);
    }
    int idx = 0;
    for (int j = 0; j < kIntInfoSize; ++j) {
      idx <<= 8;
      idx += other.header_[i * kFUllInfoSize + kFileNameSize + j];
    }
    idx += length_data1;
    for (int j = kIntInfoSize - 1; j >= 0; --j) {
      header_.push_back((idx >> (8 * j)) & 0xFF);
    }
  }
  for (int i = 0; i < other.data_.size(); ++i) {
    data_.push_back(other.data_[i]);
  }
  return *this;
}

void HamArc::Concatenate(const std::pair<std::string, std::string>& files) {
  if (files.first == "" || files.second == "") {
    std::cout << "Need two files" << std::endl;
    exit(1);
  }
  std::cout << "Concatenating " << fileFromDirectory(files.first) << " and "
            << fileFromDirectory(files.second) << " to "
            << fileFromDirectory(archive_dir_) << ".\n\n";
  HamArc ham1 = HamArc(false, files.first);
  HamArc ham2 = HamArc(false, files.second);
  *this = *this + ham1;
  *this = *this + ham2;
  Write();
}

void HamArc::Delete(const std::string& name) {
  std::pair<int, int> idxs = FindFile(name);
  std::vector<std::string> files = ListFiles();
  int file_number;

  for (int i = 0; i < files.size(); ++i) {
    if (files[i] == name) {
      file_number = i;
      break;
    }
  }

  std::vector<uint8_t> temp_header = header_;
  std::vector<uint8_t> temp_data = data_;
  header_ = {};
  data_ = {};

  for (int i = 0; i < files.size(); ++i) {
    if (i != file_number) {
      for (int j = 0; j < kFUllInfoSize; ++j) {
        header_.push_back(temp_header[i * kFUllInfoSize + j]);
      }
    }
  }
  for (int i = 0; i < temp_data.size(); ++i) {
    if (i < idxs.first || i >= idxs.second) {
      data_.push_back(temp_data[i]);
    }
  }

  ChangeHeaderInfo(GetHeaderInfo() - 1);

  Write();
}

}  // namespace HamArc