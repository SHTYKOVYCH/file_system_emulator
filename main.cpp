#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm>
#include <regex>

using namespace std;
//
//class FileSystemInstance {
//private:
//    int* uid;
//    string name;
//
//    time_t* dateCreated;
//    time_t* dateChanged;
//
//public:
//    static int LAST_UID;
//    static vector<FileSystemInstance*>* files;
//
//    FileSystemInstance(string name) {
//        this->name = name;
//        this->dateCreated = new time_t(time(0));
//        this->dateChanged = new time_t(*this->dateCreated);
//
//        this->uid = new int(++FileSystemInstance::LAST_UID);
//
//        FileSystemInstance::files->push_back(this);
//    }
//
//    FileSystemInstance(int uid, string name) {
//        for (auto file : *FileSystemInstance::files) {
//            if (*file->uid == uid) {
//                this->uid = file->uid;
//                this->name = name;
//                this->dateCreated = file->dateCreated;
//                this->dateChanged = file->dateChanged;
//                break;
//            }
//        }
//        FileSystemInstance::files->push_back(this);
//    }
//
//    virtual ~FileSystemInstance() {
//        for (auto file = FileSystemInstance::files->begin(); file < FileSystemInstance::files->end(); ++file) {
//            if ((*file)->getUid() == this->getUid()) {
//                FileSystemInstance::files->erase(file);
//            }
//        }
//        delete(this->uid);
//        delete(this->dateChanged);
//        delete(this->dateCreated);
//    }
//
//    virtual string getName() {
//        return this->name;
//    }
//
//    void setName(string name) {
//        this->name = name;
//        *this->dateChanged = time(0);
//    }
//
//    void change() {
//        *this->dateChanged = time(0);
//    }
//
//    int getUid() {
//        return *this->uid;
//    }
//};
//int FileSystemInstance::LAST_UID = 0;
//vector<FileSystemInstance*>* FileSystemInstance::files = new vector<FileSystemInstance*>;
//
//class File : public FileSystemInstance {
//private:
//    string* extension;
//public:
//    File(string name, string extension) : FileSystemInstance(name) { this->extension = new string(extension); }
//    File(int uid, string name, string* extension) : FileSystemInstance(uid, name) { this->extension = extension; };
//
//
//    virtual ~File() {
//        delete this->extension;
//    }
//
//    File* createHardLink() {
//        return new File(this->getUid(), this->getName(), this->extension);
//    }
//
//    void rename(string name, string extension) {
//        *this->extension = extension;
//        this->setName(name);
//    }
//
//    File* copy(string name) {
//        return new File(name, *this->extension);
//    }
//};
//
//class Dir: public FileSystemInstance{
//private:
//    Dir* parentDir;
//    vector<FileSystemInstance*> childs;
//public:
//    Dir(string name, Dir* parentDir) : FileSystemInstance(name) { this->parentDir = parentDir; };
//
//    void addChild(FileSystemInstance* fsi) {
//        this->childs.push_back(fsi);
//    }
//
//    virtual ~Dir() {
//        for (auto child : childs) {
//            delete child;
//        }
//    }
//
//    Dir* operator+(FileSystemInstance* smt) {
//        this->addChild(smt);
//        return this;
//    }
//};
//
//class SoftLink : public FileSystemInstance {
//private:
//    int pointingFile;
//    int type;
//public:
//    SoftLink(int type, string name) : FileSystemInstance(name) { this->type = type; };
//};

int main() {
    //Dir* root = new Dir("root", nullptr);
    string userInput;

    do {
        cout << " > ";
        getline(cin, userInput, '\n');

        while (userInput[0] == ' ') {
            userInput.erase(userInput.begin());
        }

        while (*(userInput.end() - 1) == ' ') {
            userInput.erase(userInput.end() - 1);
        }

        while (userInput.find("  ") != string::npos) {
            userInput = std::regex_replace(userInput, regex("  "), " ");
        }

        regex splitter(" ");
        vector<string> commandNArgs(sregex_token_iterator(userInput.begin(), userInput.end(), splitter, -1), sregex_token_iterator());

        if (commandNArgs[0] == "exit") {
            break;
        }

    } while (true);

    //delete FileSystemInstance::files;
    //delete root;

    return 0;
}
