#ifndef ICOMMAND_H
#define ICOMMAND_H

class ICommand {
public:
  virtual ~ICommand() {}

  virtual void execute() = 0;
  virtual void update() {}
  virtual bool resolved() const { return true; }
  virtual void cancel() {}
};

#endif
