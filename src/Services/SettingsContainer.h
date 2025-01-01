#pragma once

#include "Settings/Settings.h"

#include <memory>

namespace RustRconClient {

struct SettingsContainerPrivate;

class SettingsContainer {

 public:
  SettingsContainer();
  virtual ~SettingsContainer();

  /// @brief Чтение настроек. Если файл настроек отсутствует, возвращает false. Если прочитан, но формат не соответствует ожидаемому, испускается исключение
  bool read();

  /// @brief Запись настроек
  void write();

  /// @brief Производится чтение настроек, в случае их отсутствия создаются настройки по умолчанию
  void readOrCreateDefault();

  const Settings& settings() const;

 private:
  std::unique_ptr<SettingsContainerPrivate> _pImpl;
};

}  // namespace RustRconClient