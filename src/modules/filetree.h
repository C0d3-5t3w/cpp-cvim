#ifndef CVIM_FILETREE_H
#define CVIM_FILETREE_H

#include <string>
#include <vector>
#include <memory>

namespace cvim {

struct FileNode {
    std::string name;
    std::string path;
    bool isDirectory;
    std::vector<std::shared_ptr<FileNode> > children;
};

class FileTree {
public:
    FileTree();
    ~FileTree();
    
    bool loadDirectory(const std::string& path);
    std::shared_ptr<FileNode> getRoot() const;
    
    std::string getSelectedFile() const;
    void navigateUp();
    void navigateDown();
    void navigateIn();
    void navigateOut();
    
private:
    void buildTree(const std::string& path, std::shared_ptr<FileNode> node);
    
    std::shared_ptr<FileNode> root_;
    std::shared_ptr<FileNode> currentNode_;
    int selectedIndex_;
};

} // namespace cvim

#endif // CVIM_FILETREE_H
