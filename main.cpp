#include <iostream>
#include <string>
#include <ctime>
#include <vector>
#include <algorithm>
#include <regex>
#include <queue>

using namespace std;

enum FILE_SYSTEM_INSTANCE_TYPES
{
    FILES,
    DIRS,
    LINKS
};

class FileSystemInstance
{
private:
    int *uid;
    string name;

    time_t *dateCreated;
    time_t *dateChanged;

    int type;
public:
    static int LAST_UID;
    static vector<FileSystemInstance *> *files;

    FileSystemInstance(string name, int type) {
        this->name = name;
        this->dateCreated = new time_t(time(0));
        this->dateChanged = new time_t(*this->dateCreated);

        this->uid = new int(++FileSystemInstance::LAST_UID);

        this->type = type;

        FileSystemInstance::files->push_back(this);
    }

    FileSystemInstance(int uid, string name) {
        for (auto file: *FileSystemInstance::files) {
            if (*file->uid == uid) {
                this->uid = file->uid;
                this->type = file->type;
                this->name = name;
                this->dateCreated = file->dateCreated;
                this->dateChanged = file->dateChanged;
                break;
            }
        }
        FileSystemInstance::files->push_back(this);
    }

    virtual ~FileSystemInstance() {
        for (auto file = FileSystemInstance::files->begin(); file < FileSystemInstance::files->end(); ++file) {
            if ((*file)->getUid() == this->getUid()) {
                file = FileSystemInstance::files->erase(file);
            }
        }
        delete (this->uid);
        delete (this->dateChanged);
        delete (this->dateCreated);
    }

    virtual string getName() {
        return this->name;
    }

    virtual void setName(string name) {
        this->name = name;
        *this->dateChanged = time(0);
    }

    int getUid() {
        return *this->uid;
    }

    int getType() {
        return this->type;
    }

    time_t *getDateChanged() {
        return this->dateChanged;
    }

    time_t *getDateCreated() {
        return this->dateCreated;
    }

    void change() {
        *this->dateChanged = time(0);
    }

};

class File : public FileSystemInstance
{
private:
    string *extension;
    int *size;
public:
    File(string name, string extension, int size) : FileSystemInstance(name, FILES) {
        this->extension = new string(extension);
        this->size = new int(size);
    }

    File(int uid, string name, string *extension, int *size) : FileSystemInstance(uid, name) {
        this->extension = extension;
        this->size = size;
    };

    virtual ~File() {
        delete this->extension;
        delete this->size;
    }

    virtual string getName() {
        return  FileSystemInstance::getName() + *this->extension;
    }

    File *createHardLink(string name) {
        return new File(this->getUid(), name, this->extension, this->size);
    }

    virtual void setName(string name, string extension) {
        *this->extension = extension;
        FileSystemInstance::setName(name);
    }

    File *copy(string name) {
        return new File(name, *this->extension, *this->size);
    }
};

class Dir : public FileSystemInstance
{
private:
    Dir *parentDir;
    vector<FileSystemInstance *> childs;
public:
    Dir(string name, Dir *parentDir = nullptr) : FileSystemInstance(name, DIRS) { this->parentDir = parentDir; };

    virtual ~Dir() {
        for (auto i = this->childs.begin(); i < this->childs.end(); ++i) {
            for (auto j = i + 1; j < this->childs.end(); ++j) {
                if ((*i)->getUid() == (*j)->getUid()) {
                    j = this->childs.erase(j);
                }
            }
            delete *i;
        }
    }

    void addChild(FileSystemInstance *fsi) {
        this->childs.push_back(fsi);
    }

    void removeChild(FileSystemInstance *fsi) {
        this->childs.erase(find(this->childs.begin(), this->childs.end(), fsi));
    }

    void setParendDir(Dir *parentDir) {
        this->parentDir = parentDir;
    }

    Dir *getParentDir() {
        return this->parentDir;
    }

    vector<FileSystemInstance *> *getChilds() {
        return &childs;
    }

    string getPath(string assembledPath = "") {
        if (parentDir == nullptr) {
            return assembledPath;
        }

        return this->parentDir->getPath(assembledPath) + '/' + this->getName();
    }

    FileSystemInstance* findNGetInstance(string name) {
        for (auto existingFile: this->childs) {
            if (existingFile->getName() == name) {
                return existingFile;
            }
        }
        return nullptr;
    }

