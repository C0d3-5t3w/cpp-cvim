#ifndef CVIM_TABS_H
#define CVIM_TABS_H

#include <vector>
#include <memory>
#include <string>

namespace cvim {

class Buffer;

class TabManager {
public:
    TabManager();
    ~TabManager();
    
    bool isEmpty() const;
    void addTab(const std::shared_ptr<Buffer>& buffer);
    void removeTab(int index);
    void switchTab(int index);
    void nextTab();
    void prevTab();
    
    std::shared_ptr<Buffer> getCurrentBuffer() const;
    int getCurrentIndex() const;
    int getTabCount() const;
    
    std::vector<std::string> getTabNames() const;
    
private:
    std::vector<std::shared_ptr<Buffer> > tabs_;
    int currentTab_;
};

} // namespace cvim

#endif // CVIM_TABS_H
