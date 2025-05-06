#include "filetree.h"
#include "../utils/utils.h"
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>

namespace cvim {

FileTree::FileTree() : selectedIndex_(0) {
    root_ = std::make_shared<FileNode>();
    root_->name = "/";
    root_->path = "/";
    root_->isDirectory = true;
    
    currentNode_ = root_;
}

FileTree::~FileTree() {}

bool FileTree::loadDirectory(const std::string& path) {
    root_ = std::make_shared<FileNode>();
    root_->name = getFileName(path);
    root_->path = path;
    root_->isDirectory = true;
    
    buildTree(path, root_);
    currentNode_ = root_;
    selectedIndex_ = 0;
    
    return true;
}

std::shared_ptr<FileNode> FileTree::getRoot() const {
    return root_;
}

std::string FileTree::getSelectedFile() const {
    if (currentNode_->children.empty()) return "";
    if (selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(currentNode_->children.size())) return "";
    
    return currentNode_->children[selectedIndex_]->path;
}

void FileTree::navigateUp() {
    if (selectedIndex_ > 0) {
        selectedIndex_--;
    }
}

void FileTree::navigateDown() {
    if (selectedIndex_ < static_cast<int>(currentNode_->children.size()) - 1) {
        selectedIndex_++;
    }
}

void FileTree::navigateIn() {
    if (currentNode_->children.empty()) return;
    if (selectedIndex_ < 0 || selectedIndex_ >= static_cast<int>(currentNode_->children.size())) return;
    
    auto selectedNode = currentNode_->children[selectedIndex_];
    if (selectedNode->isDirectory) {
        currentNode_ = selectedNode;
        selectedIndex_ = 0;
    }
}

void FileTree::navigateOut() {
    // Need parent pointers to implement this properly
    // For now, just return to root
    currentNode_ = root_;
    selectedIndex_ = 0;
}

void FileTree::buildTree(const std::string& path, std::shared_ptr<FileNode> node) {
    DIR* dir = opendir(path.c_str());
    if (!dir) return;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;
        
        std::string fullPath = path + "/" + name;
        struct stat s;
        if (stat(fullPath.c_str(), &s) == 0) {
            auto childNode = std::make_shared<FileNode>();
            childNode->name = name;
            childNode->path = fullPath;
            childNode->isDirectory = S_ISDIR(s.st_mode);
            
            node->children.push_back(childNode);
            
            if (childNode->isDirectory) {
                buildTree(fullPath, childNode);
            }
        }
    }
    
    closedir(dir);
    
    // Sort: directories first, then alphabetically
    std::sort(node->children.begin(), node->children.end(),
        [](const std::shared_ptr<FileNode>& a, const std::shared_ptr<FileNode>& b) {
            if (a->isDirectory != b->isDirectory) {
                return a->isDirectory;
            }
            return a->name < b->name;
        });
}

} // namespace cvim