    FileSystemInstance* findNGetInstanceByType(string name, int type) {
        for (auto existingFile: this->childs) {
            if (existingFile->getType() == type && existingFile->getName() == name) {
                return existingFile;
            }
        }
        return nullptr;
    }

//    void removeChildsFromTree(int uid) {
//        if (this->parentDir != nullptr) {
//            this->parentDir->removeChildsFromTree(uid);
//        } else {
//            this->removeChild(uid);
//
//            queue<Dir*> dirsToSearch;
//            dirsToSearch.push(this);
//            do {
//                Dir* dir = dirsToSearch.pop();
//
//            }while(dirsToSearch.size());
//        }
//    }
};

//class SoftLink : public FileSystemInstance {
//private:
//    int pointingFile;
//    int type;
//public:
//    SoftLink(int type, string name) : FileSystemInstance(name) { this->type = type; };
//};


int FileSystemInstance::LAST_UID = 0;
vector<FileSystemInstance *> *FileSystemInstance::files = new vector<FileSystemInstance *>;

int main() {
    Dir *root = new Dir("root", nullptr);
    string userInput;

    Dir *currentDir = root;

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
        vector<string> commandNArgs(sregex_token_iterator(userInput.begin(), userInput.end(), splitter, -1),
                                    sregex_token_iterator());

        if (commandNArgs[0] == "exit") {
            break;
        }
        if (commandNArgs[0] == "clear") {
            system("clear");
            continue;
        }
        if (commandNArgs[0] == "ls") {
            auto files = currentDir->getChilds();

            cout << "Total: " << files->size() << endl;

            for (auto file: *files) {
                cout << file->getUid() << ' ';
                switch (file->getType()) {
                    case (FILES):
                        cout << "file ";
                        break;
                    case (DIRS):
                        cout << "dir  ";
                        break;
                    case (LINKS):
                        cout << "link ";
                        break;
                }
                char buffer[80] = {0};
                strftime(buffer, 80, "%d.%m.%Y %H:%M:%S", localtime(file->getDateCreated()));
                cout << buffer << ' ';
                strftime(buffer, 80, "%d.%m.%Y %H:%M:%S", localtime(file->getDateChanged()));
                cout << buffer << ' ';
                cout << file->getName() << endl;
            }

            continue;
        }
        if (commandNArgs[0] == "pwd") {
            cout << currentDir->getPath() << endl;
            continue;
        }

        if (commandNArgs[0] == "mkdir") {
            if (commandNArgs.size() < 2) {
                cout << "Usage: mkdir <dirname1> <dirname2> ... <dirnameN>" << endl;
                continue;
            }

            for (auto i = commandNArgs.begin() + 1; i < commandNArgs.end(); ++i) {
                bool folderExists = false;
                for (auto existingFile: *currentDir->getChilds()) {
                    if (existingFile->getType() == DIRS && existingFile->getName() == *i) {
                        folderExists = true;
                        break;
                    }
                }

                if (folderExists) {
                    cout << "Folder already exists" << endl;
                    break;
                }
                Dir *newDir = new Dir(*i, currentDir);

                currentDir->addChild(newDir);
            }

            continue;
        }

        if (commandNArgs[0] == "cd") {
            if (commandNArgs.size() < 2) {
                cout << "usage: cd <folder name>" << endl;
                continue;
            }

            if (commandNArgs[1] == "..") {
                if (currentDir->getParentDir() != nullptr) {
                    currentDir = currentDir->getParentDir();
                    continue;
                }

                cout << "Already on top of the file tree" << endl;
                continue;
            }

            FileSystemInstance *changedDir = nullptr;
            for (auto child: *currentDir->getChilds()) {
                if (child->getType() == DIRS && child->getName() == commandNArgs[1]) {
                    changedDir = child;
                    break;
                }
            }

            if (changedDir == nullptr) {
                cout << "No such dir in this directory" << endl;
                continue;
            }

            currentDir = (Dir *) changedDir;
            continue;
        }

        if (commandNArgs[0] == "rm") {
            if (commandNArgs.size() < 2) {
                cout << "Usage: rm <filename1/dirname1> <filename2/dirname2> ... <filenameN/dirnameN>" << endl;
                continue;
            }

            for (auto i = commandNArgs.begin() + 1; i < commandNArgs.end(); ++i) {
                FileSystemInstance *instance = nullptr;
                for (auto existingFile: *currentDir->getChilds()) {
                    if (existingFile->getName() == *i) {
                        instance = existingFile;
                        break;
                    }
                }

                if (instance == nullptr) {
                    cout << "No such file/dir" << endl;
                    break;
                }

                currentDir->removeChild(instance);

                delete instance;
            }
        }

        if (commandNArgs[0] == "touch") {
            if (commandNArgs.size() < 3) {
                cout << "Usage: touch <filename>.<extension> <size>" << endl;
                continue;
            }

            if (commandNArgs[1].find('.') == string::npos) {
                cout << "Filename should be written in format: <name>.<extension>" << endl;
                continue;
            }

            string nameNextension[2];

            nameNextension[0] = commandNArgs[1].substr(0, commandNArgs[1].find('.'));
            nameNextension[1] = commandNArgs[1].substr(commandNArgs[1].find('.'));

            try {
                stoi(commandNArgs[2]);
            } catch (exception e) {
                cout << "Error on converting size" << endl;
                continue;
            }

            currentDir->addChild(new File(nameNextension[0], nameNextension[1], stoi(commandNArgs[2])));

            continue;
        }

        if (commandNArgs[0] == "mv") {
            if (commandNArgs.size() < 3) {
                cout << "Usage: mv <file/dir> <destination>" << endl;
                continue;
            }

            FileSystemInstance* instance = currentDir->findNGetInstance(commandNArgs[1]);
            FileSystemInstance* destination = currentDir->findNGetInstanceByType(commandNArgs[2], DIRS);

            if (instance == nullptr) {
                cout << "No such file or directory" << endl;
                continue;
            }

            if (destination == nullptr) {
                if (commandNArgs[2] == "..") {
                    if (currentDir == root) {
                        cout << "Cannot move outside of the root" << endl;
                        continue;
                    }

                    if (currentDir->getParentDir()->findNGetInstanceByType(instance->getName(), instance->getType())) {
                        cout << "Item already exists in the destination folder" << endl;
                        continue;
                    }

                    currentDir->removeChild(instance);

                    currentDir->getParentDir()->addChild(instance);
                    continue;
                }

                if (currentDir->findNGetInstanceByType(commandNArgs[2], instance->getType()) == nullptr) {
                    if (instance->getType() == FILES) {
                        if (commandNArgs[2].find('.') == string::npos) {
                            cout << "Filename should be written in format: <name>.<extension>" << endl;
                            continue;
                        }
                        string nameNextension[2];

                        nameNextension[0] = commandNArgs[2].substr(0, commandNArgs[2].find('.'));
                        nameNextension[1] = commandNArgs[2].substr(commandNArgs[2].find('.'));

                        ((File*)instance)->setName(nameNextension[0], nameNextension[1]);
                    } else {
                        instance->setName(commandNArgs[2]);
                    }
                    continue;
                }

                cout << "Item with such name already exists" << endl;
            }

            currentDir->removeChild(instance);

            ((Dir*)destination)->addChild(instance);

            continue;
        }

        if (commandNArgs[0] == "ln") {
            if (commandNArgs.size() < 3 || commandNArgs.size() > 1 && commandNArgs[1] == "-s" && commandNArgs.size() < 4) {
                cout << "Usage: ln [-s] <file> <link>" << endl;
                continue;
            }

            if (commandNArgs[1] != "-s") {
                FileSystemInstance* file = currentDir->findNGetInstanceByType(commandNArgs[1], FILES);

                if (file == nullptr) {
                    cout << "No such file" << endl;
                    continue;
                }

                FileSystemInstance* test = currentDir->findNGetInstanceByType(commandNArgs[2], FILES);

                if (test != nullptr) {
                    cout << "File already exists" << endl;
                    continue;
                }

                if (commandNArgs[2].find('.') == string::npos) {
                    cout << "Filename should be written in format: <name>.<extension>" << endl;
                    continue;
                }

                string nameNextension[2];

                nameNextension[0] = commandNArgs[2].substr(0, commandNArgs[2].find('.'));
                nameNextension[1] = commandNArgs[2].substr(commandNArgs[2].find('.'));

                currentDir->addChild(((File*)file)->createHardLink(nameNextension[0]));
            }
        }
    } while (true);

    delete root;
    delete FileSystemInstance::files;

    return 0;
}
