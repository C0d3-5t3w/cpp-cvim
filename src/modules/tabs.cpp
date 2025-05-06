#include "tabs.h"
#include "editor.h"
#include "../utils/utils.h"

namespace cvim {

TabManager::TabManager() : currentTab_(-1) {}

TabManager::~TabManager() {}

bool TabManager::isEmpty() const {
    return tabs_.empty();
}

void TabManager::addTab(const std::shared_ptr<Buffer>& buffer) {
    tabs_.push_back(buffer);
    if (currentTab_ < 0) {
        currentTab_ = 0;
    }
}

void TabManager::removeTab(int index) {
    if (index >= 0 && index < static_cast<int>(tabs_.size())) {
        tabs_.erase(tabs_.begin() + index);
        
        // Adjust current tab index
        if (currentTab_ >= static_cast<int>(tabs_.size())) {
            currentTab_ = tabs_.empty() ? -1 : tabs_.size() - 1;
        }
    }
}

void TabManager::switchTab(int index) {
    if (index >= 0 && index < static_cast<int>(tabs_.size())) {
        currentTab_ = index;
    }
}

void TabManager::nextTab() {
    if (!tabs_.empty()) {
        currentTab_ = (currentTab_ + 1) % tabs_.size();
    }
}

void TabManager::prevTab() {
    if (!tabs_.empty()) {
        currentTab_ = (currentTab_ + tabs_.size() - 1) % tabs_.size();
    }
}

std::shared_ptr<Buffer> TabManager::getCurrentBuffer() const {
    if (currentTab_ >= 0 && currentTab_ < static_cast<int>(tabs_.size())) {
        return tabs_[currentTab_];
    }
    return nullptr;
}

int TabManager::getCurrentIndex() const {
    return currentTab_;
}

int TabManager::getTabCount() const {
    return tabs_.size();
}

std::vector<std::string> TabManager::getTabNames() const {
    std::vector<std::string> names;
    // Use traditional for loop instead of range-based for
    for (size_t i = 0; i < tabs_.size(); ++i) {
        const auto& tab = tabs_[i]; // Keep auto for clarity
        std::string name = tab->getFilePath();
        if (name.empty()) {
            name = "[No Name]";
        } else {
            name = getFileName(name);
        }
        names.push_back(name);
    }
    return names;
}

} // namespace cvim
