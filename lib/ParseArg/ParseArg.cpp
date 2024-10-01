#include "ParseArg.h"

namespace ParseArg {

bool isFile(const std::string& path) {
    return !path.empty() && path[0] != '-' && path.substr(0, 2) != "--";
}

void Arguments::Parse(int argc, char** argv) {
    std::vector<std::string> array;
    for (int i = 0; i < argc; i++) {
        array.push_back(argv[i]);
    }
    Parse(array);
}

void Arguments::Parse(const std::vector<std::string>& argv) {
    for (int idx = 1; idx < argv.size(); ++idx) {
        if (argv[idx] == "--help" || argv[idx] == "-h") {
            Help();
        }
        if (argv[idx] == "--create" || argv[idx] == "-c") {
            create_ = true;
        } else if (argv[idx].substr(0, 6) == "--file" ||
                   argv[idx].substr(0, 2) == "-f") {
            int pos = argv[idx].find("=") + 1;
            archive_name_ = argv[idx].substr(pos);
        } else if (argv[idx] == "--list" || argv[idx] == "-l") {
            list_ = true;
        } else if (argv[idx].substr(0, 9) == "--extract" ||
                   argv[idx].substr(0, 2) == "-x") {
            extract_path_ = argv[idx].substr(argv[idx].find("=") + 1);
            if (extract_path_ == "" || extract_path_ == "--extract" ||
                extract_path_ == "-x")
                extract_path_ = "./";
            extract_ = true;
            ++idx;
            while (isFile(argv[idx])) {
                extract_array_.push_back(argv[idx]);
                ++idx;
            }
            --idx;
        } else if (argv[idx] == "--append" || argv[idx] == "-a") {
            ++idx;
            while (isFile(argv[idx])) {
                append_.push_back(argv[idx]);
                ++idx;
            }
            --idx;
        } else if (argv[idx] == "--delete" || argv[idx] == "-d") {
            ++idx;
            while (isFile(argv[idx])) {
                delete_.push_back(argv[idx]);
                ++idx;
            }
            --idx;
        } else if (argv[idx] == "--concatenate" || argv[idx] == "-A") {
            concatenate_.first = argv[++idx];
            concatenate_.second = argv[++idx];
        } else {
            append_.push_back(argv[idx]);
        }
    }
}

void Arguments::Help() const {
    std::cout
        << "<<< Hamming Archivator [HamArc] >>>\n\n"
        << "Description:\n"
        << "\tПомехоустойчивый архиватор файлов без сжатия, объединяющий "
           "несколько файлов в один архив.\n"
        << "\tДля помехоустойчивого кодирования использовать Коды Хэмминга.\n\n"
        << "Options:\n"
        << "\t-h, --help - выводит наббор консольных команд архиватора.\n"
        << "\t\t(программа закончит работу после вывода)\n\n"
        << "\t-c, --create - создание архива.\n"
        << "\t\t(обязательно с командой --file)\n\n"
        << "\t-f, --file - полная директория до архива [ "
           "-f={path_to_arch/arch_name}, --file={path_to_arch/arch_name} ]\n\n"
        << "\t-l, --list - выводит список файлов в архиве.\n\n"
        << "\t-x, --extract - извлеч файлы из архива [ -x={path} {file1} "
           "{file2}... , --extract={path} {file1} {file2}... ]\n"
        << "\t\t(если не указан {path}, то извлекает в директорию "
           "{path_to_arch})\n"
        << "\t\t(если не указаны файлы извлекает всё из архива)\n\n"
        << "\t-a, --append - добавить файл в архив [ -a {path1} {path2}... , "
           "--append {path1} {path2}... ]\n"
        << "\t\t(имя файла не больше 28 символов с расширением)\n\n"
        << "\t-d, --delete - удалить файл из архива [ -d {file1} {file2}... , "
           "--delete {file1} {file2}... ]\n\n"
        << "\t-A, --concatenate - смерджить два архива [ -A {path1} {path2}, "
           "--concatenate {path1} {path2}]\n"
        << "\t\t(обязательно с командой --file)\n"
        << std::endl;
    exit(0);
}

}  // namespace ParseArg