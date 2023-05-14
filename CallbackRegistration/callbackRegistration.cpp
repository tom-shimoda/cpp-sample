#include <iostream>
#include <memory>
#include <list>
#include <functional>

using MessengerOnSuccess = std::function<void(const std::string &message)>;
using MessengerOnError = std::function<void()>;

class Messenger
{
public:
  Messenger() = default;
  ~Messenger()
  {
    for (auto &&v : on_success_)
    {
      delete v;
    }
    for (auto &&v : on_error_)
    {
      delete v;
    }
  }

  void setHandler(MessengerOnSuccess *on_success, MessengerOnError *on_error)
  {
    on_success_.push_back(on_success);
    on_error_.push_back(on_error);
  }

  void removeHandler(MessengerOnSuccess *on_success, MessengerOnError *on_error)
  {
    on_success_.remove(on_success);
    delete on_success;
    on_error_.remove(on_error);
    delete on_error;
  }

  void showMessage(const std::string &message)
  {
    if (message.size() == 0)
    {
      // Error
      for (auto &&v : on_error_)
      {
        (*v)();
      }
      return;
    }

    // Success
    for (auto &&v : on_success_)
    {
      (*v)(message);
    }
  }

private:
  std::list<MessengerOnSuccess *> on_success_;
  std::list<MessengerOnError *> on_error_;
};

class Hoge
{
  std::string name_;

public:
  explicit Hoge(std::string name) : name_(name) {}
  ~Hoge() = default;

  void success(const std::string &message)
  {
    std::cout << "call OnSuccess(): " << message << ", name: " << name_ << std::endl;
  }
  void error()
  {
    std::cout << "call OnError()"
              << ", name: " << name_ << std::endl;
  }
};

int main()
{
  auto messenger = std::make_unique<Messenger>();

  auto hoge = new Hoge("hoge1");

  MessengerOnSuccess *success = new MessengerOnSuccess([&](const std::string &m)
                                                       { hoge->success(m); });
  MessengerOnError *error = new MessengerOnError([&]()
                                                 { hoge->error(); });
  messenger->setHandler(
      success,
      error);

  messenger->setHandler(
      new MessengerOnSuccess([](const std::string &message)
                             { std::cout << "call lamda OnSuccess()" << std::endl; }),
      new MessengerOnError([]()
                           { std::cout << "call lamda OnError()" << std::endl; }));

  messenger->showMessage("Show message"); // success
  messenger->showMessage(""); // error

  messenger->removeHandler(success, error);

  std::cout << "---------------------------------" << std::endl;
  messenger->showMessage("Show message"); // success
  messenger->showMessage(""); // error

  delete hoge;
  return 0;
}