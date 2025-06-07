#pragma once

#include <memory>

struct ManagedData {
  int someValue;
  float otherValue;
};


class SystemDataManager {

public:
  SystemDataManager() {
    data = std::make_unique<ManagedData>();
  }

  ManagedData* getData() {
    return data.get();
  }

private:
  std::unique_ptr<ManagedData> data;
};