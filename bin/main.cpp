#include <iostream>
#include <fstream>

#include "lib/HamArc/HamArc.h"
#include "lib/ParseArg/ParseArg.h"

void del(const std::string& path) {
  std::ofstream output(path, std::ios::in | std::ios::binary);
  std::ifstream input(path, std::ios::binary);

  uint8_t byte;
  for (int i = 0; i < 10000; ++i) {
    input.read(reinterpret_cast<char*>(&byte), 1);
    if (i % 100 == 0) {
      output.seekp(i, output.beg);
      if (byte & 1) {
        --byte;
      } else {
        ++byte;
      }
      output.put(byte);
    }
  }
}

int main(int argc, char** argv) {
  ParseArg::Arguments args;
  args.Parse(argc, argv);
  // args.Help();

  HamArc::HamArc ham = HamArc::HamArc(args.Create(), args.ArchiveName());

  // ham.Print();

  if (args.Append()) {
    std::vector<std::string> files = args.AppendArray();
    for (auto file : files) ham.Append(file);
  }

  if (args.Extract()) {
    if (args.ExtractPath() == "./") {
      if (args.ExtractArray().empty()) {
        ham.Extract();
      } else {
        std::vector<std::string> files = args.ExtractArray();
        for (auto file : files) ham.Extract(file);
      }
    } else {
      if (args.ExtractArray().empty()) {
        ham.Extract(args.ExtractPath());
      } else {
        std::vector<std::string> files = args.ExtractArray();
        for (auto file : files) ham.Extract(args.ExtractPath(), file);
      }
    }
  }

  if (args.Concatenate()) {
    std::pair<std::string, std::string> files = args.ConcatenateArray();
    ham.Concatenate(files);
  }

  if (args.Delete()) {
    std::vector<std::string> files = args.DeleteArray();
    for (auto file : files) ham.Delete(file);
  }

  if (args.List()) ham.List();

  ham.Write();

  return 0;
}