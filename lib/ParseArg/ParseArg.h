#pragma < once>
#include <iostream>

namespace ParseArg {

class Arguments {
 private:
  bool create_ = false;
  std::string archive_name_ = "";
  bool list_ = false;
  bool extract_ = false;
  std::string extract_path_ = "";
  std::vector<std::string> extract_array_ = {};
  std::vector<std::string> append_ = {};
  std::vector<std::string> delete_ = {};
  std::pair<std::string, std::string> concatenate_ = {"", ""};

  void Parse(const std::vector<std::string>& argv);

 public:
  Arguments() = default;

  void Parse(int argc, char** argv);

  bool Create() const { return create_; }

  std::string ArchiveName() const { return archive_name_; }

  bool List() const { return list_; }

  bool Extract() const { return extract_; }

  std::string ExtractPath() const { return extract_path_; }

  std::vector<std::string> ExtractArray() const { return extract_array_; }

  bool Append() const { return !append_.empty(); }

  std::vector<std::string> AppendArray() const { return append_; }

  bool Delete() const { return !delete_.empty(); }

  std::vector<std::string> DeleteArray() const { return delete_; }

  bool Concatenate() const { return !concatenate_.first.empty(); }

  std::pair<std::string, std::string> ConcatenateArray() const { return concatenate_; }

  void Help() const;
};

}  // namespace ParseArg